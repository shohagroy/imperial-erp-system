<?php

function clone_invoice($type=ST_SUPPINVOICE, $order_no=0, $invoice_no=0) {
	global $Refs;

	$cart = $_SESSION['PO'];
	$cart->order_no = 0;
    $cart->trans_type = ST_SUPPINVOICE;
	$cart->orig_order_date = Today();
    $cart->reference = get_trans_ref(ST_SUPPINVOICE, $invoice_no);
    $cart->old_order = $order_no; // keep the old order number for later deleting purpose
    $cart->Comments .= "\n"._('Modified from invoice #').$invoice_no;
    $title = "Direct Purchase Invoice Entry From Template";
    for ($i = 0; $i < count($cart->line_items); $i++) {
        $cart->line_items[$i]->qty_received = 0;
        $cart->line_items[$i]->qty_inv = 0;
    }

    $_SESSION['PO'] = &$cart;
}

function custom_po_header(&$order) {
	global $Ajax, $Refs;

	$editable = ($order->order_no == 0);

	start_outer_table(TABLESTYLE2, "width='80%'");

	table_section(1);
	if ($editable) {
		if (!isset($_POST['supplier_id']) && (get_global_supplier() != ALL_TEXT))
			$_POST['supplier_id'] = get_global_supplier();

		supplier_list_row(_("Supplier:"), 'supplier_id', null, false, true, false, true);
		if(isset($_GET['InvoiceNo']))
			$_POST['InvoiceNo'] = $_GET['InvoiceNo'];
		hidden('InvoiceNo');
	}
	else {
		hidden('supplier_id', $order->supplier_id);
		label_row(_("Supplier:"), $order->supplier_name);
	}

	if ($order->supplier_id != get_post('supplier_id',-1)) {
		$old_supp = $order->supplier_id;
		get_supplier_details_to_order($order, $_POST['supplier_id']); 
		get_duedate_from_terms($order);
		$_POST['due_date'] = $order->due_date;

		// supplier default price update
		foreach ($order->line_items as $line_no=>$item) {
			$line = &$order->line_items[$line_no];
			$line->price =  get_purchase_price ($order->supplier_id, $line->stock_id);
			$line->quantity =
				$line->quantity/get_purchase_conversion_factor ($old_supp, $line->stock_id)
					*get_purchase_conversion_factor ($order->supplier_id, $line->stock_id);
		}
		$Ajax->activate('items_table');
		$Ajax->activate('due_date');
	}
	set_global_supplier($_POST['supplier_id']);

	$date_text = $order->trans_type == ST_PURCHORDER ? _("Order Date:") : ($order->trans_type == ST_SUPPRECEIVE ? _("Delivery Date:") : _("Invoice Date:"));
	if($_SESSION['wa_current_user']->can_access('SA_DATECHANGE'))
		date_row($date_text, 'OrderDate', '', true, 0, 0, 0, null, true);
	else {
		label_row($date_text, $order->orig_order_date);
		hidden('OrderDate', $order->orig_order_date);
	}

	if (isset($_POST['_OrderDate_changed'])) {
		$order->orig_order_date = $_POST['OrderDate'];
		get_duedate_from_terms($order);
		$_POST['due_date'] = $order->due_date;
		$Ajax->activate('due_date');
	}
	supplier_credit_row($order->supplier_id, $order->credit);

	if (!is_company_currency($order->curr_code)) {
		label_row(_("Supplier Currency:"), $order->curr_code);
		exchange_rate_display(get_company_currency(), $order->curr_code,
			$_POST['OrderDate']);
	}

	if ($editable && empty($_POST['InvoiceNo']))
		ref_row(_("Reference:"), 'ref', '', null, false, $order->trans_type, array('date'=> @$_POST['OrderDate']));
	else {
		hidden('ref', $order->reference);
		label_row(_("Reference:"), $order->reference);
	}

	table_section(2);

	if ($order->trans_type==ST_SUPPINVOICE) {
		if($_SESSION['wa_current_user']->can_access('SA_DATECHANGE'))
			date_row(_("Due Date:"), 'due_date', '', false, 0, 0, 0, null, true);
		else {
			label_row(_("Due Date:"), $order->due_date);
			hidden('due_date', $order->due_date);
		}
	}

	text_row(_("Supplier's Reference:"), 'supp_ref', null, 20, 60);

	if (get_company_pref('use_dimension'))
		dimensions_list_row(_('Dimension').':', 'dimension', null, true, _('Default'), false, 1);
	if (get_company_pref('use_dimension') == 2)
		dimensions_list_row(_('Dimension 2').':', 'dimension2', null, true, _('Default'), false, 2);
	if($_SESSION['wa_current_user']->can_access('SA_LOCCHANGE'))
		locations_list_row(_("Receive Into:"), 'StkLocation', null, false, true, $order->fixed_asset); 
	else {
		$pos = get_sales_point($_SESSION['wa_current_user']->pos);
		label_row(_("Receive Into:"), $pos['location_name']);
		$_POST['StkLocation'] = $pos['pos_location'];
		hidden('StkLocation');
	}

	table_section(3);

	if (!isset($_POST['StkLocation']) || $_POST['StkLocation'] == "" || isset($_POST['_StkLocation_update']) || !isset($_POST['delivery_address']) || $_POST['delivery_address'] == "") {
		/*If this is the first time the form loaded set up defaults */

		$loc_row = get_item_location(get_post('StkLocation'));
		if ($loc_row) {
			$_POST['delivery_address'] = $loc_row["delivery_address"];
			$Ajax->activate('delivery_address');
			$_SESSION['PO']->Location = $_POST['StkLocation'];
			$_SESSION['PO']->delivery_address = $_POST['delivery_address'];

		}
		else {
			display_error(_("The default stock location set up for this user is not a currently defined stock location. Your system administrator needs to amend your user record."));
		}
	}

	textarea_row(_('Deliver to:'), 'delivery_address', $_POST['delivery_address'], 35, 4);

	end_outer_table(); // outer table
}

function display_custom_po_items(&$order, $editable=true) {
	display_heading(_('Order Items'));

	div_start('items_table');
	start_table(TABLESTYLE, "width='80%'");

	$new = $order->order_no == 0;

	$th = array(_('Item Code'), _('Item Description'), _('Quantity'), _('Received'), _('Unit'), _('Required Delivery Date'), $order->tax_included ? _('Price after Tax') : _('Price before Tax'), _('Line Total'), '');

	if ($order->trans_type != ST_PURCHORDER)
		array_remove($th, 5);
	if ($new)
		array_remove($th, 3);

	// if (count($order->line_items)) $th[] = '';

	table_header($th);

	$id = find_submit('Edit');
	$total = 0;
	$k = 0;

	if ($id == -1 && $editable)
		custom_po_item_controls($order, $k);
	else {
		start_row();
		label_cell('&nbsp;', "colspan=".count($th));
		end_row();
	}
	
	foreach ($order->line_items as $line_no => $po_line) {

		$qty_dec = get_qty_dec($po_line->stock_id);
		$dec = user_price_dec();

		$line_total =	round($po_line->quantity * $po_line->price, $dec);
		if (!$editable || ($id != $line_no)) {
			alt_table_row_color($k);
			label_cell($po_line->stock_id);
			label_cell($po_line->item_description);
			if ($order->fixed_asset)
				label_cell(1);
			else {
				$_POST['qty'.$line_no] = number_format2($po_line->quantity, $qty_dec);
				custom_amt_cells_ex(null, 'qty'.$line_no, 15, 15, null, null, 'line='.$line_no, $qty_dec, true);
			}

			if (!$new) {
				$_POST['qty'.$line_no] = number_format2($po_line->qty_received, $qty_dec);
				custom_amt_cells_ex(null, 'qty'.$line_no, 15, 15, null, null, 'line='.$line_no, $qty_dec, true);
			}

			label_cell($po_line->units);
			if ($order->trans_type == ST_PURCHORDER)
				label_cell($po_line->req_del_date);
			
			$_POST['price'.$line_no] = number_format2($po_line->price, $dec);
			custom_amt_cells_ex(null, 'price'.$line_no, 15, 15, null, null, 'line='.$line_no, $dec, true);
			amount_cell($line_total);

			if ($editable)
				delete_button_cell("Delete$line_no", _('Delete'), _('Remove line from document'));
			
			end_row();
		}
		else
			custom_po_item_controls($order, $k, $line_no);
		
		$total += $line_total;
	}

	$colspan = count($th)-2;

	$display_sub_total = price_format($total);

	label_row(_('Sub-total'), $display_sub_total, "colspan=$colspan align=right",'align=right', 2);

	$taxes = $order->get_taxes(input_num('freight_cost'));
	$tax_total = display_edit_tax_items($taxes, $colspan, $order->tax_included, 2, $order->trans_type==ST_SUPPINVOICE);
	$display_total = price_format(($total + input_num('freight_cost') + $tax_total));

	start_row();
	label_cells(_('Amount Total'), $display_total, "colspan=$colspan align='right'", "align='right'");
	$order->trans_type == ST_SUPPINVOICE ? submit_cells('update', _('Update'), "colspan=2 align='center'", _('Refresh'), true) : label_cell('', 'colspan=2');
	end_row();

	end_table(1);
	div_end();
}

//--------------------------------------------------------------------------------

function custom_po_item_controls(&$order, &$rowcounter, $line_no=-1) {
	global $Ajax, $SysPrefs;

	alt_table_row_color($rowcounter);
	$new = $order->order_no == 0;

	$dec2 = 0;
	$id = find_submit('Edit');
	if (($id != -1) && $line_no == $id) {

		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$dec = get_qty_dec($_POST['stock_id']);
		$_POST['qty'] = qty_format($order->line_items[$id]->quantity, $_POST['stock_id'], $dec);
		$_POST['price'] = price_decimal_format($order->line_items[$id]->price, $dec2);
		if ($order->trans_type == ST_PURCHORDER)
			$_POST['req_del_date'] = $order->line_items[$id]->req_del_date;

		$_POST['units'] = $order->line_items[$id]->units;
		$_POST['item_description'] = $order->line_items[$id]->item_description;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);

		if ($order->line_items[$id]->descr_editable)
			text_cells(null,'item_description', null, 45, 150);
		else {
			hidden('item_description', $_POST['item_description']);
			label_cell($order->line_items[$id]->item_description); 
		}

		$Ajax->activate('items_table');
		$qty_rcvd = $order->line_items[$id]->qty_received;
	}
	else {

		// Manufactured item can be purchased
		if ($order->fixed_asset)
			stock_purchasable_fa_list_cells(null, 'stock_id', null, false, true, false, true, $order->line_items);
		else
			stock_items_list_cells(null, 'stock_id', null, false, true, false, true, array('editable' => 30, 'where'=>array('NOT no_purchase'), 'format'=>'custom_format_stock_items_po'));

		if (list_updated('stock_id')) {
			$Ajax->activate('price');
			$Ajax->activate('units');
			$Ajax->activate('qty');
			$Ajax->activate('req_del_date');
			$Ajax->activate('line_total');
		}

		$dec = 0;
		$_POST['units'] = '';
		$item_info = get_item_edit_info($_POST['stock_id']);

		if ($item_info) {
			$_POST['units'] = $item_info['units'];
			$dec = $item_info['decimals'];
		}
		$_POST['qty'] =	number_format2(get_purchase_conversion_factor ($order->supplier_id, $_POST['stock_id']), $dec);
		$_POST['price'] = price_decimal_format(get_purchase_price ($order->supplier_id, $_POST['stock_id']), $dec2);
		if ($order->trans_type == ST_PURCHORDER)
			$_POST['req_del_date'] = add_days(Today(), $SysPrefs->default_receival_required_by());
		$qty_rcvd = '';
	}

	if ($order->fixed_asset) {
		label_cell(1, '', 'qty');
		hidden('qty', 1);
	}
	else
		qty_cells(null, 'qty', null, null, null, $dec);

	if (!$new)
		qty_cell($qty_rcvd, false, $dec);

	label_cell($_POST['units'], '', 'units');
	if ($order->trans_type == ST_PURCHORDER)
		date_cells(null, 'req_del_date', '', null, 0, 0, 0);
	if ($qty_rcvd > 0) {
		amount_decimal_cell($_POST['price']);
		hidden('price', $_POST['price']);
	}	
	else	
		amount_cells(null, 'price', null, null, null, $dec2);

	$line_total = round(input_num('qty') * input_num('price'),  user_price_dec());
	amount_cell($line_total, false, '','line_total');

	if ($id!=-1) {
		button_cell('UpdateLine', _('Update'), _('Confirm changes'), ICON_UPDATE);
		button_cell('CancelUpdate', _('Cancel'), _('Cancel changes'), ICON_CANCEL);
		hidden('line_no', $line_no);
		set_focus('qty');
	}
	else
		submit_cells('EnterLine', _('Add Item'), "colspan=2 align='center'", _('Add new item to document'), true);

	end_row();
}
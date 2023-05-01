<?php

include_once($path_to_root.'/modules/sales_partial_payment/includes/ui/location_ui.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/ui/branches_ui.inc');

function custom_format_stock_items_sales($row) {
	$ret = (user_show_codes() ?  ($row[0] . "&nbsp;-&nbsp;") : "") . $row[1];

	if(get_company_pref('partial_show_item_status')) {
		$stock_id = $row[0];

		$price = get_kit_price($stock_id, $_SESSION['Items']->customer_currency, $_SESSION['Items']->sales_type, $_SESSION['Items']->price_factor, get_post('OrderDate'));

		$ret .= "\n"._('Price: ').number_format2($price, user_price_dec());

		$demand_qty = get_demand_qty($stock_id, $_SESSION['Items']->Location);
		$demand_qty += get_demand_asm_qty($stock_id, $_SESSION['Items']->Location);
		$qoh = get_qoh_on_date($stock_id, $_SESSION['Items']->Location);

		$mb_flag = get_mb_flag($stock_id);

		if (!is_service($mb_flag))
			$ret .= "\n"._('Available: ').number_format2($qoh - $demand_qty, get_qty_dec($stock_id));
	}

	return $ret;
}

function custom_format_stock_items_po($row) {
	$ret = (user_show_codes() ?  ($row[0] . "&nbsp;-&nbsp;") : "") . $row[1];

	if(get_company_pref('partial_show_item_status')) {
		$stock_id = $row[0];

		$price = get_purchase_price($_SESSION['PO']->supplier_id, $stock_id);

		$ret .= "\n"._('Price: ').number_format2($price, user_price_dec());

		$demand_qty = get_demand_qty($stock_id, $_SESSION['PO']->Location);
		$demand_qty += get_demand_asm_qty($stock_id, $_SESSION['PO']->Location);
		$qoh = get_qoh_on_date($stock_id, $_SESSION['PO']->Location);

		$mb_flag = get_mb_flag($stock_id);

		if (!is_service($mb_flag))
			$ret .= "\n"._('Available: ').number_format2($qoh - $demand_qty, get_qty_dec($stock_id));
	}

	return $ret;
}

function custom_sales_items_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false, $editkey=false) {
	if ($editkey)
		set_editor('item', $name, $editkey);

	if ($label != null)
		echo "<td>$label</td>\n";
	echo sales_items_list($name, $selected_id, $all_option, $submit_on_change, '', array('cells'=>true, 'max'=>50, 'format'=>'custom_format_stock_items_sales'));
}

function custom_sales_items_list($name, $selected_id=null, $all_option=false, $submit_on_change=false, $type='', $opts=array()) {
	// all sales codes
	$sql = "SELECT i.item_code, i.description, c.description, count(*)>1 as kit,
			 i.inactive, if(count(*)>1, '0', s.editable) as editable
			FROM
			".TB_PREF."stock_master s,
			".TB_PREF."item_codes i
			LEFT JOIN
			".TB_PREF."stock_category c
			ON i.category_id=c.category_id
			WHERE i.stock_id=s.stock_id
      AND mb_flag != 'F'";

	
	if ($type == 'local')	{ // exclude foreign codes
		$sql .=	" AND !i.is_foreign"; 
	} elseif ($type == 'kits') { // sales kits
		$sql .=	" AND !i.is_foreign AND i.item_code!=i.stock_id";
	}
	$sql .= " AND !i.inactive AND !s.inactive AND !s.no_sale";
	$sql .= " GROUP BY i.item_code";

	return combo_input($name, $selected_id, $sql, 'i.item_code', 'c.description',
		array_merge(
		  array(
			'format' => '_format_stock_items',
			'spec_option' => $all_option===true ?  _("All Items") : $all_option,
			'spec_id' => ALL_TEXT,
			'search_box' => true,
			'search' => array("i.item_code", "c.description", "i.description"),
			'search_submit' => get_company_pref('no_item_list')!=0,
			'size'=>15,
			'select_submit'=> $submit_on_change,
			'category' => 2,
			'order' => array("CASE 
				WHEN i.item_code = '".@$_POST['_stock_id_edit']."' THEN 0 
				WHEN i.item_code LIKE '".@$_POST['_stock_id_edit']."%' THEN 1 
				WHEN i.item_code LIKE '%".@$_POST['_stock_id_edit']."%' THEN 2 
				WHEN i.item_code LIKE '%".@$_POST['_stock_id_edit']."' THEN 3
				ELSE 4 END"),
			'editable' => 30,
			'max' => 255
		  ), $opts), $type == 'kits' ? $type : "stock_sales" );
}

function custom_amt_cells_ex($label, $name, $size, $max=null, $init=null, $title=null, $params=null, $dec=null, $submit_on_change=false) {
  	global $Ajax;

	default_focus($name);
	if (!isset($dec))
	  	$dec = user_price_dec();
	if (!isset($_POST[$name]) || $_POST[$name] == "") {
		if ($init)
			$_POST[$name] = $init;
		else
			$_POST[$name] = "";
	}
	if ($label != null)
		label_cell($label);

	if (!isset($max))
		$max = $size;

	echo "<td align='right'>";
	$class = $submit_on_change ? 'class="amountbox"' : '';
	echo "<input style='text-align:right;font-family: Verdana, Arial, Helvetica;border:0;background-color:transparent;' $class type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" dec=\"$dec\" $params value=\"" . $_POST[$name]. "\""
	 .($title ? " title='$title'": '')." >";

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $_POST[$name]);
}

function user_cash_accounts_list_row($label, $name, $selected_id=null, $submit_on_change=false, $all_option=false, $user_acc_only=false) {

	$sql = "SELECT id, bank_account_name, bank_curr_code, inactive
		FROM ".TB_PREF."bank_accounts
		WHERE account_type=".BT_CASH;

	if($user_acc_only) {
		$pos = get_sales_point($_SESSION['wa_current_user']->pos);
		$sql .= " AND id = ".db_escape($pos['pos_account']);
	}

	if ($label != null)
		echo "<tr><td class='label'>$label</td>\n";
	echo "<td>";
	echo combo_input($name, $selected_id, $sql, 'id', 'bank_account_name',
		array(
			'spec_option' => $all_option,
			'spec_id' => ALL_TEXT,
			'format' => '_format_add_curr',
			'select_submit'=> $submit_on_change,
			'async' => true
		) );
	echo "</td></tr>\n";
}

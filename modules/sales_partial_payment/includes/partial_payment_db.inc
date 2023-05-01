<?php

include_once($path_to_root.'/modules/sales_partial_payment/includes/db/location_db.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/db/branches_db.inc');

function save_partial_payment_trans($trans_no, $trans_type, $branch, $loc, $disc=0) {
	$sql = "INSERT INTO ".TB_PREF."partial_payment_trans (trans_no, type, branch_id, loc_id, discount_amt) VALUES (".db_escape($trans_no).",".db_escape($trans_type).",".db_escape($branch).",".db_escape($loc).",".db_escape($disc).")";
	db_query($sql, 'could not insert partial payment trans');
}

function get_partial_payment_trans($trans_no=false, $trans_type=false) {
	$sql = "SELECT p.*, o.contact_phone FROM ".TB_PREF."partial_payment_trans p, ".TB_PREF."debtor_trans t, ".TB_PREF."sales_orders o
		WHERE t.trans_no = p.trans_no  AND t.type = p.type AND t.order_ = o.order_no";

	if($trans_no && $trans_type)
		$sql .= " AND p.trans_no = ".db_escape($trans_no)." AND p.type = ".db_escape($trans_type);

	$result = db_query($sql, 'could not get partial payment trans');
	return $trans_no && $trans_type ? db_fetch($result) : $result;
}

function write_partial_sales_invoice(&$invoice) {
	global $Refs;

	$trans_no = $invoice->trans_no;
	if (is_array($trans_no))
		$trans_no = key($trans_no);

	$date_ = $invoice->document_date;
	$charge_shipping = $invoice->freight_cost;
	$disc_amt = $invoice->total_discount;

	begin_transaction();

	hook_db_prewrite($invoice, ST_SALESINVOICE);
	$company_data = get_company_prefs();

	$branch_data = get_branch_accounts($invoice->Branch);

	$customer = get_customer($invoice->customer_id);

	add_new_exchange_rate($customer['curr_code'], $date_, $invoice->ex_rate);

	// offer price values without freight costs
	$items_total = $invoice->get_items_total_dispatch();
	$freight_tax = $invoice->get_shipping_tax();

	if (!$invoice->is_prepaid())
		update_customer_trans_version(get_parent_type(ST_SALESINVOICE), $invoice->src_docs);
	elseif (count($invoice->prepayments)) {	// partial invoice
		$last_payment = end($invoice->prepayments);
		$gl_date = sql2date($last_payment['tran_date']);
	} else {	// final invoice
		$gl_date = $invoice->document_date;
	}

	$ov_gst = 0;
	$taxes = $invoice->get_taxes(); // all taxes with freight_tax
	$dec = user_price_dec();
	foreach ($taxes as $taxitem) {
		$taxitem['Value'] =  round2($taxitem['Value'], $dec);
		$ov_gst +=  $taxitem['Value'];
	}

	if($invoice->tax_included==0) {
		$items_added_tax = $ov_gst-$freight_tax;
		$freight_added_tax = $freight_tax;
	} else {
		$items_added_tax = 0;
		$freight_added_tax = 0;
	}

	/* Insert/update the debtor_trans */
	$sales_order = $invoice->order_no;
	if (is_array($sales_order))
			$sales_order = $sales_order[0]; // assume all crucial SO data are same for every delivery

	if ($trans_no) {
		$allocs = get_payments_for($trans_no, ST_SALESINVOICE, $invoice->customer_id);
		delete_comments(ST_SALESINVOICE, $trans_no);
		void_gl_trans(ST_SALESINVOICE, $trans_no, true);
		void_trans_tax_details(ST_SALESINVOICE, $trans_no);
	} else
		$allocs = get_payments_for($invoice->order_no, ST_SALESORDER, $invoice->customer_id);

	if ($invoice->is_prepaid()) // selected prepayment is already in cart
	{
		$allocs = $invoice->prepayments;
		// values posted are reduced by prepaid_factor
		$prepaid_factor = $invoice->prep_amount/$invoice->get_trans_total();
	} else {
		$prepaid_factor = 1;
	}

	 // write_customer_trans have to be called after optional void_cust_allocations above
	$invoice_no = write_customer_trans(ST_SALESINVOICE, $trans_no, $invoice->customer_id,
		$invoice->Branch, $date_, $invoice->reference, $items_total - $disc_amt, 0,
		$items_added_tax, $invoice->freight_cost, $freight_added_tax,
		$invoice->sales_type, $sales_order, $invoice->ship_via, 
		$invoice->due_date, 0, 0, $invoice->dimension_id, 
		$invoice->dimension2_id, $invoice->payment, $invoice->tax_included, $invoice->prep_amount); 

	if ($trans_no == 0) {
		$invoice->trans_no = array($invoice_no=>0);
	} else
		move_trans_attachments(ST_SALESINVOICE, $trans_no, $invoice_no);

	$total = 0;
	// for prepayments use deferred income account if set
	$sales_account = $invoice->is_prepaid() ? get_company_pref('deferred_income_act') : 0;

	foreach ($invoice->line_items as $line_no => $invoice_line) {
		$qty = $invoice_line->qty_dispatched;
		$line_taxfree_price = get_tax_free_price_for_item($invoice_line->stock_id,
			$invoice_line->price * $qty, 0, $invoice->tax_included,
			$invoice->tax_group_array);

		$line_tax = get_full_price_for_item($invoice_line->stock_id,
			$invoice_line->price * $qty, 0, $invoice->tax_included,
			$invoice->tax_group_array) - $line_taxfree_price;

		write_customer_trans_detail_item(ST_SALESINVOICE, $invoice_no, $invoice_line->stock_id,
			$invoice_line->item_description, $invoice_line->qty_dispatched,
			$invoice_line->line_price(), $qty ? $line_tax/$qty : 0, $invoice_line->discount_percent,
			$invoice_line->standard_cost, $invoice_line->src_id,
			$trans_no ? $invoice_line->id : 0);

		// Update delivery items for the quantity invoiced
		if ($invoice_line->qty_old != $invoice_line->qty_dispatched)
		{
			if ($invoice->is_prepaid())
				update_prepaid_so_line($invoice_line->src_id, $invoice_line->qty_dispatched-$invoice_line->qty_old);
			else
				update_parent_line(ST_SALESINVOICE, $invoice_line->src_id, ($invoice_line->qty_dispatched-$invoice_line->qty_old));
		}
		if ($invoice_line->qty_dispatched != 0) {
			$stock_gl_code = get_stock_gl_code($invoice_line->stock_id);

			if ($invoice_line->line_price() != 0) {
				//Post sales transaction to GL credit sales

				// If there is a Branch Sales Account, then override with this,
				// else take the Item Sales Account
				if (!$invoice->is_prepaid())
					$sales_account = ($branch_data['sales_account'] != "" ? $branch_data['sales_account'] : $stock_gl_code['sales_account']);
				// If there is a Customer Dimension, then override with this,
				// else take the Item Dimension (if any)
				$dim = ($invoice->dimension_id != $customer['dimension_id'] ? $invoice->dimension_id : 
					($customer['dimension_id'] != 0 ? $customer["dimension_id"] : $stock_gl_code["dimension_id"]));
				$dim2 = ($invoice->dimension2_id != $customer['dimension2_id'] ? $invoice->dimension2_id : 
					($customer['dimension2_id'] != 0 ? $customer["dimension2_id"] : $stock_gl_code["dimension2_id"]));
				$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_, $sales_account, $dim, $dim2,
					-$line_taxfree_price*$prepaid_factor,
					$invoice->customer_id, "The sales price GL posting could not be inserted");

				if ($invoice_line->discount_percent != 0) {

					$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_,
						$branch_data["sales_discount_account"], $dim, $dim2,
						($line_taxfree_price * $invoice_line->discount_percent)*$prepaid_factor,
						$invoice->customer_id, "The sales discount GL posting could not be inserted");
				} /*end of if discount !=0 */
			}
		} /*quantity dispatched is more than 0 */
	} /*end of delivery_line loop */

	if (($items_total + $charge_shipping) != 0) {
		$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_, $branch_data["receivables_account"], $invoice->dimension_id, $invoice->dimension2_id,
			($items_total + $charge_shipping + $items_added_tax + $freight_added_tax - $disc_amt)*$prepaid_factor,
			$invoice->customer_id, "The total debtor GL posting could not be inserted");
	}
	$to_allocate = ($items_total + $charge_shipping + $items_added_tax + $freight_added_tax) - $disc_amt;

	if ($charge_shipping != 0) {
		$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_, $company_data["freight_act"], $invoice->dimension_id, $invoice->dimension2_id,
			-$invoice->get_tax_free_shipping()*$prepaid_factor, $invoice->customer_id,
			"The freight GL posting could not be inserted");
	}
	if ($disc_amt != 0) {
		$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_, $branch_data["sales_discount_account"], $invoice->dimension_id, $invoice->dimension2_id, $disc_amt * $prepaid_factor, $invoice->customer_id,
			"The discount GL posting could not be inserted");
	}
	// post all taxes
	foreach ($taxes as $taxitem) {
		if ($taxitem['Net'] != 0) {
			$ex_rate = get_exchange_rate_from_home_currency(get_customer_currency($invoice->customer_id), $date_);
			add_trans_tax_details(ST_SALESINVOICE, $invoice_no, $taxitem['tax_type_id'],
				$taxitem['rate'], $invoice->tax_included, $prepaid_factor*$taxitem['Value'],
				 $taxitem['Net'], $ex_rate, $date_, $invoice->reference, TR_OUTPUT);
			if (isset($taxitem['sales_gl_code']) && !empty($taxitem['sales_gl_code']) && $taxitem['Value'] != 0)
				$total += add_gl_trans_customer(ST_SALESINVOICE, $invoice_no, $date_, $taxitem['sales_gl_code'], $invoice->dimension_id, $invoice->dimension2_id,
					(-$taxitem['Value'])*$prepaid_factor, $invoice->customer_id,
					"A tax GL posting could not be inserted");
		}
	}

	/*Post a balance post if $total != 0 */
	add_gl_balance(ST_SALESINVOICE, $invoice_no, $date_, -$total, PT_CUSTOMER, $invoice->customer_id);	

	add_comments(ST_SALESINVOICE, $invoice_no, $date_, $invoice->Comments);

	if ($trans_no == 0) {
		$Refs->save(ST_SALESINVOICE, $invoice_no, $invoice->reference, null, $invoice->fixed_asset);
		if ($invoice->payment_terms['cash_sale'] && $invoice->pos['pos_account']) {
			$amount = $items_total + $items_added_tax + $invoice->freight_cost 
				+ $freight_added_tax;
			if ($amount != 0) {
				// to use debtors.pmt_discount on cash sale:
				// extend invoice entry page with final amount after discount 
				// and change line below.
				$discount = 0; // $invoice->cash_discount*$amount;
				$payment_info = $invoice->pos['pos_name'].' #'.$invoice_no;
				if (!empty($invoice->payment_info))
					$payment_info .= ' ' . $invoice->payment_info;
				$pmtno = write_customer_payment(0, $invoice->customer_id, 
					$invoice->Branch, $invoice->pos['pos_account'], $date_,
					$Refs->get_next(ST_CUSTPAYMENT, null, array('customer' => $invoice->customer_id,
						'branch' => $invoice->Branch, 'date' => $date_)),
					$amount-$discount-$disc_amt, $discount, $payment_info,
					0,0,0,$invoice->dimension_id, $invoice->dimension2_id);
				add_cust_allocation($amount-$disc_amt, ST_CUSTPAYMENT, $pmtno, ST_SALESINVOICE, $invoice_no, $invoice->customer_id, $date_);

				update_debtor_trans_allocation(ST_SALESINVOICE, $invoice_no, $invoice->customer_id);
				update_debtor_trans_allocation(ST_CUSTPAYMENT, $pmtno, $invoice->customer_id);
			}
		}
	}
	reallocate_payments($invoice_no, ST_SALESINVOICE, $date_, $to_allocate, $allocs, $invoice->customer_id);
	hook_db_postwrite($invoice, ST_SALESINVOICE);

	commit_transaction();

	return $invoice_no;
}

function get_disc_from_invoice($trans_no) {
	$sql = "SELECT discount_amt FROM ".TB_PREF."partial_payment_trans WHERE trans_no = ".db_escape($trans_no)." AND type = ".ST_SALESINVOICE;
	$result = db_query($sql, 'cannot retrieved discount_amt');

	$row = db_fetch($result);

	return isset($row[0]) ? $row[0] : 0;
}

function get_trans_ref($type=ST_SALESINVOICE, $trans_no=0) {

	$table = 'debtor_trans';
	$type_col = 'type';
	$trans = 'trans_no';
	if($type == ST_SUPPINVOICE)
		$table = 'supp_trans';
	elseif($type == ST_SALESORDER) {
		$table = 'sales_orders';
		$type_col = 'trans_type';
		$trans = 'order_no';
	}

	$sql = "SELECT reference FROM ".TB_PREF."$table WHERE $trans=".db_escape($trans_no)." AND $type_col = ".db_escape($type);

	$res = db_query ($sql, 'cannot find invoice reference');

	return db_fetch($res)[0];
}

function get_customer_inquiry_sql($from, $to, $cust_id=ALL_TEXT, $user_id=false, $filter=ALL_TEXT, $show_voided=0, $phone_number='') {

  	$sql = "SELECT 
  		trans.type, 
		trans.trans_no, 
		trans.order_, 
		trans.reference,
		trans.tran_date, 
		trans.due_date, 
		debtor.name, 
		sales_orders.contact_phone, 
		branch.br_name,
		user.user_id,
		IF(trans.prep_amount, trans.prep_amount, trans.ov_amount + trans.ov_gst + trans.ov_freight 
			+ trans.ov_freight_tax + trans.ov_discount)	AS TotalAmount,"
		. "IF(trans.type IN(".implode(',',  array(ST_CUSTCREDIT,ST_CUSTPAYMENT,ST_BANKDEPOSIT))."), -1, 1)
				*(IF(trans.prep_amount, trans.prep_amount, trans.ov_amount + trans.ov_gst + trans.ov_freight 
			+ trans.ov_freight_tax + trans.ov_discount)-trans.alloc) Balance, 
		debtor.debtor_no,";

		$sql .= "trans.alloc AS Allocated,
		((trans.type = ".ST_SALESINVOICE." || trans.type = ".ST_JOURNAL.")
			AND trans.due_date < '" . date2sql(Today()) . "') AS OverDue ,
		Sum(line.quantity-line.qty_done) AS Outstanding,
		Sum(line.qty_done) AS HasChild,
		trans.prep_amount
		FROM "
			.TB_PREF."debtor_trans as trans
			LEFT JOIN ".TB_PREF."sales_orders as sales_orders
				ON trans.order_=sales_orders.order_no AND trans.type =".ST_SALESINVOICE."
			LEFT JOIN ".TB_PREF."debtor_trans_details as line
				ON trans.trans_no=line.debtor_trans_no AND trans.type=line.debtor_trans_type
			LEFT JOIN ".TB_PREF."voided as v
				ON trans.trans_no=v.id AND trans.type=v.type
                        LEFT JOIN ".TB_PREF."audit_trail as audit ON (trans.type=audit.type AND trans.trans_no=audit.trans_no)
                        LEFT JOIN ".TB_PREF."users as user ON (audit.user=user.id)
			LEFT JOIN ".TB_PREF."cust_branch as branch ON trans.branch_code=branch.branch_code,"
			.TB_PREF."debtors_master as debtor
		WHERE debtor.debtor_no = trans.debtor_no"; // exclude voided transactions and self-balancing (journal) transactions:
    if (!$show_voided) 	
    	 	$sql .= " AND ISNULL(v.date_) AND (trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount) != 0";

	if ($filter == '2')
		$sql .= " AND ABS(IF(trans.prep_amount, trans.prep_amount, trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount)-trans.alloc)>".FLOAT_COMP_DELTA;
	else
		$sql .= " AND trans.tran_date >= '".date2sql($from)."' AND trans.tran_date <= '".date2sql($to)."'";

   	if ($cust_id != ALL_TEXT)
   		$sql .= " AND trans.debtor_no = ".db_escape($cust_id);

   	if ($filter != ALL_TEXT) {
   		if ($filter == '1')
   			$sql .= " AND (trans.type = ".ST_SALESINVOICE.") ";
   		elseif ($filter == '2')
   			$sql .= " AND (trans.type <> ".ST_CUSTDELIVERY.") ";
   		elseif ($filter == '3')
			$sql .= " AND (trans.type = ".ST_CUSTPAYMENT." OR trans.type = ".ST_BANKDEPOSIT." OR trans.type = ".ST_BANKPAYMENT.") ";
   		elseif ($filter == '4')
			$sql .= " AND trans.type = ".ST_CUSTCREDIT." ";
   		elseif ($filter == '5')
			$sql .= " AND trans.type = ".ST_CUSTDELIVERY." ";

    	if ($filter == '2')
    		$sql .= " AND trans.due_date < '".date2sql(Today())."' AND (trans.ov_amount + trans.ov_gst + trans.ov_freight_tax + trans.ov_freight + trans.ov_discount - trans.alloc > 0) ";
   	}
   	if($user_id != false)
   		$sql .= " AND user.user_id = ".db_escape($user_id);
   	if($phone_number != '')
   		$sql .= " AND sales_orders.contact_phone LIKE '%".$phone_number."%' ";
   	
	$sql .= " GROUP BY trans.trans_no, trans.type";

   	return $sql;
}

function get_loc_transfer_sql($from, $to, $str='', $loc=false, $item=false) {
	$sql = "SELECT from_tbl.trans_no, from_tbl.reference, from_tbl.tran_date, from_tbl.loc_from, to_tbl.loc_to, from_tbl.description as description, to_tbl.qty, from_tbl.units, u.user_id, from_tbl.stock_id, from_tbl.loc_code as loc_code_from, to_tbl.loc_code as loc_code_to FROM 

	(SELECT move.*, item.description, item.mb_flag, item.units, stock.location_name as loc_from
		FROM ".TB_PREF."stock_moves move,".TB_PREF."locations stock,".TB_PREF."stock_master item
		WHERE move.stock_id = item.stock_id
		AND stock.loc_code=move.loc_code
		AND move.qty < 0 
		AND type=".ST_LOCTRANSFER
		." ORDER BY trans_id) from_tbl
	LEFT JOIN 
	(SELECT move.*, item.description, stock.location_name as loc_to
		FROM ".TB_PREF."stock_moves move,".TB_PREF."locations stock,".TB_PREF."stock_master item
		WHERE move.stock_id = item.stock_id
		AND stock.loc_code=move.loc_code
		AND move.qty > 0 
		AND type=".ST_LOCTRANSFER
		." ORDER BY trans_id) to_tbl
	ON from_tbl.stock_id = to_tbl.stock_id AND from_tbl.trans_no = to_tbl.trans_no AND from_tbl.type = to_tbl.type
	LEFT JOIN ".TB_PREF."audit_trail a ON a.type = from_tbl.type AND a.trans_no = to_tbl.trans_no AND a.gl_date = to_tbl.tran_date
	LEFT JOIN ".TB_PREF."users u ON u.id = a.user
	
	WHERE from_tbl.tran_date >= '".date2sql($from)."' AND from_tbl.tran_date <= '".date2sql($to)."'";

	if(!empty($str))
		$sql .= " AND (from_tbl.description LIKE ".db_escape("%$str%")." OR from_tbl.reference LIKE ".db_escape("%$str%")." OR from_tbl.loc_from LIKE ".db_escape("%$str%")." OR to_tbl.loc_to LIKE ".db_escape("%$str%")." OR from_tbl.stock_id LIKE ".db_escape("%$str%")." OR user_id LIKE ".db_escape("%$str%").")";
	if($loc)
		$sql .= " AND (from_tbl.loc_code = ".db_escape($loc)." OR to_tbl.loc_code = ".db_escape($loc).")";
	if($item)
		$sql .= " AND from_tbl.stock_id = ".db_escape($item);

	return $sql;
}

function write_transfer_review($trans_id, $reviewed=0) {
	$review_exist = check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."transfer_review WHERE trans_id = ".db_escape($trans_id));

	if(!empty($review_exist))
		$sql = "UPDATE ".TB_PREF."transfer_review SET reviewed = ".db_escape($reviewed)." WHERE trans_id = ".db_escape($trans_id);
	else
		$sql = "INSERT INTO ".TB_PREF."transfer_review (trans_id, reviewed) VALUES (".db_escape($trans_id).",".db_escape($reviewed).")";

	db_query($sql, 'could not write to transfer_review table');

	// echo $review_exist ? $review_exist : 'empty';
}

function get_audit_trail_edited($trans_type, $trans_no) {
	$sql = "SELECT * FROM ".TB_PREF."audit_trail"
		." WHERE type=".db_escape($trans_type).
			" AND trans_no=".db_escape($trans_no)." AND ISNULL(gl_seq)";

	$res = db_query($sql, "Cannot get edited audit info for transaction");
	if ($res)
		return db_fetch($res);
	return false;
}

function update_audit_trial_user($trans_type, $trans_no, $user) {
	$sql = "UPDATE ".TB_PREF."audit_trail SET user = ".db_escape($user)." WHERE type = ".db_escape($trans_type)." AND trans_no = ".db_escape($trans_no)." AND NOT ISNULL(gl_seq)";
	db_query($sql, 'could not update audit trail user');
}

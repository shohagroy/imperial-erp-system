<?php

$page_security = $_POST['PARAM_0'] == $_POST['PARAM_1'] ? 'SA_SALESTRANSVIEW' : 'SA_SALESBULKREP';

include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/includes/date_functions.inc');
include_once($path_to_root.'/includes/data_checks.inc');
include_once($path_to_root.'/sales/includes/sales_db.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/partial_payment_db.inc');

//----------------------------------------------------------------------------------------------------

function get_invoice_range($from, $to, $currency=false) {
	global $SysPrefs;

	$ref = ($SysPrefs->print_invoice_no() == 1 ? "trans_no" : "reference");

	$sql = "SELECT trans.trans_no, trans.reference, u.user_id, a.stamp";

	$sql .= " FROM ".TB_PREF."debtor_trans trans 
			LEFT JOIN ".TB_PREF."voided voided ON trans.type=voided.type AND trans.trans_no=voided.id
			LEFT JOIN ".TB_PREF."audit_trail a ON a.trans_no = trans.trans_no AND a.type = trans.type 
			LEFT JOIN ".TB_PREF."users u ON a.user = u.id ";

	if ($currency !== false)
		$sql .= " LEFT JOIN ".TB_PREF."debtors_master cust ON trans.debtor_no=cust.debtor_no";

	$sql .= " WHERE trans.type=".ST_SALESINVOICE
		." AND ISNULL(voided.id)"
		." AND trans.trans_no BETWEEN ".db_escape($from)." AND ".db_escape($to);			

	if ($currency !== false)
		$sql .= " AND cust.curr_code=".db_escape($currency);

	$sql .= " ORDER BY trans.tran_date, trans.$ref";

	return db_query($sql, "Cannot retrieve invoice range");
}

function get_open_balance($debtorno, $to=false) {
	if($to)
		$to = date2sql($to);
	$sql = "SELECT SUM(IF(t.type != ".ST_SALESINVOICE." AND NOT(t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.")), t.alloc * -1, t.alloc)) AS Allocated,
		SUM(IF(t.type = ".ST_SALESINVOICE." OR (t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.") AND t.ov_amount>0), 1, -1) *
			(abs(t.ov_amount + t.ov_gst + t.ov_freight + t.ov_freight_tax + t.ov_discount) - abs(t.alloc))) AS OutStanding
		FROM ".TB_PREF."debtor_trans t
		WHERE t.debtor_no = ".db_escape($debtorno)." 
		AND t.type <> ".ST_CUSTDELIVERY;
	if ($to)
		$sql .= " AND t.tran_date < '$to'";
	$sql .= " GROUP BY debtor_no";

	$result = db_query($sql, "No transactions were returned");
	return db_fetch($result);
}

print_invoices();

//----------------------------------------------------------------------------------------------------

function print_invoices() {
	global $path_to_root, $SysPrefs;
	
	$show_this_payment = true; // include payments invoiced here in summary

	include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$currency = $_POST['PARAM_2'];
	$email = $_POST['PARAM_3'];
	$pay_service = $_POST['PARAM_4'];
	$comments = $_POST['PARAM_5'];
	$customer = $_POST['PARAM_6'];
	$orientation = $_POST['PARAM_7'];

	if (!$from || !$to) return;

	$orientation = ($orientation ? 'L' : 'P');
	$dec = user_price_dec();

	$fno = explode("-", $from);
	$tno = explode("-", $to);
	$from = min($fno[0], $tno[0]);
	$to = max($fno[0], $tno[0]);

	//-----------#--Desc-unit--qty--prc--Disc--Tot--//
	$cols = array(0, 30, 290, 340, 390, 440, 515);

	// $headers in doctext.inc
	$aligns = array('center',	'left',	'center', 'center', 'right', 'right');

	$params = array('comments' => $comments);

	$cur = get_company_Pref('curr_default');

	if ($email == 0)
		$rep = new FrontReport(_('INVOICE'), "InvoiceBulk", user_pagesize(), 9, $orientation);
	if ($orientation == 'L')
		recalculate_cols($cols);

	$range = Array();
	if ($currency == ALL_TEXT)
		$range = get_invoice_range($from, $to);
	else
		$range = get_invoice_range($from, $to, $currency);

	while($row = db_fetch($range)) {

		if (!exists_customer_trans(ST_SALESINVOICE, $row['trans_no']))
			continue;
		$sign = 1;
		$myrow = get_customer_trans($row['trans_no'], ST_SALESINVOICE);
		$result = get_customer_trans_details(ST_SALESINVOICE, $row['trans_no']);

		if ($customer && $myrow['debtor_no'] != $customer)
			continue;
			
		$baccount = get_default_bank_account($myrow['curr_code']);
		$params['bankaccount'] = $baccount['id'];

		$branch = get_branch($myrow["branch_code"]);
		$sales_order = get_sales_order_header($myrow["order_"], ST_SALESORDER);
		if ($email == 1) {
			$rep = new FrontReport("", "", user_pagesize(), 9, $orientation);
			$rep->title = _('INVOICE');
			$rep->filename = "Invoice" . $myrow['reference'] . ".pdf";
		}	
		$rep->currency = $cur;
		$rep->Font();
		$rep->Info($params, $cols, null, $aligns);

		$contacts = get_branch_contacts($branch['branch_code'], 'invoice', $branch['debtor_no'], true);
		$baccount['payment_service'] = $pay_service;
		$rep->SetCommonData($myrow, $branch, $sales_order, $baccount, ST_SALESINVOICE, $contacts);
		$rep->formData['location_name'] = $sales_order['location_name'];
		$rep->formData['user_id'] = $row['user_id'];
		$rep->formData['contact_phone'] = $sales_order['contact_phone'];
		$rep->formData['time_stamp'] = date('h:i A', strtotime($row['stamp']));
		$top = $rep->pageHeight - $rep->topMargin;
		$num_rows = db_num_rows($result) > 24 ? 24 : db_num_rows($result);
		$rep->formData['line_bottom'] = ($top - 21*$rep->lineHeight) - ($num_rows*$rep->lineHeight) + 4;
		$rep->SetHeaderType('header4');
		$rep->NewPage();
		// calculate summary start row for later use
		$summary_start_row = $rep->bottomMargin + (19 * $rep->lineHeight);

		if ($rep->formData['prepaid']) {
			$result = get_sales_order_invoices($myrow['order_']);
			$prepayments = array();
			while($inv = db_fetch($result)) {
				$prepayments[] = $inv;
				if ($inv['trans_no'] == $row['trans_no'])
					break;
			}

			if (count($prepayments) > ($show_this_payment ? 0 : 1))
				$summary_start_row += (count($prepayments)) * $rep->lineHeight;
			else
				unset($prepayments);
		}

		$SubTotal = 0;
		$Total = $sign*($myrow["ov_freight"] + $myrow["ov_gst"] + $myrow["ov_amount"]+$myrow["ov_freight_tax"]);

		// Bottom Invoice Dues Status
		$balance = get_open_balance($myrow['debtor_no'])['OutStanding'];
		$show_balance = $balance < 0 ? '('.number_format2(-$balance, $dec).')' : number_format2($balance, $dec);
		$current_dues = $Total - $myrow['alloc'];
		$show_curnt_dues = $current_dues < 0 ? '('.number_format2(-$current_dues, $dec).')' : number_format2($current_dues, $dec);
		$prev_dues = $balance - $current_dues;
		$show_prev_dues = $prev_dues < 0 ? '('.number_format2(-$prev_dues, $dec).')' : number_format2($prev_dues, $dec);
		$row_temp = $rep->row;
		$rep->row = $rep->bottomMargin + 12*$rep->lineHeight;
		$rep->TextWrapLines($rep->cols[0], $rep->cols[3], get_company_Pref('legal_text'));
		$str_pad = strtoupper($_SESSION['wa_current_user']->prefs->pagesize) == 'LETTER' ? 36 : 30;
		$rep->NewLine();
		$rep->TextWrapLines($rep->cols[0], $rep->cols[3], $comments);
		$rep->row = $rep->bottomMargin + 13*$rep->lineHeight;
		$rep->Font('bold');
		$rep->Text($rep->cols[4], _('Dues Status:'), $rep->cols[5]);
		$rep->TextWrap($rep->cols[4], $rep->row, $rep->cols[6], str_pad('', 11, '_'), 'L');
		$rep->NewLine();
		$rep->Text($rep->cols[4], _('Grand Total:'), $rep->cols[5]);
		$rep->Text($rep->cols[5], number_format2($Total, $dec), $rep->cols[6]-2, 0, 0, 'R');
		$rep->NewLine(1.5);
		$rep->Text($rep->cols[4], _('Paid Amount:'), $rep->cols[5]);
		$rep->TextWrap($rep->cols[4], $rep->row-1, $rep->cols[6], str_pad('', $str_pad, '_'), 'L');
		$rep->Text($rep->cols[5], number_format2($myrow['alloc'], $dec), $rep->cols[6]-2, 0, 0, 'R');
		$rep->NewLine();
		$rep->Text($rep->cols[4], _('Current Dues:'), $rep->cols[5]);
		$rep->Text($rep->cols[5], $show_curnt_dues, $rep->cols[6]-2, 0, 0, 'R');
		$rep->NewLine(1.5);
		$rep->Text($rep->cols[4], _('Previous Dues:'), $rep->cols[5]);
		$rep->TextWrap($rep->cols[4], $rep->row-1, $rep->cols[5], str_pad('', $str_pad, '_'), 'L');
		$rep->Text($rep->cols[5], $show_prev_dues, $rep->cols[6]-2, 0, 0, 'R');
		$rep->NewLine();
		$rep->Text($rep->cols[4], _('Total Dues:'), $rep->cols[5]);
		$rep->Text($rep->cols[5], $show_balance, $rep->cols[6]-2, 0, 0, 'R');
		$rep->Font();
		$rep->row = $row_temp;
		// End of dues status

		$row_count = 1;
		while ($myrow2=db_fetch($result)) {
			if ($myrow2["quantity"] == 0)
				continue;

			$Net = round2($sign * ((1 - $myrow2["discount_percent"]) * $myrow2["unit_price"] * $myrow2["quantity"]), user_price_dec());
			$SubTotal += $Net;
			$DisplayPrice = number_format2($myrow2["unit_price"], $dec);
			$DisplayQty = number_format2($sign*$myrow2["quantity"], get_qty_dec($myrow2['stock_id']));
			$DisplayNet = number_format2($Net, $dec);
			if ($myrow2["discount_percent"] == 0)
				$DisplayDiscount ="";
			else
				$DisplayDiscount = number_format2($myrow2["discount_percent"]*100,user_percent_dec()) . "%";
			$c=0;
			$rep->TextCol($c++, $c,	$row_count, -2);
			$oldrow = $rep->row;
			$rep->TextCol($c++, $c, $myrow2['StockDescription'], -2);
			if (!empty($SysPrefs->prefs['long_description_invoice']) && !empty($myrow2['StockLongDescription'])) {
				$c--;
				$rep->TextCol($c++, $c, $myrow2['StockLongDescription'], -2);
			}
			$newrow = $rep->row;
			$rep->row = $oldrow;
			if ($Net != 0.0 || !is_service($myrow2['mb_flag']) || !$SysPrefs->no_zero_lines_amount()) {
				$rep->TextCol($c++, $c,	$myrow2['units'], -2);
				$rep->TextCol($c++, $c,	$DisplayQty, -2);
				
				$rep->TextCol($c++, $c,	$DisplayPrice, -2);
				// $rep->TextCol($c++, $c,	$DisplayDiscount, -2);
				$rep->TextCol($c++, $c,	$DisplayNet, -2);
			}
			$rep->Line($rep->row - 2);
			$rep->row = $newrow;
			$rep->NewLine();

			if ($rep->row < $summary_start_row)
				$rep->NewPage();
			$row_count++;
		}

		$memo = get_comments_string(ST_SALESINVOICE, $row['trans_no']);
		if ($memo != "") {
			$rep->NewLine();
			$rep->TextColLines(1, 3, $memo, -2);
		}

		$DisplaySubTot = number_format2($SubTotal,$dec);

		// set to start of summary line:
		$rep->row = $summary_start_row;
		if (isset($prepayments)) {
			// Partial invoices table
			$rep->TextCol(0, 3,_("Prepayments invoiced to this order up to day:"));
			$rep->TextCol(0, 3,	str_pad('', 150, '_'));
			$rep->cols[2] -= 20;
			$rep->aligns[2] = 'right';
			$rep->NewLine(); $c = 0; $tot_pym=0;
			$rep->TextCol(0, 3,	str_pad('', 150, '_'));
			$rep->TextCol($c++, $c, _("Date"));
			$rep->TextCol($c++, $c,	_("Invoice reference"));
			$rep->TextCol($c++, $c,	_("Amount"));

			foreach ($prepayments as $invoice) {
				if ($show_this_payment || ($invoice['reference'] != $myrow['reference'])) {
					$rep->NewLine();
					$c = 0; $tot_pym += $invoice['prep_amount'];
					$rep->TextCol($c++, $c,	sql2date($invoice['tran_date']));
					$rep->TextCol($c++, $c,	$invoice['reference']);
					$rep->TextCol($c++, $c, number_format2($invoice['prep_amount'], $dec));
				}
				if ($invoice['reference']==$myrow['reference']) break;
			}
			$rep->TextCol(0, 3,	str_pad('', 150, '_'));
			$rep->NewLine();
			$rep->TextCol(1, 2,	_("Total payments:"));
			$rep->TextCol(2, 3,	number_format2($tot_pym, $dec));
		}

		$doctype = ST_SALESINVOICE;
		$rep->row = $summary_start_row;
		$rep->cols[2] += 20;
		$rep->cols[3] += 20;
		$rep->aligns[3] = 'left';

		$rep->TextCol(3, 5, _("Sub-total"), -2);
		$rep->TextCol(5, 6,	$DisplaySubTot, -2);
		$rep->NewLine();
		if ($myrow['ov_freight'] != 0.0) {
			$DisplayFreight = number_format2($sign*$myrow["ov_freight"],$dec);
			$rep->TextCol(3, 5, _("Shipping"), -2);
			$rep->TextCol(5, 6,	$DisplayFreight, -2);
			$rep->NewLine();
		}	
		$tax_items = get_trans_tax_details(ST_SALESINVOICE, $row['trans_no']);
		$first = true;
		while ($tax_item = db_fetch($tax_items)) {
			if ($tax_item['amount'] == 0)
				continue;
			$DisplayTax = number_format2($sign*$tax_item['amount'], $dec);

			if ($SysPrefs->suppress_tax_rates() == 1)
				$tax_type_name = $tax_item['tax_type_name'];
			else
				$tax_type_name = $tax_item['tax_type_name']." (".$tax_item['rate']."%) ";

			if ($myrow['tax_included']) {
				if ($SysPrefs->alternative_tax_include_on_docs() == 1) {
					if ($first) {
						$rep->TextCol(3, 5, _("Total Tax Excluded"), -2);
						$rep->TextCol(5, 6,	number_format2($sign*$tax_item['net_amount'], $dec), -2);
						$rep->NewLine();
					}
					$rep->TextCol(3, 5, $tax_type_name, -2);
					$rep->TextCol(5, 6,	$DisplayTax, -2);
					$first = false;
				}
				else
					$rep->TextCol(3, 5, _("Included") . " " . $tax_type_name . _("Amount") . ": " . $DisplayTax, -2);
			}
			else {
				$rep->TextCol(3, 5, $tax_type_name, -2);
				$rep->TextCol(5, 6,	$DisplayTax, -2);
			}
			$rep->NewLine();
		}
		$disc_amt = get_disc_from_invoice($row['trans_no']);
		$rep->TextCol(3, 5, _('Discount Amount'), -2);
		$rep->TextCol(5, 6,	number_format2(-$disc_amt, $dec), -2);

		if ($myrow['ov_freight'] != 0.0)
			$rep->NewLine();
		else
			$rep->NewLine(2);
		$rep->Font('bold');
		if (!$myrow['prepaid'])
			$rep->Font('bold');
		$rep->TextCol(3, 5, $rep->formData['prepaid'] ? _("TOTAL ORDER VAT INCL.") : _("TOTAL INVOICE"), - 2);
		$rep->TextCol(5, 6, number_format2($Total, $dec), -2);
		if ($rep->formData['prepaid']) {
			$rep->NewLine();
			$rep->Font('bold');
			$rep->TextCol(3, 5, $rep->formData['prepaid']=='final' ? _("THIS INVOICE") : _("TOTAL INVOICE"), - 2);
			$rep->TextCol(5, 6, number_format2($myrow['prep_amount'], $dec), -2);
		}
		$words = price_in_words($rep->formData['prepaid'] ? $myrow['prep_amount'] : $Total, array( 'type' => ST_SALESINVOICE, 'currency' => $myrow['curr_code']));
		if ($words != "") {
			$r_temp = $rep->row;
			$rep->row += 2*$rep->lineHeight;
			$rep->aligns[0] = 'left';
			$rep->TextCol(0, 2, ' '._('In words:'));
			$rep->NewLine();
			$rep->TextColLines(0, 4, ' '.$myrow['curr_code'] . ": " . $words);
			$rep->row = $r_temp;
		}
			
		$rep->Font();
		if ($email == 1)
			$rep->End($email);
	}
	if ($email == 0)
		$rep->End();
}
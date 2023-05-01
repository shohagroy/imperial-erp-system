<?php

$page_security = 'SA_CUSTPAYMREP';

include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/includes/date_functions.inc');
include_once($path_to_root.'/includes/data_checks.inc');
include_once($path_to_root.'/gl/includes/gl_db.inc');
include_once($path_to_root.'/sales/includes/db/customers_db.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/partial_payment_db.inc');

//----------------------------------------------------------------------------------------------------

print_customer_balances();

function get_open_balance($debtorno, $to) {
	if($to)
		$to = date2sql($to);
	$sql = "SELECT SUM(IF(t.type = ".ST_SALESINVOICE." OR (t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.") AND t.ov_amount>0),
			 -abs(t.ov_amount + t.ov_gst + t.ov_freight + t.ov_freight_tax + t.ov_discount), 0)) AS charges,";

	$sql .= "SUM(IF(t.type != ".ST_SALESINVOICE." AND NOT(t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.") AND t.ov_amount>0),
			 abs(t.ov_amount + t.ov_gst + t.ov_freight + t.ov_freight_tax + t.ov_discount) * -1, 0)) AS credits,";		

	$sql .= "SUM(IF(t.type != ".ST_SALESINVOICE." AND NOT(t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.")), t.alloc * -1, t.alloc)) AS Allocated,";

	$sql .=	"SUM(IF(t.type = ".ST_SALESINVOICE." OR (t.type IN (".ST_JOURNAL." , ".ST_BANKPAYMENT.") AND t.ov_amount>0), 1, -1) *
			(abs(t.ov_amount + t.ov_gst + t.ov_freight + t.ov_freight_tax + t.ov_discount) - abs(t.alloc))) AS OutStanding
		FROM ".TB_PREF."debtor_trans t
		WHERE t.debtor_no = ".db_escape($debtorno)
		." AND t.type <> ".ST_CUSTDELIVERY;
	if ($to)
		$sql .= " AND t.tran_date < '$to'";
	$sql .= " GROUP BY debtor_no";

	$result = db_query($sql, 'No transactions were returned');
	return db_fetch($result);
}

function get_transactions($debtorno, $from, $to) {
	$from = date2sql($from);
	$to = date2sql($to);

	$allocated_from = 
			"(SELECT trans_type_from as trans_type, trans_no_from as trans_no, date_alloc, sum(amt) amount
			FROM ".TB_PREF."cust_allocations alloc
				WHERE person_id=".db_escape($debtorno)."
					AND date_alloc <= '$to'
				GROUP BY trans_type_from, trans_no_from) alloc_from";
	$allocated_to = 
			"(SELECT trans_type_to as trans_type, trans_no_to as trans_no, date_alloc, sum(amt) amount
			FROM ".TB_PREF."cust_allocations alloc
				WHERE person_id=".db_escape($debtorno)."
					AND date_alloc <= '$to'
				GROUP BY trans_type_to, trans_no_to) alloc_to";

	 $sql = "SELECT trans.*,
		(trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount) AS TotalAmount,
		IFNULL(alloc_from.amount, alloc_to.amount) AS Allocated,
		((trans.type = ".ST_SALESINVOICE.")	AND trans.due_date < '$to') AS OverDue
		FROM ".TB_PREF."debtor_trans trans
			LEFT JOIN ".TB_PREF."voided voided ON trans.type=voided.type AND trans.trans_no=voided.id
			LEFT JOIN $allocated_from ON alloc_from.trans_type = trans.type AND alloc_from.trans_no = trans.trans_no
			LEFT JOIN $allocated_to ON alloc_to.trans_type = trans.type AND alloc_to.trans_no = trans.trans_no

		WHERE trans.tran_date >= '$from'
			AND trans.tran_date <= '$to'
			AND trans.debtor_no = ".db_escape($debtorno)."
			AND trans.type <> ".ST_CUSTDELIVERY."
			AND ISNULL(voided.id)
		ORDER BY trans.tran_date";
	return db_query($sql, 'No transactions were returned');
}

//----------------------------------------------------------------------------------------------------

function print_customer_balances() {
	global $path_to_root, $systypes_array, $SysPrefs;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$fromcust = $_POST['PARAM_2'];
	$show_balance = $_POST['PARAM_3'];
	$currency = $_POST['PARAM_4'];
	$no_zeros = $_POST['PARAM_5'];
	$comments = $_POST['PARAM_6'];
	$orientation = $_POST['PARAM_7'];
	$destination = $_POST['PARAM_8'];
	if ($destination)
		include_once($path_to_root.'/reporting/includes/excel_report.inc');
	else
		include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$orientation = ($orientation ? 'L' : 'P');
	if ($fromcust == ALL_TEXT)
		$cust = _('All');
	else
		$cust = get_customer_name($fromcust);
		$dec = user_price_dec();

	$sb = _('Yes');

	if ($currency == ALL_TEXT) {
		$convert = true;
		$currency = _('Balances in Home Currency');
	}
	else
		$convert = false;

	if ($no_zeros)
		$nozeros = _('Yes');
	else
		$nozeros = _('No');

	$cols = array(0, 60, 140, 270, 350, 400, 460, 525);

	$headers = array(_('Date'), _('Trans #').' ('._('Type').')', _('Particulars'), '', _('Charges'), _('Credits'), _('Balance'));

	$aligns = array('left',	'left',	'left',	'left', 'right', 'right', 'right');

	$params =   array( 	0 => $comments,
						1 => array('text' => _('Period'), 'from' => $from, 		'to' => $to),
						2 => array('text' => _('Customer'), 'from' => $cust,   	'to' => ''),
						3 => array('text' => _('Show Balance'), 'from' => $sb,   	'to' => ''),
						4 => array('text' => _('Currency'), 'from' => $currency, 'to' => ''),
						5 => array('text' => _('Suppress Zeros'), 'from' => $nozeros, 'to' => ''));

	$rep = new FrontReport(_('Customer Detail Transaction'), 'CustomerDetailTrans', user_pagesize(), 9, $orientation);
	if ($orientation == 'L')
		recalculate_cols($cols);
	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->NewPage();

	$grandtotal = array(0,0,0);

	$sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master ";
	if ($fromcust != ALL_TEXT)
		$sql .= "WHERE debtor_no=".db_escape($fromcust);
	$sql .= " ORDER BY name";
	$result = db_query($sql, 'The customers could not be retrieved');

	while ($myrow = db_fetch($result)) {
		if (!$convert && $currency != $myrow['curr_code'])
			continue;

		$accumulate = 0;
		$rate = $convert ? get_exchange_rate_from_home_currency($myrow['curr_code'], Today()) : 1;
		$bal = get_open_balance($myrow['debtor_no'], $from);
		$bal['charges'] = isset($bal['charges']) ? $bal['charges'] : 0;
		$bal['credits'] = isset($bal['credits']) ? $bal['credits'] : 0;
		$init[0] = $init[1] = 0.0;
		$init[0] = round2(abs($bal['charges']*$rate), $dec);
		$init[1] = round2(abs($bal['credits']*$rate), $dec);
		$init[2] = $init[0] - $init[1];
		$accumulate += $init[2];

		$res = get_transactions($myrow['debtor_no'], $from, $to);
		if ($no_zeros && db_num_rows($res) == 0)
			continue;

		$rep->fontSize += 2;
		$rep->Font('bold');
		$rep->TextCol(0, 2, $myrow['name']);
		if ($convert)
			$rep->TextCol(2, 3,	$myrow['curr_code']);
		$rep->Font();
		$rep->fontSize -= 2;
		$rep->TextCol(3, 4,	_('Open Balance').':');
		$rep->AmountCol(4, 5, $init[0], $dec);
		$rep->AmountCol(5, 6, $init[1], $dec);
		$rep->AmountCol(6, 7, $init[2], $dec);
		$total = array(0, 0, 0, 0);
		for ($i = 0; $i < 3; $i++) {
			$total[$i] += $init[$i];
			$grandtotal[$i] += $init[$i];
		}
		$rep->NewLine(1, 2);
		$rep->Line($rep->row + 4);
		if (db_num_rows($res)==0) {
			$rep->NewLine(1, 2);
			continue;
		}

		while ($trans = db_fetch($res)) {
			if ($no_zeros && floatcmp($trans['TotalAmount'], $trans['Allocated']) == 0)
				continue;

			$rep->NewLine(1, 2);
			$rep->DateCol(0, 1,	$trans['tran_date'], true);
			$rep->TextCol(1, 3,	$trans['reference'].' ('.$systypes_array[$trans['type']].')');
			$item[0] = $item[1] = 0.0;
			if ($trans['type'] == ST_CUSTCREDIT || $trans['type'] == ST_CUSTPAYMENT || $trans['type'] == ST_BANKDEPOSIT)
				$trans['TotalAmount'] *= -1;
			if ($trans['TotalAmount'] > 0.0) {
				$item[0] = round2(abs($trans['TotalAmount']) * $rate, $dec);
				$rep->AmountCol(4, 5, $item[0], $dec);
				$accumulate += $item[0];
				$item[2] = round2($trans['Allocated'] * $rate, $dec);
			}
			else {
				$item[1] = round2(Abs($trans['TotalAmount']) * $rate, $dec);
				$rep->AmountCol(5, 6, $item[1], $dec);
				$accumulate -= $item[1];
				$item[2] = round2($trans['Allocated'] * $rate, $dec) * -1;
			}
			if (($trans['type'] == ST_JOURNAL && $item[0]) || $trans['type'] == ST_SALESINVOICE || $trans['type'] == ST_BANKPAYMENT)
				$item[2] = $item[0] - $item[1];
			else	
				$item[2] = -$item[0] - $item[1];

			$rep->AmountCol(6, 7, $accumulate, $dec);

			for ($i = 0; $i < 3; $i++) {
				$total[$i] += $item[$i];
				$grandtotal[$i] += $item[$i];
			}
			$total[2] = $total[0] - $total[1];

			if($trans['type'] == ST_SALESINVOICE || $trans['type'] == ST_CUSTCREDIT) { // Begining of invoice details
				$myrow1 = get_customer_trans($trans['trans_no'], $trans['type']);
				$details = get_customer_trans_details($trans['type'], $trans['trans_no']);
				$InvTotal = $myrow1['ov_freight'] + $myrow1['ov_gst'] + $myrow1['ov_amount']+$myrow1['ov_freight_tax'];
				$DisplayFreight = number_format2($myrow1['ov_freight'], $dec);

				$rep->NewLine();
				$dtail_oldrow = $rep->row + (3*$rep->lineHeight/4);
				$rep->Font('bold');
				$rep->TextCol(2, 3,	_('QTY').' / '._('Item'), -2);
				// $rep->TextCol(3, 4,	_('QTY'), -2);
				// $rep->TextCol(4, 5,	_('Unit'), -2);
				// $rep->TextCol(5, 6,	_('Price'), -2);
				// $rep->TextCol(6, 7,	_('Disc'), -2);
				$rep->aligns[3] = 'right';
				$rep->TextCol(3, 4,	_('Total'), -2);
				$rep->Font();

				$SubTotal = 0;
				while ($myrow2 = db_fetch($details)) {
					if ($myrow2['quantity'] == 0)
						continue;
					$Net = round2((1 - $myrow2['discount_percent']) * $myrow2['unit_price'] * $myrow2['quantity'], user_price_dec());
					$SubTotal += $Net;
					$DisplayPrice = number_format2($myrow2['unit_price'], $dec);
					$DisplayQty = number_format2($myrow2['quantity'], get_qty_dec($myrow2['stock_id']));
					$DisplayNet = number_format2($Net, $dec);
					if ($myrow2['discount_percent'] == 0)
						$DisplayDiscount ='';
					else
						$DisplayDiscount = number_format2($myrow2['discount_percent']*100,user_percent_dec()).'%';
					$rep->NewLine();
					$oldrow = $rep->row;
					$rep->TextCol(2, 3, $DisplayQty.' x '.$myrow2['StockDescription'], -2);
					if (!empty($SysPrefs->prefs['long_description_invoice']) && !empty($myrow2['StockLongDescription'])) {
						$c--;
						$rep->TextColLines($c++, $c, $myrow2['StockLongDescription'], -2);
					}
					$newrow = $rep->row;
					$rep->row = $oldrow;
					if ($Net != 0.0 || !is_service($myrow2['mb_flag']) || !$SysPrefs->no_zero_lines_amount()) {
						// $rep->TextCol(3, 4,	$DisplayQty, -2);
						// $rep->TextCol(4, 5,	$myrow2['units'], -2);
						// $rep->TextCol(5, 6,	$DisplayPrice, -2);
						// $rep->TextCol(6, 7,	$DisplayDiscount, -2);
						$rep->TextCol(3, 4,	$DisplayNet, -2);
					}
				}

				$rep->LineTo($cols[2]+40, $rep->row-2, $cols[5]-10, $rep->row-2);
				$rep->aligns[2] = 'right';

				$tax_items = get_trans_tax_details($trans['type'], $trans['trans_no']);
				while ($tax_item = db_fetch($tax_items)) {
					$tax = number_format2(abs($tax_item['amount']), $dec);

					if($tax_item['amount'] == 0)
						continue;
					$rep->NewLine();
					if ($tax_item['included_in_price']) {
						$tax_name = _('Included').' '.$tax_item['tax_type_name'].' ('.$tax_item['rate'].'%) '.': '.$tax;
						$rep->TextCol(2, 3, $tax_name, -2);
					}
					else {
						$tax_name = $tax_item['tax_type_name'] . " (" . $tax_item['rate'] . "%)";
						$rep->TextCol(2, 3, $tax_name, -2);
						$rep->TextCol(3, 4,	number_format2($tax, $dec), -2);
						$SubTotal += $tax;
					}
				}
				
				$rep->NewLine();
				$rep->TextCol(2, 3, _("Shipping"), -2);
				$rep->TextCol(3, 4,	$DisplayFreight, -2);
				$disc_amt = get_disc_from_invoice($trans['trans_no']);

				$SubTotal += ($myrow1['ov_freight'] - $disc_amt);

				if($trans['type'] == ST_SALESINVOICE) {
					$rep->NewLine();
					$rep->TextCol(2, 3, _('Discount Amount'), -2);
					$rep->TextCol(3, 4,	number_format2(-$disc_amt, $dec), -2);
				}
				
				$rep->NewLine();
				$rep->TextCol(2, 3, _('Total Amount'), -2);
				$rep->TextCol(3, 4,	number_format2($SubTotal, $dec), -2);
				$rep->aligns[2] = 'left';
				$rep->aligns[3] = 'left';
			} // End of invoice details

		}

		$rep->Line($rep->row - 8);
		$rep->NewLine(2);
		$rep->TextCol(0, 3, _('Total'));
		for ($i = 0; $i < 3; $i++)
			$rep->AmountCol($i + 4, $i + 5, $total[$i], $dec);
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
	}

	$rep->fontSize += 2;
	$rep->TextCol(0, 3, _('Grand Total'));
	$rep->fontSize -= 2;
	$grandtotal[2] = $grandtotal[0] - $grandtotal[1];
	for ($i = 0; $i < 3; $i++)
		$rep->AmountCol($i + 4, $i + 5, $grandtotal[$i], $dec);
	$rep->Line($rep->row  - 4);
	$rep->NewLine();
	$rep->End();
}
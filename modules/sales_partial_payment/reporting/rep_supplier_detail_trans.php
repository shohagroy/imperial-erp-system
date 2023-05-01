<?php

$page_security = 'SA_SUPPLIERANALYTIC';

include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/includes/date_functions.inc');
include_once($path_to_root.'/includes/data_checks.inc');
include_once($path_to_root.'/gl/includes/gl_db.inc');

//----------------------------------------------------------------------------------------------------

print_supplier_balances();

function get_open_balance($supplier_id, $to) {
	if ($to)
		$to = date2sql($to);

	$sql = "SELECT SUM(IF(t.type = ".ST_SUPPINVOICE." OR (t.type IN (".ST_JOURNAL." , ".ST_BANKDEPOSIT.") AND t.ov_amount>0),
		-abs(t.ov_amount + t.ov_gst + t.ov_discount), 0)) AS charges,";

	$sql .= "SUM(IF(t.type != ".ST_SUPPINVOICE." AND NOT(t.type IN (".ST_JOURNAL." , ".ST_BANKDEPOSIT.") AND t.ov_amount>0),
		abs(t.ov_amount + t.ov_gst + t.ov_discount) * -1, 0)) AS credits,";

	$sql .= "SUM(IF(t.type != ".ST_SUPPINVOICE." AND NOT(t.type IN (".ST_JOURNAL." , ".ST_BANKDEPOSIT.")), t.alloc * -1, t.alloc)) 
		AS Allocated,";

	$sql .= "SUM(IF(t.type = ".ST_SUPPINVOICE.", 1, -1) *
		(abs(t.ov_amount + t.ov_gst + t.ov_discount) - abs(t.alloc))) AS OutStanding
		FROM ".TB_PREF."supp_trans t
		WHERE t.supplier_id = ".db_escape($supplier_id);
	if ($to)
		$sql .= " AND t.tran_date < '$to'";
	$sql .= " GROUP BY supplier_id";

	$result = db_query($sql, 'No transactions were returned');
	return db_fetch($result);
}

function getTransactions($supplier_id, $from, $to) {
	$from = date2sql($from);
	$to = date2sql($to);

	$sql = "SELECT *,
				(ov_amount + ov_gst + ov_discount) AS TotalAmount,
				alloc AS Allocated,
				((type = ".ST_SUPPINVOICE.") AND due_date < '$to') AS OverDue
			FROM ".TB_PREF."supp_trans
			WHERE tran_date >= '$from' AND tran_date <= '$to' 
				AND supplier_id = '$supplier_id' AND ov_amount!=0
					ORDER BY tran_date";

	$TransResult = db_query($sql, 'No transactions were returned');

	return $TransResult;
}

//----------------------------------------------------------------------------------------------------

function print_supplier_balances() {
	global $path_to_root, $systypes_array;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$fromsupp = $_POST['PARAM_2'];
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
	if ($fromsupp == ALL_TEXT)
		$supp = _('All');
	else
		$supp = get_supplier_name($fromsupp);
	$dec = user_price_dec();

	if ($currency == ALL_TEXT) {
		$convert = true;
		$currency = _('Balances in Home currency');
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
				1 => array('text' => _('Period'), 'from' => $from, 'to' => $to),
				2 => array('text' => _('Supplier'), 'from' => $supp, 'to' => ''),
				3 => array(  'text' => _('Currency'),'from' => $currency, 'to' => ''),
				4 => array('text' => _('Suppress Zeros'), 'from' => $nozeros, 'to' => ''));

	$rep = new FrontReport(_('Supplier Detail Transaction'), "SupplierBalances", user_pagesize(), 9, $orientation);
	if ($orientation == 'L')
		recalculate_cols($cols);

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->NewPage();

	$total = array();
	$grandtotal = array(0,0,0);

	$sql = "SELECT supplier_id, supp_name AS name, curr_code FROM ".TB_PREF."suppliers";
	if ($fromsupp != ALL_TEXT)
		$sql .= " WHERE supplier_id=".db_escape($fromsupp);
	$sql .= " ORDER BY supp_name";
	$result = db_query($sql, "The customers could not be retrieved");

	while ($myrow=db_fetch($result)) {
		if (!$convert && $currency != $myrow['curr_code'])
			continue;
		$accumulate = 0;
		$rate = $convert ? get_exchange_rate_from_home_currency($myrow['curr_code'], Today()) : 1;
		$bal = get_open_balance($myrow['supplier_id'], $from);
		$init = array();
		$bal['charges'] = isset($bal['charges']) ? $bal['charges'] : 0;
		$bal['credits'] = isset($bal['credits']) ? $bal['credits'] : 0;
		$bal['Allocated'] = isset($bal['Allocated']) ? $bal['Allocated'] : 0;
		$bal['OutStanding'] = isset($bal['OutStanding']) ? $bal['OutStanding'] : 0;
		$init[0] = round2(abs($bal['charges']*$rate), $dec);
		$init[1] = round2(Abs($bal['credits']*$rate), $dec);
		$init[2] = $init[0] - $init[1];
		$accumulate += $init[2];

		$res = getTransactions($myrow['supplier_id'], $from, $to);
		if ($no_zeros && db_num_rows($res) == 0) continue;

		$rep->fontSize += 2;
		$rep->TextCol(0, 2, $myrow['name']);
		if ($convert) $rep->TextCol(2, 3,	$myrow['curr_code']);
		$rep->fontSize -= 2;
		$rep->TextCol(3, 4,	_("Open Balance"));
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
			if ($no_zeros && floatcmp(abs($trans['TotalAmount']), $trans['Allocated']) == 0)
				continue;
			$rep->NewLine(1, 2);
			$rep->DateCol(0, 1,	$trans['tran_date'], true);
			$rep->TextCol(1, 3,	$trans['reference'].' ('.$systypes_array[$trans['type']].')');
			$item[0] = $item[1] = 0.0;
			if ($trans['TotalAmount'] > 0.0) {
				$item[0] = round2(abs($trans['TotalAmount']) * $rate, $dec);
				$rep->AmountCol(4, 5, $item[0], $dec);
				$accumulate += $item[0];
				$item[2] = round2($trans['Allocated'] * $rate, $dec);
			}
			else {
				$item[1] = round2(abs($trans['TotalAmount']) * $rate, $dec);
				$rep->AmountCol(5, 6, $item[1], $dec);
				$accumulate -= $item[1];
				$item[2] = round2($trans['Allocated'] * $rate, $dec) * -1;
			}
			// $rep->AmountCol(6, 7, $item[2], $dec);
			if ($trans['TotalAmount'] > 0.0)
				$item[2] = $item[0] - $item[1];
			else	
				$item[2] = -$item[0] - $item[1];
			$rep->AmountCol(6, 7, $accumulate, $dec);

			for ($i = 0; $i < 3; $i++) {
				$total[$i] += $item[$i];
				$grandtotal[$i] += $item[$i];
			}

			$total[2] = $total[0] - $total[1];

			if($trans['type'] == ST_SUPPINVOICE || $trans['type'] == ST_SUPPCREDIT) { // Begining of invoice details

				$myrow1 = get_supp_trans($trans['trans_no'], $trans['type']);
				$details = get_supp_invoice_items($trans['type'], $trans['trans_no']);
				$InvTotal = $myrow1['Total'];

				$rep->NewLine();
				$dtail_oldrow = $rep->row + (3*$rep->lineHeight/4);
				$rep->Font('bold');
				$rep->TextCol(2, 3,	_('QTY').' / '._('Item'), -2);
				$rep->aligns[3] = 'right';
				$rep->TextCol(3, 4,	_('Total'), -2);
				$rep->Font();

				$SubTotal = 0;
				while ($myrow2 = db_fetch($details)) {
					if ($myrow2['quantity'] == 0 && $myrow2['grn_item_id'] != -1 && empty($myrow2['gl_code']))
						continue;

					if($myrow2['grn_item_id'] == -1 && !empty($myrow2['gl_code'])) {
						$myrow2['description'] = get_gl_account_name($myrow2['gl_code']);
						$Net = round2($myrow2['unit_price'], user_price_dec());
						$DisplayQty = '';
					}
					else {
						$Net = round2($myrow2['unit_price'] * $myrow2['quantity'], user_price_dec());
						$DisplayQty = number_format2($myrow2['quantity'], get_qty_dec($myrow2['stock_id'])).' x ';
					}
					$SubTotal += $Net;
					$DisplayPrice = number_format2($myrow2['unit_price'], $dec);
					$DisplayNet = number_format2($Net, $dec);

					$rep->NewLine();
					$oldrow = $rep->row;
					$rep->TextCol(2, 3, $DisplayQty.$myrow2['description'], -2);

					$newrow = $rep->row;
					$rep->row = $oldrow;
					if ($Net != 0.0 || !is_service($myrow2['mb_flag']) || !$SysPrefs->no_zero_lines_amount()) {
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
				$rep->TextCol(2, 3, _('Total Amount'), -2);
				$rep->TextCol(3, 4,	number_format2($SubTotal, $dec), -2);
				$rep->aligns[2] = 'left';
				$rep->aligns[3] = 'left';
			} // End of invoice details
		}
		$rep->Line($rep->row - 8);
		$rep->NewLine(2);
		$rep->TextCol(0, 3,	_('Total'));
		for ($i = 0; $i < 3; $i++) {
			$rep->AmountCol($i + 4, $i + 5, $total[$i], $dec);
			$total[$i] = 0.0;
		}
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
	}
	$rep->fontSize += 2;
	$rep->TextCol(0, 3,	_('Grand Total'));
	$rep->fontSize -= 2;
	if ($show_balance)
		$grandtotal[2] = $grandtotal[0] - $grandtotal[1];
	for ($i = 0; $i < 3; $i++)
		$rep->AmountCol($i + 4, $i + 5, $grandtotal[$i], $dec);
	$rep->Line($rep->row  - 4);
	$rep->NewLine();
	$rep->End();
}
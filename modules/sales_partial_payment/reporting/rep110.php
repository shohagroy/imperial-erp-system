<?php

$page_security = $_POST['PARAM_0'] == $_POST['PARAM_1'] ? 'SA_SALESTRANSVIEW' : 'SA_SALESBULKREP';

include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/includes/date_functions.inc');
include_once($path_to_root.'/includes/data_checks.inc');
include_once($path_to_root.'/sales/includes/sales_db.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/partial_payment_db.inc');

//----------------------------------------------------------------------------------------------------

function get_user_trans($trans_no, $type) {
	$sql = "SELECT u.user_id FROM ".TB_PREF."users u, ".TB_PREF."audit_trail a WHERE a.trans_no = ".db_escape($trans_no)." AND a.type = ".db_escape($type)." AND a.user = u.id";
	$result = db_query($sql, 'user retreive failed');

	return db_fetch($result);
}

print_deliveries();

//----------------------------------------------------------------------------------------------------

function print_deliveries() {
	global $path_to_root, $SysPrefs;

	include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$email = $_POST['PARAM_2'];
	$packing_slip = $_POST['PARAM_3'];
	$comments = $_POST['PARAM_4'];
	$orientation = $_POST['PARAM_5'];

	if (!$from || !$to) return;

	$orientation = ($orientation ? 'L' : 'P');
	$dec = user_price_dec();

	$fno = explode("-", $from);
	$tno = explode("-", $to);
	$from = min($fno[0], $tno[0]);
	$to = max($fno[0], $tno[0]);

	$cols = array(4, 30, 120, 260, 430, 480, 515);

	// $headers in doctext.inc
	$aligns = array('left', 'left', 'left', 'left', 'right', 'right');

	$params = array('comments' => $comments, 'packing_slip' => $packing_slip);

	$cur = get_company_Pref('curr_default');

	if ($email == 0) {
		if ($packing_slip == 0)
			$rep = new FrontReport(_('DELIVERY'), "DeliveryNoteBulk", user_pagesize(), 9, $orientation);
		else
			$rep = new FrontReport(_('PACKING SLIP'), "PackingSlipBulk", user_pagesize(), 9, $orientation);
	}
	if ($orientation == 'L')
		recalculate_cols($cols);
	for ($i = $from; $i <= $to; $i++) {
			if (!exists_customer_trans(ST_CUSTDELIVERY, $i))
				continue;
			$myrow = get_customer_trans($i, ST_CUSTDELIVERY);
			$result = get_customer_trans_details(ST_CUSTDELIVERY, $i);
			$branch = get_branch($myrow["branch_code"]);
			$sales_order = get_sales_order_header($myrow["order_"], ST_SALESORDER); // ?
			if ($email == 1) {
				$rep = new FrontReport("", "", user_pagesize(), 9, $orientation);
				if ($packing_slip == 0) {
					$rep->title = _('DELIVERY NOTE');
					$rep->filename = "Delivery" . $myrow['reference'].".pdf";
				}
				else {
					$rep->title = _('PACKING SLIP');
					$rep->filename = "Packing_slip" . $myrow['reference'].".pdf";
				}
			}
			$rep->currency = $cur;
			$rep->Font();
			$rep->Info($params, $cols, null, $aligns);

			$contacts = get_branch_contacts($branch['branch_code'], 'delivery', $branch['debtor_no'], true);
			$rep->SetCommonData($myrow, $branch, $sales_order, '', ST_CUSTDELIVERY, $contacts);
			$rep->SetHeaderType('header5');
			$user = get_user_trans($myrow['trans_no'], ST_CUSTDELIVERY);
			$rep->formData['user_id'] = isset($user['user_id']) ? $user['user_id'] : '';
			$rep->formData['contact_phone'] = $sales_order['contact_phone'];
			$top = $rep->pageHeight - $rep->topMargin;
			$rep->formData['line_bottom'] = ($top - 21*$rep->lineHeight) - (db_num_rows($result)*$rep->lineHeight) + 4;
			$rep->NewPage();

			$TotalQty = 0;
			$row_count = 1;
			while ($myrow2=db_fetch($result)) {
				if ($myrow2["quantity"] == 0)
					continue;

				$DisplayQty = number_format2($myrow2["quantity"], get_qty_dec($myrow2['stock_id']));

				$rep->TextCol(0, 1,	$row_count, -2);
				$rep->TextCol(1, 2,	$myrow2['stock_id'], -2);
				$oldrow = $rep->row;
				$rep->TextColLines(2, 4, $myrow2['StockDescription'], -2);
				$newrow = $rep->row;
				$rep->row = $oldrow;
				if (!is_service($myrow2['mb_flag']) || !$SysPrefs->no_zero_lines_amount()) {
					$rep->TextCol(4, 5,	$myrow2['units'], -2);
					$rep->TextCol(5, 6,	$DisplayQty, -2);
				}
				$rep->Line($rep->row - 2);
				$rep->row = $newrow;
				//$rep->NewLine(1);
				if ($rep->row < $rep->bottomMargin + (15 * $rep->lineHeight))
					$rep->NewPage();
				$row_count++;
				$TotalQty += $myrow2['quantity'];
			}

			$memo = get_comments_string(ST_CUSTDELIVERY, $i);
			if ($memo != '') {
				$rep->NewLine();
				$rep->TextColLines(1, 4, $memo, -2);
			}

			$rep->row = $rep->bottomMargin + (15 * $rep->lineHeight);
			$doctype=ST_CUSTDELIVERY;
			// if ($packing_slip == 0) {
				$rep->Font('bold');
				$rep->TextCol(4, 5, _("Total"), -2);
				$rep->TextCol(5, 6,	number_format2($TotalQty, user_qty_dec()), -2);
				$rep->NewLine();
				$rep->Font();
			// }
			if ($email == 1)
				$rep->End($email);
	}
	if ($email == 0)
		$rep->End();
}


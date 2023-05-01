<?php

$page_security = 'SA_SALESTRANSVIEW';

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . '/modules/sales_partial_payment/includes/db/branches_db.inc');

//----------------------------------------------------------------------------------------------------

function get_user_receipts($from, $to, $user, $customer=false) {
	$sql = "SELECT trans.*, au.user,
			(trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax) AS Total,
			trans.ov_discount, 
			debtor.name AS DebtorName,
			debtor.debtor_ref,
			debtor.curr_code,
			debtor.payment_terms,
			debtor.tax_id AS tax_id,
			debtor.address
			FROM ".TB_PREF."debtor_trans trans,"
				.TB_PREF."debtors_master debtor,"
				.TB_PREF."audit_trail au
			WHERE trans.debtor_no = debtor.debtor_no
			AND trans.type = ".ST_CUSTPAYMENT."
			AND au.type = ".ST_CUSTPAYMENT."
			AND au.trans_no = trans.trans_no
			AND trans.tran_date >= '".date2sql($from)."'
			AND trans.tran_date <= '".date2sql($to)."'
			AND au.user = ".db_escape($user);
	if($customer)
		$sql .= " AND trans.debtor_no = ".db_escape($customer);
	

	return db_query($sql, "The receipts cannot be retrieved");
}

function get_user_receipts__($from, $to, $user) {
	$sql = "SELECT tran.*, au.user FROM ".TB_PREF."audit_trail au, ".TB_PREF."debtor_trans tran WHERE au.type = ".ST_CUSTPAYMENT." AND tran.type = ".ST_CUSTPAYMENT." AND tran.trans_no = au.trans_no AND tran.tran_date >= '".date2sql($from)."' AND tran.tran_date <= '".date2sql($to)."' AND au.user = ".db_escape($user);
	return db_query($sql, 'could not get user receipts');
}

print_user_receipts();

//----------------------------------------------------------------------------------------------------

function print_user_receipts() {
	global $path_to_root, $SysPrefs;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$customer = $_POST['PARAM_2'];
	$user = $_POST['PARAM_3'];
	$comments = $_POST['PARAM_4'];
	$orientation = $_POST['PARAM_5'];
	$destination = $_POST['PARAM_6'];

	$cust_filter = $customer ? get_customer_name($customer) : _('All Customers');

	$dec = user_price_dec();

	$user_filter = $user == -1 ? _('All users') : get_user($user)['user_id'];

	if ($destination)
		include_once($path_to_root.'/reporting/includes/excel_report.inc');
	else
		include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$orientation = ($orientation ? 'L' : 'P');

	$cols = array(0, 75, 280, 330, 380, 445, 515);
	$cols2 = array(0, 75, 280, 350, 380, 445, 515);

	$headers = array(_('Payment #'), _('Customer'), _('Date'), _('Amount'), _('Discount'), _('Total'));
	$headers2 = array(_('User Name'), _('Real Name'), _('Phone'), _('Email'), '', '');

	$aligns = array('left',	'left',	'left', 'right', 'right', 'right');
	$aligns2 = array('left', 'left', 'left', 'left', 'left', 'left');

	$params =   array(0 => $comments,
					1 => array('text' => _('Period'), 'from' => $from, 'to' => $to),
					2 => array('text' => _('Customer'),	'from' => $cust_filter, 'to' => ''),		
					3 => array('text' => _('User'), 'from' => $user_filter, 'to' => '')
				);

	$rep = new FrontReport(_('User Cash Receipt'), "UserCashReceipt", user_pagesize(), 9, $orientation);
	if ($orientation == 'L')
		recalculate_cols($cols);
	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns, $cols2, $headers2, $aligns2);
	$rep->NewPage();

	$users = get_users();

	while($user_row = db_fetch($users)) {

		if($user != -1 && $user_row['id'] != $user)
			continue;

		$rep->Font('bold');
		$rep->TextCol2(0, 1, $user_row['user_id']);
		$rep->TextCol2(1, 2, $user_row['real_name']);
		$rep->TextCol2(2, 3, $user_row['phone']);
		$rep->TextCol2(3, 6, $user_row['email']);
		$rep->Font();

		$receipts = get_user_receipts($from, $to, $user_row['id'], $customer);

		$total_amt = 0;
		$total_disc = 0;
		$Total = 0;

		foreach($receipts as $receipt) {
			$customer_name = $receipt['DebtorName'];

			// if($receipt['Total'] == 0)
				// continue;

			$rep->NewLine();
			$rep->TextCol(0, 1, $receipt['reference']);
			$rep->TextCol(1, 2, $customer_name);
			$rep->TextCol(2, 3, sql2date($receipt['tran_date']));
			$rep->TextCol(3, 4, number_format2($receipt['Total'], $dec));
			$rep->TextCol(4, 5, number_format2($receipt['ov_discount'], $dec));
			$rep->TextCol(5, 6, number_format2($receipt['Total'] - $receipt['ov_discount'], $dec));

			$total_amt += $receipt['Total'];
			$total_disc += $receipt['ov_discount'];
			$Total += ($receipt['Total'] - $receipt['ov_discount']);
		}
		$rep->NewLine();
		$rep->Line($rep->row + 4);
		$rep->NewLine();
		$rep->TextCol(0, 1, _('Total'));
		$rep->TextCol(3, 4, number_format2($total_amt, $dec));
		$rep->TextCol(4, 5, number_format2($total_disc, $dec));
		$rep->TextCol(5, 6, number_format2($Total, $dec));
		$rep->Line($rep->row - 4);
		$rep->NewLine(2);
	}

	$rep->End();
}
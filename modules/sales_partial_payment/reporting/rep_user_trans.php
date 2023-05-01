<?php

$page_security = 'SA_SALESTRANSVIEW';

include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/db/branches_db.inc');

//----------------------------------------------------------------------------------------------------

function get_user_invoices($from, $to, $branch=0, $user=-1, $customer=false) {
	$sql = "SELECT tran.*, p.discount_amt, au.user, d.name FROM ".TB_PREF."audit_trail au, ".TB_PREF."debtor_trans tran, ".TB_PREF."partial_payment_trans p, ".TB_PREF."debtors_master d WHERE au.type = ".ST_SALESINVOICE." AND tran.type = ".ST_SALESINVOICE." AND p.type = ".ST_SALESINVOICE." AND tran.trans_no = au.trans_no AND tran.trans_no = p.trans_no AND tran.tran_date >= '".date2sql($from)."' AND tran.tran_date <= '".date2sql($to)."' AND au.user = ".db_escape($user)." AND d.debtor_no = tran.debtor_no";
	if($customer)
		$sql .= " AND tran.debtor_no = ".db_escape($customer);
	if($branch)
		$sql .= " AND p.branch_id = ".db_escape($branch);
	return db_query($sql, 'could not get user invoices');
}

print_user_sales();

//----------------------------------------------------------------------------------------------------

function print_user_sales() {
	global $path_to_root, $SysPrefs;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$branch = $_POST['PARAM_2'];
	$customer = $_POST['PARAM_3'];
	$user = $_POST['PARAM_4'];
	$comments = $_POST['PARAM_5'];
	$orientation = $_POST['PARAM_6'];
	$destination = $_POST['PARAM_7'];

	$cust_name = $customer ? get_customer_name($customer) : false;

	$dec = user_price_dec();

	$branch_filter = $branch == 0 ? _('All branches') : get_custom_branch_name($branch);
	$user_filter = $user == -1 ? _('All users') : get_user($user)['user_id'];

	if ($destination)
		include_once($path_to_root.'/reporting/includes/excel_report.inc');
	else
		include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$orientation = ($orientation ? 'L' : 'P');

	$cols = array(0, 70, 160, 250, 300, 360, 420, 460, 515);
	$cols2 = array(0, 75, 200, 270, 350, 400, 430, 480, 515);

	$headers1 = array(_('Invoice #'), _('Customer'), _('Branch'), _('Date'), _('Due Date'), _('Amount'), _('Discount'), _('Total'));
	$headers2 = array(_('User Name'), _('Real Name'), _('Phone'), _('Email'));

	$aligns = array('left',	'left',	'left',	'left', 'left', 'right', 'right', 'right');
	$aligns2 = array('left', 'left', 'left', 'left', 'left', 'left', 'left', 'left');

	$params = array(0 => $comments,
					1 => array('text' => _('Period'), 'from' => $from, 'to' => $to),
					2 => array('text' => _('Branch'), 'from' => $branch_filter, 'to' => ''),
					3 => array('text' => _('Customer'),	'from' => $cust_name, 'to' => ''),		
					4 => array('text' => _('User'), 'from' => $user_filter, 'to' => ''));

	$rep = new FrontReport(_('User Transactions'), "UserTransactions", user_pagesize(), 9, $orientation);
	if ($orientation == 'L')
		recalculate_cols($cols);
	$rep->Font();
	$rep->Info($params, $cols, $headers1, $aligns, $cols2, $headers2, $aligns2);
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

		$invoices = get_user_invoices($from, $to, $branch, $user_row['id'], $customer);

		$total_amt = 0;
		$total_disc = 0;
		$Total = 0;

		foreach($invoices as $invoice) {
			$br_name = get_branch_name($invoice['branch_code']);
			$inv_amt = $invoice["ov_freight"] + $invoice["ov_gst"] + $invoice["ov_amount"] + $invoice["ov_freight_tax"] + $invoice['discount_amt'];
			$inv_total = $inv_amt - $invoice['discount_amt'];
			$rep->NewLine();
			$rep->TextCol(0, 1, $invoice['reference']);
			$rep->TextCol(1, 2, $invoice['name']);
			$rep->TextCol(2, 3, $br_name);
			$rep->TextCol(3, 4, sql2date($invoice['tran_date']));
			$rep->TextCol(4, 5, sql2date($invoice['due_date']));
			$rep->TextCol(5, 6, number_format2($inv_amt, $dec));
			$rep->TextCol(6, 7, number_format2($invoice['discount_amt'], $dec));
			$rep->TextCol(7, 8, number_format2($inv_total, $dec));

			$total_amt += $inv_amt;
			$total_disc += $invoice['discount_amt'];
			$Total += $inv_total;
		}
		$rep->NewLine();
		$rep->Line($rep->row + 4);
		$rep->NewLine();
		$rep->TextCol(0, 1, _('Total'));
		$rep->TextCol(5, 6, number_format2($total_amt, $dec));
		$rep->TextCol(6, 7, number_format2($total_disc, $dec));
		$rep->TextCol(7, 8, number_format2($Total, $dec));
		$rep->Line($rep->row - 4);
		$rep->NewLine(2);

	}

	$rep->End();
}
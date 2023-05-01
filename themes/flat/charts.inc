<?php

function dashboard($sel_app) {
	$selected_app = $sel_app;

	if (is_object($sel_app) && !$_SESSION["wa_current_user"]->check_application_access($selected_app))
		return;
	$dir = company_path(). '/pdf_files';

	if ($d = @opendir($dir)) {
		while (($file = readdir($d)) !== false) {
			if (!is_file($dir.'/'.$file) || $file == 'index.php') continue;
			$ftime = filemtime($dir.'/'.$file);
			if (time()-$ftime > 180){
				unlink($dir.'/'.$file);
			}
		}
		closedir($d);
	}
	
	$dashboard_header = new dashboard_header;
	$dashboard_header -> get_header($selected_app);
	$dashboard_header -> show_header();
	
	if ($selected_app == "orders")
		display_sales_statistics();
	elseif ($selected_app == "AP")
		display_supplier_topten();
	elseif ($selected_app == "stock")
		display_stock_topten();
	elseif ($selected_app == "manuf")
		display_stock_topten(1);
	elseif ($selected_app == "assets")
		display_stock_topten(2);
	elseif ($selected_app == "proj")
		display_dimension_topten();
	elseif ($selected_app == "GL")
		display_gl_info();
	elseif ($selected_app == "FrontHrm")
		display_hrm_info();
	elseif ($selected_app == "extendedhrm")
		display_extendedhrm_info();
	elseif ($selected_app == "pos")
		display_pos_statistics();
	elseif ($selected_app == "trade_finance")
		display_trade_finance_statistics();
	elseif ($selected_app == 'grm')
		display_grm_statistics();
	elseif($selected_app == 'weigh_bridge')
		display_scale_statistics();
	elseif($selected_app == 'booking')
		display_booking_statistics();
	elseif($selected_app == 'hospital')
		display_hospital_statistics();
	else	
		display_all();
}

class dashboard_header {

	var $inner_desc1;
	var $inner_desc2;
	var $inner_desc3;
	var $inner_desc4;
	
	var $count_item1;
	var $count_item2;
	var $count_item3;
	var $count_item4;

	function __construct() {
		$this->inner_desc1 = $this->inner_desc2 = $this->inner_desc3 = $this->inner_desc4 = _('undefined');
		$this->count_item1 = $this->count_item2 = $this->count_item3 = $this->count_item4 = '0';
	}
	
	function get_header($selapp=null) {
		
		if ($selapp == 'orders')
			$this->generate_header(_('Customers'), _('Branches'), _('Sales People'), _('Sales Areas'), $selapp);
		elseif ($selapp == 'AP')
			$this->generate_header(_('Suppliers'), _('Purchasable items'), _('Today Orders'), _('Today Invoices'), $selapp);
		elseif ($selapp == 'stock')
			$this->generate_header(_('Items'), _('Zero Items'), _('Systems'), _('to be Reordered'), $selapp);
		elseif ($selapp == 'manuf')
			$this->generate_header(_('Manufactured Items'), _('Outstanding WOs'), _('Work Orders'), _('Overdue WOs'), $selapp);
		elseif ($selapp == 'assets')
			$this->generate_header(_('Fixed Assets'), _('Categories'), _('Classes'), _('Locations'), $selapp);
		elseif ($selapp == 'proj')
			$this->generate_header(_('Dimensions'), _('Outstanding'), _('Currents'), _('Overdue'), $selapp);
		elseif ($selapp == 'GL')
			$this->generate_header(_('Receivables'), _('Payables'), _('Today Deposits'), _('Today Payments'), $selapp);
		elseif ($selapp == 'school')
			$this->generate_header(_('Students'), _('Teachers'), _('Month Receivables'), _('Late Payments'), $selapp);
		elseif ($selapp == 'FrontHrm')
			$this->generate_header(_("Employees"), _('Doc Expiry'), _('Departments'), _('Unpaid Salaries'), $selapp);
		elseif ($selapp == 'system')
			$this->generate_header(_('Users'), _('Access Roles'), _('History'), _('Mb data'), $selapp);
		elseif ($selapp == 'kanban')
			$this->generate_header(_('Projects'), _('Overdue Projects'), _('Tasks'), _('Overdue Tasks'), $selapp);
		elseif($selapp == 'pos')
			$this->generate_header(_('Customers'), _('Branches'), _('Sales People'), _('Sales Areas'), $selapp);
		elseif($selapp == 'extendedhrm')
			$this->generate_header(_('Employees'), _('Leaves to be Approved'), _('Departments'), '', $selapp);
		elseif($selapp == 'trade_finance')
			$this->generate_header(_('Openning Docs'), _('Expiry Today'), _('Closed'), _('Overdue'), $selapp);
		elseif($selapp == 'grm')
			$this->generate_header(_('Upcoming Event'), _('Today Event'), _('Queued SMS'), _('Overdue Event'), $selapp);
		elseif($selapp == 'weigh_bridge')
			$this->generate_header(_('Delivery Notes'), _('Outstanding Notes'), _('Today Deliveries'), _('Today Processed'), $selapp);
		elseif($selapp == 'booking')
			$this->generate_header(_('New Reservations'), _('In Progress'), _('Today Invoiced'), _('Overdue'), $selapp);
		elseif($selapp == 'hospital')
			$this->generate_header(_('Patients'), _('Categories'), _('Today Invoiced'), _('New Patients'), $selapp);
		else
			$this->generate_header();
	}
	
	function generate_header($inner1='Undefined', $inner2='Undefined', $inner3='Undefined', $inner4='Undefined', $selapp=null) {
		global $db_connections;

		$this->inner_desc1 = $inner1 == '' ? $this->inner_desc1 : $inner1;
		$this->inner_desc2 = $inner2 == '' ? $this->inner_desc2 : $inner2;
		$this->inner_desc3 = $inner3 == '' ? $this->inner_desc3 : $inner3;
		$this->inner_desc4 = $inner4 == '' ? $this->inner_desc4 : $inner4;

		$dec = user_price_dec();
		
		if($selapp == 'orders') {
			$debtors_no = db_fetch(db_query("SELECT COUNT(debtor_no) FROM ".TB_PREF."debtors_master WHERE !inactive"))[0];
			$branches_no = db_fetch(db_query("SELECT COUNT(branch_code) FROM ".TB_PREF."cust_branch WHERE !inactive"))[0];
			$sales_people_no = db_fetch(db_query("SELECT COUNT(salesman_code) FROM ".TB_PREF."salesman WHERE !inactive"))[0];
			$areas_no = db_fetch(db_query("SELECT COUNT(area_code) FROM ".TB_PREF."areas WHERE !inactive"))[0];

			$this->count_item1 = $debtors_no;
			$this->count_item2 = $branches_no;
			$this->count_item3 = $sales_people_no;
			$this->count_item4 = $areas_no;
		}
		elseif ($selapp == 'AP') {
			$sup_no = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."suppliers WHERE !inactive"))[0];
			$purchasable_items = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE !no_purchase"))[0];
			$today_orders = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."purch_orders WHERE ord_date = '".date2sql(Today())."'"))[0];
			$today_invoices = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."supp_trans WHERE tran_date = '".date2sql(Today())."' AND type = ".ST_SUPPINVOICE))[0];

			$this->count_item1 = $sup_no;
			$this->count_item2 = $purchasable_items;
			$this->count_item3 = $today_orders;
			$this->count_item4 = $today_invoices;
		}
		elseif ($selapp == 'stock') {
			$items_no = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE !inactive"))[0];
			$zero_items = 0;
			foreach(db_query("SELECT stock_id FROM ".TB_PREF."stock_master WHERE !inactive", 'could not get stock') as $row) {
				if(get_qoh_on_date($row['stock_id']) == 0)
					$zero_items ++;
			}
			$systems = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE !inactive AND category_id=3"))[0];
			$tobe_reorder = 0;
			foreach(db_query("SELECT stock_id FROM ".TB_PREF."stock_master WHERE !inactive", 'could not get stock') as $row) {
				$level = db_fetch(get_loc_details($row['stock_id']))['reorder_level'];
				if(get_qoh_on_date($row['stock_id']) <= $level && $level > 0)
					$tobe_reorder ++;
			}

			$this->count_item1 = $items_no;
			$this->count_item2 = $zero_items;
			$this->count_item3 = $systems;
			$this->count_item4 = $tobe_reorder;
		}
		elseif ($selapp == 'manuf') {
			$manufactured = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE !inactive AND mb_flag='M'"))[0];
			$outstanding = db_num_rows(db_query(get_sql_for_work_orders(1, ALL_TEXT), _('could not get work orders data')));
			$wo_no = db_num_rows(db_query(get_sql_for_work_orders(0, ALL_TEXT), _('could not get work orders data')));
			$overdues = db_num_rows(db_query(get_sql_for_work_orders(1, ALL_TEXT, ALL_TEXT, '', '', true), 'could not get work orders data'));

			$this->count_item1 = $manufactured;
			$this->count_item2 = $outstanding;
			$this->count_item3 = $wo_no;
			$this->count_item4 = $overdues;
		}
		elseif ($selapp == 'assets') {
			$assets = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE !inactive AND fa_class_id != ''"))[0];
			$categories = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_category WHERE !inactive AND dflt_mb_flag = 'F'"))[0];
			$classes = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."stock_fa_class WHERE !inactive"))[0];
			$locations = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."locations WHERE !inactive AND fixed_asset=1"))[0];

			$this->count_item1 = $assets;
			$this->count_item2 = $categories;
			$this->count_item3 = $classes;
			$this->count_item4 = $locations;
		}
		elseif ($selapp == "proj") {
			$dims = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."dimensions"))[0];
			$outstandings = db_num_rows(db_query("SELECT id FROM ".TB_PREF."dimensions WHERE closed = 0"));
			$overdue_dims = db_num_rows(db_query("SELECT id FROM ".TB_PREF."dimensions WHERE closed = 0 AND due_date < '".date2sql(Today())."'"));

			$this->count_item1 = $dims;
			$this->count_item2 = $outstandings;
			$this->count_item4 = $overdue_dims;
		}
		elseif ($selapp == "GL") {
			$receivable_act = get_company_pref('debtors_act');
			$payable_act = get_company_pref('creditors_act');
			$receivables = get_balance($receivable_act, 0, 0, begin_fiscalyear(), Today(), true, true)['balance'];
			$payables = get_balance($payable_act, 0, 0, begin_fiscalyear(), Today(), true, true)['balance'];

			$receivable = empty($receivables) ? 0 : round2($receivables, $dec);
			$payable = empty($payables) ? 0 : round2($payables, $dec);

			$today_deposits = db_fetch(db_query("SELECT SUM(amount) FROM ".TB_PREF."bank_trans WHERE trans_date = '".date2sql(Today())."' AND (type = ".ST_BANKDEPOSIT." OR type = ".ST_CUSTPAYMENT.")"))[0];
			$today_payments = db_fetch(db_query("SELECT SUM(amount) FROM ".TB_PREF."bank_trans WHERE trans_date = '".date2sql(Today())."' AND (type = ".ST_BANKPAYMENT." OR type = ".ST_SUPPAYMENT.")"))[0];

			$this->count_item1 = number_format2($receivable, $dec);
			$this->count_item2 = number_format2($payable, $dec);
			$this->count_item3 = $today_deposits ? number_format2($today_deposits, $dec) : 0;
			$this->count_item4 = $today_payments ? number_format2($today_payments, $dec) : 0;
		}
		elseif ($selapp == 'school'){
			$students = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."students"))[0];
			$teachers = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."teachers"))[0];

			$this->count_item1 = $students;
			$this->count_item2 = $teachers;
		}
		elseif ($selapp == 'FrontHrm') {
			$employees = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."employee"))[0];
			$doc = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."employee_docs d, ".TB_PREF."document_types t WHERE d.type_id = t.type_id AND DATE_ADD(expiry_date, INTERVAL -notify_before DAY) <= '".date2sql(Today())."'"))[0];
			$depts = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."department"))[0];
			$unpay_salaries = db_fetch(db_query("SELECT COUNT(p.payslip_no) FROM ".TB_PREF."payslip p LEFT JOIN ".TB_PREF."employee_trans t ON p.payslip_no = t.payslip_no WHERE t.payslip_no IS NULL"))[0];

			$this->count_item1 = $employees;
			$this->count_item2 = $doc;
			$this->count_item3 = $depts;
			$this->count_item4 = $unpay_salaries;
		}
		elseif ($selapp == 'extendedhrm') {
			$employees = db_fetch(db_query("SELECT COUNT(id) FROM ".TB_PREF."kv_empl_info"))[0];
			$depts = db_fetch(db_query("SELECT COUNT(id) FROM ".TB_PREF."kv_empl_departments WHERE !inactive"))[0];
			$leaves = db_fetch(db_query("SELECT COUNT(id) FROM ".TB_PREF."kv_empl_leave_applied WHERE status = 0"))[0];

			$this->count_item1 = $employees;
			$this->count_item2 = $leaves;
			$this->count_item3 = $depts;
		}
		elseif ($selapp == 'system') {
			$users_no = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."users WHERE !inactive"))[0];
			$roles = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."security_roles WHERE !inactive"))[0];
			$fiscal_years = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."fiscal_year"))[0];
			$data_used = db_fetch(db_query("SELECT sum(round(((data_length + index_length) / 1024 / 1024), 2)) FROM information_schema.TABLES WHERE table_schema = '".$db_connections[user_company()]['dbname']."'"))[0];

			$this->count_item1 = $users_no;
			$this->count_item2 = $roles;
			$this->count_item3 = $fiscal_years;
			$this->count_item4 = $data_used;
		}
		elseif ($selapp == 'kanban') {
			$projects_no = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."projects WHERE !closed"))[0];
			$overdue_projects = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."projects WHERE !closed AND end_date < '".date2sql(Today())."' AND end_date != '0000-00-00'"))[0];
			$tasks_no = 0;
			foreach(db_query("SELECT proj_id FROM ".TB_PREF."projects WHERE !closed", 'Could not get projects data') as $proj) {
				$data_file = company_path().'/kanban_data/'.$proj['proj_id'];
				$fh = fopen($data_file, 'r');
				$data = fread($fh, filesize($data_file));
				$data = json_decode($data);
				$tasks_no += count(get_object_vars($data));
			}

			$this->count_item1 = $projects_no;
			$this->count_item2 = $overdue_projects;
			$this->count_item3 = number_format2($tasks_no);
			$this->count_item4 = 0;
		}
		elseif($selapp == 'pos') {
			$this->count_item1 = db_fetch(db_query("SELECT COUNT(debtor_no) FROM ".TB_PREF."debtors_master WHERE !inactive"))[0];
			$this->count_item2 = db_fetch(db_query("SELECT COUNT(branch_code) FROM ".TB_PREF."cust_branch WHERE !inactive"))[0];
			$this->count_item3 = db_fetch(db_query("SELECT COUNT(salesman_code) FROM ".TB_PREF."salesman WHERE !inactive"))[0];
			$this->count_item4 = db_fetch(db_query("SELECT COUNT(area_code) FROM ".TB_PREF."areas WHERE !inactive"))[0];
		}
		elseif($selapp == 'trade_finance') {

			$today = date2sql(Today());

			$outstanding = db_fetch(db_query("SELECT COUNT(*) FROM (SELECT id FROM ".TB_PREF."out_tfd_pdc WHERE closed = 0 UNION ALL SELECT id FROM ".TB_PREF."in_tfd_pdc WHERE closed = 0 UNION ALL SELECT id FROM ".TB_PREF."forex WHERE closed = 0) x"))[0];
			$expiry = db_fetch(db_query("SELECT COUNT(*) FROM (SELECT id FROM ".TB_PREF."out_tfd_pdc WHERE closed = 0 AND expiry_date = '$today' UNION ALL SELECT id FROM ".TB_PREF."in_tfd_pdc WHERE closed = 0 AND expiry_date = '$today') x"))[0];
			$closed = db_fetch(db_query("SELECT COUNT(*) FROM (SELECT id FROM ".TB_PREF."out_tfd_pdc WHERE closed = 1 UNION ALL SELECT id FROM ".TB_PREF."in_tfd_pdc WHERE closed = 1 UNION ALL SELECT id FROM ".TB_PREF."forex WHERE closed = 1) x"))[0];
			$overdue = db_fetch(db_query("SELECT COUNT(*) FROM (SELECT id FROM ".TB_PREF."out_tfd_pdc WHERE closed = 0 AND expiry_date < '$today' UNION ALL SELECT id FROM ".TB_PREF."in_tfd_pdc WHERE closed = 0 AND expiry_date < '$today') x"))[0];

			$this->count_item1 = $outstanding;
			$this->count_item2 = $expiry;
			$this->count_item3 = $closed;
			$this->count_item4 = $overdue;
		}
		elseif($selapp == 'grm') {
			$today = date2sql(Today());
			$incoming_events = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."events WHERE start_date > '".$today."' AND !inactive"))[0];
			$today_events = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."events WHERE start_date = '".$today."' AND !inactive"))[0];
			$sms = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."sms_record WHERE !sent"))[0];
			$overdue_events = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."events WHERE start_date < '".$today."' AND !inactive"))[0];

			$this->count_item1 = $incoming_events;
			$this->count_item2 = $today_events;
			$this->count_item3 = $sms;
			$this->count_item4 = $overdue_events;
		}
		elseif($selapp == 'weigh_bridge') {
			$today = date2sql(Today());
			$deliveries = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."scale_transaction"))[0];
			$outstandings = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."scale_transaction WHERE !inactive"))[0];
			$today_notes = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."scale_transaction WHERE date = '$today'"))[0];
			$today_processed = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."scale_transaction WHERE date = '$today' AND inactive"))[0];

			$this->count_item1 = $deliveries;
			$this->count_item2 = $outstandings;
			$this->count_item3 = $today_notes;
			$this->count_item4 = $today_processed;
		}
		elseif($selapp == 'booking') {
			$today = date2sql(Today());
			$new_orders = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."booking_orders WHERE booking_id NOT IN (SELECT booking_id FROM ".TB_PREF."booking_trans)"))[0];
			$inprocess = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."booking_orders WHERE booking_id IN (SELECT booking_id FROM ".TB_PREF."booking_trans WHERE trans_type = ".ST_CUSTDELIVERY.") AND booking_id NOT IN (SELECT booking_id FROM ".TB_PREF."booking_trans WHERE trans_type = ".ST_SALESINVOICE.")"))[0];
			$today_invoiced = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."booking_orders WHERE booking_id IN (SELECT booking_id FROM ".TB_PREF."booking_trans WHERE trans_type = ".ST_SALESINVOICE.") AND DATE(start_time) = '".$today."'"))[0];
			$overdue = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."booking_orders WHERE DATE(start_time) < '$today' AND booking_id NOT IN (SELECT booking_id FROM ".TB_PREF."booking_trans)"))[0];

			$this->count_item1 = $new_orders;
			$this->count_item2 = $inprocess;
			$this->count_item3 = $today_invoiced;
			$this->count_item4 = $overdue;
		}
		elseif($selapp == 'hospital') {
			$today = date2sql(Today());
			$patients = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."hospital_patient_info WHERE !inactive"))[0];
			$patient_categories = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."patient_category"))[0];
			$today_invoiced = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."debtor_trans t, ".TB_PREF."hospital_patient_info p WHERE t.debtor_no = p.debtor_no AND t.type = ".ST_SALESINVOICE." AND t.tran_date = '".$today."'"))[0];
			$new_patient = db_fetch(db_query("SELECT COUNT(*) FROM ".TB_PREF."hospital_patient_info WHERE debtor_no NOT IN (SELECT debtor_no FROM ".TB_PREF."debtor_trans)"))[0];

			$this->count_item1 = $patients;
			$this->count_item2 = $patient_categories;
			$this->count_item3 = $today_invoiced;
			$this->count_item4 = $new_patient;
		}
	}
	
	function show_header() {
	
		echo "<div class='header_box'>";
		echo "<div class='header_item item_0'>
			<span>".$this->count_item1."</span><p>".$this->inner_desc1."<p>
			</div>";
		echo "<div class='header_item item_1'>
			<span>".$this->count_item2."</span><p>".$this->inner_desc2."<p>
			</div>";
		echo "<div class='header_item item_2'>
			<span>".$this->count_item3."</span><p>".$this->inner_desc3."<p>
			</div>";
		echo "<div class='header_item item_3'>
			<span>".$this->count_item4."</span><p>".$this->inner_desc4."<p>
			</div>";
		echo '</div>';
	}
}

function display_title($title, $add_class = '', $panels=array()) {

	echo "<div class='widget_box bootstrap-iso ".$add_class."'>";
	echo "<div class='widget_title'>";
	echo "<span>".$title."</span>";

	if(count($panels) > 0) {
		echo "<ul class='nav nav-tabs'>";
		foreach($panels as $key=>$val) {
			if($val == 'D')
				echo "<li><a data-toggle='tab' href='#".$add_class."_panel$key' title='Days'>D</a></li>";
			elseif($val == 'M')
				echo "<li><a data-toggle='tab' href='#".$add_class."_panel$key' title='Months'>M</a></li>";
			elseif($val == 'Y')
				echo "<li><a data-toggle='tab' href='#".$add_class."_panel$key' title='Years'>Y</a></li>";
			else
				echo "<li><a data-toggle='tab' href='#".$add_class."_panel$key'><i class='fa fa-$val'></i></a></li>";
		}
		echo '</ul>';
	}
	echo '</div>';
}

function display_sales_statistics() {
	$today = Today();
	customer_top($today, 10, 100, array('bar-chart','table','pie-chart'));
	sales_areas($today, 100, array('bar-chart','table','pie-chart'));
	customer_trans($today);
	customer_recurrent_invoices($today);
}
function display_supplier_topten() {
	$today = Today();
	supplier_top($today, 10, 100);
	supplier_top($today, 10, 100, array('bar-chart','pie-chart'));
	supplier_trans($today);
}
function display_stock_topten($type=0) {
	$today = Today();
	stock_top($today, 10, 100, $type);
	stock_top($today, 10, 100, $type, array('bar-chart'));
	stock_turnover_trend($today, 100, array('M', 'Y'));
	stock_values($today, 100, array('bar-chart','table','pie-chart'));
}
function display_dimension_topten() {
	$today = Today();
	dimension_top($today, 10, 100);
	dimension_top($today, 10, 100, array('bar-chart', 'pie-chart'));
}	
function display_gl_info() {
	$today = Today();
	cash_flow($today, 100, array('D', 'M', 'Y'));
	gl_performance($today, 100, 10);
	gl_top($today, 100, array('pie-chart', 'bar-chart', 'table'));
	receivables_by_age($today, 100, array('pie-chart', 'bar-chart', 'table'));
	debtor_top($today, 5, 100, array('bar-chart', 'pie-chart', 'table'));
	bank_balance($today, 100);
}
function display_hrm_info() {
	$today = Today();
	department_expenses($today, 100, array('bar-chart','table','pie-chart'));
	department_employees($today, 100, array('pie-chart','table','bar-chart'));
	employee_trend($today, 100, array('line-chart', 'bar-chart'));
	age_employees($today, 100, array('pie-chart','table','bar-chart'));
}
function display_extendedhrm_info() {
	emp_by_dept(Today(), 100, array('pie-chart','table','bar-chart'));
	expenses_by_dept(Today(), 100, array('bar-chart','table','pie-chart'));
}
function display_all() {
	$today = Today();

	gl_top($today, 100);
	gl_top($today, 100, array('pie-chart'));
	stock_top($today, 3, 100, 0, array('table', 'bar-chart'));
	customer_top($today, 3, 100, array('table', 'bar-chart', 'pie-chart'));
	supplier_top($today, 3, 100, array('table', 'bar-chart', 'pie-chart'));
	dimension_top($today, 3, 100, array('table', 'bar-chart', 'pie-chart'));
	stock_top($today, 3, 100, 2);
	stock_top($today, 3, 100, 1, array('table', 'bar-chart', 'pie-chart'));
}
function display_pos_statistics() {
	$today = Today();
	pos_sales_trend($today, 100, array('D', 'M', 'Y'));
	Shift_sales_top($today, 10, 100, array('table', 'bar-chart', 'pie-chart'));
	pos_stock_top($today, 10, 100, 0, array('table', 'bar-chart'));
	sales_areas($today, 100, array('bar-chart','table','pie-chart'));
	salesman_top($today, 10, 100, array('bar-chart','table','pie-chart'));
	customer_top($today, 10, 100, array('bar-chart','table','pie-chart'));
}
function display_trade_finance_statistics() {
	$today = Today();
	today_exp_docs($today);
	upcoming_exp_docs($today);
	expired_docs($today);
}
function display_grm_statistics() {
	$today = Today();
	today_events($today);
	upcoming_events($today);
	overdue_events($today);
	queued_sms($today);
}
function display_scale_statistics() {
	$today = Today();
	customer_top_deliveries($today, 10, 100, array('bar-chart','table','pie-chart'));
	stock_top_deliveries($today, 10, 100, array('bar-chart','table','pie-chart'));
	customer_top_weight($today, 10, 100, array('bar-chart','table','pie-chart'));
	delivery_trend($today, 100, array('M', 'Y'));
}
function display_booking_statistics() {
	$today = Today();
	booking_trend($today, 100, array('line-chart', 'bar-chart'));
	booking_customer_top($today, 10, 100, array('bar-chart','table','pie-chart'));
	revenue_trend($today, 100, array('D', 'M', 'Y'));
	booking_stock_top($today, 10, 100, 0, array('table', 'bar-chart'));
}
function display_hospital_statistics() {
	$today = Today();
	top_common_diseases($today, 10, 100, array('bar-chart','table','pie-chart'));
	patient_trend($today, 100, array('D', 'M', 'Y'));
	age_patients($today, 100, array('pie-chart','table','bar-chart'));
	hospital_gl_performance($today, 100, 10);
}

function sales_areas($today, $width='33', $panels=array()) {

	$sql = "SELECT SUM((ov_amount + ov_discount) * rate * IF(t.type = ".ST_CUSTCREDIT.", -1, 1)) total, a.description name FROM ".TB_PREF."areas a, ".TB_PREF."cust_branch b, ".TB_PREF."debtor_trans t WHERE (t.type = ".ST_SALESINVOICE." OR t.type = ".ST_CUSTCREDIT.") AND t.branch_code = b.branch_code AND b.area = a.area_code GROUP BY name";

	$result = db_query($sql, 'Transactions could not be calculated');

	$title = _('Sales by Regions');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'areas_top', $icons);
	else
		display_title($title, 'areas_top');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='areas_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Area'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {
		$name = $myrow['name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='areas_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'areas_top');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='areas_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'areas_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function customer_top($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM((ov_amount + ov_discount) * rate * IF(trans.type = ".ST_CUSTCREDIT.", -1, 1)) AS total, d.debtor_no, d.debtor_ref, d.name FROM ".TB_PREF."debtor_trans AS trans, ".TB_PREF."debtors_master AS d WHERE trans.debtor_no = d.debtor_no AND (trans.type = ".ST_SALESINVOICE." OR trans.type = ".ST_CUSTCREDIT.") AND tran_date >= '$begin' AND tran_date <= '$today1' GROUP by d.debtor_no ORDER BY total DESC, d.debtor_no LIMIT $limit";

	$result = db_query($sql, 'could not get customer data');

	$title = sprintf(_("Top %s customers this year"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'cus_top', $icons);
	else
		display_title($title, 'cus_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='cus_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Customer'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {
		$name = $myrow['debtor_no'].' '.$myrow['name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}
		
		$pg1['x'][] = $myrow['debtor_ref'];
		$pg1['y'][] = array(
			'meta'=>$myrow['debtor_ref'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec));

		$pg2['x'][] = $myrow['debtor_ref'];
		$pg2['y'][] = array('meta'=>$myrow['debtor_ref'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='cus_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'cus_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='cus_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'cus_top_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function debtor_top($today, $limit=10, $width="33", $panels=array()) {

	$sql = "SELECT debtor_no, name, debtor_ref, curr_code FROM ".TB_PREF."debtors_master WHERE !inactive";
	$result = db_query($sql, 'could not get customers data');

	$deb_arr = array();
	
	while ($myrow = db_fetch($result)) {
		$debtor = get_customer_details($myrow['debtor_no']);
		$home_curr = get_company_pref('curr_default');
		if($home_curr != $myrow['curr_code'])
			$rate = get_exchange_rate_from_home_currency($myrow['curr_code'], Today());
		else
			$rate = 1.0;
		$deb_arr[] = array('debtor_no' => $myrow['debtor_no'], 'balance' => ($debtor['Balance'] * $rate));
	}

	usort($deb_arr, function($a, $b){return $b['balance'] - $a['balance'];});

	$title = sprintf(_("Top %s Debtors"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'deb_top', $icons);
	else
		display_title($title, 'deb_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='deb_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Customer"), _("Amount"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');

	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$k = 0;
	$sum = 0;
	$dec = user_price_dec();

	if($limit > count($deb_arr))
		$limit = count($deb_arr);

	foreach ($deb_arr as $v) {
		$sum += $v['balance'];
	}

	for ($i = 0; $i < $limit; $i++) {
		$name = get_customer_name($deb_arr[$i]['debtor_no']);
		$ref = get_customer($deb_arr[$i]['debtor_no'])['debtor_ref'];
		$bal = round2($deb_arr[$i]['balance'], $dec);

		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($bal);
		}
		
		$pg1['x'][] = $ref;
		$pg1['y'][] = array(
			'meta'=>$ref.' - '.round((abs($bal)/$sum)*100).'%',
			'value'=>abs($bal)
		);
		$pg2['x'][] = $ref;
		$pg2['y'][] = array('meta'=>$ref, 'value'=>abs($bal));

		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='deb_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'deb_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='deb_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'deb_top_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function supplier_top($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM((trans.ov_amount + trans.ov_discount) * rate) AS total, s.supplier_id, s.supp_ref, s.supp_name FROM ".TB_PREF."supp_trans AS trans, ".TB_PREF."suppliers AS s WHERE trans.supplier_id=s.supplier_id AND (trans.type = ".ST_SUPPINVOICE." OR trans.type = ".ST_SUPPCREDIT.") AND tran_date >= '$begin' AND tran_date <= '$today1' GROUP by s.supplier_id ORDER BY total DESC, s.supplier_id LIMIT $limit";

	$result = db_query($sql, 'could not get supplier data');

	$title = sprintf(_("Top %s suppliers this year"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, "sup_top", $icons);
	else
		display_title($title, "sup_top");
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='sup_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Supplier"), _("Amount"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_SUPPTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow["supplier_id"]." ".$myrow["supp_name"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg1['x'][] = $myrow['supp_name']; 
		$pg1['y'][] = array(
			'meta'=>$myrow['supp_name'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$pg2['x'][] = $myrow['supp_ref']; 
		$pg2['y'][] = array('meta'=>$myrow['supp_ref'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='sup_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'sup_top');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='sup_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'sup_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	$pg = array('x'=>array(), 'y'=>array());
}

function stock_top($today, $limit=10, $width='33', $type=0, $panels=array()) {
	if ($type == 2)
		$sec = 'SA_ASSETSANALYTIC';
	elseif ($type == 1)
		$sec = 'SA_WORKORDERANALYTIC';
	else
		$sec = 'SA_ITEMSTRANSVIEW';

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	if ($type == 0) {
		$sql = "SELECT SUM((trans.unit_price * trans.quantity) * d.rate) AS total, s.stock_id, s.description, SUM(trans.quantity) AS qty, SUM((s.material_cost + s.overhead_cost + s.labour_cost) * trans.quantity) AS costs FROM ".TB_PREF."debtor_trans_details AS trans, ".TB_PREF."stock_master AS s, ".TB_PREF."debtor_trans AS d WHERE trans.stock_id=s.stock_id AND trans.debtor_trans_type=d.type AND trans.debtor_trans_no=d.trans_no AND (d.type = ".ST_SALESINVOICE." OR d.type = ".ST_CUSTCREDIT.") ";
	}
	else {
		$sql = "SELECT SUM(m.qty * (s.material_cost + s.labour_cost + s.overhead_cost)) AS total, s.stock_id, s.description, SUM(qty) AS qty FROM ".TB_PREF."stock_master AS s, ".TB_PREF."stock_moves AS m WHERE s.stock_id=m.stock_id ";

		if ($type == 1)
			$sql .= "AND s.mb_flag='M' AND m.qty > 0 ";
		elseif ($type == 2)	
			$sql .= "AND s.mb_flag='F' ";
	}
	if ($type != 2)
		$sql .= "AND tran_date >= '$begin' ";
	$sql .= "AND tran_date <= '$today1' GROUP by s.stock_id ORDER BY total DESC, s.stock_id LIMIT $limit";
	$result = db_query($sql, 'could not get stock data');
	if ($type == 1)
		$title = sprintf(_("Top %s Manufactured Items in fiscal year"), $limit);
	elseif ($type == 2)
		$title = sprintf(_("Top %s Fixed Assets"), $limit);
	else	
		$title = sprintf(_("Top %s Sold Items in fiscal year"), $limit);

	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, "stock_top_$type", $icons);
	else
		display_title($title, "stock_top_$type");

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='stock_top_".$type."_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		if ($type == 0)	
			$th = array(_('Item'), _('Sales'), _('Costs'), _('Quantity'));
		else	
			$th = array(_('Item'), _('Amount'), _('Quantity'));
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security($sec);
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['description'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
			if ($type == 0)
				amount_cell($myrow['costs']);
			qty_cell($myrow['qty']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec));
		if ($type == 0)
			$pg['z'][] = $myrow['costs'];
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade in active' id='stock_top_".$type."_panel$key'>";
			}
			if($type == 0)
				double_bar($today, false, $pg, "stock_top_$type");
			else
				single_bar($today, false, $pg, "stock_top_$type");
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='stock_top_".$type."_panel$key'>";
			}
			pie_chart($today, false, $pg, "stock_top_".$type."_pie");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	return $title;
}

function stock_turnover_trend($today, $width="33", $panels=array()) {

	$date = begin_month($today);
	$date = add_months($date, -11);
	list($dy, $my, $yy) = explode_date_to_dmy($date);

	$month13 = date('Y-m-d',mktime(0,0,0,$my+12,1,$yy));
	$month12 = date('Y-m-d',mktime(0,0,0,$my+11,1,$yy));
	$month11 = date('Y-m-d',mktime(0,0,0,$my+10,1,$yy));
	$month10 = date('Y-m-d',mktime(0,0,0,$my+9,1,$yy));
	$month09 = date('Y-m-d',mktime(0,0,0,$my+8,1,$yy));
	$month08 = date('Y-m-d',mktime(0,0,0,$my+7,1,$yy));
	$month07 = date('Y-m-d',mktime(0,0,0,$my+6,1,$yy));
	$month06 = date('Y-m-d',mktime(0,0,0,$my+5,1,$yy));
	$month05 = date('Y-m-d',mktime(0,0,0,$my+4,1,$yy));
	$month04 = date('Y-m-d',mktime(0,0,0,$my+3,1,$yy));
	$month03 = date('Y-m-d',mktime(0,0,0,$my+2,1,$yy));
	$month02 = date('Y-m-d',mktime(0,0,0,$my+1,1,$yy));
	$month01 = date('Y-m-d',mktime(0,0,0,$my,1,$yy));
	
	$months = array($month01,$month02,$month03,$month04,$month05,$month06,$month07,$month08,$month09,$month10,$month11,$month12,$month13);
	$days = array();

	$result_m = array();
	$result_y = array();
	
	foreach ($months as $k=>$month) {
		$month1 = sql2date($month);

		if($k < count($months)-1)
			$next_month = sql2date($months[$k+1]);
		$def_cogs = get_company_pref('default_cogs_act');
		$cogs_group = get_gl_account($def_cogs)['account_type'];
		$accounts = get_gl_accounts(null, null, $cogs_group);
		$month_cogs = 0;
		while ($account = db_fetch($accounts)) {
			$month_cogs += get_balance($account["account_code"], 0, 0, $month1, $next_month, true, true)['balance'];
		}
		
		if($k < count($months)-1) {
			$month_avrg_val = get_average_stock_value($month1, $next_month);
			$result_y[$month] = $month_avrg_val == 0 ? 0 : $month_cogs / $month_avrg_val;
		}
	}
	
	$title = _('Inventory Turnover');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'stock_turnover', $icons);
	else
		display_title($title, 'stock_turnover');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='stock_turnover_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_m as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr_m = array();
	$cols_arr_y = array();
	$hidden_arr_m = array();
	$hidden_arr_y = array();
	$val_arr_m = array();
	$val_arr_y = array();
	$pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);

	$dec = user_price_dec();

	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col));
			$hidden_arr_m[] = date('Y-m-d', strtotime($col));
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_y[] = date('Y-m-d', strtotime($col));
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 2 == 0)
			$pg_m['x'][] = $cols_arr_m[$i]; 
		else
			$pg_m['x'][] = '';

		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = $cols_arr_m[$i];
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 2 == 0)
			$pg_y['x'][] = $cols_arr_y[$i];
		else
			$pg_y['x'][] = '';

		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = $cols_arr_y[$i];
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
					echo "<div class='tab-pane fade' id='stock_turnover_panel$key'>";
			}
			line_chart($today, false, $pg_m, "stock_turnover_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade in active' id='stock_turnover_panel$key'>";
			}
			line_chart($today, false, $pg_y, "stock_turnover_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function stock_values($today, $width="33", $panels=array()) {

	$sql = "SELECT tbl.cat_description, SUM(tbl.ItemTotal) AS ItemTotal FROM (SELECT item.category_id, category.description AS cat_description, SUM(move.qty) * item.material_cost AS ItemTotal FROM ".TB_PREF."stock_master item,".TB_PREF."stock_category category,".TB_PREF."stock_moves move WHERE item.stock_id=move.stock_id AND item.category_id=category.category_id AND item.mb_flag <> 'D' AND mb_flag <> 'F' AND move.tran_date <= '".date2sql($today)."' GROUP BY item.category_id, category.description, item.stock_id HAVING SUM(move.qty) != 0 ORDER BY item.category_id) AS tbl GROUP BY tbl.category_id";

	$result = db_query($sql, 'Transactions could not be calculated');

	$title = _('Values by Category');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'stock_values', $icons);
	else
		display_title($title, 'stock_values');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='stock_values_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Category'), _('Total'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['ItemTotal'];
	}

	foreach($result as $myrow) {
		
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($myrow['cat_description']);
			amount_cell($myrow['ItemTotal']);
		}

		$pg['x'][] = $myrow['cat_description']; 
		$pg['y'][] = array(
			'meta'=>$myrow['cat_description'].' - '.round(($myrow['ItemTotal']/$sum)*100).'%',
			'value'=>round2($myrow['ItemTotal'], $dec)
		);
		
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}

	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='stock_values_panel$key'>";
			}
			single_bar($today, false, $pg, 'stock_values');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='stock_values_panel$key'>";
			}
			pie_chart($today, false, $pg, 'stock_values_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function dimension_top($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM(-t.amount) AS total, d.reference, d.name FROM ".TB_PREF."gl_trans AS t,".TB_PREF."dimensions AS d WHERE (t.dimension_id = d.id OR t.dimension2_id = d.id) AND t.tran_date >= '$begin' AND t.tran_date <= '$today1' GROUP BY d.id ORDER BY total DESC LIMIT $limit";

	$result = db_query($sql, 'Transactions could not be calculated');
	$title = sprintf(_("Top %s Dimensions in fiscal year"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'dim_top', $icons);
	else
		display_title($title, 'dim_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='dim_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Dimension"), _("Amount"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	$pg = array('x'=>array(), 'y'=>array());
	check_page_security('SA_DIMTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['reference']." ".$myrow["name"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2(abs($myrow['total']), $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade in active' id='dim_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'dim_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='dim_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'dim_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	return $title;
}

function gl_top($today, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM(amount) AS total, c.class_name, c.ctype FROM ".TB_PREF."gl_trans,".TB_PREF."chart_master AS a, ".TB_PREF."chart_types AS t, ".TB_PREF."chart_class AS c WHERE account = a.account_code AND a.account_type = t.id AND t.class_id = c.cid AND IF(c.ctype > 3, tran_date >= '$begin', tran_date >= '0000-00-00') AND tran_date <= '$today1' GROUP BY c.cid ORDER BY c.cid";

	$result = db_query($sql, 'Transactions could not be calculated');

	$title = _('Class Balances');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'gl_top', $icons);
	else
		display_title($title, 'gl_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='gl_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels))
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");

	check_page_security('SA_GLANALYTIC');
	$pg = array('x'=>array(), 'y'=>array());
	$i = 0;
	$total = 0;
	$dec = user_price_dec();
	while ($myrow = db_fetch($result))
	{
		if ($myrow['ctype'] > 3)
		{
			$total += $myrow['total'];
			$myrow['total'] = -$myrow['total'];
			$pg['x'][] = $myrow['class_name']; 
			$pg['y'][] = array('meta'=>$myrow['class_name'], 'value'=>round2(abs($myrow['total']), $dec));
			$i++;
		}
		if(count($panels) == 0 || in_array('table', $panels))
			label_row($myrow['class_name'], number_format2($myrow['total'], $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
	}
	$calculated = _('Return');

	if(count($panels) == 0 || in_array('table', $panels)) {
		label_row("&nbsp;", '');
		label_row($calculated, number_format2(-$total, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
	}
	$pg['x'][$i] = $calculated; 
	$pg['y'][$i] = array('meta'=>$calculated, 'value'=>round2(-$total, $dec));

	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade' id='gl_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'gl_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='gl_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'gl_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; //content
	echo '</div>'; //box
	
	return $title;
}

function receivables_by_age($today, $width="33", $panels=array()) {

	$sql = "SELECT debtor_no, curr_code FROM ".TB_PREF."debtors_master WHERE !inactive";
	$result = db_query($sql, 'could not get customers data');
	
	$result0 = 0;
	$result1 = 0;
	$result2 = 0;
	$result3 = 0;
	
	while ($myrow = db_fetch($result)) {
		$debtor = get_customer_details($myrow['debtor_no'], null, false);
		$home_curr = get_company_pref('curr_default');
		if($home_curr != $myrow['curr_code'])
			$rate = get_exchange_rate_from_home_currency($myrow['curr_code'], Today());
		else
			$rate = 1.0;
		$result0 += ($debtor['Balance'] - $debtor['Due']) * $rate; 
		$result1 += ($debtor['Due'] - $debtor['Overdue1']) * $rate;
		$result2 += ($debtor['Overdue1'] - $debtor['Overdue2']) * $rate;
		$result3 += $debtor['Overdue2'] * $rate;
	}

	$total = $result3 + $result2 + $result1 + $result0;

	$title = _('Receivables by Age');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'receivables_by_age', $icons);
	else
		display_title($title, 'receivables_by_age');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='receivables_by_age_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels))
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");

	check_page_security('SA_GLANALYTIC');
	$pg = array('x'=>array(), 'y'=>array());
	$sum = $total;

	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
	$current = _('Current');
	$nowdue = "1-" . $past1 . " " . _('Days');
	$pastdue1 = $past1 + 1 . "-" . $past2 . " " . _('Days');
	$pastdue2 = _('Over') . " " . $past2 . " " . _('Days');
	$dec = user_price_dec();
	
	$pg['x'][0] = $current;
	$pg['x'][1] = $nowdue;
	$pg['x'][2] = $pastdue1;
	$pg['x'][3] = $pastdue2;
	
	$pg['y'][0] = array(
		'meta'=>$current.' - '.round(($result0/$sum)*100).'%',
		'value'=>round2(abs($result0), $dec)
	);
	$pg['y'][1] = array(
		'meta'=>$nowdue.' - '.round(($result1/$sum)*100).'%',
		'value'=>round2(abs($result1), $dec)
	);
	$pg['y'][2] = array(
		'meta'=>$pastdue1.' - '.round(($result2/$sum)*100).'%',
		'value'=>round2(abs($result2), $dec)
	);
	$pg['y'][3] = array(
		'meta'=>$pastdue2.' - '.round(($result3/$sum)*100).'%',
		'value'=>round2(abs($result3), $dec)
	);

	if(count($panels) == 0 || in_array('table', $panels)) {
		label_row($current, number_format2($result0, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
		label_row($nowdue, number_format2($result1, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
		label_row($pastdue1, number_format2($result2, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
		label_row($pastdue2, number_format2($result3, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
		label_row("&nbsp;", "");
		label_row(_('Total'), number_format2($total, $dec), "class='' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
	}

	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade' id='receivables_by_age_panel$key'>";
			}
			single_bar($today, false, $pg, 'receivables_by_age');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='receivables_by_age_panel$key'>";
			}
			pie_chart($today, false, $pg, 'receivables_by_age_pie');
			echo '</div>';
		}
	}
	echo '</div>'; //content
	echo '</div>'; //box
	
	return $title;
}

function cash_flow($today, $width="33", $panels=array()) {

	$date = begin_month($today);
	$date = add_months($date, -11);
	list($dy, $my, $yy) = explode_date_to_dmy($date);
	list($dm, $mm, $ym) = explode_date_to_dmy($today);

	$month13 = date('Y-m-d',mktime(0,0,0,$my+12,1,$yy));
	$month12 = date('Y-m-d',mktime(0,0,0,$my+11,1,$yy));
	$month11 = date('Y-m-d',mktime(0,0,0,$my+10,1,$yy));
	$month10 = date('Y-m-d',mktime(0,0,0,$my+9,1,$yy));
	$month09 = date('Y-m-d',mktime(0,0,0,$my+8,1,$yy));
	$month08 = date('Y-m-d',mktime(0,0,0,$my+7,1,$yy));
	$month07 = date('Y-m-d',mktime(0,0,0,$my+6,1,$yy));
	$month06 = date('Y-m-d',mktime(0,0,0,$my+5,1,$yy));
	$month05 = date('Y-m-d',mktime(0,0,0,$my+4,1,$yy));
	$month04 = date('Y-m-d',mktime(0,0,0,$my+3,1,$yy));
	$month03 = date('Y-m-d',mktime(0,0,0,$my+2,1,$yy));
	$month02 = date('Y-m-d',mktime(0,0,0,$my+1,1,$yy));
	$month01 = date('Y-m-d',mktime(0,0,0,$my,1,$yy));
	
	$sql_m = "SELECT";

	for($i=30; $i>0; $i--) {
		${"date$i"} = date('Y-m-d', mktime(0,0,0,$mm,$dm-$i,$ym));
		$sql_m .= " SUM(CASE WHEN trans_date = '".${"date$i"}."' THEN amount ELSE 0 END) AS '${"date$i"}',";
	}
	$sql_m .= " SUM(CASE WHEN trans_date = '$ym-$mm-$dm' THEN amount ELSE 0 END) AS '$ym-$mm-$dm' FROM ".TB_PREF."bank_trans";
		
	$sql_y = "SELECT SUM(CASE WHEN trans_date >= '$month01' AND trans_date < '$month02' THEN amount ELSE 0 END) AS '$month01',
		SUM(CASE WHEN trans_date >= '$month02' AND trans_date < '$month03' THEN amount ELSE 0 END) AS '$month02',
		SUM(CASE WHEN trans_date >= '$month03' AND trans_date < '$month04' THEN amount ELSE 0 END) AS '$month03',
		SUM(CASE WHEN trans_date >= '$month04' AND trans_date < '$month05' THEN amount ELSE 0 END) AS '$month04',
		SUM(CASE WHEN trans_date >= '$month05' AND trans_date < '$month06' THEN amount ELSE 0 END) AS '$month05',
		SUM(CASE WHEN trans_date >= '$month06' AND trans_date < '$month07' THEN amount ELSE 0 END) AS '$month06',
		SUM(CASE WHEN trans_date >= '$month07' AND trans_date < '$month08' THEN amount ELSE 0 END) AS '$month07',
		SUM(CASE WHEN trans_date >= '$month08' AND trans_date < '$month09' THEN amount ELSE 0 END) AS '$month08',
		SUM(CASE WHEN trans_date >= '$month09' AND trans_date < '$month10' THEN amount ELSE 0 END) AS '$month09',
		SUM(CASE WHEN trans_date >= '$month10' AND trans_date < '$month11' THEN amount ELSE 0 END) AS '$month10',
		SUM(CASE WHEN trans_date >= '$month11' AND trans_date < '$month12' THEN amount ELSE 0 END) AS '$month11',
		SUM(CASE WHEN trans_date >= '$month12' AND trans_date < '$month13' THEN amount ELSE 0 END) AS '$month12'
		FROM ".TB_PREF."bank_trans";


	$result_m = db_fetch(db_query($sql_m, 'Transactions could not be calculated'));
	$result_y = db_fetch(db_query($sql_y, 'Transactions could not be calculated'));
	
	$title = _('Cash Flow');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, "cash_flow", $icons);
	else
		display_title($title, "cash_flow");

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='cash_flow_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_m as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$dec = user_price_dec();
	$k = 0;

	$cols_arr_m = $cols_arr_y = $hidden_arr_y = $hidden_arr_m = array();
	$val_arr_m = $val_arr_y = array();
	$pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=> array());
	alt_table_row_color($k);

	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col));
			$hidden_arr_m[] = date('Y-m-d', strtotime($col));
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_y[] = date('Y-m-d', strtotime($col));
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 3 == 0)
			$pg_m['x'][] = $cols_arr_m[$i]; 
		else
			$pg_m['x'][] = '';
		
		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = sql2date($hidden_arr_m[$i]);
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 2 == 0)
			$pg_y['x'][] = $cols_arr_y[$i]; 
		else
			$pg_y['x'][] = '';
		
		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = sql2date($hidden_arr_y[$i]);
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
					echo "<div class='tab-pane fade' id='cash_flow_panel$key'>";
			}
			line_chart($today, false, $pg_m, "cash_flow_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade in active' id='cash_flow_panel$key'>";
			}
			line_chart($today, false, $pg_y, "cash_flow_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function gl_performance($today, $width="33", $weeks=5) {
	global $SysPrefs;

	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);
	$sep = $SysPrefs->dateseps[user_date_sep()];

	$sql = "SELECT week_name, sales, costs FROM(SELECT DATE_FORMAT(tran_date, '%Y{$sep}%u') AS week_name, SUM(IF(c.ctype = 4, amount * -1, 0)) AS sales, SUM(IF(c.ctype = 6, amount, 0)) AS costs FROM ".TB_PREF."gl_trans, ".TB_PREF."chart_master AS a, ".TB_PREF."chart_types AS t, ".TB_PREF."chart_class AS c WHERE(c.ctype = 4 OR c.ctype = 6) AND account = a.account_code AND a.account_type = t.id AND t.class_id = c.cid AND tran_date >= '$begin' AND tran_date <= '$today1' GROUP BY week_name ORDER BY week_name DESC LIMIT 0, $weeks) b GROUP BY week_name ORDER BY week_name ASC";

	$result = db_query($sql, 'Transactions could not be calculated');
	$title = sprintf(_("Last %s weeks Performance"), $weeks);
	check_page_security('SA_GLANALYTIC');

	$dec = user_price_dec();

	while ($myrow = db_fetch($result)) {

		$wk_array = explode($sep, $myrow['week_name']);
		$wk_meta = $wk_array[0].' '._('Week:').$wk_array[1];
		$pg['x'][] = $myrow['week_name']; 
		$pg['y'][] = array('meta'=>$wk_meta, 'value'=>round2($myrow['sales'], $dec));
		$pg['z'][] = round2($myrow['costs'], $dec);
	}	
	
	double_bar($today, $title, $pg, 'gl_perfomance');
}

function department_expenses($today, $width='33', $panels=array()) {

	$sql = "SELECT SUM(p.payable_amount) amt, d.dept_name, e.emp_id FROM ".TB_PREF."payslip p, ".TB_PREF."department d, ".TB_PREF."employee e WHERE p.emp_id = e.emp_id AND e.department_id = d.dept_id GROUP BY dept_name";

	$result = db_query($sql, 'could not get payslip data');

	$title = _('Expenses by Department');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'dept_top', $icons);
	else
		display_title($title, 'dept_top');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='dept_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Department'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();
	$sum = 0;

	foreach ($result as $v) {
		$sum += $v['amt'];
	}

	foreach($result as $myrow) {

		$name = $myrow['dept_name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['amt']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['amt']/$sum)*100).'%',
			'value'=>round2($myrow['amt'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='dept_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'dept_top');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='dept_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'dept_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function department_employees($today, $width="33", $panels=array()) {

	$sql = "SELECT COUNT(e.emp_id) total, d.dept_name FROM ".TB_PREF."employee e, ".TB_PREF."department d WHERE e.department_id = d.dept_id GROUP BY dept_id";

	$result = db_query($sql, 'could not get employee data');

	$title = _('Employees by Department');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'dept_emp', $icons);
	else
		display_title($title, 'dept_emp');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='dept_emp_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Department'), _('Employees'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['dept_name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='dept_emp_panel$key'>";
			}
			single_bar($today, false, $pg, 'dept_emp');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='dept_emp_panel$key'>";
			}
			pie_chart($today, false, $pg, 'dept_emp_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function employee_trend($today, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$date = begin_month($today);
	$date = add_months($date, -35);
	list($da, $mo, $yr) = explode_date_to_dmy($date);

	$date37 = date('Y-m-d',mktime(0,0,0,$mo+36,1,$yr));
	$date36 = date('Y-m-d',mktime(0,0,0,$mo+35,1,$yr));
	$date35 = date('Y-m-d',mktime(0,0,0,$mo+34,1,$yr));
	$date34 = date('Y-m-d',mktime(0,0,0,$mo+33,1,$yr));
	$date33 = date('Y-m-d',mktime(0,0,0,$mo+32,1,$yr));
	$date32 = date('Y-m-d',mktime(0,0,0,$mo+31,1,$yr));
	$date31 = date('Y-m-d',mktime(0,0,0,$mo+30,1,$yr));
	$date30 = date('Y-m-d',mktime(0,0,0,$mo+29,1,$yr));
	$date29 = date('Y-m-d',mktime(0,0,0,$mo+28,1,$yr));
	$date28 = date('Y-m-d',mktime(0,0,0,$mo+27,1,$yr));
	$date27 = date('Y-m-d',mktime(0,0,0,$mo+26,1,$yr));
	$date26 = date('Y-m-d',mktime(0,0,0,$mo+25,1,$yr));
	$date25 = date('Y-m-d',mktime(0,0,0,$mo+24,1,$yr));

	$date24 = date('Y-m-d',mktime(0,0,0,$mo+23,1,$yr));
	$date23 = date('Y-m-d',mktime(0,0,0,$mo+22,1,$yr));
	$date22 = date('Y-m-d',mktime(0,0,0,$mo+21,1,$yr));
	$date21 = date('Y-m-d',mktime(0,0,0,$mo+20,1,$yr));
	$date20 = date('Y-m-d',mktime(0,0,0,$mo+19,1,$yr));
	$date19 = date('Y-m-d',mktime(0,0,0,$mo+18,1,$yr));
	$date18 = date('Y-m-d',mktime(0,0,0,$mo+17,1,$yr));
	$date17 = date('Y-m-d',mktime(0,0,0,$mo+16,1,$yr));
	$date16 = date('Y-m-d',mktime(0,0,0,$mo+15,1,$yr));
	$date15 = date('Y-m-d',mktime(0,0,0,$mo+14,1,$yr));
	$date14 = date('Y-m-d',mktime(0,0,0,$mo+13,1,$yr));
	$date13 = date('Y-m-d',mktime(0,0,0,$mo+12,1,$yr));

	$date12 = date('Y-m-d',mktime(0,0,0,$mo+11,1,$yr));
	$date11 = date('Y-m-d',mktime(0,0,0,$mo+10,1,$yr));
	$date10 = date('Y-m-d',mktime(0,0,0,$mo+9,1,$yr));
	$date09 = date('Y-m-d',mktime(0,0,0,$mo+8,1,$yr));
	$date08 = date('Y-m-d',mktime(0,0,0,$mo+7,1,$yr));
	$date07 = date('Y-m-d',mktime(0,0,0,$mo+6,1,$yr));
	$date06 = date('Y-m-d',mktime(0,0,0,$mo+5,1,$yr));
	$date05 = date('Y-m-d',mktime(0,0,0,$mo+4,1,$yr));
	$date04 = date('Y-m-d',mktime(0,0,0,$mo+3,1,$yr));
	$date03 = date('Y-m-d',mktime(0,0,0,$mo+2,1,$yr));
	$date02 = date('Y-m-d',mktime(0,0,0,$mo+1,1,$yr));
	$date01 = date('Y-m-d',mktime(0,0,0,$mo,1,$yr));

	$sql = "SELECT 
		(SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date01') 
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date01') AS '$date01' ,

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date02')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date02')) AS '$date02' ,

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date03')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date03')) AS '$date03' ,

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date04')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date04')) AS '$date04' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date05')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date05')) AS '$date05' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date06')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date06')) AS '$date06' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date07')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date07')) AS '$date07' ,

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date08')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date08')) AS '$date08' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date09')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date09')) AS '$date09' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date10')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date10')) AS '$date10' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date11')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date11')) AS '$date11' , 

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date12')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date12')) AS '$date12' ,

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date13')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date13')) AS '$date13',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date14')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date14')) AS '$date14',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date15')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date15')) AS '$date15',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date16')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date16')) AS '$date16',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date17')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date17')) AS '$date17',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date18')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date18')) AS '$date18',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date19')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date19')) AS '$date19',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date20')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date20')) AS '$date20',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date21')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date21')) AS '$date21',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date22')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date22')) AS '$date22',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date23')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date23')) AS '$date23',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date24')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date24')) AS '$date24',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date25')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date25')) AS '$date25',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date26')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date26')) AS '$date26',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date27')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date27')) AS '$date27',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date28')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date28')) AS '$date28',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date29')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date29')) AS '$date29',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date30')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date30')) AS '$date30',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date31')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date31')) AS '$date31',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date32')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date32')) AS '$date32',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date33')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date33')) AS '$date33',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date34')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date34')) AS '$date34',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date35')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date35')) AS '$date35',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date36')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date36')) AS '$date36',

		((SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_hiredate < '$date37')
		- (SELECT COUNT(emp_id) FROM ".TB_PREF."employee WHERE emp_releasedate > '0000-00-00' AND emp_releasedate < '$date37')) AS '$date37' ";
		
	$result = db_fetch(db_query($sql, 'could not get employee data'));
	
	$title = _('Employee Trend');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'emp_trend', $icons);
	else
		display_title($title, 'emp_trend');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='emp_trend_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr = $hidden_arr = array();
	$val_arr = array();
	$pg = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);
	foreach($result as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cur_date = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$cols_arr[] = $cur_date;
			$hidden_arr[] = date('Y-m-d', strtotime($col));
			$val_arr[] = round2($val, user_price_dec());
		}
	}
	for($i=0; $i < count($cols_arr); $i++) {
		if($i % 4 == 0)
			$pg['x'][] = $cols_arr[$i]; 
		else
			$pg['x'][] = '';

		$pg['y'] = $val_arr;
		$pg['x-hidden'][] = sql2date($hidden_arr[$i]);
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('line-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'line-chart')
					echo "<div class='tab-pane fade in active' id='emp_trend_panel$key'>";
			}
			line_chart($today, false, $pg, "emp_trend_line", 'false');
			echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade' id='emp_trend_panel$key'>";
			}
			single_bar($today, false, $pg, "emp_trend_bar");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function age_employees($today, $width="33", $panels=array()) {
	
	$ages = array(20, 25, 30, 35, 40, 45, 50, 55, 60, 65);

	$sql = "SELECT 
	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age < ".$ages[0].") AS 'Under ".$ages[0]."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[0]." AND tbl.age < ".$ages[1].") AS '".$ages[0]."-".($ages[1]-1)."',

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[1]." AND tbl.age < ".$ages[2].") AS '".$ages[1]."-".($ages[2]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[2]." AND tbl.age < ".$ages[3].") AS '".$ages[2]."-".($ages[3]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[3]." AND tbl.age < ".$ages[4].") AS '".$ages[3]."-".($ages[4]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[4]." AND tbl.age < ".$ages[5].") AS '".$ages[4]."-".($ages[5]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[5]." AND tbl.age < ".$ages[6].") AS '".$ages[5]."-".($ages[6]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[6]." AND tbl.age < ".$ages[7].") AS '".$ages[6]."-".($ages[7]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[7]." AND tbl.age < ".$ages[8].") AS '".$ages[7]."-".($ages[8]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[8]." AND tbl.age < ".$ages[9].") AS '".$ages[8]."-".($ages[9]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, emp_birthdate, CURDATE()) AS age FROM ".TB_PREF."employee WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[9].") AS 'Above ".$ages[9]."'" ;

	$result = db_query($sql, 'could not get employee data');

	$title = _('Employees by Ages');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'ages_emp', $icons);
	else
		display_title($title, 'ages_emp');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='ages_emp_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Age'), _('Employees'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $row) {
		foreach($row as $val) {
			$sum += $val;
		}
	}

	foreach ($result as $row) {
		foreach($row as $name => $val) {
			if(count($panels) == 0 || in_array('table', $panels)) {
				alt_table_row_color($k);
				label_cell($name);
				amount_cell($val);
			}

			$pg['x'][] = $name; 
			$pg['y'][] = array(
				'meta'=>$sum > 0 ? $name.' - '.round(($val/$sum)*100).'%' : '',
				'value'=>round2($val, $dec)
			);
			$i++;
			if(count($panels) == 0 || in_array('table', $panels))
				end_row();
		}
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='ages_emp_panel$key'>";
			}
			single_bar($today, false, $pg, 'ages_emp');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='ages_emp_panel$key'>";
			}
			pie_chart($today, false, $pg, 'ages_emp_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function emp_by_dept($today, $width="33", $panels=array()) {
	$sql = "SELECT COUNT(ej.empl_id) total, d.description FROM ".TB_PREF."kv_empl_job ej, ".TB_PREF."kv_empl_departments d WHERE ej.department = d.id GROUP BY d.id";

	$result = db_query($sql, 'could not get employee data');

	$title = _('Employees by Department');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'dept_emp', $icons);
	else
		display_title($title, 'dept_emp');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='dept_emp_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Department"), _("Employees"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	while ($myrow = db_fetch($result))
	{
		$name = $myrow["description"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='dept_emp_panel$key'>";
			}
			single_bar($today, false, $pg, 'dept_emp');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='dept_emp_panel$key'>";
			}
			pie_chart($today, false, $pg, 'dept_emp_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function Shift_sales_top($today, $limit=10, $width="33", $panels=array()) {
	global $security_areas;

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM((ov_amount + ov_discount) * rate * IF(trans.type = ".ST_CUSTCREDIT.", -1, 1)) AS total, shift.shift_name FROM ".TB_PREF."debtor_trans AS trans, ".TB_PREF."pos_trans as pt, ".TB_PREF."pos_shift as shift WHERE (TIME_FORMAT(pt.trans_time, '%H:%i:%s') BETWEEN shift.start_time AND shift.end_time) AND trans.trans_no = pt.trans_no AND trans.type = ".ST_SALESINVOICE." AND tran_date >= '$begin' AND tran_date <= '$today1' ";

	if(!array_search($security_areas['SA_POSSUPERADMIN'][0], $_SESSION['wa_current_user']->role_set))
		$sql .= " AND pt.pos_id = ".$_SESSION['wa_current_user']->pos;

	$sql .= " GROUP by shift.id ORDER BY total DESC LIMIT $limit";

	$result = db_query($sql, 'could not get pos sales data');

	$title = _('Sales by Shift');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'shift_top', $icons);
	else
		display_title($title, 'shift_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='shift_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Shift'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {
		$name = $myrow['shift_name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}
		
		$pg1['x'][] = $myrow['shift_name'];
		$pg1['y'][] = array(
			'meta'=>$myrow['shift_name'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec));

		$pg2['x'][] = $myrow['shift_name'];
		$pg2['y'][] = array('meta'=>$myrow['shift_name'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='shift_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'shift_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='shift_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'shift_top_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function pos_sales_trend($today, $width="33", $panels=array('line_chart')) {
	global $security_areas, $path_to_root;

	$date = begin_month($today);
	$date = add_months($date, -35);
	list($dm, $mm, $ym) = explode_date_to_dmy($date);
	list($dd, $md, $yd) = explode_date_to_dmy($today);

	$month37 = date('Y-m-d',mktime(0,0,0,$mm+36,1,$ym));
	$month36 = date('Y-m-d',mktime(0,0,0,$mm+35,1,$ym));
	$month35 = date('Y-m-d',mktime(0,0,0,$mm+34,1,$ym));
	$month34 = date('Y-m-d',mktime(0,0,0,$mm+33,1,$ym));
	$month33 = date('Y-m-d',mktime(0,0,0,$mm+32,1,$ym));
	$month32 = date('Y-m-d',mktime(0,0,0,$mm+31,1,$ym));
	$month31 = date('Y-m-d',mktime(0,0,0,$mm+30,1,$ym));
	$month30 = date('Y-m-d',mktime(0,0,0,$mm+29,1,$ym));
	$month29 = date('Y-m-d',mktime(0,0,0,$mm+28,1,$ym));
	$month28 = date('Y-m-d',mktime(0,0,0,$mm+27,1,$ym));
	$month27 = date('Y-m-d',mktime(0,0,0,$mm+26,1,$ym));
	$month26 = date('Y-m-d',mktime(0,0,0,$mm+25,1,$ym));
	$month25 = date('Y-m-d',mktime(0,0,0,$mm+24,1,$ym));
	$month24 = date('Y-m-d',mktime(0,0,0,$mm+23,1,$ym));
	$month23 = date('Y-m-d',mktime(0,0,0,$mm+22,1,$ym));
	$month22 = date('Y-m-d',mktime(0,0,0,$mm+21,1,$ym));
	$month21 = date('Y-m-d',mktime(0,0,0,$mm+20,1,$ym));
	$month20 = date('Y-m-d',mktime(0,0,0,$mm+19,1,$ym));
	$month19 = date('Y-m-d',mktime(0,0,0,$mm+18,1,$ym));
	$month18 = date('Y-m-d',mktime(0,0,0,$mm+17,1,$ym));
	$month17 = date('Y-m-d',mktime(0,0,0,$mm+16,1,$ym));
	$month16 = date('Y-m-d',mktime(0,0,0,$mm+15,1,$ym));
	$month15 = date('Y-m-d',mktime(0,0,0,$mm+14,1,$ym));
	$month14 = date('Y-m-d',mktime(0,0,0,$mm+13,1,$ym));
	$month13 = date('Y-m-d',mktime(0,0,0,$mm+12,1,$ym));
	$month12 = date('Y-m-d',mktime(0,0,0,$mm+11,1,$ym));
	$month11 = date('Y-m-d',mktime(0,0,0,$mm+10,1,$ym));
	$month10 = date('Y-m-d',mktime(0,0,0,$mm+9,1,$ym));
	$month09 = date('Y-m-d',mktime(0,0,0,$mm+8,1,$ym));
	$month08 = date('Y-m-d',mktime(0,0,0,$mm+7,1,$ym));
	$month07 = date('Y-m-d',mktime(0,0,0,$mm+6,1,$ym));
	$month06 = date('Y-m-d',mktime(0,0,0,$mm+5,1,$ym));
	$month05 = date('Y-m-d',mktime(0,0,0,$mm+4,1,$ym));
	$month04 = date('Y-m-d',mktime(0,0,0,$mm+3,1,$ym));
	$month03 = date('Y-m-d',mktime(0,0,0,$mm+2,1,$ym));
	$month02 = date('Y-m-d',mktime(0,0,0,$mm+1,1,$ym));
	$month01 = date('Y-m-d',mktime(0,0,0,$mm,1,$ym));
	
	$sql_d = "SELECT";

	for($i=30; $i>0; $i--) {
		${"date$i"} = date('Y-m-d', mktime(0,0,0,$md,$dd-$i,$yd));
		$sql_d .= " SUM(CASE WHEN DATE(trans_time) = '".${"date$i"}."' THEN cash_amount ELSE 0 END) AS '${"date$i"}',";
	}
	$sql_d .= " SUM(CASE WHEN DATE(trans_time) = '$yd-$md-$dd' THEN cash_amount ELSE 0 END) AS '$yd-$md-$dd' FROM ".TB_PREF."pos_trans ";
		
	$sql_m = "SELECT SUM(CASE WHEN DATE(trans_time) >= '$month25' AND DATE(trans_time) < '$month26' THEN cash_amount ELSE 0 END) AS '$month25',
		SUM(CASE WHEN DATE(trans_time) >= '$month26' AND DATE(trans_time) < '$month27' THEN cash_amount ELSE 0 END) AS '$month26',
		SUM(CASE WHEN DATE(trans_time) >= '$month27' AND DATE(trans_time) < '$month28' THEN cash_amount ELSE 0 END) AS '$month27',
		SUM(CASE WHEN DATE(trans_time) >= '$month28' AND DATE(trans_time) < '$month29' THEN cash_amount ELSE 0 END) AS '$month28',
		SUM(CASE WHEN DATE(trans_time) >= '$month29' AND DATE(trans_time) < '$month30' THEN cash_amount ELSE 0 END) AS '$month29',
		SUM(CASE WHEN DATE(trans_time) >= '$month30' AND DATE(trans_time) < '$month31' THEN cash_amount ELSE 0 END) AS '$month30',
		SUM(CASE WHEN DATE(trans_time) >= '$month31' AND DATE(trans_time) < '$month32' THEN cash_amount ELSE 0 END) AS '$month31',
		SUM(CASE WHEN DATE(trans_time) >= '$month32' AND DATE(trans_time) < '$month33' THEN cash_amount ELSE 0 END) AS '$month32',
		SUM(CASE WHEN DATE(trans_time) >= '$month33' AND DATE(trans_time) < '$month34' THEN cash_amount ELSE 0 END) AS '$month33',
		SUM(CASE WHEN DATE(trans_time) >= '$month34' AND DATE(trans_time) < '$month35' THEN cash_amount ELSE 0 END) AS '$month34',
		SUM(CASE WHEN DATE(trans_time) >= '$month35' AND DATE(trans_time) < '$month36' THEN cash_amount ELSE 0 END) AS '$month35',
		SUM(CASE WHEN DATE(trans_time) >= '$month36' AND DATE(trans_time) < '$month37' THEN cash_amount ELSE 0 END) AS '$month36'
		FROM ".TB_PREF."pos_trans ";

	$sql_y = "SELECT SUM(CASE WHEN DATE(trans_time) >= '$month01' AND DATE(trans_time) < '$month02' THEN cash_amount END) AS '$month01',
		SUM(CASE WHEN DATE(trans_time) >= '$month02' AND DATE(trans_time) < '$month03' THEN cash_amount END) AS '$month02',
		SUM(CASE WHEN DATE(trans_time) >= '$month03' AND DATE(trans_time) < '$month04' THEN cash_amount END) AS '$month03',
		SUM(CASE WHEN DATE(trans_time) >= '$month04' AND DATE(trans_time) < '$month05' THEN cash_amount END) AS '$month04',
		SUM(CASE WHEN DATE(trans_time) >= '$month05' AND DATE(trans_time) < '$month06' THEN cash_amount END) AS '$month05',
		SUM(CASE WHEN DATE(trans_time) >= '$month06' AND DATE(trans_time) < '$month07' THEN cash_amount END) AS '$month06',
		SUM(CASE WHEN DATE(trans_time) >= '$month07' AND DATE(trans_time) < '$month08' THEN cash_amount END) AS '$month07',
		SUM(CASE WHEN DATE(trans_time) >= '$month08' AND DATE(trans_time) < '$month09' THEN cash_amount END) AS '$month08',
		SUM(CASE WHEN DATE(trans_time) >= '$month09' AND DATE(trans_time) < '$month10' THEN cash_amount END) AS '$month09',
		SUM(CASE WHEN DATE(trans_time) >= '$month10' AND DATE(trans_time) < '$month11' THEN cash_amount END) AS '$month10',
		SUM(CASE WHEN DATE(trans_time) >= '$month11' AND DATE(trans_time) < '$month12' THEN cash_amount END) AS '$month11',
		SUM(CASE WHEN DATE(trans_time) >= '$month12' AND DATE(trans_time) < '$month13' THEN cash_amount END) AS '$month12',
		SUM(CASE WHEN DATE(trans_time) >= '$month13' AND DATE(trans_time) < '$month14' THEN cash_amount END) AS '$month13',
		SUM(CASE WHEN DATE(trans_time) >= '$month14' AND DATE(trans_time) < '$month15' THEN cash_amount END) AS '$month14',
		SUM(CASE WHEN DATE(trans_time) >= '$month15' AND DATE(trans_time) < '$month16' THEN cash_amount END) AS '$month15',
		SUM(CASE WHEN DATE(trans_time) >= '$month16' AND DATE(trans_time) < '$month17' THEN cash_amount END) AS '$month16',
		SUM(CASE WHEN DATE(trans_time) >= '$month17' AND DATE(trans_time) < '$month18' THEN cash_amount END) AS '$month17',
		SUM(CASE WHEN DATE(trans_time) >= '$month18' AND DATE(trans_time) < '$month19' THEN cash_amount END) AS '$month18',
		SUM(CASE WHEN DATE(trans_time) >= '$month19' AND DATE(trans_time) < '$month20' THEN cash_amount END) AS '$month19',
		SUM(CASE WHEN DATE(trans_time) >= '$month20' AND DATE(trans_time) < '$month21' THEN cash_amount END) AS '$month20',
		SUM(CASE WHEN DATE(trans_time) >= '$month21' AND DATE(trans_time) < '$month22' THEN cash_amount END) AS '$month21',
		SUM(CASE WHEN DATE(trans_time) >= '$month22' AND DATE(trans_time) < '$month23' THEN cash_amount END) AS '$month22',
		SUM(CASE WHEN DATE(trans_time) >= '$month23' AND DATE(trans_time) < '$month24' THEN cash_amount END) AS '$month23',
		SUM(CASE WHEN DATE(trans_time) >= '$month24' AND DATE(trans_time) < '$month25' THEN cash_amount END) AS '$month24',
		SUM(CASE WHEN DATE(trans_time) >= '$month25' AND DATE(trans_time) < '$month26' THEN cash_amount END) AS '$month25',
		SUM(CASE WHEN DATE(trans_time) >= '$month26' AND DATE(trans_time) < '$month27' THEN cash_amount END) AS '$month26',
		SUM(CASE WHEN DATE(trans_time) >= '$month27' AND DATE(trans_time) < '$month28' THEN cash_amount END) AS '$month27',
		SUM(CASE WHEN DATE(trans_time) >= '$month28' AND DATE(trans_time) < '$month29' THEN cash_amount END) AS '$month28',
		SUM(CASE WHEN DATE(trans_time) >= '$month29' AND DATE(trans_time) < '$month30' THEN cash_amount END) AS '$month29',
		SUM(CASE WHEN DATE(trans_time) >= '$month30' AND DATE(trans_time) < '$month31' THEN cash_amount END) AS '$month30',
		SUM(CASE WHEN DATE(trans_time) >= '$month31' AND DATE(trans_time) < '$month32' THEN cash_amount END) AS '$month31',
		SUM(CASE WHEN DATE(trans_time) >= '$month32' AND DATE(trans_time) < '$month33' THEN cash_amount END) AS '$month32',
		SUM(CASE WHEN DATE(trans_time) >= '$month33' AND DATE(trans_time) < '$month34' THEN cash_amount END) AS '$month33',
		SUM(CASE WHEN DATE(trans_time) >= '$month34' AND DATE(trans_time) < '$month35' THEN cash_amount END) AS '$month34',
		SUM(CASE WHEN DATE(trans_time) >= '$month35' AND DATE(trans_time) < '$month36' THEN cash_amount END) AS '$month35',
		SUM(CASE WHEN DATE(trans_time) >= '$month36' AND DATE(trans_time) < '$month37' THEN cash_amount END) AS '$month36' 
		FROM ".TB_PREF."pos_trans ";

	if(!array_search($security_areas['SA_POSSUPERADMIN'][0], $_SESSION['wa_current_user']->role_set)) {
		$sql_d .= " WHERE pos_id = ".$_SESSION['wa_current_user']->pos;
		$sql_m .= " WHERE pos_id = ".$_SESSION['wa_current_user']->pos;
		$sql_y .= " WHERE pos_id = ".$_SESSION['wa_current_user']->pos;
	}

	$result_d = db_fetch(db_query($sql_d, 'Transactions could not be calculated'));
	$result_m = db_fetch(db_query($sql_m, 'Transactions could not be calculated'));
	$result_y = db_fetch(db_query($sql_y, 'Transactions could not be calculated'));
	
	$title = _('Sales Trend');
	if(array_search($security_areas['SA_POSSUPERADMIN'][0], $_SESSION['wa_current_user']->role_set)) {
		// include_once($path_to_root . '/modules/pos/includes/pos_ui.inc');
		// $title .= '&nbsp;'.pos_sales_point_list('pos_id', null, true);
	}
	else {
		$pos_name = get_sales_point_name($_SESSION['wa_current_user']->pos);
		$title .= "&nbsp;(".$pos_name.")";
	}
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'pos_sales_trend', $icons);
	else
		display_title($title, 'pos_sales_trend');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='pos_sales_trend_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_d as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr_d = array();
	$cols_arr_m = array();
	$cols_arr_y = array();
	$hidden_arr_d = array();
	$hidden_arr_m = array();
	$hidden_arr_y = array();
	$val_arr_d = $val_arr_m = $val_arr_y = array();
	$pg_d = $pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);
	$dec = user_price_dec();

	foreach($result_d as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_d[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col));
			$hidden_arr_d[] = date('Y-m-d', strtotime($col));
			$val_arr_d[] = round2($val, $dec);
		}
	}
	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_m[] = date('Y-m-d', strtotime($col));
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}
		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_y[] = date('Y-m-d', strtotime($col));
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_d); $i++) {
		if($i % 5 == 0)
			$pg_d['x'][] = $cols_arr_d[$i]; 
		else
			$pg_d['x'][] = '';

		$pg_d['y'] = $val_arr_d;
		$pg_d['x-hidden'][] = sql2date($hidden_arr_d[$i]);
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 2 == 0)
			$pg_m['x'][] = $cols_arr_m[$i]; 
		else
			$pg_m['x'][] = '';

		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = sql2date($hidden_arr_m[$i]); 
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 5 == 0)
			$pg_y['x'][] = $cols_arr_y[$i]; 
		else
			$pg_y['x'][] = '';

		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = sql2date($hidden_arr_y[$i]);
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('D', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'D')
					echo "<div class='tab-pane fade in active' id='pos_sales_trend_panel$key'>";
			}
			line_chart($today, false, $pg_d, "pos_sales_trend_d");
			echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
				echo "<div class='tab-pane fade' id='pos_sales_trend_panel$key'>";
			}
			line_chart($today, false, $pg_m, "pos_sales_trend_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade' id='pos_sales_trend_panel$key'>";
			}
			line_chart($today, false, $pg_y, "pos_sales_trend_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function pos_stock_top($today, $limit=10, $width="33", $type=0, $panels=array()) {
	
	$sec = 'SA_ITEMSTRANSVIEW';

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM((trans.unit_price * trans.quantity) * d.rate) AS total, s.stock_id, s.description, SUM(trans.quantity) AS qty, SUM((s.material_cost + s.overhead_cost + s.labour_cost) * trans.quantity) AS costs FROM ".TB_PREF."debtor_trans_details AS trans, ".TB_PREF."stock_master AS s, ".TB_PREF."debtor_trans AS d, ".TB_PREF."pos_trans p WHERE trans.stock_id=s.stock_id AND trans.debtor_trans_type=d.type AND trans.debtor_trans_no=d.trans_no AND d.trans_no = p.trans_no AND (d.type = ".ST_SALESINVOICE." OR d.type = ".ST_CUSTCREDIT.") ";

	if ($type != 2)
		$sql .= "AND tran_date >= '$begin' ";
	$sql .= "AND tran_date <= '$today1' GROUP by s.stock_id ORDER BY total DESC, s.stock_id LIMIT $limit";
	$result = db_query($sql, 'could not get stock data');
	
	$title = _('Best Selling Items');

	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, "pos_stock_top_$type", $icons);
	else
		display_title($title, "pos_stock_top_$type");

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='pos_stock_top_".$type."_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Item"), _("Sales"), _("Costs"), _("Quantity"));

		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security($sec);
	$k = 0;
	$i = 0;
	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());
	$dec = user_price_dec();

	while ($myrow = db_fetch($result)) {
		$name = $myrow["description"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
			amount_cell($myrow['costs']);
			qty_cell($myrow['qty']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array('meta'=>$name, 'value'=>round2($myrow['total'], $dec));
		$pg['z'][] = round2($myrow['costs'], $dec);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='pos_stock_top_".$type."_panel$key'>";
			}
			double_bar($today, false, $pg, "pos_stock_top_$type");

			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='pos_stock_top_".$type."_panel$key'>";
			}
			pie_chart($today, false, $pg, "pos_stock_top_".$type."_pie");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	return $title;
}

function expenses_by_dept($today, $width="33", $panels=array()) {
	
	$year = get_company_pref('f_year');

	$sql = "SELECT SUM(sa.net_pay) amt, d.description, ej.empl_id FROM ".TB_PREF."kv_empl_salary sa, ".TB_PREF."kv_empl_departments d, ".TB_PREF."kv_empl_job ej WHERE sa.empl_id = ej.empl_id AND ej.department = d.id AND sa.year = ".db_escape($year)." GROUP BY description";

	$result = db_query($sql, 'could not get payslip data');

	$title = _('Expenses by Department');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'dept_top', $icons);
	else
		display_title($title, 'dept_top');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='dept_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Department"), _("Amount"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$pg = array('x'=>array(), 'y'=>array());
	while ($myrow = db_fetch($result))
	{
		$name = $myrow["description"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['amt']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array('meta'=>$name, 'value'=>round2($myrow['amt'], user_price_dec()));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='dept_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'dept_top');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='dept_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'dept_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function salesman_top($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM(ov_amount+ov_discount) AS InvoiceTotal,
			salesman.salesman_code, salesman.salesman_name ref 
		FROM ".TB_PREF."debtor_trans trans,
			 ".TB_PREF."debtors_master cust,
			 ".TB_PREF."sales_orders sorder,
			 ".TB_PREF."cust_branch branch,
			".TB_PREF."salesman salesman
		WHERE sorder.order_no=trans.order_
			AND sorder.branch_code=branch.branch_code
			AND branch.salesman=salesman.salesman_code
			AND trans.debtor_no=cust.debtor_no
			AND (trans.type=".ST_SALESINVOICE." OR trans.type=".ST_CUSTCREDIT.")
			AND trans.tran_date>='$begin'
			AND trans.tran_date<='$today1'
		GROUP BY salesman.salesman_code ORDER BY salesman.salesman_code, trans.tran_date";

	$result = db_query($sql, 'could not get customer data');

	$title = sprintf(_('Revenue by Salesman'), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'salesman_top', $icons);
	else
		display_title($title, 'salesman_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='salesman_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Salesman'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESMANREP');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['InvoiceTotal'];
	}

	foreach($result as $myrow) {
			$name = $myrow['ref'].' '.$myrow['ref'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['InvoiceTotal']);
		}
		
		$pg1['x'][] = $myrow['ref'];
		$pg1['y'][] = array(
			'meta'=>$myrow['ref'].' - '.round(($myrow['InvoiceTotal']/$sum)*100).'%',
			'value'=>round2($myrow['InvoiceTotal'], $dec)
		);
		$pg2['x'][] = $myrow['ref'];
		$pg2['y'][] = array('meta'=>$myrow['ref'], 'value'=>round2($myrow['InvoiceTotal'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='salesman_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'salesman_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='salesman_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'salesman_top_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function customer_top_deliveries($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT COUNT(w.id) AS total, d.debtor_no, d.debtor_ref, d.name FROM ".TB_PREF."scale_transaction AS w, ".TB_PREF."debtors_master AS d WHERE w.customer_id = d.debtor_no AND w.date >= '$begin' AND w.date <= '$today1' GROUP by d.debtor_no ORDER BY total DESC, d.debtor_no LIMIT $limit";

	$result = db_query($sql, 'could not get customer data');

	$title = sprintf(_("Top %s customers by deliveries"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'cus_top_notes', $icons);
	else
		display_title($title, 'cus_top_notes');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='cus_top_notes_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Customer"), _("Deliveries"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow["debtor_no"]." ".$myrow["name"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg1['x'][] = $myrow['name'];
		$pg1['y'][] = array(
			'meta'=>$myrow['name'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$pg2['x'][] = $myrow['debtor_ref'];
		$pg2['y'][] = array('meta'=>$myrow['debtor_ref'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='cus_top_notes_panel$key'>";
			}
			single_bar($today, false, $pg2, 'cus_top_notes');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='cus_top_notes_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'cus_top_notes_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function customer_top_weight($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM(w.total_weight - w.empty_weight) AS total, d.debtor_no, d.debtor_ref, d.name FROM ".TB_PREF."scale_transaction AS w, ".TB_PREF."debtors_master AS d WHERE w.customer_id = d.debtor_no AND w.date >= '$begin' AND w.date <= '$today1' GROUP by d.debtor_no ORDER BY total DESC, d.debtor_no LIMIT $limit";

	$result = db_query($sql, 'could not get customer data');

	$title = sprintf(_("Top %s customers by weight"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'cus_top_weight', $icons);
	else
		display_title($title, 'cus_top_weight');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='cus_top_weight_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Customer'), _('Total Weight'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['debtor_no'].' '.$myrow['name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg1['x'][] = $myrow['name'];
		$pg1['y'][] = array(
			'meta'=>$myrow['name'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$pg2['x'][] = $myrow['debtor_ref'];
		$pg2['y'][] = array('meta'=>$myrow['debtor_ref'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='cus_top_weight_panel$key'>";
			}
			single_bar($today, false, $pg2, 'cus_top_weight');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='cus_top_weight_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'cus_top_weight_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function stock_top_deliveries($today, $limit=10, $width="33", $panels=array()) {

	$sec = 'SA_ITEMSTRANSVIEW';

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT COUNT(w.id) AS total, s.stock_id, s.description, SUM(w.total_weight - w.empty_weight) AS qty FROM ".TB_PREF."scale_transaction AS w, ".TB_PREF."stock_master AS s WHERE w.stock_id = s.stock_id AND w.date >= '$begin' AND w.date <= '$today1' GROUP by s.stock_id ORDER BY total DESC, s.stock_id LIMIT $limit";

	$result = db_query($sql, 'could not get stock data');
	
	$title = sprintf(_("Top %s Delivery Items in fiscal year"), $limit);

	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'stock_top_notes', $icons);
	else
		display_title($title, 'stock_top_notes');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='stock_top_notes_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Item'), _('Deliveries'), _('Quantity'));
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security($sec);
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['description'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
			qty_cell($myrow['qty']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);

		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade in active' id='stock_top_notes_panel$key'>";
			}
			single_bar($today, false, $pg, "stock_top_notes");
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='stock_top_notes_panel$key'>";
			}
			pie_chart($today, false, $pg, "stock_top_notes_pie");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	return $title;
}

function delivery_trend($today, $width='33', $panels=array()) {

	$date = begin_month($today);
	$date = add_months($date, -11);
	list($dy, $my, $yy) = explode_date_to_dmy($date);
	list($dm, $mm, $ym) = explode_date_to_dmy($today);

	$month13 = date('Y-m-d',mktime(0,0,0,$my+12,1,$yy));
	$month12 = date('Y-m-d',mktime(0,0,0,$my+11,1,$yy));
	$month11 = date('Y-m-d',mktime(0,0,0,$my+10,1,$yy));
	$month10 = date('Y-m-d',mktime(0,0,0,$my+9,1,$yy));
	$month09 = date('Y-m-d',mktime(0,0,0,$my+8,1,$yy));
	$month08 = date('Y-m-d',mktime(0,0,0,$my+7,1,$yy));
	$month07 = date('Y-m-d',mktime(0,0,0,$my+6,1,$yy));
	$month06 = date('Y-m-d',mktime(0,0,0,$my+5,1,$yy));
	$month05 = date('Y-m-d',mktime(0,0,0,$my+4,1,$yy));
	$month04 = date('Y-m-d',mktime(0,0,0,$my+3,1,$yy));
	$month03 = date('Y-m-d',mktime(0,0,0,$my+2,1,$yy));
	$month02 = date('Y-m-d',mktime(0,0,0,$my+1,1,$yy));
	$month01 = date('Y-m-d',mktime(0,0,0,$my,1,$yy));
	
	$sql_m = "SELECT";

	for($i=30; $i>0; $i--) {
		${"date$i"} = date('Y-m-d', mktime(0,0,0,$mm,$dm-$i,$ym));
		$sql_m .= " CASE WHEN s.date = '".${"date$i"}."' THEN COUNT(id) ELSE 0 END AS '${"date$i"}',";
	}
	$sql_m .= " CASE WHEN s.date = '$ym-$mm-$dm' THEN COUNT(id) ELSE 0 END AS '$ym-$mm-$dm' FROM ".TB_PREF."scale_transaction s";
		
	$sql_y = "SELECT CASE WHEN date >= '$month01' AND date < '$month02' THEN COUNT(id) ELSE 0 END AS '$month01',
		CASE WHEN s.date >= '$month02' AND s.date < '$month03' THEN COUNT(id) ELSE 0 END AS '$month02',
		CASE WHEN s.date >= '$month03' AND s.date < '$month04' THEN COUNT(id) ELSE 0 END AS '$month03',
		CASE WHEN s.date >= '$month04' AND s.date < '$month05' THEN COUNT(id) ELSE 0 END AS '$month04',
		CASE WHEN s.date >= '$month05' AND s.date < '$month06' THEN COUNT(id) ELSE 0 END AS '$month05',
		CASE WHEN s.date >= '$month06' AND s.date < '$month07' THEN COUNT(id) ELSE 0 END AS '$month06',
		CASE WHEN s.date >= '$month07' AND s.date < '$month08' THEN COUNT(id) ELSE 0 END AS '$month07',
		CASE WHEN s.date >= '$month08' AND s.date < '$month09' THEN COUNT(id) ELSE 0 END AS '$month08',
		CASE WHEN s.date >= '$month09' AND s.date < '$month10' THEN COUNT(id) ELSE 0 END AS '$month09',
		CASE WHEN s.date >= '$month10' AND s.date < '$month11' THEN COUNT(id) ELSE 0 END AS '$month10',
		CASE WHEN s.date >= '$month11' AND s.date < '$month12' THEN COUNT(id) ELSE 0 END AS '$month11',
		CASE WHEN s.date >= '$month12' AND s.date < '$month13' THEN COUNT(id) ELSE 0 END AS '$month12'
		FROM ".TB_PREF."scale_transaction s";


	$result_m = db_fetch(db_query($sql_m, 'Transactions could not be calculated'));
	$result_y = db_fetch(db_query($sql_y, 'Transactions could not be calculated'));
	
	$title = _('Delivery Trend');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'delivery_trend', $icons);
	else
		display_title($title, 'delivery_trend');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='delivery_trend_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_m as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr_m = $cols_arr_y = $hidden_arr_m = $hidden_arr_y = array();
	$val_arr_m = $val_arr_y = array();
	$pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);

	$dec = user_price_dec();

	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col));
			$hidden_arr_m[] = date('Y-m-d', strtotime($col));
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_y[] = date('Y-m-d', strtotime($col));
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 3 == 0)
			$pg_m['x'][] = $cols_arr_m[$i];
		else
			$pg_m['x'][] = '';

		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = sql2date($hidden_arr_m[$i]);
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 2 == 0)
			$pg_y['x'][] = $cols_arr_y[$i];
		else
			$pg_y['x'][] = '';

		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = sql2date($hidden_arr_y[$i]);
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
					echo "<div class='tab-pane fade' id='delivery_trend_panel$key'>";
			}
			line_chart($today, false, $pg_m, "delivery_trend_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade in active' id='delivery_trend_panel$key'>";
			}
			line_chart($today, false, $pg_y, "delivery_trend_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function booking_trend($today, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$date = begin_month($today);
	$date = add_months($date, -35);
	list($da, $mo, $yr) = explode_date_to_dmy($date);

	$date37 = date('Y-m-d',mktime(0,0,0,$mo+36,1,$yr));
	$date36 = date('Y-m-d',mktime(0,0,0,$mo+35,1,$yr));
	$date35 = date('Y-m-d',mktime(0,0,0,$mo+34,1,$yr));
	$date34 = date('Y-m-d',mktime(0,0,0,$mo+33,1,$yr));
	$date33 = date('Y-m-d',mktime(0,0,0,$mo+32,1,$yr));
	$date32 = date('Y-m-d',mktime(0,0,0,$mo+31,1,$yr));
	$date31 = date('Y-m-d',mktime(0,0,0,$mo+30,1,$yr));
	$date30 = date('Y-m-d',mktime(0,0,0,$mo+29,1,$yr));
	$date29 = date('Y-m-d',mktime(0,0,0,$mo+28,1,$yr));
	$date28 = date('Y-m-d',mktime(0,0,0,$mo+27,1,$yr));
	$date27 = date('Y-m-d',mktime(0,0,0,$mo+26,1,$yr));
	$date26 = date('Y-m-d',mktime(0,0,0,$mo+25,1,$yr));
	$date25 = date('Y-m-d',mktime(0,0,0,$mo+24,1,$yr));

	$date24 = date('Y-m-d',mktime(0,0,0,$mo+23,1,$yr));
	$date23 = date('Y-m-d',mktime(0,0,0,$mo+22,1,$yr));
	$date22 = date('Y-m-d',mktime(0,0,0,$mo+21,1,$yr));
	$date21 = date('Y-m-d',mktime(0,0,0,$mo+20,1,$yr));
	$date20 = date('Y-m-d',mktime(0,0,0,$mo+19,1,$yr));
	$date19 = date('Y-m-d',mktime(0,0,0,$mo+18,1,$yr));
	$date18 = date('Y-m-d',mktime(0,0,0,$mo+17,1,$yr));
	$date17 = date('Y-m-d',mktime(0,0,0,$mo+16,1,$yr));
	$date16 = date('Y-m-d',mktime(0,0,0,$mo+15,1,$yr));
	$date15 = date('Y-m-d',mktime(0,0,0,$mo+14,1,$yr));
	$date14 = date('Y-m-d',mktime(0,0,0,$mo+13,1,$yr));
	$date13 = date('Y-m-d',mktime(0,0,0,$mo+12,1,$yr));

	$date12 = date('Y-m-d',mktime(0,0,0,$mo+11,1,$yr));
	$date11 = date('Y-m-d',mktime(0,0,0,$mo+10,1,$yr));
	$date10 = date('Y-m-d',mktime(0,0,0,$mo+9,1,$yr));
	$date09 = date('Y-m-d',mktime(0,0,0,$mo+8,1,$yr));
	$date08 = date('Y-m-d',mktime(0,0,0,$mo+7,1,$yr));
	$date07 = date('Y-m-d',mktime(0,0,0,$mo+6,1,$yr));
	$date06 = date('Y-m-d',mktime(0,0,0,$mo+5,1,$yr));
	$date05 = date('Y-m-d',mktime(0,0,0,$mo+4,1,$yr));
	$date04 = date('Y-m-d',mktime(0,0,0,$mo+3,1,$yr));
	$date03 = date('Y-m-d',mktime(0,0,0,$mo+2,1,$yr));
	$date02 = date('Y-m-d',mktime(0,0,0,$mo+1,1,$yr));
	$date01 = date('Y-m-d',mktime(0,0,0,$mo,1,$yr));

	$sql = "SELECT 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date01') AS '$date01' ,
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date02') AS '$date02' ,
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date03') AS '$date03' ,
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date04') AS '$date04' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date05') AS '$date05' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date06') AS '$date06' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date07') AS '$date07' ,
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date08') AS '$date08' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date09') AS '$date09' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date10') AS '$date10' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date11') AS '$date11' , 
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date12') AS '$date12' ,
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date13') AS '$date13',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date14') AS '$date14',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date15') AS '$date15',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date16') AS '$date16',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date17') AS '$date17',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date18') AS '$date18',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date19') AS '$date19',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date20') AS '$date20',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date21') AS '$date21',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date22') AS '$date22',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date23') AS '$date23',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date24') AS '$date24',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date25') AS '$date25',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date26') AS '$date26',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date27') AS '$date27',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date28') AS '$date28',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date29') AS '$date29',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date30') AS '$date30',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date31') AS '$date31',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date32') AS '$date32',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date33') AS '$date33',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date34') AS '$date34',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date35') AS '$date35',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date36') AS '$date36',
		(SELECT COUNT(t.booking_id) FROM ".TB_PREF."booking_trans t, ".TB_PREF."booking_orders o WHERE o.booking_id = t.booking_id AND DATE(o.start_time) < '$date37') AS '$date37' ";
		
	$result = db_fetch(db_query($sql, 'could not get booking data'));
	
	$title = _('Booking Trend');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'book_trend', $icons);
	else
		display_title($title, 'book_trend');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='book_trend_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr = array();
	$val_arr = array();
	$pg = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);
	foreach($result as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cur_date = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$cols_arr[] = $cur_date;
			$val_arr[] = array('meta'=>$cur_date, 'value'=>round2($val, user_price_dec()));
		}
	}
	for($i=0; $i < count($cols_arr); $i++) {
		if($i % 4 == 0)
			$pg['x'][] = $cols_arr[$i];
		else
			$pg['x'][] = '';

		$pg['y'] = $val_arr;
		$pg['x-hidden'][] = $cols_arr[$i]; 
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('line-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'line-chart')
					echo "<div class='tab-pane fade in active' id='book_trend_panel$key'>";
			}
			line_chart($today, false, $pg, "book_trend_line", 'false');
			echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
				echo "<div class='tab-pane fade' id='book_trend_panel$key'>";
			}
			single_bar($today, false, $pg, "book_trend_bar");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function booking_customer_top($today, $limit=10, $width="33", $panels=array()) {

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM((ov_amount + ov_discount) * rate * IF(trans.type = ".ST_CUSTCREDIT.", -1, 1)) AS total,d.debtor_no, d.debtor_ref, d.name FROM ".TB_PREF."debtor_trans AS trans, ".TB_PREF."debtors_master AS d WHERE trans.debtor_no=d.debtor_no AND (trans.type = ".ST_SALESINVOICE." OR trans.type = ".ST_CUSTCREDIT.") AND tran_date >= '$begin' AND tran_date <= '$today1' AND trans.trans_no IN (SELECT trans_no FROM ".TB_PREF."booking_trans) GROUP by d.debtor_no ORDER BY total DESC, d.debtor_no LIMIT $limit";

	$result = db_query($sql, 'could not get customer data');

	$title = sprintf(_("Top %s customers this year"), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'book_cus_top', $icons);
	else
		display_title($title, 'book_cus_top');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='book_cus_top_panel$key'>";
		}
	}

	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Customer'), _('Amount'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg1 = $pg2 = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['total'];
	}

	foreach($result as $myrow) {

		$name = $myrow['debtor_no'].' '.$myrow['name'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
		}

		$pg1['x'][] = $myrow['name'];
		$pg1['y'][] = array(
			'meta'=>$myrow['name'].' - '.round(($myrow['total']/$sum)*100).'%',
			'value'=>round2($myrow['total'], $dec)
		);
		$pg2['x'][] = $myrow['debtor_ref'];
		$pg2['y'][] = array('meta'=>$myrow['debtor_ref'], 'value'=>round2($myrow['total'], $dec));
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='book_cus_top_panel$key'>";
			}
			single_bar($today, false, $pg2, 'book_cus_top');
			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='book_cus_top_panel$key'>";
			}
			pie_chart($today, false, $pg1, 'book_cus_top_pie');
			echo '</div>';
		}
	}

	echo '</div>'; // content
	echo '</div>'; // box	

	return $title;
}

function revenue_trend($today, $width="33", $panels=array()) {

	$date = begin_month($today);
	$date = add_months($date, -35);
	list($dm, $mm, $ym) = explode_date_to_dmy($date);
	list($dd, $md, $yd) = explode_date_to_dmy($today);

	$month37 = date('Y-m-d',mktime(0,0,0,$mm+36,1,$ym));
	$month36 = date('Y-m-d',mktime(0,0,0,$mm+35,1,$ym));
	$month35 = date('Y-m-d',mktime(0,0,0,$mm+34,1,$ym));
	$month34 = date('Y-m-d',mktime(0,0,0,$mm+33,1,$ym));
	$month33 = date('Y-m-d',mktime(0,0,0,$mm+32,1,$ym));
	$month32 = date('Y-m-d',mktime(0,0,0,$mm+31,1,$ym));
	$month31 = date('Y-m-d',mktime(0,0,0,$mm+30,1,$ym));
	$month30 = date('Y-m-d',mktime(0,0,0,$mm+29,1,$ym));
	$month29 = date('Y-m-d',mktime(0,0,0,$mm+28,1,$ym));
	$month28 = date('Y-m-d',mktime(0,0,0,$mm+27,1,$ym));
	$month27 = date('Y-m-d',mktime(0,0,0,$mm+26,1,$ym));
	$month26 = date('Y-m-d',mktime(0,0,0,$mm+25,1,$ym));
	$month25 = date('Y-m-d',mktime(0,0,0,$mm+24,1,$ym));
	$month24 = date('Y-m-d',mktime(0,0,0,$mm+23,1,$ym));
	$month23 = date('Y-m-d',mktime(0,0,0,$mm+22,1,$ym));
	$month22 = date('Y-m-d',mktime(0,0,0,$mm+21,1,$ym));
	$month21 = date('Y-m-d',mktime(0,0,0,$mm+20,1,$ym));
	$month20 = date('Y-m-d',mktime(0,0,0,$mm+19,1,$ym));
	$month19 = date('Y-m-d',mktime(0,0,0,$mm+18,1,$ym));
	$month18 = date('Y-m-d',mktime(0,0,0,$mm+17,1,$ym));
	$month17 = date('Y-m-d',mktime(0,0,0,$mm+16,1,$ym));
	$month16 = date('Y-m-d',mktime(0,0,0,$mm+15,1,$ym));
	$month15 = date('Y-m-d',mktime(0,0,0,$mm+14,1,$ym));
	$month14 = date('Y-m-d',mktime(0,0,0,$mm+13,1,$ym));
	$month13 = date('Y-m-d',mktime(0,0,0,$mm+12,1,$ym));
	$month12 = date('Y-m-d',mktime(0,0,0,$mm+11,1,$ym));
	$month11 = date('Y-m-d',mktime(0,0,0,$mm+10,1,$ym));
	$month10 = date('Y-m-d',mktime(0,0,0,$mm+9,1,$ym));
	$month09 = date('Y-m-d',mktime(0,0,0,$mm+8,1,$ym));
	$month08 = date('Y-m-d',mktime(0,0,0,$mm+7,1,$ym));
	$month07 = date('Y-m-d',mktime(0,0,0,$mm+6,1,$ym));
	$month06 = date('Y-m-d',mktime(0,0,0,$mm+5,1,$ym));
	$month05 = date('Y-m-d',mktime(0,0,0,$mm+4,1,$ym));
	$month04 = date('Y-m-d',mktime(0,0,0,$mm+3,1,$ym));
	$month03 = date('Y-m-d',mktime(0,0,0,$mm+2,1,$ym));
	$month02 = date('Y-m-d',mktime(0,0,0,$mm+1,1,$ym));
	$month01 = date('Y-m-d',mktime(0,0,0,$mm,1,$ym));
	
	$sql_d = "SELECT";

	for($i=30; $i>0; $i--) {
		${"date$i"} = date('Y-m-d', mktime(0,0,0,$md,$dd-$i,$yd));
		$sql_d .= " SUM(CASE WHEN DATE(start_time) = '".${"date$i"}."' THEN (unit_price * invoiced) ELSE 0 END) AS '${"date$i"}',";
	}
	$sql_d .= " SUM(CASE WHEN DATE(start_time) = '$yd-$md-$dd' THEN (unit_price * invoiced) ELSE 0 END) AS '$yd-$md-$dd' FROM ".TB_PREF."booking_orders o, ".TB_PREF."booking_order_details d WHERE o.booking_id = d.booking_id ";
		
	$sql_m = "SELECT SUM(CASE WHEN DATE(start_time) >= '$month25' AND DATE(start_time) < '$month26' THEN (unit_price * invoiced) ELSE 0 END) AS '$month25',
		SUM(CASE WHEN DATE(start_time) >= '$month26' AND DATE(start_time) < '$month27' THEN (unit_price * invoiced) ELSE 0 END) AS '$month26',
		SUM(CASE WHEN DATE(start_time) >= '$month27' AND DATE(start_time) < '$month28' THEN (unit_price * invoiced) ELSE 0 END) AS '$month27',
		SUM(CASE WHEN DATE(start_time) >= '$month28' AND DATE(start_time) < '$month29' THEN (unit_price * invoiced) ELSE 0 END) AS '$month28',
		SUM(CASE WHEN DATE(start_time) >= '$month29' AND DATE(start_time) < '$month30' THEN (unit_price * invoiced) ELSE 0 END) AS '$month29',
		SUM(CASE WHEN DATE(start_time) >= '$month30' AND DATE(start_time) < '$month31' THEN (unit_price * invoiced) ELSE 0 END) AS '$month30',
		SUM(CASE WHEN DATE(start_time) >= '$month31' AND DATE(start_time) < '$month32' THEN (unit_price * invoiced) ELSE 0 END) AS '$month31',
		SUM(CASE WHEN DATE(start_time) >= '$month32' AND DATE(start_time) < '$month33' THEN (unit_price * invoiced) ELSE 0 END) AS '$month32',
		SUM(CASE WHEN DATE(start_time) >= '$month33' AND DATE(start_time) < '$month34' THEN (unit_price * invoiced) ELSE 0 END) AS '$month33',
		SUM(CASE WHEN DATE(start_time) >= '$month34' AND DATE(start_time) < '$month35' THEN (unit_price * invoiced) ELSE 0 END) AS '$month34',
		SUM(CASE WHEN DATE(start_time) >= '$month35' AND DATE(start_time) < '$month36' THEN (unit_price * invoiced) ELSE 0 END) AS '$month35',
		SUM(CASE WHEN DATE(start_time) >= '$month36' AND DATE(start_time) < '$month37' THEN (unit_price * invoiced) ELSE 0 END) AS '$month36'
		FROM ".TB_PREF."booking_orders o, ".TB_PREF."booking_order_details d WHERE o.booking_id = d.booking_id ";

	$sql_y = "SELECT SUM(CASE WHEN DATE(start_time) >= '$month01' AND DATE(start_time) < '$month02' THEN (unit_price * invoiced) ELSE 0 END) AS '$month01',
		SUM(CASE WHEN DATE(start_time) >= '$month02' AND DATE(start_time) < '$month03' THEN (unit_price * invoiced) ELSE 0 END) AS '$month02',
		SUM(CASE WHEN DATE(start_time) >= '$month03' AND DATE(start_time) < '$month04' THEN (unit_price * invoiced) ELSE 0 END) AS '$month03',
		SUM(CASE WHEN DATE(start_time) >= '$month04' AND DATE(start_time) < '$month05' THEN (unit_price * invoiced) ELSE 0 END) AS '$month04',
		SUM(CASE WHEN DATE(start_time) >= '$month05' AND DATE(start_time) < '$month06' THEN (unit_price * invoiced) ELSE 0 END) AS '$month05',
		SUM(CASE WHEN DATE(start_time) >= '$month06' AND DATE(start_time) < '$month07' THEN (unit_price * invoiced) ELSE 0 END) AS '$month06',
		SUM(CASE WHEN DATE(start_time) >= '$month07' AND DATE(start_time) < '$month08' THEN (unit_price * invoiced) ELSE 0 END) AS '$month07',
		SUM(CASE WHEN DATE(start_time) >= '$month08' AND DATE(start_time) < '$month09' THEN (unit_price * invoiced) ELSE 0 END) AS '$month08',
		SUM(CASE WHEN DATE(start_time) >= '$month09' AND DATE(start_time) < '$month10' THEN (unit_price * invoiced) ELSE 0 END) AS '$month09',
		SUM(CASE WHEN DATE(start_time) >= '$month10' AND DATE(start_time) < '$month11' THEN (unit_price * invoiced) ELSE 0 END) AS '$month10',
		SUM(CASE WHEN DATE(start_time) >= '$month11' AND DATE(start_time) < '$month12' THEN (unit_price * invoiced) ELSE 0 END) AS '$month11',
		SUM(CASE WHEN DATE(start_time) >= '$month12' AND DATE(start_time) < '$month13' THEN (unit_price * invoiced) ELSE 0 END) AS '$month12',
		SUM(CASE WHEN DATE(start_time) >= '$month13' AND DATE(start_time) < '$month14' THEN (unit_price * invoiced) ELSE 0 END) AS '$month13',
		SUM(CASE WHEN DATE(start_time) >= '$month14' AND DATE(start_time) < '$month15' THEN (unit_price * invoiced) ELSE 0 END) AS '$month14',
		SUM(CASE WHEN DATE(start_time) >= '$month15' AND DATE(start_time) < '$month16' THEN (unit_price * invoiced) ELSE 0 END) AS '$month15',
		SUM(CASE WHEN DATE(start_time) >= '$month16' AND DATE(start_time) < '$month17' THEN (unit_price * invoiced) ELSE 0 END) AS '$month16',
		SUM(CASE WHEN DATE(start_time) >= '$month17' AND DATE(start_time) < '$month18' THEN (unit_price * invoiced) ELSE 0 END) AS '$month17',
		SUM(CASE WHEN DATE(start_time) >= '$month18' AND DATE(start_time) < '$month19' THEN (unit_price * invoiced) ELSE 0 END) AS '$month18',
		SUM(CASE WHEN DATE(start_time) >= '$month19' AND DATE(start_time) < '$month20' THEN (unit_price * invoiced) ELSE 0 END) AS '$month19',
		SUM(CASE WHEN DATE(start_time) >= '$month20' AND DATE(start_time) < '$month21' THEN (unit_price * invoiced) ELSE 0 END) AS '$month20',
		SUM(CASE WHEN DATE(start_time) >= '$month21' AND DATE(start_time) < '$month22' THEN (unit_price * invoiced) ELSE 0 END) AS '$month21',
		SUM(CASE WHEN DATE(start_time) >= '$month22' AND DATE(start_time) < '$month23' THEN (unit_price * invoiced) ELSE 0 END) AS '$month22',
		SUM(CASE WHEN DATE(start_time) >= '$month23' AND DATE(start_time) < '$month24' THEN (unit_price * invoiced) ELSE 0 END) AS '$month23',
		SUM(CASE WHEN DATE(start_time) >= '$month24' AND DATE(start_time) < '$month25' THEN (unit_price * invoiced) ELSE 0 END) AS '$month24',
		SUM(CASE WHEN DATE(start_time) >= '$month25' AND DATE(start_time) < '$month26' THEN (unit_price * invoiced) ELSE 0 END) AS '$month25',
		SUM(CASE WHEN DATE(start_time) >= '$month26' AND DATE(start_time) < '$month27' THEN (unit_price * invoiced) ELSE 0 END) AS '$month26',
		SUM(CASE WHEN DATE(start_time) >= '$month27' AND DATE(start_time) < '$month28' THEN (unit_price * invoiced) ELSE 0 END) AS '$month27',
		SUM(CASE WHEN DATE(start_time) >= '$month28' AND DATE(start_time) < '$month29' THEN (unit_price * invoiced) ELSE 0 END) AS '$month28',
		SUM(CASE WHEN DATE(start_time) >= '$month29' AND DATE(start_time) < '$month30' THEN (unit_price * invoiced) ELSE 0 END) AS '$month29',
		SUM(CASE WHEN DATE(start_time) >= '$month30' AND DATE(start_time) < '$month31' THEN (unit_price * invoiced) ELSE 0 END) AS '$month30',
		SUM(CASE WHEN DATE(start_time) >= '$month31' AND DATE(start_time) < '$month32' THEN (unit_price * invoiced) ELSE 0 END) AS '$month31',
		SUM(CASE WHEN DATE(start_time) >= '$month32' AND DATE(start_time) < '$month33' THEN (unit_price * invoiced) ELSE 0 END) AS '$month32',
		SUM(CASE WHEN DATE(start_time) >= '$month33' AND DATE(start_time) < '$month34' THEN (unit_price * invoiced) ELSE 0 END) AS '$month33',
		SUM(CASE WHEN DATE(start_time) >= '$month34' AND DATE(start_time) < '$month35' THEN (unit_price * invoiced) ELSE 0 END) AS '$month34',
		SUM(CASE WHEN DATE(start_time) >= '$month35' AND DATE(start_time) < '$month36' THEN (unit_price * invoiced) ELSE 0 END) AS '$month35',
		SUM(CASE WHEN DATE(start_time) >= '$month36' AND DATE(start_time) < '$month37' THEN (unit_price * invoiced) ELSE 0 END) AS '$month36' 
		FROM ".TB_PREF."booking_orders o, ".TB_PREF."booking_order_details d WHERE o.booking_id = d.booking_id ";

	$result_d = db_fetch(db_query($sql_d, 'Transactions could not be calculated'));
	$result_m = db_fetch(db_query($sql_m, 'Transactions could not be calculated'));
	$result_y = db_fetch(db_query($sql_y, 'Transactions could not be calculated'));
	
	$title = _('Revenue Trend');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'book_revenue', $icons);
	else
		display_title($title, 'book_revenue');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='book_revenue_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_d as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr_d = $cols_arr_m = $cols_arr_y = array();
	$val_arr_d = $val_arr_m = $val_arr_y = array();
	$pg_d = $pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);

	$dec = user_price_dec();

	foreach($result_d as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_d[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col)); 
			$val_arr_d[] = round2($val, $dec);
		}
	}
	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col)); 
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}
		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col)); 
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_d); $i++) {
		if($i % 2 == 0)
			$pg_d['x'][] = $cols_arr_d[$i]; 
		else
			$pg_d['x'][] = '';

		$pg_d['y'] = $val_arr_d;
		$pg_d['x-hidden'][] = $cols_arr_d[$i];
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 2 == 0)
			$pg_m['x'][] = $cols_arr_m[$i]; 
		else
			$pg_m['x'][] = '';

		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = $cols_arr_m[$i];
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 2 == 0)
			$pg_y['x'][] = $cols_arr_y[$i]; 
		else
			$pg_y['x'][] = '';

		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = $cols_arr_y[$i];
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('D', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'D')
					echo "<div class='tab-pane fade' id='book_revenue_panel$key'>";
			}
			line_chart($today, false, $pg_d, "book_revenue_d");
			echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
				echo "<div class='tab-pane fade in active' id='book_revenue_panel$key'>";
			}
			line_chart($today, false, $pg_m, "book_revenue_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade' id='book_revenue_panel$key'>";
			}
			line_chart($today, false, $pg_y, "book_revenue_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function booking_stock_top($today, $limit=10, $width="33", $type=0, $panels=array()) {
	
	$sec = 'SA_ITEMSTRANSVIEW';

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);

	$sql = "SELECT SUM(d.unit_price * d.quantity) * (1 - discount_percent) AS total, s.stock_id, s.description, SUM(d.quantity) AS qty, SUM((s.material_cost + s.overhead_cost + s.labour_cost) * d.quantity) AS costs FROM ".TB_PREF."booking_order_details d, ".TB_PREF."stock_master s, ".TB_PREF."booking_trans bt, ".TB_PREF."booking_orders o WHERE o.booking_id = d.booking_id AND d.stock_id = s.stock_id AND bt.booking_id = d.booking_id AND bt.trans_type = ".ST_SALESINVOICE;

	if ($type != 2)
		$sql .= " AND DATE(o.start_time) >= '$begin' ";
	$sql .= "AND o.start_time <= '$today1' GROUP by s.stock_id ORDER BY total DESC, s.stock_id LIMIT $limit";
	$result = db_query($sql, 'could not get stock data');
	
	$title = _('Best Selling Items');

	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, "book_stock_top_$type", $icons);
	else
		display_title($title, "book_stock_top_$type");

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='book_stock_top_".$type."_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Item"), _("Sales"), _("Costs"), _("Quantity"));

		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security($sec);
	$k = 0;
	$i = 0;
	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());
	$dec = user_price_dec();

	while ($myrow = db_fetch($result)) {
		$name = $myrow["description"];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			amount_cell($myrow['total']);
			amount_cell($myrow['costs']);
			qty_cell($myrow['qty']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = round2($myrow['total'], $dec);
		$pg['z'][] = round2($myrow['costs'], $dec);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='book_stock_top_".$type."_panel$key'>";
			}
			double_bar($today, false, $pg, "book_stock_top_$type");

			echo '</div>';
		}

		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='book_stock_top_".$type."_panel$key'>";
			}
			pie_chart($today, false, $pg, "book_stock_top_".$type."_pie");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box
	return $title;
}

function top_common_diseases($today, $limit=10, $width="33", $panels=array()) {

	$sql = "SELECT disease, qty FROM (";

	$sql .= "SELECT 'High Blood Pressure' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE high_blood_pressure = 1 
		UNION ALL 
		SELECT 'High Cholesterol' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE high_cholesterol = 1 
		UNION ALL 
		SELECT 'Vein Trouble' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE vein_trouble = 1 
		UNION ALL 
		SELECT 'Kidney Disease' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE kidney_disease = 1 
		UNION ALL 
		SELECT 'Thyroid Problem' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE thyroid_problem = 1 
		UNION ALL 
		SELECT 'Drug Abuse/Alcoholism' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE drug_abuse_alcoholism = 1 
		UNION ALL 
		SELECT 'Replacement' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE replacement = 1 
		UNION ALL 
		SELECT 'DVT' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE dvt = 1 
		UNION ALL 
		SELECT 'Pulmonary Embolus' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE pulmonary_embolus = 1 
		UNION ALL 
		SELECT 'Tuberculosis' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE tuberculosis = 1 
		UNION ALL 
		SELECT 'Nervous Disorder' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE nervous_disorder = 1 
		UNION ALL 
		SELECT 'Sinus' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE sinus = 1 
		UNION ALL 
		SELECT 'Tonsillitis' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE tonsillitis = 1 
		UNION ALL 
		SELECT 'Bleeding Tendencies' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE bleeding_tendencies = 1 
		UNION ALL 
		SELECT 'Lung Disease' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE lung_disease = 1 
		UNION ALL 
		SELECT 'Asthma' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE asthma = 1 
		UNION ALL 
		SELECT 'Heart Trouble' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE heart_trouble = 1 
		UNION ALL 
		SELECT 'Seasonal Allergies' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE seasonal_allergies = 1 
		UNION ALL 
		SELECT 'Arthitis' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE arthritis = 1 
		UNION ALL 
		SELECT 'Cancer' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE cancer = 1 
		UNION ALL 
		SELECT 'Stroke' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE stroke = 1 
		UNION ALL 
		SELECT 'MH-Diabetes' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE mh_diabetes = 1 
		UNION ALL 
		SELECT 'Pneumonia' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE pneumonia = 1 
		UNION ALL 
		SELECT 'HIV' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE hiv = 1 
		UNION ALL 
		SELECT 'Hepatitis' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE hepatitis = 1 
		UNION ALL 
		SELECT 'Osteoporosis' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE osteoporosis = 1 
		UNION ALL 
		SELECT 'MH-Gastrointestinal' disease, COUNT(patient_id) qty FROM ".TB_PREF."hospital_patient_info WHERE mh_gastrointestinal = 1";
	$sql .= ") AS tb ORDER BY qty DESC LIMIT $limit";

	$result = db_query($sql, 'could not get patient disease data');

	$title = sprintf(_('%s common diseases'), $limit);
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'diseases_top', $icons);
	else
		display_title($title, 'diseases_top');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='diseases_top_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_('Disease'), _('Number of Cases'));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $v) {
		$sum += $v['qty'];
	}

	foreach($result as $myrow) {

		$name = $myrow['disease'];
		if(count($panels) == 0 || in_array('table', $panels)) {
			alt_table_row_color($k);
			label_cell($name);
			qty_cell($myrow['qty']);
		}

		$pg['x'][] = $name; 
		$pg['y'][] = array(
			'meta'=>$name.' - '.round(($myrow['qty']/$sum)*100).'%',
			'value'=>round2($myrow['qty'], $dec)
		);
		$i++;
		if(count($panels) == 0 || in_array('table', $panels))
			end_row();
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade in active' id='diseases_top_panel$key'>";
			}
			single_bar($today, false, $pg, 'diseases_top');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade' id='diseases_top_panel$key'>";
			}
			pie_chart($today, false, $pg, 'diseases_top_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function patient_trend($today, $width="33", $panels=array('line_chart')) {

	$date = begin_month($today);
	$date = add_months($date, -35);
	list($dm, $mm, $ym) = explode_date_to_dmy($date);
	list($dd, $md, $yd) = explode_date_to_dmy($today);

	$month37 = date('Y-m-d',mktime(0,0,0,$mm+36,1,$ym));
	$month36 = date('Y-m-d',mktime(0,0,0,$mm+35,1,$ym));
	$month35 = date('Y-m-d',mktime(0,0,0,$mm+34,1,$ym));
	$month34 = date('Y-m-d',mktime(0,0,0,$mm+33,1,$ym));
	$month33 = date('Y-m-d',mktime(0,0,0,$mm+32,1,$ym));
	$month32 = date('Y-m-d',mktime(0,0,0,$mm+31,1,$ym));
	$month31 = date('Y-m-d',mktime(0,0,0,$mm+30,1,$ym));
	$month30 = date('Y-m-d',mktime(0,0,0,$mm+29,1,$ym));
	$month29 = date('Y-m-d',mktime(0,0,0,$mm+28,1,$ym));
	$month28 = date('Y-m-d',mktime(0,0,0,$mm+27,1,$ym));
	$month27 = date('Y-m-d',mktime(0,0,0,$mm+26,1,$ym));
	$month26 = date('Y-m-d',mktime(0,0,0,$mm+25,1,$ym));
	$month25 = date('Y-m-d',mktime(0,0,0,$mm+24,1,$ym));
	$month24 = date('Y-m-d',mktime(0,0,0,$mm+23,1,$ym));
	$month23 = date('Y-m-d',mktime(0,0,0,$mm+22,1,$ym));
	$month22 = date('Y-m-d',mktime(0,0,0,$mm+21,1,$ym));
	$month21 = date('Y-m-d',mktime(0,0,0,$mm+20,1,$ym));
	$month20 = date('Y-m-d',mktime(0,0,0,$mm+19,1,$ym));
	$month19 = date('Y-m-d',mktime(0,0,0,$mm+18,1,$ym));
	$month18 = date('Y-m-d',mktime(0,0,0,$mm+17,1,$ym));
	$month17 = date('Y-m-d',mktime(0,0,0,$mm+16,1,$ym));
	$month16 = date('Y-m-d',mktime(0,0,0,$mm+15,1,$ym));
	$month15 = date('Y-m-d',mktime(0,0,0,$mm+14,1,$ym));
	$month14 = date('Y-m-d',mktime(0,0,0,$mm+13,1,$ym));
	$month13 = date('Y-m-d',mktime(0,0,0,$mm+12,1,$ym));
	$month12 = date('Y-m-d',mktime(0,0,0,$mm+11,1,$ym));
	$month11 = date('Y-m-d',mktime(0,0,0,$mm+10,1,$ym));
	$month10 = date('Y-m-d',mktime(0,0,0,$mm+9,1,$ym));
	$month09 = date('Y-m-d',mktime(0,0,0,$mm+8,1,$ym));
	$month08 = date('Y-m-d',mktime(0,0,0,$mm+7,1,$ym));
	$month07 = date('Y-m-d',mktime(0,0,0,$mm+6,1,$ym));
	$month06 = date('Y-m-d',mktime(0,0,0,$mm+5,1,$ym));
	$month05 = date('Y-m-d',mktime(0,0,0,$mm+4,1,$ym));
	$month04 = date('Y-m-d',mktime(0,0,0,$mm+3,1,$ym));
	$month03 = date('Y-m-d',mktime(0,0,0,$mm+2,1,$ym));
	$month02 = date('Y-m-d',mktime(0,0,0,$mm+1,1,$ym));
	$month01 = date('Y-m-d',mktime(0,0,0,$mm,1,$ym));
	
	$sql_d = "SELECT";

	for($i=30; $i>0; $i--) {
		${"date$i"} = date('Y-m-d', mktime(0,0,0,$md,$dd-$i,$yd));
		$sql_d .= " COUNT(CASE WHEN entry_date = '".${"date$i"}."' THEN 1 END) AS '${"date$i"}',";
	}
	$sql_d .= " COUNT(CASE WHEN entry_date = '$yd-$md-$dd' THEN 1 END) AS '$yd-$md-$dd' FROM ".TB_PREF."hospital_patient_info ";
		
	$sql_m = "SELECT COUNT(CASE WHEN entry_date >= '$month25' AND entry_date < '$month26' THEN 1 END) AS '$month25',
		COUNT(CASE WHEN entry_date >= '$month26' AND entry_date < '$month27' THEN 1 END) AS '$month26',
		COUNT(CASE WHEN entry_date >= '$month27' AND entry_date < '$month28' THEN 1 END) AS '$month27',
		COUNT(CASE WHEN entry_date >= '$month28' AND entry_date < '$month29' THEN 1 END) AS '$month28',
		COUNT(CASE WHEN entry_date >= '$month29' AND entry_date < '$month30' THEN 1 END) AS '$month29',
		COUNT(CASE WHEN entry_date >= '$month30' AND entry_date < '$month31' THEN 1 END) AS '$month30',
		COUNT(CASE WHEN entry_date >= '$month31' AND entry_date < '$month32' THEN 1 END) AS '$month31',
		COUNT(CASE WHEN entry_date >= '$month32' AND entry_date < '$month33' THEN 1 END) AS '$month32',
		COUNT(CASE WHEN entry_date >= '$month33' AND entry_date < '$month34' THEN 1 END) AS '$month33',
		COUNT(CASE WHEN entry_date >= '$month34' AND entry_date < '$month35' THEN 1 END) AS '$month34',
		COUNT(CASE WHEN entry_date >= '$month35' AND entry_date < '$month36' THEN 1 END) AS '$month35',
		COUNT(CASE WHEN entry_date >= '$month36' AND entry_date < '$month37' THEN 1 END) AS '$month36'
		FROM ".TB_PREF."hospital_patient_info ";

	$sql_y = "SELECT COUNT(CASE WHEN entry_date >= '$month01' AND entry_date < '$month02' THEN 1 END) AS '$month01',
		COUNT(CASE WHEN entry_date >= '$month02' AND entry_date < '$month03' THEN 1 END) AS '$month02',
		COUNT(CASE WHEN entry_date >= '$month03' AND entry_date < '$month04' THEN 1 END) AS '$month03',
		COUNT(CASE WHEN entry_date >= '$month04' AND entry_date < '$month05' THEN 1 END) AS '$month04',
		COUNT(CASE WHEN entry_date >= '$month05' AND entry_date < '$month06' THEN 1 END) AS '$month05',
		COUNT(CASE WHEN entry_date >= '$month06' AND entry_date < '$month07' THEN 1 END) AS '$month06',
		COUNT(CASE WHEN entry_date >= '$month07' AND entry_date < '$month08' THEN 1 END) AS '$month07',
		COUNT(CASE WHEN entry_date >= '$month08' AND entry_date < '$month09' THEN 1 END) AS '$month08',
		COUNT(CASE WHEN entry_date >= '$month09' AND entry_date < '$month10' THEN 1 END) AS '$month09',
		COUNT(CASE WHEN entry_date >= '$month10' AND entry_date < '$month11' THEN 1 END) AS '$month10',
		COUNT(CASE WHEN entry_date >= '$month11' AND entry_date < '$month12' THEN 1 END) AS '$month11',
		COUNT(CASE WHEN entry_date >= '$month12' AND entry_date < '$month13' THEN 1 END) AS '$month12',
		COUNT(CASE WHEN entry_date >= '$month13' AND entry_date < '$month14' THEN 1 END) AS '$month13',
		COUNT(CASE WHEN entry_date >= '$month14' AND entry_date < '$month15' THEN 1 END) AS '$month14',
		COUNT(CASE WHEN entry_date >= '$month15' AND entry_date < '$month16' THEN 1 END) AS '$month15',
		COUNT(CASE WHEN entry_date >= '$month16' AND entry_date < '$month17' THEN 1 END) AS '$month16',
		COUNT(CASE WHEN entry_date >= '$month17' AND entry_date < '$month18' THEN 1 END) AS '$month17',
		COUNT(CASE WHEN entry_date >= '$month18' AND entry_date < '$month19' THEN 1 END) AS '$month18',
		COUNT(CASE WHEN entry_date >= '$month19' AND entry_date < '$month20' THEN 1 END) AS '$month19',
		COUNT(CASE WHEN entry_date >= '$month20' AND entry_date < '$month21' THEN 1 END) AS '$month20',
		COUNT(CASE WHEN entry_date >= '$month21' AND entry_date < '$month22' THEN 1 END) AS '$month21',
		COUNT(CASE WHEN entry_date >= '$month22' AND entry_date < '$month23' THEN 1 END) AS '$month22',
		COUNT(CASE WHEN entry_date >= '$month23' AND entry_date < '$month24' THEN 1 END) AS '$month23',
		COUNT(CASE WHEN entry_date >= '$month24' AND entry_date < '$month25' THEN 1 END) AS '$month24',
		COUNT(CASE WHEN entry_date >= '$month25' AND entry_date < '$month26' THEN 1 END) AS '$month25',
		COUNT(CASE WHEN entry_date >= '$month26' AND entry_date < '$month27' THEN 1 END) AS '$month26',
		COUNT(CASE WHEN entry_date >= '$month27' AND entry_date < '$month28' THEN 1 END) AS '$month27',
		COUNT(CASE WHEN entry_date >= '$month28' AND entry_date < '$month29' THEN 1 END) AS '$month28',
		COUNT(CASE WHEN entry_date >= '$month29' AND entry_date < '$month30' THEN 1 END) AS '$month29',
		COUNT(CASE WHEN entry_date >= '$month30' AND entry_date < '$month31' THEN 1 END) AS '$month30',
		COUNT(CASE WHEN entry_date >= '$month31' AND entry_date < '$month32' THEN 1 END) AS '$month31',
		COUNT(CASE WHEN entry_date >= '$month32' AND entry_date < '$month33' THEN 1 END) AS '$month32',
		COUNT(CASE WHEN entry_date >= '$month33' AND entry_date < '$month34' THEN 1 END) AS '$month33',
		COUNT(CASE WHEN entry_date >= '$month34' AND entry_date < '$month35' THEN 1 END) AS '$month34',
		COUNT(CASE WHEN entry_date >= '$month35' AND entry_date < '$month36' THEN 1 END) AS '$month35',
		COUNT(CASE WHEN entry_date >= '$month36' AND entry_date < '$month37' THEN 1 END) AS '$month36' 
		FROM ".TB_PREF."hospital_patient_info ";

	$result_d = db_fetch(db_query($sql_d, 'Transactions could not be calculated'));
	$result_m = db_fetch(db_query($sql_m, 'Transactions could not be calculated'));
	$result_y = db_fetch(db_query($sql_y, 'Transactions could not be calculated'));
	
	$title = _('Patient Trend');
	$icons = array();
	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'patient_trend', $icons);
	else
		display_title($title, 'patient_trend');

	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='patient_trend_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		
		$th = array();
		foreach($result_d as $col=>$val) {
			if(!is_numeric($col))
				$th[] = date('M', strtotime($col))." ".date('Y', strtotime($col));
		}
	
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	check_page_security('SA_GLANALYTIC');
	$k = 0;

	$cols_arr_d = array();
	$cols_arr_m = array();
	$cols_arr_y = array();
	$hidden_arr_d = array();
	$hidden_arr_m = array();
	$hidden_arr_y = array();
	$val_arr_d = $val_arr_m = $val_arr_y = array();
	$pg_d = $pg_m = $pg_y = array('x'=>array(), 'y'=>array(), 'x-hidden'=>array());
	alt_table_row_color($k);
	$dec = user_price_dec();

	foreach($result_d as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_d[] = _(date('d', strtotime($col)))."\n".date('M', strtotime($col));
			$hidden_arr_d[] = date('Y-m-d', strtotime($col));
			$val_arr_d[] = round2($val, $dec);
		}
	}
	foreach($result_m as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}

		if(!is_numeric($col)) {
			$cols_arr_m[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_m[] = date('Y-m-d', strtotime($col));
			$val_arr_m[] = round2($val, $dec);
		}
	}
	foreach($result_y as $col=>$val) {
		if(count($panels) == 0 || in_array('table', $panels)) {
			
			if(!is_numeric($col))
				amount_cell($val);
		}
		if(!is_numeric($col)) {
			$cols_arr_y[] = _(date('M', strtotime($col)))."\n".date('Y', strtotime($col));
			$hidden_arr_y[] = date('Y-m-d', strtotime($col));
			$val_arr_y[] = round2($val, $dec);
		}
	}
	for($i=0; $i < count($cols_arr_d); $i++) {
		if($i % 5 == 0)
			$pg_d['x'][] = $cols_arr_d[$i]; 
		else
			$pg_d['x'][] = '';

		$pg_d['y'] = $val_arr_d;
		$pg_d['x-hidden'][] = sql2date($hidden_arr_d[$i]);
	}
	for($i=0; $i < count($cols_arr_m); $i++) {
		if($i % 2 == 0)
			$pg_m['x'][] = $cols_arr_m[$i]; 
		else
			$pg_m['x'][] = '';

		$pg_m['y'] = $val_arr_m;
		$pg_m['x-hidden'][] = sql2date($hidden_arr_m[$i]); 
	}
	for($i=0; $i < count($cols_arr_y); $i++) {
		if($i % 5 == 0)
			$pg_y['x'][] = $cols_arr_y[$i]; 
		else
			$pg_y['x'][] = '';

		$pg_y['y'] = $val_arr_y;
		$pg_y['x-hidden'][] = sql2date($hidden_arr_y[$i]);
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		end_row();
		end_table();
	}

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('D', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'D')
					echo "<div class='tab-pane fade' id='patient_trend_panel$key'>";
			}
			line_chart($today, false, $pg_d, "patient_trend_d");
			echo '</div>';
		}
		if(in_array('M', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'M')
				echo "<div class='tab-pane fade in active' id='patient_trend_panel$key'>";
			}
			line_chart($today, false, $pg_m, "patient_trend_m");
			echo '</div>';
		}
		if(in_array('Y', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'Y')
				echo "<div class='tab-pane fade' id='patient_trend_panel$key'>";
			}
			line_chart($today, false, $pg_y, "patient_trend_y");
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>';  // box
	return $title;
}

function age_patients($today, $width="33", $panels=array()) {
	
	$ages = array(10, 15, 25, 30, 40, 50, 60, 70, 80, 90);

	$sql = "SELECT 
	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age < ".$ages[0].") AS 'Under ".$ages[0]."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[0]." AND tbl.age < ".$ages[1].") AS '".$ages[0]."-".($ages[1]-1)."',

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[1]." AND tbl.age < ".$ages[2].") AS '".$ages[1]."-".($ages[2]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[2]." AND tbl.age < ".$ages[3].") AS '".$ages[2]."-".($ages[3]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[3]." AND tbl.age < ".$ages[4].") AS '".$ages[3]."-".($ages[4]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[4]." AND tbl.age < ".$ages[5].") AS '".$ages[4]."-".($ages[5]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[5]." AND tbl.age < ".$ages[6].") AS '".$ages[5]."-".($ages[6]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[6]." AND tbl.age < ".$ages[7].") AS '".$ages[6]."-".($ages[7]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[7]." AND tbl.age < ".$ages[8].") AS '".$ages[7]."-".($ages[8]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[8]." AND tbl.age < ".$ages[9].") AS '".$ages[8]."-".($ages[9]-1)."', 

	(SELECT COUNT(*) FROM (SELECT TIMESTAMPDIFF(YEAR, birth_date, CURDATE()) AS age FROM ".TB_PREF."hospital_patient_info WHERE !inactive) AS tbl WHERE tbl.age >= ".$ages[9].") AS 'Above ".$ages[9]."'" ;

	$result = db_query($sql, 'could not get patient data');

	$title = _('Patients by Ages');
	$icons = array();

	foreach($panels as $val) {
		$icons[] = $val;
	}
	if(count($panels) > 1)
		display_title($title, 'ages_patient', $icons);
	else
		display_title($title, 'ages_patient');
	
	echo "<div class='widget_content tab-content'>";

	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo "<div class='tab-pane fade' id='ages_patient_panel$key'>";
		}
	}
	if(count($panels) == 0 || in_array('table', $panels)) {
		$th = array(_("Age"), _("Employees"));
		start_table(TABLESTYLE_NOBORDER, "width='$width%'");
		table_header($th);
	}
	
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0;
	$i = 0;
	$sum = 0;
	$pg = array('x'=>array(), 'y'=>array());
	$dec = user_price_dec();

	foreach ($result as $row) {
		foreach($row as $val) {
			$sum += $val;
		}
	}

	foreach ($result as $row) {
		foreach($row as $name => $val) {
			if(count($panels) == 0 || in_array('table', $panels)) {
				alt_table_row_color($k);
				label_cell($name);
				amount_cell($val);
			}

			$pg['x'][] = $name; 
			$pg['y'][] = array(
				'meta'=>$name.' - '.round(($val/$sum)*100).'%',
				'value'=>round2($val, $dec)
			);
			$i++;
			if(count($panels) == 0 || in_array('table', $panels))
				end_row();
		}
	}
	if(count($panels) == 0 || in_array('table', $panels))
		end_table();
	if(count($panels) > 0) {
		foreach($panels as $key=>$val) {
			if($val == 'table')
				echo '</div>';
		}
		if(in_array('bar-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'bar-chart')
					echo "<div class='tab-pane fade' id='ages_patient_panel$key'>";
			}
			single_bar($today, false, $pg, 'ages_patient');
			echo '</div>';
		}
		if(in_array('pie-chart', $panels)) {
			foreach($panels as $key=>$val) {
				if($val == 'pie-chart')
					echo "<div class='tab-pane fade in active' id='ages_patient_panel$key'>";
			}
			pie_chart($today, false, $pg, 'ages_patient_pie');
			echo '</div>';
		}
	}
	echo '</div>'; // content
	echo '</div>'; // box

	return $title;
}

function hospital_gl_performance($today, $width="33", $weeks=5) {
	global $SysPrefs;

	$pg = array('x'=>array(), 'y'=>array(), 'z'=>array());

	$begin = date2sql(begin_fiscalyear());
	$today1 = date2sql($today);
	$sep = $SysPrefs->dateseps[user_date_sep()];

	$sql = "SELECT week_name, sales, costs FROM(SELECT DATE_FORMAT(tran_date, '%Y{$sep}%u') AS week_name, SUM(IF(c.ctype = 4, amount * -1, 0)) AS sales, SUM(IF(c.ctype = 6, amount, 0)) AS costs FROM ".TB_PREF."gl_trans, ".TB_PREF."chart_master AS a, ".TB_PREF."chart_types AS t, ".TB_PREF."chart_class AS c WHERE person_type_id = ".PT_CUSTOMER." AND person_id IN (SELECT debtor_no FROM ".TB_PREF."hospital_patient_info) AND (c.ctype = 4 OR c.ctype = 6) AND account = a.account_code AND a.account_type = t.id AND t.class_id = c.cid AND tran_date >= '$begin' AND tran_date <= '$today1' GROUP BY week_name ORDER BY week_name DESC LIMIT 0, $weeks) b GROUP BY week_name ORDER BY week_name ASC";

	$result = db_query($sql, 'Transactions could not be calculated');
	$title = sprintf(_('Last %s weeks Performance'), $weeks);
	check_page_security('SA_GLANALYTIC');
	$dec = user_price_dec();

	while ($myrow = db_fetch($result)) {

		$pg['x'][] = $myrow['week_name']; 
		$pg['y'][] = round2($myrow['sales'], $dec);
		$pg['z'][] = round2($myrow['costs'], $dec);
	}	
	
	double_bar($today, $title, $pg, 'gl_perfomance');
}

function source_graphic($today, $title, $x_axis, $pg, $graphic1, $graphic2=null, $type=2, $img_width = '425px') {
	if (count($pg->y) ==0 || (count($pg->y) == 1 && $pg->y[0] == 0))
		return;
	display_title("$title ($today)", 'graph_box');	
	//$pg->title     = $title . " - " . $today;
	$pg->axis_x    = $x_axis;
	$pg->axis_y    = _('Amount');
	$pg->graphic_1 = $graphic1;
	if ($graphic2 != null)
		$pg->graphic_2 = $graphic2;
	$pg->type      = $type;
	$pg->skin      = 1;
	$pg->built_in  = false;
	$filename = company_path().'/pdf_files/'.random_id().'.png';
	
	echo "<div class='widget_content'>";
	$pg->display($filename);
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	start_row("class='graph_row'");
	echo "<td>";
	echo "<img src='$filename' border='0' width='$img_width' alt='$title'>";
	echo "</td>";
	end_row();
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function double_bar($today, $title=false, $pg, $id='') {
	if($title) {
		display_title($title.' ('.$today.')', 'graph_box');	
		echo "<div class='widget_content'>";
	}

	echo "<div class='ct-chart double-bar-chart' id='$id' style='position:relative'></div>";

	if($title) {
		echo '</div>'; //content
		echo '</div>'; //box
	}
	echo "<script>
	var data_$id = {
		labels: ".json_encode($pg['x']).",
		series: [
			".json_encode($pg['y']).",
			".json_encode($pg['z'])."
		]
	};
	var options_$id = {
		seriesBarDistance: 10,
		plugins: [Chartist.plugins.tooltip({appendToBody: true,currency :' '})],
		axisY: {
			labelInterpolationFnc: function(value) {
				if(value > 1000000000000 || value < -1000000000000)
					return value / 1000000000000 + 't';
				if(value > 1000000000 || value < -1000000000)
					return value / 1000000000 + 'b';
				if(value > 1000000 || value < -1000000)
					return value / 1000000 + 'm';
				if(value >= 1000 || value <= -1000)
					return value / 1000 + 'k'
				return value;
			}
		}
	};
	var responsiveOptions_$id = [
		['screen and (max-width: 640px)', {
			seriesBarDistance: 5,
			axisX: {
				labelInterpolationFnc: function (value) {return value[0];}
			}
		}]
	];
	new Chartist.Bar('#$id', data_$id, options_$id, responsiveOptions_$id);";
	echo "</script>";
}

function single_bar($today, $title=false, $pg, $id='') {
	if($title) {
		display_title("$title($today)", "graph_box");
		echo "<div class='widget_content'>";
	}
	echo "<div class='ct-chart' id='$id' style='position:relative'></div>";

	if($title) {
		echo '</div>'; //content
		echo '</div>'; //box
	}

	echo "<script>
	var data_$id = {
		labels: ".json_encode($pg['x']).",
		series: ".json_encode($pg['y'])."
	};
	var option_$id = {
		distributeSeries: true,
		axisY: {
			labelInterpolationFnc: function(value) {
				if(value > 1000000000000 || value < -1000000000000)
					return value / 1000000000000 + 't';
				if(value > 1000000000 || value < -1000000000)
					return value / 1000000000 + 'b';
				if(value > 1000000 || value < -1000000)
					return value / 1000000 + 'm';
				if(value >= 1000 || value <= -1000)
					return value / 1000 + 'k'
				return value;
			}
		},
		plugins: [Chartist.plugins.tooltip({appendToBody: true,currency :' '})]
	};

	new Chartist.Bar('#$id', data_$id, option_$id);
	</script>";
}

function line_chart($today, $title=false, $pg, $id='', $point='true') {
	if($title) {
		display_title("$title($today)", "graph_box");
		echo "<div class='widget_content'>";
	}
	echo "<div class='ct-chart ct-line-chart' id='$id' style='position:relative'></div>";

	if($title) {
		echo '</div>'; //content
		echo '</div>'; //box
	}

	echo "<script>
	var data_$id = {
		hidden_labels: ".json_encode($pg['x-hidden']).",
		labels: ".json_encode($pg['x']).",
		series: [".json_encode($pg['y'])."]
	};

	var option_$id = {
		distributeSeries: true,
		showArea: true,
		
		axisY: {
			labelInterpolationFnc: function(value) {
				if(value > 1000000000000 || value < -1000000000000)
					return value / 1000000000000 + 't';
				if(value > 1000000000 || value < -1000000000)
					return value / 1000000000 + 'b';
				if(value > 1000000 || value < -1000000)
					return value / 1000000 + 'm';
				if(value >= 1000 || value <= -1000)
					return value / 1000 + 'k'
				return value;
			}
		},
		plugins: [
		  Chartist.plugins.ctColumnTooltips({currency: ' '})
		]
	};

	new Chartist.Line('#$id', data_$id, option_$id);
	</script>";
}

function pie_chart($today, $title=false, $pg=null, $id='') {
	if($title) {
		display_title("$title($today)", "graph_box");
		echo "<div class='widget_content'>";
	}
	
	echo "<div class='ct-chart ct-pie-chart' id='$id' style='position:relative'></div>";
	echo "<ul class='ct-legend' id='$id"."_legend"."'></ul>";

	if($title) {
		echo '</div>'; //content
		echo '</div>'; //box
	}
	echo "<script>";

	echo "var data_series = ".json_encode($pg['y']).";

	$.each(data_series, function(i, arr) {
		if(arr.value < 0)
			data_series[i].value = -data_series[i].value;
	});

	var data_arr = [];
	$.each(data_series, function(i, arr) {
		data_arr[i] = arr.value;
	});

	var data_$id = {series: data_series, series2: data_arr};
	
	var class_arr_$id = ".json_encode($pg['x']).";
	
	var options_$id = {
		labelInterpolationFnc: function(value) {
			function sum(a, b) { return a + b };
			var percent = Math.round(value / data_$id.series2.reduce(sum) * 100);
			
			if(percent > 5)
				return percent + '%';
		},
		plugins: [Chartist.plugins.tooltip({
			metaIsHTML: true,
			appendToBody: true,
			currency :' '
		})]
	};

	var responsiveOptions_$id = [
		['', {
			labelOffset: 50,
			chartPadding: 20,
			labelDirection: 'explode'
		}],
		//['screen and (max-width: 480px)', {
			//labelOffset: 20
		//}]
	];

	new Chartist.Pie('#$id', data_$id, options_$id, responsiveOptions_$id);

	var legend_$id = $('#$id' + '_legend');
	$.each(class_arr_$id, function(i, val) {
		var listItem = $('<li />')
		.addClass('ct-series-' + i)
		.html(class_arr_".$id."[i])
		.appendTo(legend_$id);
	});";
	echo "</script>";
}

function customer_trans($today) {

	$today = date2sql($today);

	$sql = "SELECT trans.trans_no, trans.reference,	trans.tran_date, trans.due_date, debtor.debtor_no, debtor.name, branch.br_name, debtor.curr_code, (trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount) AS total, (trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount - trans.alloc) AS remainder, DATEDIFF('$today', trans.due_date) AS days FROM ".TB_PREF."debtor_trans as trans, ".TB_PREF."debtors_master as debtor, ".TB_PREF."cust_branch as branch WHERE debtor.debtor_no = trans.debtor_no AND trans.branch_code = branch.branch_code AND trans.type = ".ST_SALESINVOICE." AND (trans.ov_amount + trans.ov_gst + trans.ov_freight + trans.ov_freight_tax + trans.ov_discount - trans.alloc) > ".FLOAT_COMP_DELTA." AND DATEDIFF('$today', trans.due_date) > 0 ORDER BY days DESC";

	$result = db_query($sql, 'error getting customer transaction');
	$title = db_num_rows($result) . _(' overdue Sales Invoices');
	display_title($title, 'cus_trans');
	$th = array(_('#'), _('Ref.'), _('Date'), _('Due Date'), _('Customer'), _('Branch'), _('Currency'), _('Total'), _('Remainder'),	_('Days'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['reference'])) {
			alt_table_row_color($k);
			label_cell(get_trans_view_str(ST_SALESINVOICE, $myrow['trans_no']), "align='center'");
			label_cell($myrow['reference']);
			label_cell(sql2date($myrow['tran_date']), "align='center'");
			label_cell(sql2date($myrow['due_date']), "align='center'");
			$name = $myrow['debtor_no']."&nbsp;".$myrow['name'];
			label_cell($name);
			label_cell($myrow['br_name']);
			label_cell($myrow['curr_code'], "align='center'");
			amount_cell($myrow['total']);
			amount_cell($myrow['remainder']);
			label_cell($myrow['days'], "align='right'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function calculate_next_invoice($myrow) {

	if ($myrow['last_sent'] == '0000-00-00')
		$next = sql2date($myrow['begin']);
	else
		$next = sql2date($myrow['last_sent']);
	$next = add_months($next, $myrow['monthly']);
	$next = add_days($next, $myrow['days']);
	return add_days($next,-1);
}

function customer_recurrent_invoices($today) {

	$result = get_recurrent_invoices($today);
	$title = _('Overdue Recurrent Invoices');
	display_title($title, 'cus_rec_inv');
	$th = array(_('Description'), _('Template No'),_('Customer'),_('Branch').'/'._('Group'),_('Next invoice'));
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	while ($myrow = db_fetch($result)) {
		if (!$myrow['overdue'])
			continue;
		alt_table_row_color($k);

		label_cell($myrow['description']);
		label_cell(get_customer_trans_view_str(ST_SALESORDER, $myrow['order_no']));
		if ($myrow['debtor_no'] == 0) {
			label_cell("");
			label_cell(get_sales_group_name($myrow['group_no']));
		}
		else {
			label_cell(get_customer_name($myrow['debtor_no']));
			label_cell(get_branch_name($myrow['group_no']));
		}
		label_cell(calculate_next_invoice($myrow),  "align='center'");
		end_row();
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function supplier_trans($today) {

	$today = date2sql($today);

	$sql = "SELECT trans.trans_no, trans.reference, trans.tran_date, trans.due_date, s.supplier_id, s.supp_name, s.curr_code, (trans.ov_amount + trans.ov_gst + trans.ov_discount) AS total, (trans.ov_amount + trans.ov_gst + trans.ov_discount - trans.alloc) AS remainder, DATEDIFF('$today', trans.due_date) AS days FROM ".TB_PREF."supp_trans as trans, ".TB_PREF."suppliers as s WHERE s.supplier_id = trans.supplier_id AND trans.type = ".ST_SUPPINVOICE." AND (ABS(trans.ov_amount + trans.ov_gst + trans.ov_discount) - trans.alloc) > ".FLOAT_COMP_DELTA." AND DATEDIFF('$today', trans.due_date) > 0 ORDER BY days DESC";

	$result = db_query($sql, 'error getting supplier transaction');
	$title = db_num_rows($result) . _(' overdue Purchase Invoices');

	display_title($title, 'sup_trans');
	$th = array(_('#'), _('Ref.'), _('Date'), _('Due Date'), _('Supplier'), _('Currency'), _('Total'), _('Remainder'),	_('Days'));
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['reference'])) {
			alt_table_row_color($k);
			label_cell(get_trans_view_str(ST_SUPPINVOICE, $myrow['trans_no']), "align='center'");
			label_cell($myrow['reference']);
			label_cell(sql2date($myrow['tran_date']), "align='center'");
			label_cell(sql2date($myrow['due_date']), "align='center'");
			$name = $myrow['supplier_id']."&nbsp;".$myrow['supp_name'];
			label_cell($name);
			label_cell($myrow['curr_code']);
			amount_cell($myrow['total']);
			amount_cell($myrow['remainder']);
			label_cell($myrow['days'], "align='right'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; // content
	echo '</div>'; // box
}

function bank_balance($today, $width) {

	$today = date2sql($today);

	$sql = "SELECT bank_act, bank_account_name, bank_curr_code, SUM(amount) balance FROM ".TB_PREF."bank_trans bt INNER JOIN ".TB_PREF."bank_accounts ba ON bt.bank_act = ba.id WHERE trans_date <= '$today' AND inactive <> 1 GROUP BY bank_act, bank_account_name ORDER BY bank_account_name";

	$result = db_query($sql, 'error getting bank transaction');
	$title = _('Bank Account Balances');
	
	display_title($title, 'bank_balance');
	$th = array(_('Account'), _('Currency'), _('Balance'));
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width='$width%'");
	table_header($th);
	$k = 0;
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		label_cell(viewer_link($myrow['bank_account_name'], 'gl/inquiry/bank_inquiry.php?bank_account='.$myrow['bank_act']));
		label_cell($myrow['bank_curr_code']);
		amount_cell($myrow['balance']);
		end_row();
	}
	end_table();
	echo '</div>'; // content
	echo '</div>'; // box
}

function upcoming_exp_docs($today) {
	global $path_to_root;

	$today = date2sql($today);
	$alert = get_company_pref('tf_default_alert');

	$sql = "SELECT * FROM (SELECT id, issue_date, expiry_date, amount, curr_code, closed, customer_id FROM ".TB_PREF."in_tfd_pdc UNION ALL SELECT id, issue_date, expiry_date, amount, curr_code, closed, NULL FROM ".TB_PREF."out_tfd_pdc) AS docs WHERE !closed AND DATEDIFF(docs.expiry_date, '$today') <= $alert ORDER BY expiry_date ASC";

	$result = db_query($sql, 'error getting document data');
	$title = db_num_rows($result).'&nbsp;'. _('Expire in '.$alert._(' days'));
	display_title($title, 'tf_nearly_expired');
	$th = array(_('#'), _('Type'), _('Issued Date'), _('Expiry Date'), _('Amount'), _('Currency'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			alt_table_row_color($k);

			if(empty($myrow['customer_id'])) {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/outgoing_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewPayment=Yes&doc_id=".$myrow['id']."'>"._('Payment')."</a>");
			}
			else {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/incoming_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewDeposit=Yes&doc_id=".$myrow['id']."'>"._('Deposit')."</a>");
			}
			label_cell(sql2date($myrow['issue_date']), "align='center'");
			label_cell(sql2date($myrow['expiry_date']), "align='center'");
			amount_cell($myrow['amount']);
			label_cell('&nbsp;&nbsp;'.$myrow['curr_code'], "align='left'");
			
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function today_exp_docs($today) {
	global $path_to_root;

	$today = date2sql($today);

	$sql = "SELECT * FROM (SELECT id, issue_date, expiry_date, amount, curr_code, closed, customer_id FROM ".TB_PREF."in_tfd_pdc UNION ALL SELECT id, issue_date, expiry_date, amount, curr_code, closed, NULL FROM ".TB_PREF."out_tfd_pdc) AS docs WHERE !closed AND DATEDIFF('$today', docs.expiry_date) = 0 ORDER BY issue_date ASC";

	$result = db_query($sql, 'error getting document data');
	$title = db_num_rows($result).'&nbsp;'._('Expire today');
	display_title($title, 'tf_today_expired');
	$th = array(_('#'), _('Type'), _('Issued Date'), _('Expiry Date'), _('Amount'), _('Currency'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			alt_table_row_color($k);
			if(empty($myrow['customer_id'])) {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/outgoing_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewPayment=Yes&doc_id=".$myrow['id']."'>"._('Payment')."</a>");
			}
			else {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/incoming_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewDeposit=Yes&doc_id=".$myrow['id']."'>"._('Deposit')."</a>");
			}
			label_cell(sql2date($myrow['issue_date']), "align='center'");
			label_cell(sql2date($myrow['expiry_date']), "align='center'");
			amount_cell($myrow['amount']);
			label_cell('&nbsp;&nbsp;'.$myrow['curr_code'], "align='left'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function expired_docs($today) {
	global $path_to_root;

	$today = date2sql($today);

	$sql = "SELECT * FROM (SELECT id, issue_date, expiry_date, amount, curr_code, closed, customer_id FROM ".TB_PREF."in_tfd_pdc UNION ALL SELECT id, issue_date, expiry_date, amount, curr_code, closed, NULL FROM ".TB_PREF."out_tfd_pdc) AS docs WHERE !closed AND DATEDIFF('$today', docs.expiry_date) > 0 ORDER BY expiry_date ASC";

	$result = db_query($sql, 'error getting document data');
	$title = db_num_rows($result).'&nbsp;'._('Expired');
	display_title($title, 'tf_docs_expired');
	$th = array(_('#'), _('Type'), _('Issued Date'), _('Expiry Date'), _('Amount'), _('Currency'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			alt_table_row_color($k);
			if(empty($myrow['customer_id'])) {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/outgoing_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewPayment=Yes&doc_id=".$myrow['id']."'>"._('Payment')."</a>");
			}
			else {
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/incoming_tfd.php?trans_no=".$myrow['id']."'>".$myrow['id']."</a>");
				label_cell("<a href='$path_to_root/modules/trade_finance/manage/gl_bank.php?NewDeposit=Yes&doc_id=".$myrow['id']."'>"._('Deposit')."</a>");
			}
			label_cell(sql2date($myrow['issue_date']), "align='center'");
			label_cell(sql2date($myrow['expiry_date']), "align='center'");
			amount_cell($myrow['amount']);
			label_cell('&nbsp;&nbsp;'.$myrow['curr_code'], "align='left'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

//---------------------------------------------------------------------------------------------

function today_events($today) {
	global $path_to_root;

	$today = date2sql($today);

	$sql = "SELECT e.*, c.category_name as category FROM ".TB_PREF."events e, ".TB_PREF."event_categories c WHERE e.start_date = '".$today."' AND !e.inactive AND e.category_id = c.category_id ORDER BY hour ASC, minute ASC";

	$result = db_query($sql, 'error getting event data');
	$title = db_num_rows($result).'&nbsp;'._('Event today');
	display_title($title, 'grm_today_event');
	$th = array(_('#'), _('Category'), _('Title'), _('Start Time'), _('SMS'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			$alert = $myrow['sms_alert'] == 0 ? _('No') : _('Yes');
			alt_table_row_color($k);
			label_cell("<a href='$path_to_root/modules/grm/manage/events.php?EventId=".$myrow['id']."'>".$myrow['id']."</a>");
			label_cell($myrow['category']);
			label_cell($myrow['title']);
			label_cell($myrow['hour'].':'.$myrow['minute'], "align='center'");
			label_cell($alert, "align='center'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function upcoming_events($today) {
	global $path_to_root;

	$today = date2sql($today);

	$sql = "SELECT e.*, c.category_name as category FROM ".TB_PREF."events e, ".TB_PREF."event_categories c WHERE e.start_date > '".$today."' AND !e.inactive AND e.category_id = c.category_id ORDER BY start_date ASC, hour ASC, minute ASC";

	$result = db_query($sql, 'error getting event data');
	$title = db_num_rows($result).'&nbsp;'. _('Upcoming event');
	display_title($title, 'grm_upcoming_event');
	$th = array(_('#'), _('Category'), _('Title'), _('Start Date'), _('Start Time'), _('SMS'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			$alert = $myrow['sms_alert'] == 0 ? _('No') : _('Yes');
			alt_table_row_color($k);
			label_cell("<a href='$path_to_root/modules/grm/manage/events.php?EventId=".$myrow['id']."'>".$myrow['id']."</a>");
			label_cell($myrow['category']);
			label_cell($myrow['title']);
			label_cell(sql2date($myrow['start_date']), "align='center'");
			label_cell($myrow['hour'].':'.$myrow['minute'], "align='center'");
			label_cell($alert, "align='center'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function overdue_events($today) {
	global $path_to_root;

	$today = date2sql($today);

	$sql = "SELECT e.*, c.category_name as category FROM ".TB_PREF."events e, ".TB_PREF."event_categories c WHERE e.start_date < '".$today."' AND !e.inactive AND e.category_id = c.category_id ORDER BY start_date ASC, hour ASC, minute ASC";

	$result = db_query($sql, 'error getting event data');
	$title = db_num_rows($result).'&nbsp;'. _('Overdue event');
	display_title($title, 'grm_overdue_event');
	$th = array(_('#'), _('Category'), _('Title'), _('Start Date'), _('Start Time'), _('SMS'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			$alert = $myrow['sms_alert'] == 0 ? _('No') : _('Yes');
			alt_table_row_color($k);
			label_cell("<a href='$path_to_root/modules/grm/manage/events.php?EventId=".$myrow['id']."'>".$myrow['id']."</a>");
			label_cell($myrow['category']);
			label_cell($myrow['title']);
			label_cell(sql2date($myrow['start_date']), "align='center'");
			label_cell($myrow['hour'].':'.$myrow['minute'], "align='center'");
			label_cell($alert, "align='center'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

function queued_sms($today) {
	global $path_to_root;
	include_once($path_to_root.'/modules/grm/includes/grm_ui.inc');
	include_once($path_to_root.'/includes/ui/contacts_view.inc');

	$today = date2sql($today);

	$sql = "SELECT * FROM ".TB_PREF."sms_record WHERE !sent ORDER BY queued_time";

	$result = db_query($sql, 'error getting sms data');
	$title = db_num_rows($result).'&nbsp;'. _("SMS to be sent");
	display_title($title, 'grm_sms');
	$th = array(_('#'), _('Person Type'), _('Person Name'), _('Phone Number'));
		
	echo "<div class='widget_content'>";
	start_table(TABLESTYLE_NOBORDER, "width=100%");
	table_header($th);
	$k = 0;
	for($i = 0; $i < 100; $i++) {

		$myrow = db_fetch($result);
		if(!empty($myrow['id'])) {
			$person = get_crm_person($myrow['person_id']);
			$type = '';
			if($myrow['person_type'] == CT_CUSTOMER) $type = _('Customer');
			elseif($myrow['person_type'] == CT_SUPPLIER) $type = _('Supplier');
			elseif($myrow['person_type'] == CT_EMPLOYEE) $type = _('Employee');
			else $type = _('Miscellaneous');
			alt_table_row_color($k);
			label_cell("<a href='$path_to_root/modules/grm/manage/process_queue.php?'>".$myrow['id']."</a>");
			label_cell($type);
			label_cell($person['name']);
			label_cell($myrow['to_number'], "align='center'");
			end_row();
		}
	}
	end_table();
	echo '</div>'; //content
	echo '</div>'; //box
}

//---------------------------------------------------------------------------------------------

function get_stock_total_value($date) {
	$sql = "SELECT SUM(tbl.total) FROM (SELECT SUM(move.qty) * item.material_cost AS total FROM ".TB_PREF."stock_master item, ".TB_PREF."stock_moves move WHERE item.stock_id = move.stock_id AND mb_flag <> 'D' AND mb_flag <> 'F' AND move.tran_date <= '".date2sql($date)."' GROUP BY item.stock_id) AS tbl";
	$result = db_fetch(db_query($sql, 'could not get stock data'));

	return $result[0];
}

function get_average_stock_value($from, $to) {
	$begin = new DateTime(date2sql($from));
	$end = new DateTime(date2sql($to));

	$interval = DateInterval::createFromDateString('1 day');
	$period = new DatePeriod($begin, $interval, $end);
	
	$days_no = 0;
	$total = 0;
	foreach ($period as $dt) {
		$date = sql2date($dt->format('Y-m-d'));
		$total += get_stock_total_value($date);
		$days_no++;
	}

	return $total / $days_no;
}
<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
$path_to_root = '../..';

$page_security = 'SA_SMSLOG';
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/sms.inc");
add_access_extensions();
page("SMS Log");

simple_page_mode(true);

function status_($row){
	return ($row["status"] == 0 ? 'Not Send' : ($row['status'] == 1 ? 'Sent' : '' ));
}

function FA_date_Format($row){
	return sql2date(date('Y-m-d', strtotime($row['created_at']))) .date(' H i:s', strtotime($row['created_at']));
}
function gateway_name($row)
{
	return ucfirst($row['gateway']);
}
function status_text($row){
	$loaded =smsautoloader($row['gateway']);
	if($loaded){
		$sms = new $row['gateway']();
		$status_text =$sms->get_status_names($row['status']);
		if($status_text)
			return $status_text;
		else
			return _("None");
	}else{
		return $row['status'];
	}
    
}
if(isset($_POST['Confirmclearlog'])){
	$sql ='TRUNCATE TABLE '.TB_PREF.'kv_sms_logs';
	db_query($sql);
	display_notification(_('Sms logs has been cleared successfully'));
}
function log_clear()
{
	if (!isset($_POST['clearlogs'])){
		submit_center('clearlogs', _("Clear Logs"), true, '', '','cancel');
	}else{
		display_warning(_("Are you sure you want to delete all sms logs?"), 0, 1);
	   			br();
		submit_center_first('Confirmclearlog', _("Proceed"), '', false,'default');
		submit_center_last('Cancelclearlog', _("Cancel"), '', false,'cancel');
	}
}
start_form();


start_table(TABLESTYLE2);
	start_row();
		text_cells(_("Mobile No"), 'phone_no', null);
		submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), 'default');
	end_row();
end_table(1);




if (get_post('RefreshInquiry') || list_updated('filterType'))
{
	$filter = ['phone_no' => '%'.$_POST['phone_no'].'%'];
	$Ajax->activate('_page_body');
} else {
	$filter = null;
}
//if(list_updated('mobile'))
$sql =SMS_GetDataJoin("kv_sms_logs As log", 
		[array('join' => 'LEFT', 'table_name' => 'users AS usr', 'conditions' => 'usr.id = log.user_id')], 

		['log.gateway','log.phone_no', 'log.sms', 'log.status', 'log.created_at', 'usr.real_name'], $filter, array('created_at'=>'DESC'), null, false, true);

$cols = array(
	_("Gateway") => array('name'=>'status', 'fun' => 'gateway_name'),
	_("To Phone"),
	_("SMS") => array('name'=>'subject'), 
	_("Status") => array('name'=>'status', 'fun' => 'status_text'),
	_("Date") => array('name'=>'created_at', 'fun' => 'FA_date_Format'), 
	_("Sent by User") => array('name' => 'real_name'),
	// _("created_at") => array(sql2date('date_time'))
	);

		$table =& new_db_pager('sms_log', $sql, $cols);
		$table->width = "80%";
		display_db_pager($table);
br(2);
log_clear();
end_form();
end_page();
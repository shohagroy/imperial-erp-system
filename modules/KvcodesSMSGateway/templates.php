<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
$path_to_root = '../..';

$page_security = 'SA_SMSTEMP';
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/sms.inc");
add_access_extensions();
global $systypes_array,$sms_types;
page("SMS templates");

simple_page_mode(true);

// $result =SMS_GetDataFilter('kv_sms_templates',array('count(type) AS Count','type'),null,null,array('type'));
$sms_types =array(
	ST_SALESORDER=>$systypes_array[ST_SALESORDER],
	ST_SALESQUOTE=>$systypes_array[ST_SALESQUOTE],
	ST_CUSTDELIVERY=>$systypes_array[ST_CUSTDELIVERY],
	ST_SALESINVOICE=>$systypes_array[ST_SALESINVOICE],
	ST_CUSTPAYMENT=>$systypes_array[ST_CUSTPAYMENT],
	'overdue'=>_("Over Due"),
);
function can_process(){
	$error =0;
	if($_POST['type'] ==ALL_NUMERIC){
		display_error(_('Type must be selected'));
		set_focus('type');
		$error =1;
	}
	if(strlen(trim($_POST['subject'])) ==0){
		display_error(_('Subject Cannot be empty'));
		set_focus('subject');
		$error =1;
	}
	if(strlen(trim($_POST['template'])) ==0){
		display_error(_('Template Cannot be empty'));
		set_focus('template');
		$error =1;
	}
	if($error)
		return false;
	else 
		return true;
}
if (($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') && can_process()){
	if(check_value('active') && SMS_GetSingleValue('kv_sms_templates','type',array('type'=>$_POST['type']))){
		SMS_Update('kv_sms_templates',array('type'=>$_POST['type']),array('active'=>0));
	}
	if ($selected_id != -1) {
		SMS_Update('kv_sms_templates', array('id' => $selected_id) , array('subject' => $_POST['subject'], 'template' => $_POST['template'], 'active' => check_value('active'),'type'=>$_POST['type']));
	} else {
		SMS_Insert('kv_sms_templates', array('subject' => $_POST['subject'], 'template' => $_POST['template'], 'active' => check_value('active') ,'type'=>$_POST['type']) );
	}
	$Mode = 'RESET';
}

//-------------------------------------------------------------------------------------------------
if ($Mode == 'RESET'){
 	$selected_id = -1;
	$sav = get_post('show_inactive', null);
	unset($_POST);	// clean all input fields
	$_POST['show_inactive'] = $sav;
	$Ajax->activate('FullDiv');
}
function get_type_name($row)
{
	global $sms_types;
	if(isset($sms_types[$row['type']])){
		return $sms_types[$row['type']];
	}
	return '';
}

function status_($row){
	return ($row["active"] == 1 ?'<span style="color:green;">'._('Active')."</span>" : '<span style="color:red;">'._('Inactive')."</span>" );
}
function edit_link($row){
  	return button("Edit".$row["id"],_("Edit"), '', ICON_EDIT);
}

function del_link($row) {	
	return button("Delete".$row["id"],_("Delete"), '', ICON_DELETE);
	
}
function no_of_sms($row)
{	
	$active_smsgateway =SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'active_smsgateway'));
	if(!$active_smsgateway)
		return _("none");
	$loaded =smsautoloader($active_smsgateway);
	if($loaded){
		$sms = new $active_smsgateway();
		$char_count =$sms->get_sms_char();
		$sms_count =strlen($row['template'])/$char_count;
		return ceil($sms_count);
	}else{
		return _("none");
	}
	
}
if(list_updated('type')){
	$Ajax->activate('TemplatesAddEdit');
}
start_form();
div_start('FullDiv');

// start_table(TABLESTYLE2);
// 	array_selector_row(_("Send SMS to"), 'type',null, $sms_types,['spec_option'=>_("Select Template"),'spec_id'=>ALL_TEXT,'select_submit'=>true]);
// end_table();


$sql =SMS_GetAll("kv_sms_templates",null,  array('type'=>'ASC'),  null, null, false, true);

$cols = array(
	
	_("ID") => 'skip', 
	_("Type") => array('name'=>'type','fun'=>'get_type_name'), 
	_("Subject") => array('name'=>'subject'), 
	_("Template") => array('name'=>'template'),
	_("Status") => array('name'=>'active', 'fun' => 'status_'),
	_("No of SMS") => array('name'=>'template', 'fun' => 'no_of_sms')
	,
	// _("created_at") => array(sql2date('date_time'))
	);
$cols[] = array('insert'=>true, 'fun'=>'edit_link');
		$cols[] = array('insert'=>true, 'fun'=>'del_link');
		$table =& new_db_pager('sms_templates', $sql, $cols);
		$table->width = "80%";
		display_db_pager($table);


br(2);

//-------------------------------------------------------------------------------------------------
// if($selected_id !=-1){
div_start('TemplatesAddEdit');
display_note("<pre>
	Use <b>{reference}</b> to get document reference,
	Use <b>{amount}</b> to get document amount
	<b>Only for Over Due</b>
	Use <b>{customer_name}</b> to get customer name
	Use <b>{due_amount}</b> to get over due amount
	Use <b>{date}</b> to get upcoming over due date</pre>"
);
start_table(TABLESTYLE2);

if ($selected_id != -1) {
  	if ($Mode == 'Edit') {
		//editing an existing User
		$myrow = SMS_GetRow('kv_sms_templates', ['id' => $selected_id]);

		$_POST['id'] = $myrow["id"];
		$_POST['subject'] = $myrow["subject"];
		$_POST['template'] = $myrow["template"];
		$_POST['active'] = $myrow["active"];
		$_POST['type'] = $myrow["type"];
	}
	hidden('selected_id', $selected_id);
} 
array_selector_row(_("Select Type"),'type', null, $sms_types,array('spec_option'=>_("Select Type"),'spec_id'=>ALL_NUMERIC ,'select_submit'=>true));
text_row_ex(_("Subject").":", 'subject', 50, 60);
textarea_row(_("Template").":", 'template',  null, 50, 5);
check_row(_("Active"), 'active', null);
end_table();
br();
submit_add_or_update_center($selected_id == -1, '', 'both');
br();
div_end();
// }
div_end();

end_form();
end_page();
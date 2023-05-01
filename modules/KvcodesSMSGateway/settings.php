<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
$path_to_root = '../..';

$page_security = 'SA_SMSSET';

include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/sms.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/kvcodes.inc");
add_access_extensions();
page("SMS Settings");

/*---------------------------------------------------------------------*/
global $all_instance;
$all_instance =array();

$active_smsgateway =SMS_GetSingleValue('sys_prefs','value',array('name'=>'active_smsgateway'));

$send_sms_to =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_to'));

$send_sms_sales_quotation =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_sales_quotation'));
$send_sms_sales_order =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_sales_order'));
$sms_testing_mode =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_testing_mode'));
$send_sms_sales_invoice =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_sales_invoice'));
$send_sms_delivery =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_delivery'));
$send_sms_customer_payment =SMS_GetSingleValue('sys_prefs','value',array('name'=>'send_sms_customer_payment'));

$sms_2way_auth =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_2way_auth'));

if(!isset($_POST['sms_2way_expiry'])){
	$_POST['sms_2way_expiry'] =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_2way_expiry'));
}

if(!isset($_POST['sms_company_phone'])){
	$_POST['sms_company_phone'] =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_company_phone'));
}

if(!isset($_POST['gateway'])){
	$_POST['gateway'] =$active_smsgateway;
}

if(!isset($_POST['send_sms_to'])){
	$_POST['send_sms_to'] =$send_sms_to;
}

if(!isset($_POST['sms_next_over_due_days'])){
	$_POST['sms_next_over_due_days'] =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_next_over_due_days'));
}

$sms_to_types =array(
	'customer_contact'=>_("Customer Contact"),
	'report_contact'=>_("Report Contact"),

);

function gateway_details($gateway){
	global $all_instance;
	$loaded =smsautoloader($gateway);
	if($loaded){
		$sms = new $gateway();
		echo $sms->form();
	}else{
		display_error(_('Cannot get the getway class file'));
	}
	
}


/*---------------------------------------------------------------------*/
function can_process() {
	$can =1;
	if($_POST['gateway'] ==ALL_TEXT){
		display_error(_('Gateway must be selected'));
		return false ;
	}

	$loaded =smsautoloader($_POST['gateway']);
	if($loaded){
		$sms = new $_POST['gateway']();
		$can = $sms->update();
	}else{
		$can =0;
		display_error(_('Cannot get the getway class file'));
	}
	if($can)
		return true;	
	else
		return false;
}
if (isset($_POST['addupdate'])&& can_process()) {	

	if(check_value('active_smsgateway')){
		SMS_Update('sys_prefs',array('name'=>'active_smsgateway'),array('value'=>$_POST['gateway']));

		$active_smsgateway =$_POST['gateway'];
	}
	elseif($active_smsgateway ==$_POST['gateway']){
		SMS_Update('sys_prefs',array('name'=>'active_smsgateway'),array('value'=>''));
		$active_smsgateway ="";
	}

	SMS_Update('sys_prefs',array('name'=>'send_sms_to'),array('value'=>$_POST['send_sms_to']));
	$send_sms_to =$_POST['send_sms_to'];


	SMS_Update('sys_prefs',array('name'=>'send_sms_sales_quotation'),array('value'=>check_value('send_sms_sales_quotation')));

	$send_sms_sales_quotation =check_value('send_sms_sales_quotation');

	SMS_Update('sys_prefs',array('name'=>'send_sms_sales_order'),array('value'=>check_value('send_sms_sales_order')));
	$send_sms_sales_order =check_value('send_sms_sales_order');

	SMS_Update('sys_prefs',array('name'=>'sms_testing_mode'),array('value'=>check_value('sms_testing_mode')));
	$sms_testing_mode =check_value('sms_testing_mode');

	SMS_Update('sys_prefs',array('name'=>'send_sms_sales_invoice'),array('value'=>check_value('send_sms_sales_invoice')));
	$send_sms_sales_invoice =check_value('send_sms_sales_invoice');

	SMS_Update('sys_prefs',array('name'=>'send_sms_delivery'),array('value'=>check_value('send_sms_delivery')));
	$send_sms_delivery =check_value('send_sms_delivery');

	SMS_Update('sys_prefs',array('name'=>'sms_2way_auth'),array('value'=>check_value('sms_2way_auth')));
	$sms_2way_auth =check_value('sms_2way_auth');

	SMS_Update('sys_prefs',array('name'=>'send_sms_customer_payment'),array('value'=>check_value('send_sms_customer_payment')));

	SMS_Update('sys_prefs',array('name'=>'sms_company_phone'),array('value'=>$_POST['sms_company_phone']));

	SMS_Update('sys_prefs',array('name'=>'sms_2way_expiry'),array('value'=>$_POST['sms_2way_expiry']));

	display_notification(_("Selected SMS gateway has been updated successfully"));
	// meta_forward($_SERVER['PHP_SELF'], "SMS_Updated=".$_POST['gateway']);	
}


if(isset($_POST['test_sms']) && $_POST['test_sms']){
	if(strlen($_POST['test_contact'])>0){
		$send =send_sms(_("Test Message"),$_POST['test_contact'],$_POST['gateway']);
		if($send){
			display_notification(_("SMS send successfully"));
		}else{
			display_error(_("Could not send SMS"));
		}
	}else{
		display_error(_("Phone number cannot be empty for sending test sms"));
	}
}
/*---------------------------------------------------------------------*/


if(list_updated('gateway')){
	$Ajax->activate('sms_table');
} 


/*---------------------------------------------------------------------*/

start_form();

	div_start('sms_table');
	start_table(TABLESTYLE2);
		sms_gateway_list_row(_("Choose Gateway"), 'gateway', null,false,true);
	end_table(1);

	start_outer_table(TABLESTYLE2); 
	
	table_section(1);
	if(get_post('gateway') != ALL_TEXT ){
		
		$check_sms_testing_mode =false;
		$check_active_smsgateway =false;
		$check_sales_quotation =false;
		$check_sales_order =false;
		$check_sales_invoice =false;
		$check_delivery =false;
		$check_customer_payment =false;
		if($active_smsgateway ==$_POST['gateway']){
			$check_active_smsgateway =true;
		}
		if($sms_testing_mode){
			$check_sms_testing_mode =true;
		}

		if($send_sms_sales_quotation){
			$check_sales_quotation =true;
		}
		if($send_sms_sales_order){
			$check_sales_order =true;
		}
		if($send_sms_sales_invoice){
			$check_sales_invoice =true;
		}
		if($send_sms_delivery){
			$check_delivery =true;
		}

		if($send_sms_customer_payment){
			$check_customer_payment =true;
		}
		if($sms_2way_auth){
			$sms_2way_auth =true;
		}

		table_section_title(_("General settings"));
		check_row(_("Testing Mode"), 'sms_testing_mode',$check_sms_testing_mode);
		check_row(_("Activate this gateway"), 'active_smsgateway',$check_active_smsgateway);
		array_selector_row(_("Send SMS to"), 'send_sms_to',$send_sms_to, $sms_to_types);
		array_selector_row(_("Next over due days"), 'sms_next_over_due_days',null, range(0,100));
		text_row(_("Company Contact"),'sms_company_phone',null,20,30);

		table_section_title(_("Two way authentication"));
		check_row(_("Activate"), 'sms_2way_auth',$sms_2way_auth);
		// qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
		number_list_row(_("Vaild thru"),'sms_2way_expiry', null, 3, 30);

		table_section_title(_("Send For"));
		check_row(_("Sales Quotations"), 'send_sms_sales_quotation',$check_sales_quotation);
		check_row(_("Sales Orders"), 'send_sms_sales_order',$check_sales_order);
		check_row(_("Sales Invoice"), 'send_sms_sales_invoice',$check_sales_invoice);
		check_row(_("Delivery"), 'send_sms_delivery',$check_delivery);
		check_row(_("Customer Payment"), 'send_sms_customer_payment',$check_customer_payment);

		table_section(2);
		gateway_details($_POST['gateway']);
		table_section_title(_("Testing"));
		text_row(_("Phone no"),'test_contact',null,20,30);
		start_row();
		echo '<td colspan="2">';
		submit_center('test_sms', _("Send Test SMS"),true, '', 'default');
		echo "</td>";
		end_row();
	}
	end_outer_table(1);
	div_end();
	br();
	submit_center('addupdate', _("Submit"), true, '', 'default');
end_form();  
end_page(); ?>
<style>select { width: auto !important;} </style>
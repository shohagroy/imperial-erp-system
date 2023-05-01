<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
$path_to_root = '../..';

$page_security = 'SA_SMSSEND';
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/sms.inc");
include_once($path_to_root . "/includes/db/crm_contacts_db.inc");
add_access_extensions();
global $systypes_array,$sms_types;
page("Manual SMS");

simple_page_mode(true);
$sales_group_customers_contacts =array();
// $result =SMS_GetDataFilter('kv_sms_templates',array('count(type) AS Count','type'),null,null,array('type'));

if(isset($_POST['sales_group']) && $_POST['sales_group'] !=ALL_TEXT){
	$sales_group_customers_contacts =$sql =SMS_GetDataJoin("cust_branch branch", 
		[
			array('join' => 'LEFT', 'table_name' => 'crm_contacts AS contacts', 'conditions' => "(contacts.type ='cust_branch' AND contacts.entity_id=branch.branch_code) OR (contacts.type ='customer' AND contacts.entity_id=branch.debtor_no)"),
			array('join' => 'LEFT', 'table_name' => 'crm_persons AS persons', 'conditions' => "persons.id=contacts.person_id")
		], 

		array('persons.id','persons.name',"COALESCE(persons.phone, persons.phone2) AS phone"), array('branch.group_no'=>$_POST['sales_group']),null,['persons.id']);
}


function can_process(){
	$error =0;
	if($_POST['sales_group'] ==ALL_TEXT){
		display_error(_('Sales group must be selected'));
		set_focus('sales_group');
		$error =1;
	}
	if(strlen(trim($_POST['message'])) ==0){
		display_error(_('Subject Cannot be empty'));
		set_focus('message');
		$error =1;
	}
	if($error)
		return false;
	else 
		return true;
}

function sms_sales_group_list_row($label,$name,$selected_id,$options){

	$sql = "SELECT id, description, inactive FROM ".TB_PREF."groups";
	echo "<tr><td class='label'>$label</td>";
	echo "<td>";
	echo  combo_input($name, $selected_id, $sql, 'id', 'description', 
		array(
		'spec_option' => $options['spec_option']===true ? ' ' : $options['spec_option'],
		'order' => 'description', 
		'spec_id' =>$options['spec_id'],
		'select_submit'=>$options['select_submit'],
	));
	echo "</td>\n";
	echo "</tr>\n";
}

function get_overdue_invoices_between($debtorno, $due_date)
{
    $sql = "SELECT trans.type,
        trans.trans_no,
        trans.order_,
        trans.reference,
        trans.tran_date,
        trans.due_date,
        ABS(ov_amount + ov_gst + ov_freight + ov_freight_tax + ov_discount) As due_amount
		FROM ".TB_PREF."debtor_trans trans
		LEFT JOIN ".TB_PREF."voided as v
            ON trans.trans_no=v.id AND trans.type=v.type
        WHERE due_date ='$due_date' AND debtor_no = ".db_escape($debtorno)."
			AND trans.type <> ".ST_CUSTDELIVERY." AND ISNULL(v.date_)
			AND ABS(ov_amount + ov_gst + ov_freight + ov_freight_tax + ov_discount) > ". FLOAT_COMP_DELTA;
	

	$sql .= " AND ABS(ABS(ov_amount + ov_gst + ov_freight +	ov_freight_tax + ov_discount) - alloc) > ". FLOAT_COMP_DELTA;

	$sql .= " ORDER BY tran_date";
    $query = db_query($sql,"No transactions were returned");
    return $query;
}
/*---------------------------------------------------------------------*/
if (isset($_POST['send_overdue_sms'])){
	$counter =0;
	$sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master";
	// if ($fromcust != ALL_TEXT)
	// 	$sql .= " WHERE debtor_no=".db_escape($fromcust);
	$sql .= " ORDER BY name";
	$result = db_query($sql, "The customers could not be retrieved");
	while ($myrow=db_fetch($result))
	{
		$today =Today();
		$next_over_due_date =date('Y-m-d', strtotime($today. ' + '.$_POST['sms_next_over_due_days'].' days'));
		$phone ='';
		$contacts = get_customer_contacts($myrow['debtor_no']);
		if($contacts && is_array($contacts)){
			$contact =$contacts[0];
            if($contact['phone']){
                $phone=$contact['phone'];
            }elseif($contact['phone2']){
                $phone=$contact['phone2'];
            }
		}

		$custrec = get_customer_details($myrow['debtor_no'],null,false);
		$message_def =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>'overdue','active'=>1));

		$next_over_due =get_overdue_invoices_between($myrow['debtor_no'],$next_over_due_date);

		if(db_num_rows($next_over_due) > 0 ){
			while ($next_due=db_fetch($next_over_due)){
				if(!$custrec){
					$custrec['Balance']=0;
				}
				$message =$message_def ;
				$contents =array(
					'amount'=>$next_due['due_amount'],
					'due_amount'=>$custrec['Balance'],
					'next_due_date'=>$next_due['due_date'],
					'reference'=>$next_due['reference'],
					'customer_id'=>$myrow['debtor_no'],
				);
				$message =sms_content_replacement($message,$contents);
				$send =send_sms($message,$phone);
				if($send){
					$counter ++;
				}
			}
		}elseif($custrec && $custrec['Balance']){
			$contents =array(
				'amount'=>'',
				'due_amount'=>$custrec['Balance'],
				'next_due_date'=>$next_over_due_date,
				'reference'=>'',
				'customer_id'=>$myrow['debtor_no'],
			);
			$message =sms_content_replacement($message_def,$contents);
			$send =send_sms($message,$phone);
			if($send){
				$counter ++;
			}
		}
	}

	if($counter){
		display_notification(sprintf(_("Over due Message send successfully to %d contacts"),$counter));
	}else{
		display_error(_("There is no customers have over due invoices"));
	}
}


if (isset($_POST['send_sms']) && can_process()){
	$selected_contacts=0;
	$counter =0;
	if($_POST['sales_group'] !=ALL_TEXT &&$sales_group_customers_contacts ){
		foreach ($sales_group_customers_contacts as $row) {
			if(check_value('contact_'.$row['id'])){
				$send =send_sms($_POST['message'],$row['phone']);
				$selected_contacts ++;
				if($send){
					$counter ++;
				}
			}
		}
	}
	if(!$selected_contacts){
		display_error(_("Atleat one contact shoule be selected"));
	}elseif($counter){
		display_notification(sprintf(_("Message send successfully to %d contacts"),$selected_contacts));
	}else{
		display_error(_("No SMS send to the selected contact(s)"));
	}
}

if(list_updated('sales_group')){
	$Ajax->activate('bluksms_tbl');
}
start_form();
start_table(TABLESTYLE2);
start_row();
echo "<td class='label'>"._("Next over due days")."</td>\n<td>";
echo array_selector('sms_next_over_due_days',SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_next_over_due_days')),range(0,100));
echo "</td>\n";
echo "<td>";
submit_center( 'send_overdue_sms', _('Send Over Due SMS'), true, '',  'default');
echo "</td>";
end_row();
end_table(1);

div_start('bluksms_tbl');
start_table(TABLESTYLE2);
sms_sales_group_list_row(_("Sales Group"),'sales_group',null, array('spec_option'=>_("Select sales group"),'spec_id'=>ALL_TEXT,'select_submit'=>true));
if($_POST['sales_group'] !=ALL_TEXT){
	if($sales_group_customers_contacts){
		$counter =0;
		foreach ($sales_group_customers_contacts as $contacts) {
			if($contacts['phone']){
				$counter ++;
				check_row($contacts['name']."-".$contacts['phone'],'contact_'.$contacts['id']);
			}
		}
	}
}

textarea_row(_("Message").":", 'message',  null, 25, 5);
end_table(2);
div_end();
submit_center( 'send_sms', _('Send SMS'), true, '',  'default');

end_form();
end_page();
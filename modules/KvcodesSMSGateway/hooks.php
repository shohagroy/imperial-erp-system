<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
if(!defined('SS_KVSMS'))
    define ('SS_KVSMS', 233<<8);
class hooks_KvcodesSMSGateway extends hooks {

    function __construct() {
 		$this->module_name = 'KvcodesSMSGateway';
 	}
    
   function install_options($app) {
    global $path_to_root;
     switch($app->id) {
       case 'system':       
         $app->add_lapp_function(1, _("SMS Templates"), "modules/KvcodesSMSGateway/templates.php?", 'SA_SMSTEMP');
         $app->add_lapp_function(1, _("SMS log"), "modules/KvcodesSMSGateway/logs.php?", 'SA_SMSLOG');
         $app->add_rapp_function(1, _("SMS Setup"), "modules/KvcodesSMSGateway/settings.php?", 'SA_SMSSET');
         $app->add_rapp_function(1, _("Manual SMS"), "modules/KvcodesSMSGateway/manual_sms.php?", 'SA_SMSSEND');
    }
   }
            
    function activate_extension($company, $check_only=true) {
        global $db_connections;
        
        $updates = array( 'update.sql' => array('KvcodesSMSGateway'));
        
        return $this->update_databases($company, $updates, $check_only);
    }
	
    function deactivate_extension($company, $check_only=true) {
        global $db_connections;

        $updates = array('drop.sql' => array('KvcodesSMSGateway'));

       
        return $this->update_databases($company, $updates, $check_only);
    }
    function pre_header($fun_args)
    {

        global $path_to_root,$SysPrefs,$Ajax,$version;
        include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/kvcodes.inc");
        $phone =SMS_GetSingleValue('users','phone',array('id',$_SESSION['wa_current_user']->user));
        if( !isset($_SESSION['2way_logged']) && get_company_pref('sms_2way_auth') && get_company_pref('active_smsgateway')){
            require_once($path_to_root."/modules/KvcodesSMSGateway/auth.php");
            exit;
        }
    }

    function install_access()   {
        global $installed_extensions,$path_to_root;
         $security_sections[SS_KVSMS]    = _("SMS");
          $security_areas['SA_SMSSET'] = array(SS_KVSMS|1, _("SMS Settings")); 
          $security_areas['SA_SMSTEMP'] = array(SS_KVSMS|2, _("SMS Templates")); 
          $security_areas['SA_SMSSEND'] = array(SS_KVSMS|3, _("Send SMS")); 
          $security_areas['SA_SMSLOG'] = array(SS_KVSMS|4, _("View SMS Logs")); 

          return array($security_areas, $security_sections);
    }
    function db_postwrite(&$cart, $type) {
        global $path_to_root;
        include_once $path_to_root.'/modules/KvcodesSMSGateway/includes/kvcodes.inc';
        include_once $path_to_root.'/modules/KvcodesSMSGateway/includes/sms.inc';

        if(isset($cart->reference) && $cart->reference =='auto')
            return ;

        $phone =false;
        $reference =false;
        $customer_id =false;
        $document_date =false;
        $amount =0;
        $active_smsgateway =SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'active_smsgateway'));
        if(!$active_smsgateway){
            return ;
        }
        $sms_to_type =SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_to'));

        if($sms_to_type =='customer_contact'){
            $branch_code ='';
            if(isset($cart->Branch))
                $branch_code =$cart->Branch;
            elseif(isset($cart->branch_id))
                $branch_code =$cart->branch_id;
            $sql ="SELECT crm_persons.phone ,crm_persons.phone2 FROM ".TB_PREF."crm_contacts As crm_contacts LEFT JOIN ".TB_PREF."crm_persons AS crm_persons ON crm_persons.id = crm_contacts.person_id WHERE (crm_contacts.type='customer' AND crm_contacts.entity_id =".db_escape($cart->customer_id).") OR (crm_contacts.type='cust_branch' AND crm_contacts.entity_id =".db_escape($branch_code).") Group by crm_persons.id";
            $query =db_query($sql);
            $phone =array();
            if(db_num_rows($query) > 0 ){
                while ($row =db_fetch($query)) {
                    if($row['phone']){
                        if(!in_array($row['phone'],$phone))
                            $phone[]=$row['phone'];
                    }elseif($row['phone2']){
                        if(!in_array($row['phone2'],$phone))
                            $phone[]=$row['phone2'];
                    }
                }   
            }
            if(empty($phone)){
                 return;
            }
        }


        if($type == ST_SALESORDER || $type == ST_SALESQUOTE || $type == ST_SALESINVOICE || $type == ST_CUSTDELIVERY || $type == ST_CUSTPAYMENT){

            if($type == ST_SALESORDER  && SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_sales_order'))){
                if(!$phone)
                    $phone =$cart->phone;
                $reference =$cart->reference;
                $customer_id =$cart->customer_id;
                $document_date =$cart->document_date;
                $amount =GetSingleValue ('sales_orders','total',array('order_no'=>array_keys($cart->trans_no)[0],'trans_type'=>ST_SALESORDER));
                $message =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>ST_SALESORDER,'active'=>1));

            }elseif($type == ST_SALESQUOTE && SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_sales_quotation'))){
                if(!$phone)
                    $phone =$cart->phone;
                $reference =$cart->reference;
                $customer_id =$cart->customer_id;
                $document_date =$cart->document_date;
                $amount =GetSingleValue ('sales_orders','total',array('order_no'=>array_keys($cart->trans_no)[0],'trans_type'=>ST_SALESQUOTE));
                $message =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>ST_SALESQUOTE,'active'=>1));

            }elseif($type == ST_SALESINVOICE  && SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_sales_invoice'))){
                if(!$phone)
                    $phone =SMS_GetSingleValue('sales_orders', 'contact_phone', array('order_no' =>$cart->order_no,'trans_type'=>ST_SALESORDER));
                $amount =SMS_GetSingleValue('debtor_trans','ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount AS Total',array('type'=>ST_SALESINVOICE,'trans_no'=>array_keys($cart->trans_no)[0]));
                $reference =$cart->reference;
                $customer_id =$cart->customer_id;
                $document_date =$cart->document_date;

                $message =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>ST_SALESINVOICE,'active'=>1));
            }elseif($type == ST_CUSTDELIVERY && SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_delivery'))){
                if(!$phone)
                    $phone =$cart->phone;
                $reference =$cart->reference;
                $customer_id =$cart->customer_id;
                $document_date =$cart->document_date;
                $amount =SMS_GetSingleValue('debtor_trans','ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount AS Total',array('type'=>ST_CUSTDELIVERY,'trans_no'=>array_keys($cart->trans_no)[0]));

                $message =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>ST_CUSTDELIVERY,'active'=>1));

            }elseif($type == ST_CUSTPAYMENT && SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'send_sms_customer_payment'))){
                $message =SMS_GetSingleValue('kv_sms_templates', 'template', array('type' =>ST_CUSTPAYMENT,'active'=>1));
                $reference =$cart->ref;
                $customer_id =$cart->customer_id;
                $document_date =$cart->date_;

                if(!$phone){
                    $branch_code ='';
                    if(isset($cart->Branch))
                        $branch_code =$cart->Branch;
                    elseif(isset($cart->branch_id))
                        $branch_code =$cart->branch_id;
                    $sql ="SELECT crm_persons.phone ,crm_persons.phone2 FROM ".TB_PREF."crm_contacts As crm_contacts LEFT JOIN ".TB_PREF."crm_persons AS crm_persons ON crm_persons.id = crm_contacts.person_id WHERE (crm_contacts.type='customer' AND crm_contacts.entity_id =".db_escape($cart->customer_id).") OR (crm_contacts.type='cust_branch' AND crm_contacts.entity_id =".db_escape($branch_code).") Group by crm_persons.id";
                    $query =db_query($sql);
                    $phone =array();
                    if(db_num_rows($query) > 0 ){
                        while ($row =db_fetch($query)) {
                            if($row['phone']){
                                if(!in_array($row['phone'],$phone))
                                    $phone[]=$row['phone'];
                            }elseif($row['phone2']){
                                if(!in_array($row['phone2'],$phone))
                                    $phone[]=$row['phone2'];
                            }
                        }
                    }
                    $amount =$cart->amount;
                }  
            }

            if(!isset($phone) || !$phone)
                return;
            if(!$message)
                return;
            //replace the message content
            $contents =array(
                'reference'=>$reference,
                'amount'=>$amount,
                'customer_id'=>$customer_id,
                'document_date'=>$document_date
            );
            $message =sms_content_replacement($message,$contents);

            send_sms($message,$phone);
        }

    } 
}


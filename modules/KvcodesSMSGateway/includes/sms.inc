<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/

require_once($path_to_root.'/modules/KvcodesSMSGateway/includes/kvcodes.inc');

function active_sms_gateway_list() {
	$path = dirname(dirname(__FILE__)).'/gateways';

    $FilesAndFolders = array_diff(scandir($path), array('.', '..'));
	$plugins_array = array();
	$activePlugins = get_company_pref('active_sms_gateway');
	foreach ($FilesAndFolders as $single) {	
        $extensn = pathinfo($path .'/'.$single);
		if($extensn['extension'] == 'php' && $single != 'index.php')
            $plugins_array[] = $path .'/'.$single; 
    }
    $final_plugins = [];
    foreach ($plugins_array as $plugin) {	
    	$plugin_file = basename($plugin);    
    	if(basename(dirname($plugin)) != 'add-ons'){
    		$plugin_folder = basename(dirname($plugin)).'/';
    	}else
    		$plugin_folder = '';
     	
         $lines = file($plugin); //file in to an array

		$addon_name = 'Gateway name';
		$addon_n = array_filter($lines, function($var) use ($addon_name) { return preg_match("/\b$addon_name\b/i", $var); });
		$addon_nam=  implode('', $addon_n);
		$positio = strpos($addon_nam, ':');
        if($addon_nam)
		  $final_plugins[] =trim(substr($addon_nam, $positio+1));

    }
    $return =array();
    if(!empty($final_plugins)){
        foreach ($final_plugins as $value) {
           $return[strtolower($value)]=$value;
        }
    }
   
    return $return;
}

function sms_gateway_list_row($label, $name, $selected_id,$all_option=false,$submit_on_change =false)
{
    $items =active_sms_gateway_list();
    $options=array(
        'spec_option'   =>$all_option,
        'spec_id'       =>ALL_TEXT,
        'select_submit' =>$submit_on_change
    );
    array_selector_row($label, $name, $selected_id, $items, $options);
}



function sms_content_replacement($message,$contents)
{

    $customer_info =SMS_GetRow('debtors_master',array('debtor_no'=>$contents['customer_id']));

    $cust_currency =$customer_info['curr_code'];
    $currency_symbol =SMS_GetSingleValue('currencies','curr_symbol',array('curr_abrev'=> $cust_currency));

    if(isset($contents['reference']) && $contents['reference']){
        $reference =$contents['reference'];
    }else{
        $reference ='';
    }


    if(isset($contents['amount']) && $contents['amount']){
        $amount =$currency_symbol.price_format($contents['amount']);
    }else{
        $amount ='';
    }

    if(isset($contents['due_amount']) && $contents['due_amount']){
        $due_amount =$currency_symbol.price_format($contents['due_amount']);
    }else{
        $due_amount ='';
    }

    if(isset($contents['next_due_date']) && $contents['next_due_date']){
        $next_due_date =sql2date($contents['next_due_date']);
    }else{
        $next_due_date ='';
    }
    
    $message = str_replace("{reference}",$reference,$message);

    $message = str_replace("{amount}",$amount,$message);

    $message = str_replace("{customer_name}",$customer_info['name'],$message);

    $message = str_replace("{due_amount}",$due_amount,$message);

    $message = str_replace("{date}",$next_due_date,$message);

    return $message;
}

if(!function_exists('smsautoloader')){
    function smsautoloader($class) {
         global $path_to_root;
         if(file_exists($path =$path_to_root . "/modules/KvcodesSMSGateway/gateways/class-".$class."-sms.php") && !is_dir($path =$path_to_root . "/modules/KvcodesSMSGateway/gateways/class-".$class."-sms.php")){
            $path =$path_to_root . "/modules/KvcodesSMSGateway/gateways/class-".$class."-sms.php";
            require_once $path;
            return true;
         }else{
            return false;
         }
         
    }
}

function send_sms($message,$phone,$gateway="",$log=true,$master_contact=true)
{

    if(SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'sms_testing_mode'))){
        if($log)
            SMS_Insert('kv_sms_logs',array('phone_no'=>$phone,'sms'=>$message,'user_id'=>$_SESSION['wa_current_user']->user,'status'=>"Testing",'gateway'=>'Testing'));
        return true;
    }

    if($gateway==''){
        $active_smsgateway =SMS_GetSingleValue('sys_prefs', 'value', array('name' => 'active_smsgateway'));
    }else{
        $active_smsgateway =$gateway;
    }
   
    if(!$active_smsgateway){
        return false;
    }

    if(!$message || !$phone)
        return false;
    $loaded =smsautoloader($active_smsgateway);
    if(!$loaded){
        display_error(_('Cannot get the getway class file'));
        return false;
    }
    $sms = new $active_smsgateway();


    if(is_array($phone)){
        foreach ($phone as $number) {
            $return =$sms->sendsms($number,$message);
            if($log ==true)
                SMS_Insert('kv_sms_logs',array('phone_no'=>$number,'sms'=>$message,'user_id'=>$_SESSION['wa_current_user']->user,'status'=>$return['status'],'gateway'=>$active_smsgateway));
            if($return['success']){
                if($master_contact ==true) {
                    $master_contact =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_company_phone'));
                    if($master_contact)
                        $sms->sendsms($master_contact,$message);
                }
                
                return true;
            }else{
                return false;
            }
               
        }
    }else{
        $return =$sms->sendsms($phone,$message);
        if($log ==true)
            SMS_Insert('kv_sms_logs',array('phone_no'=>$phone,'sms'=>$message,'user_id'=>$_SESSION['wa_current_user']->user,'status'=>$return['status'],'gateway'=>$active_smsgateway));
        if($return['success']){
            if($master_contact ==true) {
                $master_contact =SMS_GetSingleValue('sys_prefs','value',array('name'=>'sms_company_phone'));
                if($master_contact)
                    $sms->sendsms($master_contact,$message);
            }
            
            return true;
        }else{
            return false;
        }
           
    }
}
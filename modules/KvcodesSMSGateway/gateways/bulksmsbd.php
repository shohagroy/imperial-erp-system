<?php 
/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/


/*
Gateway name: BulkSMSBD
*/
$path_to_root = '../../..';

require_once($path_to_root.'/modules/KvcodesSMSGateway/gateways/gateways.php');

class bulksmsbd extends Kvcodes_sms_gateway {

    public $name = 'BulkSMSBD';

    public function __construct(){

    }

    public function sendsms($number, $msg){
        $url = "http://66.45.237.70/api.php";
        //$number="88017,88018,88019";
        //$text="Hello Bangladesh";
        $data= array(
        'username'=>"YourID",
        'password'=>"YourPasswd",
        'number'=>"$number",
        'message'=>"$msg"
        );

        $ch = curl_init(); // Initialize cURL
        curl_setopt($ch, CURLOPT_URL,$url);
        curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($data));
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $smsresult = curl_exec($ch);
        $p = explode("|",$smsresult);
        $sendstatus = $p[0];
        if($sendstatus == 1101)
            return true;
        else
            return false;
    }

    public function form()
    {
        echo "string";
    }
}
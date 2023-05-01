<?php 
/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/


/*
Gateway name: BhashSMS
*/
// $path_to_root = '../../..';

require_once($path_to_root.'/modules/KvcodesSMSGateway/gateways/gateways.php');

class bhashsms extends Kvcodes_sms_gateway {

    public $name = 'BhashSMS';
    public function __construct(){

    }
    function RemoveUnwanted($str)
    {
        if(is_array($str)){
            $tmpstr=[];
            foreach($str as $value){
                if (substr($str, 0, 2) == "91")
                    $str = substr($str, 2);
                array_push($tmpstr,str_replace("+91", "", str_replace("-", "", str_replace(" ", "", $str))));
            }
            return $tmpstr;
        }else {
            if (substr($str, 0, 2) == "91")
                $str = substr($str, 2);
            return str_replace("+91", "", str_replace("-", "", str_replace(" ", "", $str)));
        }
    }
    function validatePhoneNumber($phonenumber)
    {
        if (is_numeric($phonenumber)) {
            if (strlen(strval($phonenumber)) == 10) {
                return true;
            }
        }
        return false;
    }
    public function sendsms($number, $msg){
        $number =$this->RemoveUnwanted($number);
        if(!$this->validatePhoneNumber($number)){
            return false;
        }
        $options =SMS_GetSingleValue('sys_prefs','value',['name'=>strtolower($this->name)."_options"]);
        if(!$options){
            return false;
        }
        http://bhashsms.com/api/sendmsg.php?user=geesys&pass=********&sender=Sender ID&phone=Mobile No&text=Test SMS&priority=Priority&stype=smstype
        $url="http://bhashsms.com/api/sendmsg.php?";
        //  $data =array(
        //     'comm'=>'sendsms',
        //     'user'=>urlencode($options['username']),
        //     'pass'=>urlencode($options['password']),
        //     'to'=>urlencode($number),
        //     'message'=>urlencode($msg),
        //     'sender'=>urlencode($options['sender']),
        // );

        $url .="user=".urlencode($options['username'])."&pass=".urlencode($options['password'])."&phone=".urlencode($number)."&text=".urlencode($msg)."&sender=".urlencode($options['sender'])."&priority=ndnd&stype=normal";
        display_error($url);
        $ch = curl_init(); // Initialize cURL
        curl_setopt($ch, CURLOPT_URL,$url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $smsresult = curl_exec($ch);
        $p = explode(":",$smsresult);
        $sendstatus = $p[0];
        $return =array('status'=>$sendstatus,'success'=>0);
        if($sendstatus == 1)
            $return['success']=1;

        return $return;
    }

    public function form()
    {
        if(list_updated('gateway')){
            unset($_POST['sender']);
            unset($_POST['username']);
            unset($_POST['password']);
        }
        $options =SMS_GetSingleValue('sys_prefs','value',['name'=>strtolower($this->name)."_options"]);
        if($options && is_array($options)){
            if(!isset($_POST['name']) && isset($options['username'])){
                $_POST['username'] =$options['username'];
            }

            if(!isset($_POST['password']) && isset($options['password'])){
                $_POST['password'] =$options['password'];
            } 
            if(!isset($_POST['sender']) && isset($options['sender'])){
                $_POST['sender'] =$options['sender'];
            }
        }
        table_section_title($this->name." "._("Credentials"));
        text_row(_("Sender"), "sender", null, 20, 100);
        text_row(_("User name"), "username", null, 20, 100);
        text_row(_("Password"), "password", null, 20, 100);
    }
    public function can_process()
    {
        $error =0;
        if(strlen($_POST['username'])==0){
            display_error(_("User name cannot be empty"));
            set_focus('username');
            $error =1;
        }
        if(strlen($_POST['password'])==0){
            display_error(_("Password cannot be empty"));
            set_focus('password');
            $error =1;
        }
        if(strlen($_POST['sender'])==0){
            display_error(_("Sender cannot be empty"));
            set_focus('sender');
            $error =1;
        }
        if($error)
            return false;
        else
            return true;
    }
    public function update()
    {
        if(!$this->can_process())
            return false;

        $options =array();
        $options ['sender'] =$_POST['sender'];
        $options ['username'] =$_POST['username'];
        $options ['password'] =$_POST['password'];
        if(SMS_GetSingleValue('sys_prefs','value',['name'=>strtolower($this->name)."_options"])){
            SMS_Update('sys_prefs',array('name'=>strtolower($this->name)."_options"),array('value'=>base64_encode(serialize($options))));
            $active_smsgateway =$_POST['gateway'];

        }else{
            SMS_Insert('sys_prefs',array('name'=>strtolower($this->name)."_options",'category'=>'setup.company','type'=>'varchar','length'=>'100','value'=>base64_encode(serialize($options))));
            $active_smsgateway =$_POST['gateway'];
        }

        return true;
    }

    public function get_status_names($code)  {
        return $code;
    }

    public function get_sms_char()
    {
        return 160;
    }
}
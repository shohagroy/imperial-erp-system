<?php 

/****************************************
/*  Author  : Kvvaradha
/*  Module  : SMS Gateway
/*  E-mail  : admin@kvcodes.com
/*  Version : 1.0
/*  Http    : www.kvcodes.com
*****************************************/
if(isset($_SESSION['2way_logged']))
	exit();
$page_security = 'SA_OPEN';

include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/sms.inc");
include_once($path_to_root . "/modules/KvcodesSMSGateway/includes/kvcodes.inc");
// add_access_extensions();
$js='';

$rtl = isset($_SESSION['language']->dir) ? $_SESSION['language']->dir : "ltr";
$title =_('Authentication') ;
$encoding = isset($_SESSION['language']->encoding) ? $_SESSION['language']->encoding : "iso-8859-1";
$def_theme = "default";

echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
echo "<html dir='$rtl' >\n";
echo "<head profile=\"http://www.w3.org/2005/10/profile\"><title>$title</title>\n";
echo "<meta http-equiv='Content-type' content='text/html; charset=$encoding' >\n";

echo '<meta name="viewport" content="width=device-width, initial-scale=1.0">';

echo "<link href='$path_to_root/themes/default/images/favicon.ico' rel='icon' type='image/x-icon'> \n";

echo "</head>\n";

echo "<body id='loginscreen'>\n";


/*---------------------------------------------------------------------*/
function send_code($phone){
	$_SESSION['verification_code'] =substr(str_shuffle("0123456789abcdefghijklmnopqrstvwxyz"), 0, 4);
	$_SESSION['valid_thru'] =time();
	if($phone){
		$message ='Your OTP for '.get_company_pref('coy_name').' is '.$_SESSION['verification_code'].' . Only valid for '.get_company_pref('sms_2way_expiry').' mins.';
		$send =send_sms($message,$phone,'',true,false);
		if($send){
			// echo '<div class="success">'._("Please check your inbox").'</div>' ;
		}else{
			echo '<div class="danger">'._("Could not send verfication code. Contact Admin").'</div>';
		}
	}else{
		echo '<div class="danger">'._("Could not send verfication code. Contact Admin").'</div>' ;
	}
	
}

/*---------------------------------------------------------------------*/
echo "<div class='log'>";
echo '<div class="coy_logo">';
$logo =company_path()."/images/".get_company_pref('coy_logo');
if(file_exists($logo) && is_file($logo)){
	echo '<img src="'.$logo.'">';
}else{
	echo '<h3>'.get_company_pref('coy_name').'</h3>';
}
echo '</div>';
echo "<div id='msgbox'>";
// verify code
if(isset($_POST['verify_code'])){
	if($_SESSION['valid_thru']+(get_company_pref('sms_2way_expiry') *60) >= time()){
		$otp =$_POST['otp-1'].$_POST['otp-2'].$_POST['otp-3'].$_POST['otp-4'];
		if($_SESSION['verification_code'] ==$otp){
			$_SESSION['2way_logged'] =1;
			unset($_SESSION['verification_code']);
			unset($_SESSION['valid_thru']);
			meta_forward($_SERVER['PHP_SELF']);
			exit();
		}else{
			echo '<div class="danger">'._("Wrong Code. Re-Enter the code").'</div>';
		}
	}else{
		echo '<div class="danger">'._("Code expired").'</div>';
	}
	
	
}
if(isset($_POST['resend_code'])){
	$js =send_code($phone);
}
if( !isset($_POST['resend_code']) && !isset($_POST['verify_code']) && !isset($_SESSION['verification_code'])){
	$js =send_code($phone);
}

if(get_company_pref('sms_testing_mode')==1){
	echo '<div class="success">CODE : <b>'.$_SESSION['verification_code'].'<b></div>' ;
}
echo "</div>";
if($phone){
	$sdigit =substr($phone,0,4);
	$edigit =substr($phone, -2);
	$rdigit =strlen($phone)-6;
	$contact =$sdigit;
	for ($i=0; $i <$rdigit ; $i++) { 
		$contact .="*";
	}
	$contact .=$edigit;
	echo'<div class="prompt">';
	echo'Please enter the verification code we sent to your mobile <b>'.$contact.'</b>';
	echo'</div>';
}
start_form();
echo "<div>";
echo "<center>";
echo'<div id="countdowntimer">';
echo'</div>';
echo "</center>";
echo "<div>";
	submit_center('resend_code',_("Resend Again?"));
echo'</div>';
echo'</div>';

echo '<div class="otp-wrapper otp-event">
<div class="otp-container">
	<input type="tel" name="otp-1" id="otp-number-input-1" class="otp-number-input" maxlength="1"  autocomplete="off">
	<input type="tel" name="otp-2" id="otp-number-input-2" class="otp-number-input" maxlength="1" autocomplete="off">
	<input type="tel" name="otp-3" id="otp-number-input-3" class="otp-number-input" maxlength="1" autocomplete="off">
	<input type="tel" name="otp-4" id="otp-number-input-4" class="otp-number-input" maxlength="1" autocomplete="off">
</div>
<div>';
submit_center('verify_code',_("Verify"));
echo '</div>
</div>';

end_form();
echo'</div>';
?>
<script type="text/javascript">
	function time_remaining(endtime){
		var t = Date.parse(endtime) - Date.parse(new Date());
		var seconds = Math.floor( (t/1000) % 60 );
		var minutes = Math.floor( (t/1000/60) % 60 );
		var hours = Math.floor( (t/(1000*60*60)) % 24 );
		var days = Math.floor( t/(1000*60*60*24) );
		return {'total':t, 'days':days, 'hours':hours, 'minutes':minutes, 'seconds':seconds};
	}
	function run_clock(id,endtime){
		var clock = document.getElementById(id);
		function update_clock(){
			var t = time_remaining(endtime);
			
			if(t.total<=0){clock.innerHTML = ''; document.getElementById('resend_code').disabled=false; clearInterval(timeinterval); }
			else{
				if(t.total ==10000){
					 document.getElementById("countdowntimer").style.color = "#dd2200";
				}
				clock.innerHTML = t.minutes+' : '+t.seconds;
			}
		}
		update_clock(); // run function once at first to avoid delay
		var timeinterval = setInterval(update_clock,1000);
	}


	function update_times(){
		var time_in_minutes = <?php echo get_company_pref('sms_2way_expiry');?>;
		var current_time = Date.parse(new Date());
		var user_time =<?php echo $_SESSION['valid_thru']*1000 ?>;
		var deadline = new Date(user_time + time_in_minutes*60*1000);
		if(current_time >user_time){
			document.getElementById('resend_code').disabled=true;
			run_clock('countdowntimer',deadline);
		}else{
			document.getElementById('resend_code').disabled=false;
		}
	}
</script>
<?php 
echo '<script type="text/javascript">update_times() </script>';
echo "</body></html>\n";
?>

<style type="text/css">
	body {
  background-color: #EEE;
}
.log {
  width: 400px;
  margin: 5% auto;
  background-color: #FFF;
  padding: 15px 30px 30px 30px;
}
.coy_logo{
	/*border-bottom: 2px solid rgba(0,0,0,.2);*/
	text-align: center;
	margin-bottom: 10px;
	padding: 10px 0px 10px 0px;
}
.coy_logo h3{
	text-align: center;
	margin: 0px;
}
.coy_logo img{
	max-width: 100%;
	height: 99px;
}
.prompt{
	text-align: center;
	color: #050f06;
	margin-bottom: 15px;
}

#countdowntimer{
	color: #007700;
	padding: 15px;
}

#resend_code{
	cursor: pointer;
	background: none;
	border: none;
	color:#007700;
}
#resend_code[disabled]{
	color: #3a3d3a;
	cursor: no-drop;
}
.otp-wrapper {
	text-align:center;
	margin-top:60px;
}

.otp-container {
		display: inline-block;
}


.otp-number-input {
	width: 26px;
	height: 33px;
	margin:0 2px;
	border:none;
	border-bottom: 2px solid rgba(0, 119, 0,.2);
	padding: 0;
	color: #007700;
	margin-bottom: 0;
	padding-bottom: 0;
	font-size: 30px;
	box-shadow: none;
	text-align: center;
	background-color:none;
	font-weight: 600;
	border-radius: 0;
	outline:0;
	transition: border .3s ease;
}

.otp-number-input:focus {
	border-color:rgba(0, 119, 0,.5);
}
.otp-number-input .otp-filled-active {
	border-color:#00bb09;
}

#verify_code {
	background: #42b549;
	border:0;
	color: #fff;
	margin-top:30px;
	padding:10px 15px;
	font-size: 14px;
	border-radius: 3px;
	letter-spacing:1px;
	font-weight:500;
	cursor: pointer;
}
#verify_code[disabled] {
	opacity:0.6;
		cursor: default;
}

#msgbox{
	margin-bottom: 25px;
}

#msgbox div{
	padding-top: 10px;
	padding-bottom: 10px;
}
.danger{
	text-align: center;
	color: #dd2200;
	/*border:1px solid #cc3300;
	background-color: #ffcccc;*/
}
.success{
	text-align: center;
	color: #007700;
	/*border:1px solid #33cc00;
	background-color: #ccffcc;*/
}
@media only screen and (max-width: 576px) {

	.log{
		width: auto;
		min-height: 100vh;
		margin: 0px 0px;
	}
}

@media only screen and (max-width: 768px) {

}

@media only screen and (max-width: 992px) {

}
</style>

<script type="text/javascript">

	// 10 minutes from now

    var container = document.getElementsByClassName("otp-container")[0];
	container.onkeyup = function(e) {
	    var target = e.srcElement || e.target;
	    var maxLength = parseInt(target.attributes["maxlength"].value, 10);
	    var myLength = target.value.length;
	    if (myLength >= maxLength) {
	        var next = target;
	        while (next = next.nextElementSibling) {
	            if (next == null){
	                break;
	            }
	            if (next.tagName.toLowerCase() === "input") {
	                next.focus();
	                break;
	            }
	        }
	    }
	    // Move to previous field if empty (user pressed backspace)
	    else if (myLength === 0) {
	        var previous = target;
	        while (previous = previous.previousElementSibling) {
	            if (previous == null)
	                break;
	            if (previous.tagName.toLowerCase() === "input") {
	                previous.focus();
	                break;
	            }
	        }
	    }
	}
</script>
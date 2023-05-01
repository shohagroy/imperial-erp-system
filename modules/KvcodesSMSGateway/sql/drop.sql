DROP TABLE IF EXISTS `0_kv_sms_templates`;
DROP TABLE IF EXISTS `0_kv_sms_logs`;
DROP TABLE IF EXISTS `0_kv_sms_settings`;

DELETE FROM `0_sys_prefs` WHERE `name`='sms_testing_mode';
DELETE FROM `0_sys_prefs` WHERE `name`='active_smsgateway';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_to';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_sales_quotation';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_sales_order';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_sales_invoice';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_delivery';
DELETE FROM `0_sys_prefs` WHERE `name`='send_sms_customer_payment';
DELETE FROM `0_sys_prefs` WHERE `name`='sms_next_over_due_days';
DELETE FROM `0_sys_prefs` WHERE `name`='sms_company_phone';
DELETE FROM `0_sys_prefs` WHERE `name`='sms_2way_auth';
DELETE FROM `0_sys_prefs` WHERE `name`='sms_2way_expiry';
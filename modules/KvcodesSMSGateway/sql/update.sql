

CREATE TABLE IF NOT EXISTS `0_kv_sms_templates` (
  `id` int NOT NULL AUTO_INCREMENT,
  `type` varchar(250) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `template` varchar(250) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `subject` text COLLATE utf8_unicode_ci NOT NULL, 
  `active` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


INSERT INTO `0_kv_sms_templates` (`id`, `type`, `template`, `subject`, `active`) VALUES
(1, '30', 'Sales Order', 'Sales Order', 0),
(2, '32', 'Sales Quotation', 'Sales Quotation', 0),
(3, '10', 'Sales Invoice', 'Sales Invoice', 0),
(4, '13', 'Delivery', 'Delivery', 0),
(5, '12', 'customer payment', 'Customer Payment', 0);


CREATE TABLE IF NOT EXISTS `0_kv_sms_logs` (
  `id` int NOT NULL AUTO_INCREMENT,
  `phone_no` varchar(15) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `created_at` DATETIME NULL DEFAULT CURRENT_TIMESTAMP,
  `user_id` int(11) NOT NULL DEFAULT '0',
  `sms` text COLLATE utf8_unicode_ci NOT NULL, 
  `status` text CHARACTER SET utf8 COLLATE utf8_unicode_ci  NULL,
  `gateway` varchar(150) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '0',
  PRIMARY KEY(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `0_kv_sms_settings` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(35) NOT NULL DEFAULT '',
  `value` text NOT NULL,
  PRIMARY KEY (`id`) 
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci ;

INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('sms_testing_mode', 'setup.company', 'varchar', '100','0'); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('active_smsgateway', 'setup.company', 'varchar', '100', ''); 

INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_to', 'setup.company', 'varchar', '100', 'report_contact'); 

INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_sales_quotation', 'setup.company', 'int', '100', ''); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_sales_order', 'setup.company', 'int', '100', '');
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_sales_invoice', 'setup.company', 'int', '100', ''); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_delivery', 'setup.company', 'int', '100', ''); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('send_sms_customer_payment', 'setup.company', 'int', '100', ''); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('sms_next_over_due_days', 'setup.company', 'int', '100', '3'); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('sms_company_phone', 'setup.company', 'int', '100', ''); 

INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('sms_2way_auth', 'setup.company', 'int', '100', '0'); 
INSERT INTO `0_sys_prefs` (`name`, `category`, `type`, `length`, `value`) VALUES ('sms_2way_expiry', 'setup.company', 'int', '100', '3'); 
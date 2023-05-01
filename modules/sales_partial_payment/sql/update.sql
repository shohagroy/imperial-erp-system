DROP TABLE IF EXISTS `0_custom_locations`;
CREATE TABLE IF NOT EXISTS `0_custom_locations` (
	`id` int(11) NOT NULL AUTO_INCREMENT,
	`loc_name` varchar(100) NOT NULL DEFAULT 0,
	`inactive` tinyint(1) NOT NULL DEFAULT 0,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 ;

INSERT INTO `0_custom_locations` (`loc_name`) VALUES ('Custom Location 1');

DROP TABLE IF EXISTS `0_custom_branches`;
CREATE TABLE IF NOT EXISTS `0_custom_branches` (
	`branch_id` int(11) NOT NULL AUTO_INCREMENT,
	`branch_name` varchar(100) NOT NULL DEFAULT 0,
	`inactive` tinyint(1) NOT NULL DEFAULT 0,
	PRIMARY KEY (`branch_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 ;

INSERT INTO `0_custom_branches` (`branch_name`) VALUES ('Custom Branch 1');

DROP TABLE IF EXISTS `0_partial_payment_trans`;
CREATE TABLE IF NOT EXISTS `0_partial_payment_trans` (
	`trans_no` int(11) unsigned NOT NULL DEFAULT '0',
	`type` smallint(6) unsigned NOT NULL DEFAULT '0',
	`branch_id` int(11) NOT NULL DEFAULT '-1',
	`loc_id` int(11) NOT NULL,
	`discount_amt` double NOT NULL DEFAULT '0',
	PRIMARY KEY (`type`,`trans_no`)
) ENGINE=InnoDB;
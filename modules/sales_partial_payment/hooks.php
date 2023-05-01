<?php

define ('SS_PARTIALPAYMENT', 105<<8);

class hooks_sales_partial_payment extends hooks {
	function __construct() {
		$this->module_name = 'sales_partial_payment';
	}
	
	function install_options($app) {
		global $path_to_root;

		switch($app->id) {
			case 'orders':
				// $app->modules[0]->lappfunctions[2] = new app_function(_("Direct &Delivery"), 'modules/sales_partial_payment/manage/sales_order_entry.php?NewDelivery=0', 'SA_SALESDELIVERY', MENU_TRANSACTION);
				$app->modules[0]->lappfunctions[3] = new app_function(_("Direct &Invoice"), 'modules/sales_partial_payment/manage/sales_order_entry.php?NewInvoice=0', 'SA_SALESINVOICE', MENU_TRANSACTION);
				$app->modules[0]->rappfunctions[4] = new app_function(_("Customer &Payments"), 'modules/sales_partial_payment/manage/customer_payments.php?', 'SA_SALESPAYMNT', MENU_TRANSACTION);
				$app->modules[1]->lappfunctions[2] = new app_function(_("Customer Transaction &Inquiry"), 'modules/sales_partial_payment/inquiry/customer_inquiry.php?', 'SA_SALESTRANSVIEW', MENU_INQUIRY);
				$app->add_rapp_function(2, _('Manage Locations'), $path_to_root.'/modules/sales_partial_payment/manage/locations.php?', 'SA_INVENTORYLOCATION', MENU_MAINTENANCE);
				$app->add_rapp_function(2, _('Manage Company Branches'), $path_to_root.'/modules/sales_partial_payment/manage/branches.php?', 'SA_INVENTORYLOCATION', MENU_MAINTENANCE);
				break;
			case 'AP':
			$app->modules[0]->lappfunctions[3] = new app_function(_('Direct Supplier &Invoice'), 'modules/sales_partial_payment/manage/po_entry_items.php?NewInvoice=Yes', 'SA_SUPPLIERINVOICE', MENU_TRANSACTION);
			$app->modules[1]->lappfunctions[1] = new app_function(_('Supplier Transaction &Inquiry'), 'modules/sales_partial_payment/inquiry/supplier_inquiry.php?', 'SA_SUPPTRANSVIEW', MENU_INQUIRY);
				break;
			case 'stock':
			$app->modules[1]->lappfunctions[3] = new app_function(_('Location &Transfer Inquiry'), 'modules/sales_partial_payment/inquiry/transfer_inquiry.php?', 'SA_ITEMSTRANSVIEW', MENU_INQUIRY);
				break;
			case 'manuf':
				break;
			case 'assets':
				break;
			case 'proj':
				break;
			case 'GL':
				break;
			case 'system':
				$app->modules[0]->lappfunctions[5] = new app_function(_('Auto Add Item Setup'), 'modules/sales_partial_payment/manage/auto_add_setup.php?', 'SA_AUTOADD', MENU_MAINTENANCE);
				break;
		}
	}
	
	function install_access() {
		$security_sections[SS_PARTIALPAYMENT] =  _('Partial Payment');
		$security_areas['SA_DATECHANGE'] = array(SS_PARTIALPAYMENT|105, _('Change Transaction Date '));
		$security_areas['SA_BANKCHANGE'] = array(SS_PARTIALPAYMENT|106, _('Change Bank Account in Transactions'));
		$security_areas['SA_SALESBRANCHES'] = array(SS_PARTIALPAYMENT|107, _('Create Edit Branches'));
		$security_areas['SA_LOCCHANGE'] = array(SS_PARTIALPAYMENT|108, _('Change Location in Transactions'));
		$security_areas['SA_AUTOADD'] = array(SS_PARTIALPAYMENT|109, _('Setup for Auto Add Item on Invoices'));
		$security_areas['SA_SALESINVOICEEDIT'] = array(SS_PARTIALPAYMENT|110, _('Edit Sales Invoices'));
		$security_areas['SA_PURCHASEINVOICEEDIT'] = array(SS_PARTIALPAYMENT|111, _('Edit Purchase Invoices'));

		return array($security_areas, $security_sections);
	}
	
	function activate_extension($company, $check_only=true) {
		global $db_connections;
		$updates = array( 'update.sql' => array('partial'));
		return $this->update_databases($company, $updates, $check_only);
	}
	
	function deactivate_extension($company, $check_only=true) {
		global $db_connections;
		$updates = array('drop.sql' => array('partial'));
		return $this->update_databases($company, $updates, $check_only);
	}

	function price_in_words($amount, $document=0) {
		
		if ($amount < 0 || $amount > 999999999999)
			return "";
		// $dec = user_price_dec();
		$dec = 2;
		if ($dec > 0) {
			$divisor = pow(10, $dec);
			$frac = round2($amount - floor($amount), $dec) * $divisor;
			$frac = sprintf("%0{$dec}d", round2($frac, 0));
			$and = _('and');
			$curr = get_currency($document['currency']);
			$hundreds_name = isset($curr['hundreds_name']) ? $curr['hundreds_name'] : '';

			if(!empty($hundreds_name))
				$frac = " $and $frac $hundreds_name";
			else
				$frac = " $and $frac/$divisor";
		}
		else
			$frac = '';
		return _number_to_words(intval($amount)) . $frac;
	}
}
<?php

include_once($path_to_root . '/modules/sales_partial_payment/includes/ui/branches_ui.inc');

function branches($name, $type) {
	if($type == 'CUSTOM_BRANCH')
		return custom_branches_list($name, null, _('No branch filter'));
}

$reports->register_controls('branches');

$reports->addReport(RC_CUSTOMER, '_customer_detail_trans', _('Customer Detail Transaction'),
	array(	_('Start Date') => 'DATE',
			_('End Date') => 'DATE',
			_('Customer') => 'CUSTOMERS_NO_FILTER',
			_('Show Balance') => 'YES_NO',
			_('Currency Filter') => 'CURRENCY',
			_('Suppress Zeros') => 'YES_NO',
			_('Comments') => 'TEXTBOX',
			_('Orientation') => 'ORIENTATION',
			_('Destination') => 'DESTINATION'));
$reports->addReport(RC_CUSTOMER, '_user_trans', _('User Wise Sales Invoices'),
	array(	_('Start Date') => 'DATE',
			_('End Date') => 'DATE',
			_('Company Branch') => 'CUSTOM_BRANCH',
			_('Customer') => 'CUSTOMERS_NO_FILTER',
			_('User') => 'USERS',
			_('Comments') => 'TEXTBOX',
			_('Orientation') => 'ORIENTATION',
			_('Destination') => 'DESTINATION'));
$reports->addReport(RC_CUSTOMER, '_invoices', _('Print Invoices With Date Filter'),
	array(	_('From') => 'DATE',
			_('To') => 'DATE',
			_('Location') => 'LOCATIONS',
			_('User Filter') => 'USERS',
			_('email Customers') => 'YES_NO',
			_('Payment Link') => 'PAYMENT_LINK',
			_('Comments') => 'TEXTBOX',
			_('Customer') => 'CUSTOMERS_NO_FILTER',
			_('Orientation') => 'ORIENTATION'
));
$reports->addReport(RC_CUSTOMER, '_cash_receipt', _('User Wise Cash Receipts'),
	array(	_('Start Date') => 'DATE',
			_('End Date') => 'DATE',
			_('Customer') => 'CUSTOMERS_NO_FILTER',
			_('User') => 'USERS',
			_('Comments') => 'TEXTBOX',
			_('Orientation') => 'ORIENTATION',
			_('Destination') => 'DESTINATION'));
$reports->addReport(RC_SUPPLIER, '_supplier_detail_trans', _('Supplier Detail Transaction'),
	array(	_('Start Date') => 'DATE',
			_('End Date') => 'DATE',
			_('Supplier') => 'SUPPLIERS_NO_FILTER',
			_('Show Balance') => 'YES_NO',
			_('Currency Filter') => 'CURRENCY',
			_('Suppress Zeros') => 'YES_NO',
			_('Comments') => 'TEXTBOX',
			_('Orientation') => 'ORIENTATION',
			_('Destination') => 'DESTINATION'));
$reports->addReport(RC_INVENTORY, '_transfer', _('Inventory &Transfer Report'),
	array(	_('Start Date') => 'DATEBEGINM',
			_('End Date') => 'DATEENDM',
			_('Items') => 'ITEMS_P',
			_('Location') => 'LOCATIONS',
			_('Comments') => 'TEXTBOX',
			_('Orientation') => 'ORIENTATION',
			_('Destination') => 'DESTINATION'));
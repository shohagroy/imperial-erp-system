<?php

$page_security = 'SA_ITEMSTRANSVIEW';
$path_to_root = '../../..';
include_once($path_to_root.'/includes/db_pager.inc');
include_once($path_to_root.'/includes/session.inc');
include_once($path_to_root.'/includes/ui.inc');
include_once($path_to_root.'/modules/sales_partial_payment/includes/partial_payment_db.inc');

$js = '';
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(900, 500);
if (user_use_date_picker())
	$js .= get_js_date_picker();

//------------------------------------------------------------------------------------------------

function view_link($row) {
	return get_trans_view_str(ST_LOCTRANSFER, $row['trans_no'], $row['reference']);
}

function check_overdue($row) {
	return false;
}

page(_($help_context = 'Location Transfer Inquiry'), false, false, '', $js);

start_form();

start_table(TABLESTYLE_NOBORDER);
start_row();

ref_cells(_('Enter search string:'), 'string', _('Enter fragment or leave empty'), null, null, true);
locations_list_cells(_('Location:'), 'location', null, _('All Locations'), true);
stock_costable_items_list_cells(_('Item:'), 'stock_id', null, _('All Items'), true);
end_row();
start_row();
date_cells(_('From:'), 'from', '', null, -user_transaction_days(), 0, 0, null, true);
date_cells(_('To:'), 'to', '', null, 0, 0, 0, null, true);
submit_cells('Search', _('Search'), '', '', 'default');
end_row();
end_table();

$sql = get_loc_transfer_sql($_POST['from'], $_POST['to'], get_post('string'), get_post('location'), get_post('stock_id'));
$cols = array(
	_('Trans #') => array('align'=>'center', 'ord'=>''),
	_('Reference') => array('fun'=>'view_link', 'align'=>'center','ord'=>''),
	_('Date') => array('type'=>'date', 'ord'=>''),
	_('From Location') => array('ord'=>''),
	_('To Location') => array('ord'=>''),
	_('Item') => array('ord'=>''),
	_('Quantity') => array('ord'=>'', 'type'=>'qty'),
	_('Unit') => array('ord'=>''),
	_('By User') => array('ord'=>'')
);

$table =& new_db_pager('trans_tbl', $sql, $cols);
// $table->set_marker('check_overdue', _('Marked items are overdue.''));

$table->width = "80%";

display_db_pager($table);

end_form();
end_page();

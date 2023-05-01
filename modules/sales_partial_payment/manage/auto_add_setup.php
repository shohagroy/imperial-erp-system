<?php

$page_security = 'SA_AUTOADD';
$path_to_root  = '../../..';

include_once($path_to_root.'/includes/session.inc');
add_access_extensions();

$js = '';
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(900, 500);

include_once($path_to_root.'/includes/ui.inc');
include_once($path_to_root.'/admin/db/company_db.inc');

//--------------------------------------------------------------------------

function can_process() {

	return true;
}

//--------------------------------------------------------------------------

if (isset($_POST['submit']) && can_process()) {

	update_company_prefs(get_post(array('partial_auto_add', 'partial_show_item_status')));

	display_notification(_('The item setup has been updated.'));
}

//--------------------------------------------------------------------------

page(_($help_context = 'Item default Settings'), false, false, '', $js);

start_form();

start_outer_table(TABLESTYLE2);

table_section(1);

$myrow = get_company_prefs();

if(!isset($myrow['partial_auto_add'])) {
	set_company_pref('partial_auto_add', 'setup.company', 'tinyint', '1', '0');
	$myrow['partial_auto_add'] = 0;
}
if(!isset($myrow['partial_show_item_status'])) {
	set_company_pref('partial_show_item_status', 'setup.company', 'tinyint', '1', '0');
	$myrow['partial_show_item_status'] = 0;
}

$_POST['partial_auto_add'] = $myrow['partial_auto_add'];
$_POST['partial_show_item_status'] = $myrow['partial_show_item_status'];

table_section_title(_('General'));

check_row(_('Auto add item on invoice:'), 'partial_auto_add', $_POST['partial_auto_add']);
check_row(_('Show item status on invoice:'), 'partial_show_item_status', $_POST['partial_show_item_status']);

end_outer_table(1);

submit_center('submit', _('Update'), true, '', 'default');

end_form(2);
end_page();
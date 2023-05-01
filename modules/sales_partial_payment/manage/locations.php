<?php

$page_security = 'SA_INVENTORYLOCATION';
$path_to_root  = '../../..';

include_once($path_to_root . '/includes/session.inc');
add_access_extensions();

include_once($path_to_root . '/includes/ui.inc');
include_once($path_to_root . '/modules/sales_partial_payment/includes/partial_payment_db.inc');
include_once($path_to_root . '/modules/sales_partial_payment/includes/partial_payment_ui.inc');

//--------------------------------------------------------------------------

page(_($help_context = 'Manage Locations'));
simple_page_mode(true);

if($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') {

	if(empty(trim($_POST['loc_name']))) {
		display_error(_('Name field cannot be empty.'));
		set_focus('loc_name');
	}
	else {

		begin_transaction();
		$id = $selected_id == -1 ? false : $selected_id;
		write_location($id, $_POST['loc_name']);

		if($selected_id == -1) {
			$new = true;
			$added_loc = db_insert_id();
		}
		else {
			$new = false;
			$added_loc = $selected_id;
		}

		commit_transaction();
		
		if ($selected_id != -1)
			display_notification(_('Selected location has been updated'));
		else
			display_notification(_('New location has been added'));
		
		$Mode = 'RESET';
	}
}

if ($Mode == 'Delete') {

	if(location_used($selected_id))
		display_error( _('This location cannot be deleted.'));
	else {
		delete_location($selected_id);
		display_notification(_('Selected location has been deleted'));
	}
	$Mode = 'RESET';
}

if($Mode == 'RESET') {
	$selected_id = -1;
	$_POST['loc_name'] = '';
}

//--------------------------------------------------------------------------

start_form();

start_table(TABLESTYLE, "width='50%'");
$th = array(_('Id'), _('Name'), '', '');
inactive_control_column($th);
table_header($th);

$result = db_query(get_custom_location(false, check_value('show_inactive')));
$k = 0;
while ($myrow = db_fetch($result)) {
	alt_table_row_color($k);

	label_cell($myrow['id']);
	label_cell($myrow['loc_name']);
	inactive_control_cell($myrow['id'], $myrow['inactive'], 'custom_locations', 'id');
	edit_button_cell('Edit'.$myrow['id'], _('Edit'));
	delete_button_cell('Delete'.$myrow['id'], _('Delete'));
	end_row();
}
inactive_control_row($th);
end_table(1);

start_table(TABLESTYLE2);

if($selected_id != -1) {
	
	if ($Mode == 'Edit') {
		
		$myrow = get_custom_location($selected_id);
		$_POST['loc_name']  = $myrow['loc_name'];
	}
	hidden('selected_id', $selected_id);
}

text_row_ex(_('Location Name:'), 'loc_name', 37, 50);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();
end_page();
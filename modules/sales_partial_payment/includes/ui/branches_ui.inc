<?php

function custom_branches_list($name, $id=null, $spec_option=false, $submit_on_change=false, $show_inactive=false) {
	$sql = "SELECT branch_id, branch_name, inactive FROM ".TB_PREF."custom_branches ";

	return combo_input($name, $id, $sql, 'branch_id', 'branch_name',
	array(
		'spec_option' => $spec_option,
		'select_submit' => $submit_on_change,
		'async' => false,
		'show_inactive' => $show_inactive
	));
}

//--------------------------------------------------------------------------

function custom_branches_list_cells($label, $name, $id=null, $all_option=false, $submit_on_change=false, $show_inactive=false) {
	if ($label != null)
		echo '<td>'.$label."</td>\n";
	echo '<td>';
	echo custom_branches_list($name, $id, $all_option, $submit_on_change, $show_inactive);
	echo "</td>\n";
}

//--------------------------------------------------------------------------

function custom_branches_list_row($label, $name, $id=null, $all_option=false, $submit_on_change=false, $show_inactive=false) {
	echo "<tr><td class='label'>".$label.'</td>';
	custom_branches_list_cells(null, $name, $id, $all_option, $submit_on_change, $show_inactive);
	echo '</tr>';
}
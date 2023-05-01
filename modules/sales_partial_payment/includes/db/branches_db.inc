<?php

function write_branches($id=false, $name) {
	
	if(!$id)
		$sql = "INSERT INTO ".TB_PREF."custom_branches (branch_name) VALUES (".db_escape($name).")";
	else
		$sql = "UPDATE ".TB_PREF."custom_branches SET branch_name = ".db_escape($name)." WHERE branch_id = ".db_escape($id);
	
	db_query($sql, 'Could not write branch data');
}

function delete_custom_branch($id) {
	$sql = "DELETE FROM ".TB_PREF."custom_branches WHERE branch_id = ".db_escape($id);
	db_query($sql, 'could not delete branch record');
}

function get_custom_branches($id=false, $all=false) {
	
	$sql = "SELECT * FROM ".TB_PREF."custom_branches ";
	$where = array();
	
	if(!$all)
		$where[] = "!inactive";
	if($id)
		$where[] = "branch_id = ".db_escape($id);
		
	if(count($where))
		$sql .= ' WHERE '.implode(' AND ', $where);
	else
		$sql .= ' WHERE 1';

	$result = db_query($sql, 'Could not get branch data.');
	
	if($id)
		return db_fetch($result);
	else
		return $sql;
}

function get_custom_branch_name($id) {
	$sql = "SELECT branch_name FROM ".TB_PREF."custom_branches WHERE branch_id = ".db_escape($id);
	$result = db_query($sql, 'cannot retrieved branch name');

	return db_fetch($result)[0];
}

function branch_used($id) {
	return  check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."partial_payment_trans WHERE branch_id = ".db_escape($id));
}

function db_has_branches() {
	return check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."custom_branches");
}
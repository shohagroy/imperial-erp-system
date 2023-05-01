<?php

function db_has_location() {
	return check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."position");
}

function write_location($id=false, $name) {
	
	if(!$id)
		$sql = "INSERT INTO ".TB_PREF."custom_locations (loc_name) VALUES (".db_escape($name).")";
	else
		$sql = "UPDATE ".TB_PREF."custom_locations SET loc_name = ".db_escape($name)." WHERE id = ".db_escape($id);
	
	db_query($sql, 'Could not write location data');
}

function delete_location($id) {
	$sql = "DELETE FROM ".TB_PREF."custom_locations WHERE id = ".db_escape($id);;
	db_query($sql, 'could not delete location record');
}

function get_custom_location($id=false, $all=false) {
	
	$sql = "SELECT * FROM ".TB_PREF."custom_locations ";
	$where = array();
	
	if(!$all)
		$where[] = "!inactive";
	if($id)
		$where[] = "id = ".db_escape($id);
		
	if(count($where))
		$sql .= ' WHERE '.implode(' AND ', $where);
	else
		$sql .= ' WHERE 1';

	$result = db_query($sql, 'Could not get location data.');
	
	if($id)
		return db_fetch($result);
	else
		return $sql;
}

function location_used($id) {
	return  check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."partial_payment_trans WHERE loc_id = ".db_escape($id));
}

function db_has_custom_locations() {
	return check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."custom_locations");
}

function get_default_loc() {
	$sql = "SELECT loc_id FROM ".TB_PREF."custom_locations";
	$result = db_query($sql, 'could not get location record');

	$row = db_fetch($result);

	return isset($row[0]) ? $row[0] : 0;
}
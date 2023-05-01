<?php
/**********************************************************************
	Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 'SA_ITEMSVALREP';

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui/ui_input.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_types_db.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");

//----------------------------------------------------------------------------------------------------

show_stock_transfers();

function get_stock_transfers($from, $to, $loc=false) {
	$from = date2sql($from);
	$to = date2sql($to);

	$sql = "SELECT loc_from.*, loc_to.*
		FROM
		(SELECT move.trans_no, tran_date, reference, move.loc_code as from_loc, loc.location_name as from_name, a.user 
			FROM ".TB_PREF."stock_moves move
				LEFT JOIN ".TB_PREF."locations loc ON loc.loc_code=move.loc_code 
				LEFT JOIN ".TB_PREF."audit_trail a ON a.trans_no = move.trans_no AND a.type = ".ST_LOCTRANSFER."
			WHERE move.type=".ST_LOCTRANSFER." AND qty < 0) loc_from,

			(SELECT trans_no, move.loc_code as to_loc, loc.location_name as to_name
			FROM ".TB_PREF."stock_moves move
				LEFT JOIN ".TB_PREF."locations loc ON loc.loc_code=move.loc_code
			WHERE type=".ST_LOCTRANSFER." AND qty > 0) loc_to 

			WHERE loc_from.trans_no = loc_to.trans_no AND tran_date >= '$from' AND tran_date <= '$to' ";

			if(!empty($loc))
				$sql .= " AND (from_loc = ".db_escape($loc)." OR to_loc = ".db_escape($loc).")";
			$sql .= "GROUP BY loc_from.trans_no";

	return db_query($sql, 'Could not get transfers data');
}

//----------------------------------------------------------------------------------------------------

function show_stock_transfers() {
	global $path_to_root;

	$from_date = $_POST['PARAM_0'];
	$to_date = $_POST['PARAM_1'];
	$stock = $_POST['PARAM_2'];
	$location = $_POST['PARAM_3'];
	$comments = $_POST['PARAM_4'];
	$orientation = $_POST['PARAM_5'];
	$destination = $_POST['PARAM_6'];

	if ($destination)
		include_once($path_to_root.'/reporting/includes/excel_report.inc');
	else
		include_once($path_to_root.'/reporting/includes/pdf_report.inc');

	$orientation = $orientation ? 'L' : 'P';

	$item_filter = empty($stock) ? _('All') : get_item($stock)['description'];
	$loc = $location == '' ? _('All') : get_location_name($location);

	$cols1 = array(0, 25, 100, 210, 320, 470, 520);
	$cols2 = array(60, 130, 350, 400, 450, 520);

	$header1 = array(_('#'), _('Reference'), _('From Location'), _('To Location'), _('Transfered By'), _('Date'));
	$header2 = array(_('Item Code'), _('Item Description'), _('Quantity'), _('Unit'));

	$aligns1 = array('left', 'left', 'left', 'left', 'left', 'center');
	$aligns2 = array('left', 'left', 'right', 'center');

	$params = array(0 => $comments,
					1 => array('text' => _('Period'), 'from' => $from_date, 'to' => $to_date),
					2 => array('text' => _('Item'), 'from' => $item_filter, 'to' => ''),
					3 => array('text' => _('Location'), 'from' => $loc, 'to' => ''));

	$rep = new FrontReport(_('Inventory Location Transfers'), "LocTransfers", user_pagesize(), 9, $orientation);

	if ($orientation == 'L')
		recalculate_cols($cols);

	$rep->Font();
	$rep->Info($params, $cols2, $header2, $aligns2, $cols1, $header1, $aligns1);
	$rep->NewPage();

	$transfers = get_stock_transfers($from_date, $to_date, $location);

	while ($transfer = db_fetch($transfers)) {

		$empty_item = empty($stock) ? 0 : 1;
		$transfer_items = get_stock_transfer_items($transfer['trans_no']);

		if(!empty($stock)) {
			foreach($transfer_items as $item) {
				if($item['stock_id'] == $stock) {
					$empty_item = 0;
					break;
				}
			}
		}

		if(empty($transfer_items) || $empty_item == 1)
			continue;

		$rep->Font('bold');
		$user = get_user($transfer['user']);
		$rep->TextCol2(0, 1, $transfer['trans_no']);
		$rep->TextCol2(1, 2, $transfer['reference']);
		$rep->TextCol2(2, 3, $transfer['from_name']);
		$rep->TextCol2(3, 4, $transfer['to_name']);
		$rep->TextCol2(4, 5, $user['user_id']);
		$rep->TextCol2(5, 6, sql2date($transfer['tran_date']));
		$rep->Line($rep->row - 2);
		$rep->Font();
		$rep->NewLine();

		while ($item = db_fetch($transfer_items)) {
			if ($item['loc_code'] == $transfer['to_loc']) {
				if(!empty($stock) && $item['stock_id'] != $stock)
					continue;
				$rep->TextCol(0, 1, $item['stock_id']);
				$rep->TextCol(1, 2, $item['description']);
				$rep->AmountCol(2, 3, $item['qty'], get_qty_dec($item['stock_id']));
				$rep->TextCol(3, 4, $item['units']);
				$rep->NewLine();
			}
		}
		$rep->NewLine();
	}

	$rep->NewLine();
	$rep->End();
}


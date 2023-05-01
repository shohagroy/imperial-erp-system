<!DOCTYPE html>
<html>
<head>
	<title>Changes</title>
</head>

<style type="text/css">
	td {
		border: 1px solid;
	}
	xmp.original {
		color: red;
	}
	xmp.modified {
		color: blue;
	}
</style>

<body>

	<h2>Themes changes</h2>
	<ul>
		<li><b>access/</b>
			<ul>
				<li>login.php</li>
				<li>logout.php</li>
				<li>password_reset.php</li>
				<li>timeout.php</li>
			</ul>
		</li>

		<li><b>includes/</b>
			<ul>
				<li><b>pages/</b>
					<ul>
						<li><b>header.inc</b>
							<table>
								<tr>
									<td>
										<xmp class='original'>
144 echo "<meta http-equiv='Content-type' content='text/html; charset=$encoding'>";
145 echo "<link href='$path_to_root/themes/default/images/favicon.ico' rel='icon' type='image/x-icon'> \n";
										</xmp>
									</td>
									<td>
										<xmp class='modified'>
144 echo "<meta http-equiv='Content-type' content='text/html; charset=$encoding'>";
145 echo "<meta name='viewport' content='width=device-width,initial-scale=1'>";
146 echo "<link href='$path_to_root/themes/default/images/favicon.ico' rel='icon' type='image/x-icon'> \n";
										</xmp>
									</td>
								</tr>
							</table>
						</li>
					</ul>
				</li>
				<li><b>ui/</b>
					<ul>
						<li><b>ui_input.inc</b>
							<ul>
								<li>function check_cells{}</li>
								<li>function text_cells_ex{}</li>
								<li>function date_cells{}</li>
								<li>function ref_cells{}</li>
								<li>function percent_row{}</li>
								<li>function amount_cells_ex{}</li>
								<li>function amount_cells{}</li>
								<li>function amount_row{}</li>
								<li>function small_amount_row{}</li>
								<li>function qty_row{}</li>
								<li>function small_amount_cells{}</li>
							</ul>
						</li>

						<li><b>ui_list.inc</b>
							<ul>
								<li>function combo_input{}</li>
								<li>function array_selector{}</li>
								<li>function supplier_list_cells{}</li>
								<li>function customer_list_cells{}</li>
								<li>function locations_list_cells{}</li>
								<li>function locations_list_row{}</li>
								<li>function dimensions_list_cells{}</li>
								<li>function stock_items_list{}</li>
								<li>function stock_items_list_cells{}</li>
								<li>function stock_costable_items_list_cells{}</li>
								<li>function bank_accounts_list_cells{}</li>
								<li>function bank_accounts_list_row{}</li>
								<li>function gl_all_accounts_list_cells{}</li>
								<li>function journal_types_list_cells{}</li>
								<li>function cust_allocations_list_cells{}</li>
								<li>function policy_list_cells{}</li>
								<li>function policy_list_row{}</li>
								<li>function users_list_cells{}</li>
								<li>function stock_manufactured_items_list{}</li>
								<li>function stock_manufactured_items_list_cells{}</li>
							</ul>
						</li>
						<li><b>ui_view.inc (needs to be checked)</b>
							<table>
								<tr>
									<td>
										<xmp class='original'>
908 function get_js_select_combo_item() {
	$js = "function selectComboItem(doc, client_id, value){
    	var element = doc.getElementById(client_id);
		if (typeof(element) != 'undefined' && element != null && element.tagName === 'SELECT' ){
			var options = element.options;
			for (var i = 0, optionsLength = options.length; i < optionsLength; i++) {
				if (options[i].value == value) {
					element.selectedIndex = i;
	        		element.onchange();
				}
			}
		} else {			
			var stock_element = doc.getElementsByName('stock_id');
	    	if( stock_element.length > 0) {
				stock_element.value = value;	
				var stock_id = doc.getElementById('_stock_id_edit'); 
				stock_id.value=value;
				stock_id.onblur();		
			}
		}			
		window.close();
	}";
	return $js;
921 }
										</xmp>
									</td>
									<td>
										<xmp class='modified'>
function get_js_select_combo_item() {
	$js = "function selectComboItem(doc, client_id, value){
    	var element = doc.getElementById(client_id);
		if (typeof(element) != 'undefined' && element != null && element.tagName === 'SELECT' ){
			var options = element.options;
			var txt = '';
			for (var i = 0, optionsLength = options.length; i < optionsLength; i++) {
				if (options[i].value == value) {
					element.selectedIndex = i;
	        		element.onchange();
	        		txt = $(options[i]).text();
				}
			}
		} else {			
			var stock_element = doc.getElementsByName('stock_id');
	    	if( stock_element.length > 0) {
				stock_element.value = value;	
				var stock_id = doc.getElementById('_stock_id_edit'); 
				stock_id.value=value;
				stock_id.onblur();		
			}
		}
		$(doc).find('#select2-'+client_id+'-container').text(txt).attr('title', txt);
		window.close();
	}";
	return $js;
}
										</xmp>
									</td>
								</tr>
							</table>
						</li>
					</ul>
				</li>
			</ul>
		</li>
		
		<li><b>admin/</b>
			<ul>
				<li><b>gl_setup.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
215 amount_row(_("Default Credit Limit:"), 'default_credit_limit', $_POST['default_credit_limit']);
253 percent_row(_("Delivery Over-Receive Allowance:"), 'po_over_receive');
255 percent_row(_("Invoice Over-Charge Allowance:"), 'po_over_charge');</xmp>
							</td>
							<td>
								<xmp class='modified'>
215 amount_row(_("Default Credit Limit:"), 'default_credit_limit', $_POST['default_credit_limit'], null, null, null, true);
253 percent_row(_("Delivery Over-Receive Allowance:"), 'po_over_receive', null, true);
255 percent_row(_("Invoice Over-Charge Allowance:"), 'po_over_charge', null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>security_roles.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
221 check_cells($parms[1], 'Area'.$parms[0], null, 
222 	false, '', "align='center'");
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
221 check_cells($parms[1], 'Area'.$parms[0], null, 
222 	false, '', "align='center'", true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>view_print_transaction.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
79  ref_cells(_("from #:"), 'FromTransNo');
81  ref_cells(_("to #:"), 'ToTransNo');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
79  ref_cells(_("From #:"), 'FromTransNo', null, null, null, false, null, null, true);
81  ref_cells(_("To #:"), 'ToTransNo', null, null, null, false, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>void_transaction.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
171 ref_cells(_("from #:"), 'FromTransNo');
173 ref_cells(_("to #:"), 'ToTransNo');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
171 ref_cells(_("From #:"), 'FromTransNo', null, null, null, false, null, null, true);
173 ref_cells(_("To #:"), 'ToTransNo', null, null, null, false, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>
		<!-- <li><b>applications/</b>
			<ul>
				<li><b>generalledger.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
16  parent::__construct("GL", _($this->help_context = "&Banking and General Ledger"));
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
16  $this->help_context = _("Banking and General Ledger");
17  parent::__construct("GL", _("&General Ledger"));
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>inventory.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
16  parent::__construct("stock", _($this->help_context = "&Items and Inventory"));
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
16  $this->help_context = _("Items And Inventory");
17  parent::__construct("stock", _("&Inventory"));
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li> -->

		<li><b>dimensions</b>
			<ul>
				<li><b>search_dimensions.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
75  number_list_cells(_("Type"), 'type_', null, 1, 2, _("All"));
79  check_cells( _("Only Overdue:"), 'OverdueOnly', null);
83  check_cells( _("Only Open:"), 'OpenOnly', null);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
75  number_list_cells('', 'type_', null, 1, 2, _("All Types"));
79  check_cells( _("Only Overdue:"), 'OverdueOnly', null, false, false, 'nowrap');
83  check_cells( _("Only Open:"), 'OpenOnly', null, false, false, 'nowrap');
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>fixed_assets/</b>
			<ul>
				<li><b>fixed_asset_classes.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
136 small_amount_row(_("Basic Depreciation Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec());
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
136 small_amount_row(_("Basic Depreciation Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec(), true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>gl/</b>
			<ul>
				<li><b>accruals.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
224 amount_row(_("Amount"), 'amount', null, null, viewer_link(_("Search Amount"), $url, "", "", ICON_VIEW));
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
224 amount_row(_("Amount"), 'amount', null, null, viewer_link(_("Search Amount"), $url, "", "", ICON_VIEW), null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>bank_account_reconcile.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
199 bank_accounts_list_cells(_("Account:"), 'bank_account', null, true);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
199 bank_accounts_list_cells(_("Account:"), 'bank_account', null, true, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>bank_transfer.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
126 amount_row(_("Amount:"), 'amount', null, null, $from_currency);
127 amount_row(_("Bank Charge:"), 'charge', null, null, $from_currency);
129 amount_row(_("Incoming Amount:"), 'target_amount', null, '', $to_currency, 2);
133 amount_row(_("Amount:"), 'amount');
134 amount_row(_("Bank Charge:"), 'charge');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
126 amount_row(_("Amount:"), 'amount', null, null, $from_currency, null, true);
127 amount_row(_("Bank Charge:"), 'charge', null, null, $from_currency, null, true);
129 amount_row(_("Incoming Amount:"), 'target_amount', null, '', $to_currency, 2, null, true);
133 amount_row(_("Amount:"), 'amount', null, null, null, null, true);
134 amount_row(_("Bank Charge:"), 'charge', null, null, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>gl_bank_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
98  null, "&nbsp;&nbsp;".submit('go', _("Go"), false, false, true));
298 'settled_amount', null, null, $person_curr, user_price_dec());
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
98  null, "&nbsp;&nbsp;".submit('go', _("Go"), false, false, true), null, true);
298 'settled_amount', null, null, $person_curr, user_price_dec(), true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>balance_sheet.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
125 date_cells(_("As at:"), 'TransToDate');
    if ($dim >= 1)
	dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, " ", false, 1);
    if ($dim > 1)
129	dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, " ", false, 2);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
125 if ($dim >= 1)
	dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, _("Dimension")." 1:", false, 1);
    if ($dim > 1)
	dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, _("Dimension")." 2:", false, 2);
129 date_cells(_("As at:"), 'TransToDate');
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>gl_account_inquiry.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
function gl_inquiry_controls()
{
	$dim = get_company_pref('use_dimension');
    start_form();

    start_table(TABLESTYLE_NOBORDER);
	start_row();
    gl_all_accounts_list_cells(_("Account:"), 'account', null, false, false, _("All Accounts"));
	date_cells(_("from:"), 'TransFromDate', '', null, -user_transaction_days());
	date_cells(_("to:"), 'TransToDate');
    end_row();
	end_table();

	start_table(TABLESTYLE_NOBORDER);
	start_row();
	if ($dim >= 1)
		dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, " ", false, 1);
	if ($dim > 1)
		dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, " ", false, 2);

	ref_cells(_("Memo:"), 'Memo', '',null, _('Enter memo fragment or leave empty'));
	small_amount_cells(_("Amount min:"), 'amount_min', null, " ");
	small_amount_cells(_("Amount max:"), 'amount_max', null, " ");
	submit_cells('Show',_("Show"),'','', 'default');
	end_row();
	end_table();

	echo '<hr>';
    end_form();
}
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
function gl_inquiry_controls()
{
	$dim = get_company_pref('use_dimension');
    start_form();

    start_table(TABLESTYLE_NOBORDER);
	start_row();
    gl_all_accounts_list_cells(_("Account:"), 'account', null, false, false, _("All Accounts"));
    if ($dim >= 1)
		dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, _("Dimension")." 1:", false, 1);
	if ($dim > 1)
		dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, _("Dimension")." 2:", false, 2);
	date_cells(_("from:"), 'TransFromDate', '', null, -user_transaction_days());
	date_cells(_("to:"), 'TransToDate');
    end_row();
//	end_table();
//
//	start_table(TABLESTYLE_NOBORDER);
	start_row();
    
	small_amount_cells(_("Amount min:"), 'amount_min', null, " ", null, null, false, true);
	small_amount_cells(_("Amount max:"), 'amount_max', null, " ", null, null, false, true);
	ref_cells(_("Memo:"), 'Memo', '',null, _('Enter memo fragment or leave empty'));
	submit_cells('Show',_("Show"),'','', 'default');
	end_row();
	end_table();

	echo '<hr>';
    end_form();
}
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>gl_trial_balance.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
58  dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, " ", false, 1);
60  dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, " ", false, 2);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
58  dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, _("Dimension")." 1:", false, 1);
60  dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, _("Dimension")." 2:", false, 2);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>profit_loss.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
184 dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, " ", false, 1);
186 dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, " ", false, 2);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
184 dimensions_list_cells(_("Dimension")." 1:", 'Dimension', null, true, _("Dimension")." 1:", false, 1);
186 dimensions_list_cells(_("Dimension")." 2:", 'Dimension2', null, true, _("Dimension")." 2:", false, 2);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>exchange_rates</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
142 amount_row(_("Exchange Rate:"), 'BuyRate', null, '',
143	submit('get_rate',_("Get"), false, _('Get current rate from') . ' ' . $xchg_rate_provider , true), 'max');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
142 amount_row(_("Exchange Rate:"), 'BuyRate', null, '',
143		submit('get_rate',_("Get"), false, _('Get current rate from') . ' ' . $xchg_rate_provider , true), 'max', true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>gl_quick_entries</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
253 amount_row(_("Default Base Amount").':', 'base_amount', price_format(0));
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
253 amount_row(_("Default Base Amount").':', 'base_amount', price_format(0), null, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>inventory</b>
			<ul>
				<li><b>cost_update.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
135 amount_row(_("Unit cost"), "material_cost", null, "class='tableheader2'", null, $dec1);
139 amount_row(_("Standard Labour Cost Per Unit"), "labour_cost", null, "class='tableheader2'", null, $dec2);
140 amount_row(_("Standard Overhead Cost Per Unit"), "overhead_cost", null, "class='tableheader2'", null, $dec3);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
135 amount_row(_("Unit cost"), "material_cost", null, "class='tableheader2'", null, $dec1, true);
139 amount_row(_("Standard Labour Cost Per Unit"), "labour_cost", null, "class='tableheader2'", null, $dec2, true);
140 amount_row(_("Standard Overhead Cost Per Unit"), "overhead_cost", null, "class='tableheader2'", null, $dec3, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>prices.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
204 small_amount_row(_("Price:"), 'price', null, '', _('per') .' '.$units);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
204 small_amount_row(_("Price:"), 'price', null, '', _('per') .' '.$units, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>purchasing_data.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
202 start_table(TABLESTYLE2);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
202 div_start('price_details');
203 start_table(TABLESTYLE2);
								</xmp>
							</td>
						</tr>
						<tr>
							<td>
								<xmp class='original'>
215 amount_row(_("Price:"), 'price', null,'', get_supplier_currency($selected_id), $dec2);
222 amount_row(_("Conversion Factor (to our UOM):"), 'conversion_factor', null, null, null, 'max');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
215 amount_row(_("Price:"), 'price', null,'', get_supplier_currency($selected_id), $dec2, true);
222 amount_row(_("Conversion Factor (to our UOM):"), 'conversion_factor', null, null, null, 'max', true);
								</xmp>
							</td>
						</tr>
						<tr>
							<td>
								<xmp class='original'>
227 submit_add_or_update_center($selected_id == -1, '', 'both');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
227 submit_add_or_update_center($selected_id == -1, '', 'both');
228 div_end();
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>item_adjustments_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
34  locations_list_row(_("Location:"), 'StockLocation', null, false, false, $order->fixed_asset);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
34  locations_list_row(_("Location:"), 'StockLocation', null, false, false, $order->fixed_asset, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>stock_transfers_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
38  locations_list_row(_("From Location:"), 'FromStockLocation', null, false, false, $order->fixed_asset);
39  locations_list_row(_("To Location:"), 'ToStockLocation', null,false, false, $order->fixed_asset);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
38  locations_list_row(_("From Location:"), 'FromStockLocation', null, false, false, $order->fixed_asset, true);
39  locations_list_row(_("To Location:"), 'ToStockLocation', null,false, false, $order->fixed_asset, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>stock_movements.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
77  end_row();
    end_table();

    start_table(TABLESTYLE_NOBORDER);
    start_row();

    locations_list_cells(_("From Location:"), 'StockLocation', null, true, false, (get_post('fixed_asset') == 1));

    date_cells(_("From:"), 'AfterDate', '', null, -user_transaction_days());
86  date_cells(_("To:"), 'BeforeDate');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
77  //end_row();
    //end_table();

    //start_table(TABLESTYLE_NOBORDER);
    //start_row();

    date_cells(_("From:"), 'AfterDate', '', null, -user_transaction_days());
    date_cells(_("To:"), 'BeforeDate');

86  locations_list_cells(_("From Location:"), 'StockLocation', null, true, false, (get_post('fixed_asset') == 1));
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>item_codes.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
185 qty_row(_("Quantity:"), 'quantity', null, '', $units, $dec);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
185 qty_row(_("Quantity:"), 'quantity', null, '', $units, $dec, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>items.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
426 small_amount_row(_("Depreciation Years").':', 'depreciation_rate', null, null, _('years'), 0);
429 small_amount_row(_("Base Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec());
431 small_amount_row(_("Depreciation Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec());
434 small_amount_row(_("Rate multiplier").':', 'depreciation_factor', null, null, '', 2);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
426 small_amount_row(_("Depreciation Years").':', 'depreciation_rate', null, null, _('years'), 0, true);
429 small_amount_row(_("Base Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec(), true);
431 small_amount_row(_("Depreciation Rate").':', 'depreciation_rate', null, null, '%', user_percent_dec(), true);
434 small_amount_row(_("Rate multiplier").':', 'depreciation_factor', null, null, '', 2, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_kits.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
243 qty_row(_("Quantity:"), 'quantity', number_format2(1, $dec), '', $units, $dec);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
243 qty_row(_("Quantity:"), 'quantity', number_format2(1, $dec), '', $units, $dec, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>manufacturing/</b>
			<ul>
				<li><b>search_work_orders.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
70  locations_list_cells(_("at Location:"), 'StockLocation', null, true);

    end_row();
    end_table();
    start_table(TABLESTYLE_NOBORDER);
    start_row();

    check_cells( _("Only Overdue:"), 'OverdueOnly', null);

    if ($outstanding_only==0)
	check_cells( _("Only Open:"), 'OpenOnly', null);

82  stock_manufactured_items_list_cells(_("for item:"), 'SelectedStockItem', null, true);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
70  stock_manufactured_items_list_cells(_("for item:"), 'SelectedStockItem', null, true, false, false, true);

    locations_list_cells(_("at Location:"), 'StockLocation', null, true);
    if ($outstanding_only==0)
	    check_cells( _("Only Open:"), 'OpenOnly', null);

    end_row();
    end_table();
    start_table(TABLESTYLE_NOBORDER);
    start_row();

    check_cells( _("Only Overdue:"), 'OverdueOnly', null);
82  
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>work_order_entry.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
385 locations_list_row(_("Destination Location:"), 'StockLocation', null);
396 qty_row(_("Quantity Required:"), 'quantity', null, null, null, $dec);
404 qty_row(_("Quantity:"), 'quantity', null, null, null, $dec);
419 amount_row($wo_cost_types[WO_LABOUR], 'Labour');
421 amount_row($wo_cost_types[WO_OVERHEAD], 'Costs');


								</xmp>
							</td>
							<td>
								<xmp class='modified'>
385 locations_list_row(_("Destination Location:"), 'StockLocation', null, false, false, false, true);
396 qty_row(_("Quantity Required:"), 'quantity', null, null, null, $dec, true);
404 qty_row(_("Quantity:"), 'quantity', null, null, null, $dec, true);
419 amount_row($wo_cost_types[WO_LABOUR], 'Labour', null, null, null, null,true);
421 amount_row($wo_cost_types[WO_OVERHEAD], 'Costs', null, null, null, null, true);


								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>bom_edit.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
172 stock_manufactured_items_list_cells(_("Select a manufacturable item:"), 'stock_id', null, false, true);
229 locations_list_row(_("Location to Draw From:"), 'loc_code', null);
233 qty_row(_("Quantity:"), 'quantity', null, null, null, $dec);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
172 stock_manufactured_items_list_cells(_("Select a manufacturable item:"), 'stock_id', null, false, true, false, true);
229 locations_list_row(_("Location to Draw From:"), 'loc_code', null, false, false, false, true);
233 qty_row(_("Quantity:"), 'quantity', null, null, null, $dec, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>purchasing/</b>
			<ul>
				<li><b>supplier_payment.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
265 start_outer_table(TABLESTYLE2, "width='60%'", 5);
316 amount_row(_("Bank Amount:"), 'bank_amount', null, '', $bank_currency);
319 amount_row(_("Bank Charge:"), 'charge', null, '', $bank_currency);
342 start_table(TABLESTYLE, "width='60%'");
343 amount_row(_("Amount of Discount:"), 'discount', null, '', $supplier_currency);
344 amount_row(_("Amount of Payment:"), 'amount', null, '', $supplier_currency);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
265 start_outer_table(TABLESTYLE2, "", 5);
316 amount_row(_("Bank Amount:"), 'bank_amount', null, '', $bank_currency, null, true);
319 amount_row(_("Bank Charge:"), 'charge', null, '', $bank_currency, null, true);
342 start_table(TABLESTYLE);
343 amount_row(_("Amount of Discount:"), 'discount', null, '', $supplier_currency, null, true);
344 amount_row(_("Amount of Payment:"), 'amount', null, '', $supplier_currency, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>grn_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
69  locations_list_row(_("Deliver Into Location"), "Location", $_POST['Location']);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
69  locations_list_row(_("Deliver Into Location"), "Location", $_POST['Location'], false, true, false, true); //Edited by Phuong (Flat theme)
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>invoice_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
479 date_cells(_("Received between"), 'receive_begin', "", null, 
480		-user_transaction_days(), 0, 0, "valign=middle");
481 date_cells(_("and"), 'receive_end', '', null, 1, 0, 0, "valign=middle");
								</xmp>
							</td>
							<td>
								<xmp class="modified">
479 date_cells(_("From:"), 'receive_begin', "", null, 
480		-user_transaction_days(), 0, 0, "valign=middle");
481 date_cells(_("To:"), 'receive_end', '', null, 1, 0, 0, "valign=middle");
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>po_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
175 locations_list_row(_("Receive Into:"), 'StkLocation', null, false, true, $order->fixed_asset);
323 locations_list_cells(null, 'Location', $_POST['Location']);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
175 locations_list_row(_("Receive Into:"), 'StkLocation', null, false, true, $order->fixed_asset, true);
323 locations_list_cells(null, 'Location', $_POST['Location'], false, true, false, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>po_search.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
69  end_table();
72  start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
69  //end_table();
72  //start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>po_search_completed.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
96  end_table();

98  start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
96  //end_table();

98  //start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				
				<li><b>suppliers.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
222 amount_row(_("Credit Limit:"), 'credit_limit', null);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
222 amount_row(_("Credit Limit:"), 'credit_limit', null, null, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>

		<li><b>reporting</b>
			<ul>
				<li><b>report_classes.inc</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
217 $st = "<input type='text' name='$name' value='$date'>";
    if (user_use_date_picker())
    {
        $calc_image = (file_exists("$path_to_root/themes/".user_theme()."/images/cal.gif")) ? 
            "$path_to_root/themes/".user_theme()."/images/cal.gif" : "$path_to_root/themes/default/images/cal.gif";
        $st .= "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
223         . "	<img src='$calc_image' style='vertical-align:middle;padding-bottom:4px;width:16px;height:16px;border:0;' alt='"._('Click Here to Pick up the date')."'></a>\n";
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
217 $st = "<div class='date_box'><input type='text' class='date' name='$name' value='$date'>";
	if (user_use_date_picker())
    {
        $calc_image = (file_exists("$path_to_root/themes/".user_theme()."/images/cal.gif")) ? 
            "$path_to_root/themes/".user_theme()."/images/cal.gif" : "$path_to_root/themes/default/images/cal.gif";
        $st .= "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
223	    . "	<img src='$calc_image' style='vertical-align:middle;padding-bottom:4px;width:16px;height:16px;border:0;' alt='"._('Click Here to Pick up the date')."'></a></div>\n";
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>
		
		<li><b>sales/</b>
			<ul>
				<li><b>credit_note_entry.php</b><i style="color: red"> Needs to be checked</i></li>
				<li><b>customer_credit_invoice.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
351 locations_list_row(_("Items Returned to Location"), 'Location', $_POST['Location']);
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
351 locations_list_row(_("Items Returned to Location"), 'Location', $_POST['Location'], false, false, false, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>customer_delivery.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
366 date_cells(_("Date"), 'DispatchDate', '', $_SESSION['Items']->trans_no==0, 0, 0, 0, "class='tableheader2'");
399 date_cells(_("Invoice Dead-line"), 'due_date', '', null, 0, 0, 0, "class='tableheader2'");
521 policy_list_row(_("Action For Balance"), "bo_policy", null);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
366 date_cells(_("Date"), 'DispatchDate', '', $_SESSION['Items']->trans_no==0, 0, 0, 0, "class='tableheader2'", false, true);
399 date_cells(_("Invoice Dead-line"), 'due_date', '', null, 0, 0, 0, "class='tableheader2'", false, true);
521 policy_list_row(_("Action For Balance"), "bo_policy", null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>customer_invoice.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
486 date_cells(_("Date"), 'InvoiceDate', '', $_SESSION['Items']->trans_no == 0, 0, 0, 0, "class='tableheader2'", true);
492 date_cells(_("Due Date"), 'due_date', '', null, 0, 0, 0, "class='tableheader2'");
								</xmp>
							</td>
							<td>
								<xmp class="modified">
486 date_cells(_("Date"), 'InvoiceDate', '', $_SESSION['Items']->trans_no == 0, 0, 0, 0, "class='tableheader2'", true, true);
492 date_cells(_("Due Date"), 'due_date', '', null, 0, 0, 0, "class='tableheader2'", false, true); 
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>customer_payments.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
310 start_outer_table(TABLESTYLE2, "width='60%'", 5);
363 amount_row(_("Payment Amount:"), 'bank_amount', null, '', $bank_currency);
366 amount_row(_("Bank Charge:"), 'charge', null, '', $bank_currency);
393 amount_row(_("Amount of Discount:"), 'discount', null, '', $cust_currency);
395 amount_row(_("Amount:"), 'amount', null, '', $cust_currency);
								</xmp> 
							</td>
							<td>
								<xmp class="modified">
310 start_outer_table(TABLESTYLE2, "width='80%'", 5);
363 amount_row(_("Payment Amount:"), 'bank_amount', null, '', $bank_currency, null, true);
366 amount_row(_("Bank Charge:"), 'charge', null, '', $bank_currency, null, true);
393 amount_row(_("Amount of Discount:"), 'discount', null, '', $cust_currency, null, true);
395 amount_row(_("Amount:"), 'amount', null, '', $cust_currency, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_order_entry.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
744 start_table(TABLESTYLE, "width='80%'", 10);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
744 start_table(TABLESTYLE, "width='100%'", 10);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_credit_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
326 locations_list_row(_("Items Returned to Location"), 'Location', $_POST['Location']);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
326 locations_list_row(_("Items Returned to Location"), 'Location', $_POST['Location'], false, false, false, true);
								</xmp>	
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_order_ui.inc</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
144 start_table(TABLESTYLE, "width='90%'");
247 start_outer_table(TABLESTYLE2, "width='80%'");
582 start_table(TABLESTYLE2, "width='60%'");
584 locations_list_row(_("Deliver from Location:"), 'Location', null, false, true);
616 locations_list_row(_("Deliver from Location:"), 'Location', null, false, true, $order->fixed_asset);
621 amount_row(_('Pre-Payment Required:'), 'prep_amount');
								</xmp>
							</td>
							<td>
								<xmp class="modified">
144 start_table(TABLESTYLE, "");
247 start_outer_table(TABLESTYLE2, "");
582 start_table(TABLESTYLE2, "");
584 locations_list_row(_("Deliver from Location:"), 'Location', null, false, true, false, true);
616 locations_list_row(_("Deliver from Location:"), 'Location', null, false, true, $order->fixed_asset, true);
621 amount_row(_('Pre-Payment Required:'), 'prep_amount', null, null, null, null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_deliveries_view.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
109 end_table();
110 start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
109 //end_table();
110 //start_table(TABLESTYLE_NOBORDER);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_orders_view.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
225	locations_list_cells(_("Location:"), 'StockLocation', null, true, true);

	if($show_dates) {
		end_row();
		end_table();

		start_table(TABLESTYLE_NOBORDER);
		start_row();
	}
	stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true, true);

	if (!$page_nested)
		customer_list_cells(_("Select a customer: "), 'customer_id', null, true, true);
	if ($trans_type == ST_SALESQUOTE)
		check_cells(_("Show All:"), 'show_all');

	submit_cells('SearchOrders', _("Search"),'',_('Select documents'), 'default');
	hidden('order_view_mode', $_POST['order_view_mode']);
	hidden('type', $trans_type);

	end_row();

247	end_table(1);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
225	if($show_dates) {
		end_row();
		start_row();
	}
	stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true, true);

	if (!$page_nested)
		customer_list_cells(_("Select a customer: "), 'customer_id', null, true, true);
	locations_list_cells(_("Location:"), 'StockLocation', null, true, true);

	hidden('order_view_mode', $_POST['order_view_mode']);
	hidden('type', $trans_type);

	end_row();
	end_table();

	start_table(TABLESTYLE_NOBORDER);
	start_row();
	if ($trans_type == ST_SALESQUOTE)
		check_cells(_("Show All:"), 'show_all');

	submit_cells('SearchOrders', _("Search"),'',_('Select documents'), 'default');
	end_row();
248	end_table();
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>customer_branches.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
236 locations_list_row(_("Default Inventory Location:"), 'default_location', null);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
236 locations_list_row(_("Default Inventory Location:"), 'default_location', null, false, false, false, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>customers.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
268 percent_row(_("Discount Percent:"), 'discount', $_POST['discount']);
269 percent_row(_("Prompt Payment Discount Percent:"), 'pymt_discount', $_POST['pymt_discount']);
270 amount_row(_("Credit Limit:"), 'credit_limit', $_POST['credit_limit']);
297 locations_list_row(_("Default Inventory Location:"), 'location');
								</xmp>
							</td>
							<td>
								<xmp class="modified">
268 percent_row(_("Discount Percent:"), 'discount', $_POST['discount'], true);
269 percent_row(_("Prompt Payment Discount Percent:"), 'pymt_discount', $_POST['pymt_discount'], true);
270 amount_row(_("Credit Limit:"), 'credit_limit', $_POST['credit_limit'], null, null, null, true);
297 locations_list_row(_("Default Inventory Location:"), 'location', null, false, false, false, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>recurrent_invoices.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
202 small_amount_row(_("Days:"), 'days', 0, null, null, 0);
204 small_amount_row(_("Monthly:"), 'monthly', 0, null, null, 0);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
202 small_amount_row(_("Days:"), 'days', 0, null, null, 0, true);
204 small_amount_row(_("Monthly:"), 'monthly', 0, null, null, 0, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_people.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
159 percent_row(_("Provision").':', 'provision');
160 amount_row(_("Turnover Break Pt Level:"), 'break_pt');
161 percent_row(_("Provision")." 2:", 'provision2');
								</xmp>
							</td>
							<td>
								<xmp class="modified">
159 percent_row(_("Provision").':', 'provision', null, true);
160 amount_row(_("Turnover Break Pt Level:"), 'break_pt', null, null, null, null, true);
161 percent_row(_("Provision")." 2:", 'provision2', null, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_points.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
139 locations_list_row(_("POS location").':', 'location');
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
139 locations_list_row(_("POS location").':', 'location', null, false, false, false, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>sales_types.php</b>
					<table>
						<tr>
							<td>
								<xmp class="original">
155 amount_row(_("Calculation factor").':', 'factor', null, null, null, 4);
								</xmp>
							</td>
							<td>
								<xmp class="modified">
155 amount_row(_("Calculation factor").':', 'factor', null, null, null, 4, true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>
		
		<li><b>taxes/</b>
			<ul>
				<li><b>tax_types.php</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
159 small_amount_row(_("Default Rate:"), 'rate', '', "", "%", user_percent_dec());
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
159 small_amount_row(_("Default Rate:"), 'rate', '', "", "%", user_percent_dec(), true);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>
		
		<li><b>js/</b>
			<ul>
				<li><b>insert.js</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
98 select.options[i].selected = true;
103 select.options[i].selected = true;
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
98 select.options[i].selected = true;
99 $(select).select2().val(select.options[i].value);
103 select.options[i].selected = true;
104 $(select).select2().val(select.options[i].value);
								</xmp>
							</td>
						</tr>
					</table>
				</li>
				<li><b>utils.js</b>
					<table>
						<tr>
							<td>
								<xmp class='original'>
36  JsHttpRequest.request= function(trigger, form, tout) {
37  //	if (trigger.type=='submit' && !validate(trigger)) return false;
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
JsHttpRequest.request= function(trigger, form, tout) {
	
	var width = 1;
    var id = setInterval(frame, 1);
    function frame() {
        if (width >= 60) {
            clearInterval(id);
        } else {
            width++; 
            document.getElementById("progress").style.width = width + '%';
        }
    }
	
//	if (trigger.type=='submit' && !validate(trigger)) return false;
								</xmp>
							</td>
						</tr>
						<tr>
							<td>
								<xmp class='original'>
93  function(result, errors) {
94          // Write the answer.
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
function(result, errors) {
			
    var width = 60;
    var id = setInterval(frame, 1);
    function frame() {
        if (width >= 100) {
            clearInterval(id);
        } else {
            width++; 
            document.getElementById("progress").style.width = width + '%';
        }
    }
        // Write the answer.
								</xmp>
							</td>
						</tr>
						<tr>
							<td>
								<xmp class='original'>
136 set_mark();

138 Behaviour.apply();
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
set_mark();
		  
setTimeout(function(){
    document.getElementById("progress").style.width = '0%';
}, 300);

Behaviour.apply();
								</xmp>
							</td>
						</tr>
						<tr>
							<td>
								<xmp class='original'>
144     if(!newwin) {
            setFocus();
        }
    }
        },
149	    false  // do not disable caching
								</xmp>
							</td>
							<td>
								<xmp class='modified'>
			if(!newwin) {
		  		setFocus();
			}
		}
			switch (user.datefmt) {
				case 0: datefmt = 'mm/dd/yy'; break;
				case 1: datefmt = 'dd/mm/yy'; break;
				case 2: datefmt = 'yy/mm/dd'; break;
				case 3: datefmt = 'MM/dd/yy'; break;
				case 4: datefmt = 'dd/MM/yy'; break;
				case 5: datefmt = 'yy/MM/dd'; break;
				default: datefmt = 'dd/mm/yy';
			}
			
			$(document).ready(function() {
				$('select').select2();
				$('select').on('select2:close', function () { $(this).focus(); });
				$('.ajaxsubmit').tooltip().click(function() {
					$('.ajaxsubmit').tooltip('close');
				})
				$('.date').datepicker({
					dateFormat: datefmt,
        			changeMonth: true,
        			changeYear: true,
					showWeek: true,
      				firstDay: 1,
					showOn: 'button',
      				buttonImage: user.theme+'../shinee/images/calendar_grey.svg',
      				buttonImageOnly: true,
      				buttonText: 'Select date'
    			});
        	});
            },
	        false  // do not disable caching
								</xmp>
							</td>
						</tr>
					</table>
				</li>
			</ul>
		</li>
		<li>END</li>
	</ul>

	<h2>Module changes</h2>
	<ul>
		<li></li>
		<li></li>
	</ul>

	<h2>Other changes</h2>
	<ul>
		<li>Add change_log folder</li>
		<li></li>
	</ul>

</body>
</html>

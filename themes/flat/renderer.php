<?php

class renderer {
	
	function get_icon($category) {
		global  $path_to_root, $SysPrefs;

		if ($SysPrefs->show_menu_category_icons) {
			if($category == MENU_TRANSACTION)
				$icon = 'swap_horiz';
			elseif($category == MENU_SYSTEM)
				$icon = 'build';
			elseif($category == MENU_UPDATE)
				$icon = 'sync';
			elseif($category == MENU_INQUIRY)
				$icon = 'find_in_page';
			elseif($category == MENU_ENTRY)
				$icon = 'library_add';
			elseif($category == MENU_REPORT)
				$icon = 'insert_chart';
			elseif($category == MENU_MAINTENANCE)
				$icon = 'create';
			elseif($category == MENU_SETTINGS)
				$icon = 'assignment';
			else
				$icon = 'arrow_forward';
		}
		else	
			$icon = 'arrow_forward';
		return "<i class='material-icons'>".$icon."</i>";
	}

	function wa_header() {
		page(_($help_context = 'Main Menu'), false, true);
	}

	function wa_footer() {
		end_page(false, true);
	}

	function menu_header($title, $no_menu, $is_index) {
		global $path_to_root, $SysPrefs, $db_connections, $help_base_url;
		
		$theme_path = $path_to_root.'/themes/'.user_theme();
		$file_ext = strpos($_SERVER['REQUEST_URI'], '?') ? '.php?' : '.php';
		$local_css = basename($_SERVER['REQUEST_URI'], $file_ext . $_SERVER['QUERY_STRING']);
			
		add_css_file($theme_path.'/libraries/select2.min.css');
		add_css_file($theme_path.'/libraries/jquery-ui-1.12.1/themes/fa/jquery-ui.css');
		add_css_file($theme_path.'/libraries/chartist-custom.min.css');
		add_css_file($theme_path.'/libraries/chartist-plugin-tooltip.css');
		add_css_file($theme_path.'/libraries/chartist-plugin-columntooltips.css');
		add_css_file($theme_path.'/libraries/chartist-legend-custom.css');

		if($local_css == 'sales_orders_view') {
			if($_SERVER['QUERY_STRING'] == 'PrepaidOrders=Yes' || $_SERVER['QUERY_STRING'] == 'type=32' || $_SERVER['QUERY_STRING'] == 'type=30')
				add_css_file($theme_path.'/local_style/sales_orders_view_2.css');
			else
				add_css_file($theme_path.'/local_style/'.$local_css.'.css');
		}
		else
			add_css_file($theme_path.'/local_style/'.$local_css.'.css');
		
		add_js_ufile($theme_path.'/libraries/jquery-1.10.2.min.js');
		add_js_ufile($theme_path.'/libraries/chartist.min.js');
		add_js_ufile($theme_path.'/libraries/chartist-plugin-tooltip.min.js');
		add_js_ufile($theme_path.'/libraries/chartist-plugin-columntooltips.js');
		add_js_ufile($theme_path.'/libraries/select2.min.js');
		add_js_ufile($theme_path.'/libraries/bootstrap.min.js');
		add_js_ufile($theme_path.'/libraries/jquery-ui-1.12.1/jquery-ui.min.js');
		
		send_css();
		send_scripts();
		
		echo "<div id='progress'></div>";
		echo "<div id='loading'></div>";

		$indicator = $theme_path.'/images/ajax-loader.gif';
		if (!$no_menu) {
			
	        $row = get_company_prefs();
             
            if ($row['coy_logo'] == '' || !file_exists(company_path().'/images/'.$row['coy_logo']))
                $User_logo = $theme_path.'/images/logo_optecherp.jpg';	
            else 
                $User_logo = company_path().'/images/'.$row['coy_logo'];

            $userName = !empty($_SESSION['wa_current_user']->name) ? $_SESSION['wa_current_user']->name : _('Unnamed User');
            
		    include_once($theme_path.'/notification.php');
            $noti = new show_notification();
            echo "<div class='page_header bootstrap-iso'>";
		    echo "<div class='logo_box'><img src='".$User_logo."' alt='User Image'/></div>";
				
		    echo "<div class='toggle_button' id='menu_toggle'><i class='fa fa-bars fa-2x'></i></div>";

		    echo "<div id='panel_open_box'><a href='#' class='fa fa-th-large fa-2x' id='module_panel_open'></a></div>";
              
            echo "<div class='dropdown username_dropdown'><span class='user_dropdown dropdown-toggle' data-toggle='dropdown'>".$userName."</span>";
            echo "<ul class='dropdown-menu dropdown-menu-right'>";

            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/admin/display_prefs.php?'><i class='fa fa-cog fa-fw'></i><span>"._('Preferences')."</span></a>";
            echo "</li>";
                
            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/admin/change_current_user_password.php?selected_id=".$_SESSION['wa_current_user']->username."'><i class='fa fa-key fa-fw'></i><span>"._('Change password')."</span></a>";
            echo "</li>";
                
            echo "<li>";
            echo "<a class='logo_dropdown_link' target = '_blank' rel='noopener noreferrer' href='".help_url()."'><i class='fa fa-book fa-fw'></i><span>"._('Documents')."</span></a>";
            echo "</li>";
				
		    echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/access/logout.php?'><i class='fa fa-sign-out fa-fw'></i><span>"._('Logout')."</span></a>";
            echo "</li>";
                
            echo "</ul></div>";
                
            echo "<div class='dropdown add_dropdown'><i class='fa fa-plus fa-fw' data-toggle='dropdown'></i>";
            echo "<ul class='dropdown-menu dropdown-menu-right'>";
            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/inventory/manage/items.php?'><i class='fa fa-plus fa-fw'></i><span>"._('New Item')."</span></a>";
            echo "</li>";
                
            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/sales/manage/customers.php?'><i class='fa fa-plus fa-fw'></i><span>"._('New Customer')."</span></a>";
            echo "</li>";
                
            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/purchasing/manage/suppliers.php?'><i class='fa fa-plus fa-fw'></i><span>"._('New Supplier')."</span></a>";
            echo "</li>";
                
            echo "<li>";
            echo "<a class='logo_dropdown_link' href='".$path_to_root."/gl/gl_journal.php?NewJournal=Yes'><i class='fa fa-pencil-square-o fa-fw'></i><span>"._('Make Journal Entry')."</span></a>";
            echo "</li></ul>";
            echo "</div>";
			echo $noti->get_receipt();
        	echo $noti->get_reorder();
        	echo $noti->get_payment();
            echo "</div>";
            
			add_access_extensions();
			$applications = $_SESSION['App']->applications;
			$local_path_to_root = $path_to_root;
			$sel_app = $_SESSION['sel_app'];
            $fa_icons = array('orders'=>'tags','AP'=>'shopping-cart','stock'=>'cubes','manuf'=>'industry','assets'=>'home','proj'=>'binoculars','GL'=>'book','FrontHrm'=>'users','extendedhrm'=>'users','school'=>'graduation-cap', 'kanban' => 'tasks', 'pos'=>'shopping-basket', 'grm'=>'commenting-o', 'trade_finance'=>'money', 'weigh_bridge'=>'balance-scale', 'additional_fields'=>'plus-square', 'booking'=>'check-square-o', 'hospital'=>'hospital-o', 'Projects'=>'check-square-o', 'system'=>'cogs');
                
			echo "<div class='frontBar fa-collapsible-nav'>";
			
			echo "<center class='frontBarRight fa-collapsible-menu'>";

			echo "<div id='panel_close_box'><a href='#' class='fa fa-chevron-left fa-2x' id='module_panel_close'></a></div>";

			foreach($applications as $app) {
				
                if ($_SESSION['wa_current_user']->check_application_access($app)) {
					if ($sel_app == $app->id)
                    	$sel_application = $app;
                    $acc = access_string($app->name);
                    $ap_title = str_replace(array('<u>','</u>'), '', $acc[0]);
                    if(!empty($app->hidden_title))
                    	$ap_title = $app->hidden_title;
                    echo "<a class='".($sel_app == $app->id ? 'selected' : 'menu_tab')."' href='".$local_path_to_root."/index.php?application=".$app->id."' title='".$ap_title."' $acc[1]><i class='fa fa-".@$fa_icons[$app->id]." fa-2x nav-icon'></i><span class='nav-text'>".$acc[0]."</span></a>";
                }
			}
			echo "</center>"; //frontBarRight

			echo "<button hidden>"._('More').'&#9662;'."</button><div class='hidden-menu-links hidden'></div>";

			echo "</div>"; //frontBar
				
			echo "<div class='tabs main-menu'>";

			foreach ($sel_application->modules as $module) {

                echo "<span class='menu_head'>".$module->name."</span>\n";
                echo "<div class='menu_body'>\n";
                $apps = array();
                foreach ($module->lappfunctions as $appfunction)
                    $apps[] = $appfunction;
                foreach ($module->rappfunctions as $appfunction)
                    $apps[] = $appfunction;
                // $application = array();
                foreach ($apps as $application) {

					$icon = $this->get_icon($application->category);
                    $lnk = access_string($application->label);
                    $lnk_title = str_replace(array('<u>','</u>'), '', $lnk[0]);

                    if ($_SESSION["wa_current_user"]->can_access_page($application->access)) {
                        if ($application->label != "") {
							if(basename($application->link) == basename($_SERVER['REQUEST_URI']))
								echo "<a href='".$path_to_root."/$application->link' class='cur_app' title='".$lnk_title."'>".$icon.$lnk[0]."</a>";
							else
								echo "<a href='".$path_to_root."/$application->link' title='".$lnk_title."'>".$icon.$lnk[0]."</a>";
                        }
                    }
                    elseif (!$_SESSION["wa_current_user"]->hide_inaccessible_menu_items())
                        echo "<a href='#' class='disabled' title='".$lnk_title."'>".$icon.$lnk[0]."</a>";
                }
                echo "</div>"; //menu_body
            }
			echo "<div id='night-mode'><i class='material-icons' title='Night Mode'>brightness_4</i></div>";
			echo "</div>"; //main-menu
			
			echo "<div class='right-body'>";
			
			if ($title && !$is_index) {
				foreach($applications as $app) {
					if($app->id == $sel_app)
						$sel_app_name = access_string($app->name)[0];
				}
				echo "<div id='title'><div class='titletext'><div>".$sel_app_name."</div>&nbsp;<i class='fa fa-angle-right'></i>&nbsp;".$title."</div><a target = '_blank' rel='noopener noreferrer' href='".help_url()."' title='"._('Help')."'><i class='fa fa-question-circle fa-lg'></i></a><div id='hints_container'>".(user_hints() ? "<span id='hints'></span>" : '')."</div></div>";
			}
		}
	}

	function menu_footer($no_menu, $is_index) {
		global $version, $path_to_root, $Pagehelp, $Ajax, $SysPrefs;

		include_once($path_to_root.'/includes/date_functions.inc');
        echo "</div>";
            
		if ($no_menu == false) {
			echo "<div id='footer'>";
			echo "<div class='footer'><a target='_blank' href='".$SysPrefs->power_url."' tabindex='-1'>".$SysPrefs->app_title." $version - "._("Theme:")." ".user_theme()." - ".show_users_online()."</a></div>";

			echo "<div class='footer'><a target='_blank' href='".$SysPrefs->power_url."' tabindex='-1'>".$SysPrefs->power_by."</a></div>";

			echo "</div>";
		}
		$sep = $SysPrefs->dateseps[user_date_sep()];
		switch (user_date_format()) {
			case 0:
				$user_date_format = 'mm'.$sep.'dd'.$sep.'yy';
				break;
			case 1:
				$user_date_format = 'dd'.$sep.'mm'.$sep.'yy';
				break;
			case 2:
				$user_date_format = 'yy'.$sep.'mm'.$sep.'dd';
				break;
			case 3:
				$user_date_format = 'MM'.$sep.'dd'.$sep.'yy';
				break;
			case 4:
				$user_date_format = 'dd'.$sep.'MM'.$sep.'yy';
				break;
			case 5:
				$user_date_format = 'yy'.$sep.'MM'.$sep.'dd';
				break;
			default:
				$user_date_format = 'dd'.$sep.'mm'.$sep.'yy';
		}
		
    	echo "<script type='text/javascript'>
	
			$(document).ready(function() {
				$('select:not([multiple]):not([name=backups])').select2({dropdownAutoWidth : true});
				$('select').on('select2:close', function() { $(this).focus(); });

				$('select').on('select2:open', function(e){
					if($('.select2-dropdown > div').length > 0)
						$('.select2-dropdown').find('div').remove();

					var onclick_val = $(e.target).parent().siblings('img').attr('onclick');
					if(typeof onclick_val != 'undefined') {
						$('.select2-dropdown').append('<div><i class=\"fa fa-search\"></i>"._('Advanced Search')."</div>');
						$('.select2-dropdown > div').attr('onclick', onclick_val).click(function(){
							$('select').select2('close');
						});
					}
					$.each(editors, function(key, val) {
						if($(e.target).attr('id') == val[1]) {
							$('.select2-dropdown').append('<div key=\"'+key+'\"><i class=\"fa fa-plus\"></i>"._('Add new')." (F' + (key-111) + ')</div>');
							$('.select2-dropdown > div').attr('key', key).click(function(){
								var e2 = jQuery.Event('keydown');
                                e2.keyCode = key;                     
                                $('.select2-dropdown').trigger(e2);
							    $('select').select2('close');
						    });
						}
					});
				});
				
				//$(document).tooltip().off('focusin focusout');
				//$('.ajaxsubmit, input[type=checkbox], .editbutton').tooltip().click(function() {
				//	$(this).tooltip('close');
				//})

    			$('.date').datepicker({
    				onSelect:function(){
    					$(this).attr('_last_val', this.value);
    					if($(this).hasClass('active'))
    						$('button[name=datepicker_hidden_btn]').trigger('click');
    				},
					dateFormat: '$user_date_format',
        			changeMonth: true,
        			changeYear: true,
					showWeek: true,
      				firstDay: 1,
					showOn: 'button',
      				buttonImage: '$path_to_root/themes/flat/images/calendar_grey.svg',
      				buttonImageOnly: true,
      				buttonText: 'Select date'
    			});
			
				$('#menu_toggle').click(function() {
    				$('.right-body').toggleClass('full_width').toggleClass('has_side_bar');
					$('.main-menu').toggleClass('deactive_sidebar').toggleClass('active_sidebar');
					$('.frontBar').toggleClass('frontBar_full_width');
					$('#footer .footer').toggleClass('footer_full_width');
					if($(window).width() < 481)
						$('html, body').animate({scrollTop: '0px'}, 0);
					localStorage.setItem('hide_sidebar', $('.main-menu').hasClass('deactive_sidebar'));
				});
				$('#module_panel_open, #module_panel_close').click(function() {
    				$('.frontBar').toggleClass('frontBar_full_screen');
					$('.right-body').toggleClass('right_body_hide');
				});
				if(localStorage.getItem('dark') == 'true') {
					$('.main-menu').addClass('dark-menu');
					$('.frontBar').addClass('dark-frontBar');
				}
				if(localStorage.getItem('hide_sidebar') == 'true') {
					$('.main-menu').addClass('deactive_sidebar');
					$('.right-body').addClass('full_width');
					$('.frontBar').addClass('frontBar_full_width');
				}
				$('#night-mode').click(function() {
					$('.main-menu').toggleClass('dark-menu');
					$('.frontBar').toggleClass('dark-frontBar');
					localStorage.setItem('dark', $('.main-menu').hasClass('dark-menu'));
				});
				$('a[data-toggle=\'tab\']').on('shown.bs.tab', function (e) {
					$(e.currentTarget.hash).find('.ct-chart').each(function(el, tab) {
						tab.__chartist__.update();
					});
				});
			});
		</script>";
	}

	function display_applications(&$waapp) {
		global $path_to_root, $SysPrefs;

		include_once($path_to_root.'/reporting/includes/class.graphic.inc');
		include_once('charts.inc');
		echo "<link rel='stylesheet' type='text/css' href='".$path_to_root."/themes/".user_theme()."/local_style/dashboard.css'>";
		$js = '';
		if ($SysPrefs->use_popup_windows)
			$js .= get_js_open_window(800, 500);

		page(_($help_context = 'Dashboard'), false, false, '', $js);
		if (isset($_GET['application']))
			dashboard($_GET['application']);
		else
			dashboard('GL');
		end_page();
  	}
}
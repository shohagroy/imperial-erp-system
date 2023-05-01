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
	class renderer
	{
		function get_icon($category)
		{
			global  $path_to_root, $SysPrefs;

			if ($SysPrefs->show_menu_category_icons)
				$img = $category == '' ? 'right.gif' : $category.'.png';
			else	
				$img = 'right.gif';
			return "<img src='$path_to_root/themes/".user_theme()."/images/$img' style='vertical-align:middle;' border='0'>&nbsp;&nbsp;";
		}

		function wa_header()
		{
			page(_($help_context = "Main Menu"), false, true);
		}

		function wa_footer()
		{
			end_page(false, true);
		}

		function menu_header($title, $no_menu, $is_index)
		{
			global $path_to_root, $SysPrefs, $db_connections;
			echo "<table class='callout_main' border='0' cellpadding='0' cellspacing='0'>\n";
			echo "<tr>\n";
			echo "<td colspan='2' rowspan='2'>\n";

			echo "<table class='main_page' border='0' cellpadding='0' cellspacing='0'>\n";
			echo "<tr>\n";
			echo "<td>\n";
			echo "<table width='100%' border='0' cellpadding='0' cellspacing='0'>\n";
			echo "<tr>\n";
			echo "<td class='quick_menu'>\n"; // tabs



			$indicator = "$path_to_root/themes/".user_theme(). "/images/ajax-loader.gif";

			

			if (!$no_menu)
			{
				$applications = $_SESSION['App']->applications;
				$local_path_to_root = $path_to_root;
				$sel_app = $_SESSION['sel_app'];

				// top status bar
				$rimg = "<img src='$path_to_root/themes/".user_theme()."/images/report.png'  class='top_right_menu' alt='"._('Dashboard')."'>&nbsp;&nbsp;";
				$pimg = "<img src='$local_path_to_root/themes/".user_theme()."/images/preferences.gif' class='top_right_menu' alt='"._('Preferences')."'>&nbsp;&nbsp;";
				$limg = "<img src='$local_path_to_root/themes/".user_theme()."/images/lock.gif'  class='top_right_menu' alt='"._('Change Password')."'>&nbsp;&nbsp;";
				$img = "<img src='$local_path_to_root/themes/".user_theme()."/images/login.gif'  class='top_right_menu' alt='"._('Logout')."'>&nbsp;&nbsp;";
				$himg = "<img src='$local_path_to_root/themes/".user_theme()."/images/help.gif'  class='top_right_menu' alt='"._('Help')."'>&nbsp;&nbsp;";
				echo "<table class='logoutBar'>";
				echo "<tr><td class='headingtext3'>" . $db_connections[user_company()]["name"] . " | " . $_SERVER['SERVER_NAME'] . " | " . $_SESSION["wa_current_user"]->name . "</td>";
				echo "<td class='logoutBarRight'><img id='ajaxmark' src='$indicator' align='center' style='visibility:hidden; width:35px;' alt='ajaxmark'></td>";
				echo "<td class='logoutBarRight'><a href='$path_to_root/admin/dashboard.php?sel_app=$sel_app'>$rimg" . _("Dashboard") . "</a>&nbsp;&nbsp;&nbsp;\n";
				
				echo "<a class='shortcut' href='$path_to_root/admin/display_prefs.php?'>$pimg" . _("Preferences") . "</a>&nbsp;&nbsp;&nbsp;\n";
				echo "  <a class='shortcut' href='$path_to_root/admin/change_current_user_password.php?selected_id=" . $_SESSION["wa_current_user"]->username . "'>$limg" . _("Change password") . "</a>&nbsp;&nbsp;&nbsp;\n";

				if ($SysPrefs->help_base_url != null)
				{
					echo "<a target = '_blank' onclick=" .'"'."javascript:openWindow(this.href,this.target); return false;".'" '. "href='". help_url()."'>$himg" . _("Help") . "</a>&nbsp;&nbsp;&nbsp;";
				}
				echo "<a class='shortcut' href='$local_path_to_root/access/logout.php?'>$img" . _("Logout") . "</a>&nbsp;&nbsp;&nbsp;";
				echo "</td></tr><tr><td colspan=3>";
				echo "</td></tr></table>";



				echo "<table cellpadding='0' cellspacing='0' width='100%'><tr><td>";
				echo "<nav> <ul>";
				foreach($applications as $app)
				{
                    if ($_SESSION["wa_current_user"]->check_application_access($app))
                    {
                        $acc = access_string($app->name);
                        echo "<li class='".($sel_app == $app->id ? 'selected' : 'menu_tab')
                            ."' > <a  href='$local_path_to_root/index.php?application=".$app->id
                            ."'$acc[1]>" .$acc[0] . "</a> </li>";
                    }
				}
				echo "</ul></nav>";
				echo "</td></tr></table>";
				
			}
			echo "</td></tr></table>";

			if ($no_menu)
			{	// ajax indicator for installer and popups
				echo "<center><table class='tablestyle_noborder'>"
					."<tr><td><img id='ajaxmark' src='$indicator' align='center' style='visibility:hidden;' alt='ajaxmark'></td></tr>"
					."</table></center>";
			} elseif ($title && !$is_index)
			{
				echo "<center><table id='title'><tr><td width='100%' class='titletext'>$title</td>"
				."<td align=right>"
				.(user_hints() ? "<span id='hints'></span>" : '')
				."</td>"
				."</tr></table></center>";
			}
		}

		function menu_footer($no_menu, $is_index)
		{
			global $version, $path_to_root, $Pagehelp, $Ajax, $SysPrefs;

			include_once($path_to_root . "/includes/date_functions.inc");

			echo "</td></tr></table>\n"; // 'main_page'
			
			echo "</td></tr> </table>\n"; // 'callout_main'
			if ($no_menu == false)
			{
				echo "<table align='center' id='footer'>\n";
				echo "<tr>\n";
				echo "<td style='text-align:left; line-height:2em;' ><a target='_blank' href='".$SysPrefs->power_url."' tabindex='-1'>".$SysPrefs->app_title
					." $version </td> <td style='text-align:right;' > " . _("Theme:") . " <a href='http://www.kvcodes.com' target='_blank'>  " . user_theme() . "</a> ".($SysPrefs->show_users_online == 1 ? " - ". show_users_online() : '')."</font></a></td>\n";

					if (isset($_SESSION['wa_current_user'])) {
					$phelp = implode('; ', $Pagehelp);
					echo "<td class='bottomBarCell'>" . Today() . " | " . Now() . "</td>\n";
					$Ajax->addUpdate(true, 'hotkeyshelp', $phelp);
					echo "<td id='hotkeyshelp'>".$phelp."</td>";
				}
				echo "</tr>\n";
				
				echo "</table>\n";
			}
		}

		function display_applications(&$waapp)
		{
			global $path_to_root;

			$selected_app = $waapp->get_selected_application();
			if (!$_SESSION["wa_current_user"]->check_application_access($selected_app))
				return;

			if (method_exists($selected_app, 'render_index'))
			{
				$selected_app->render_index();
				return;
			}

			echo "<table width='100%' cellpadding='0' cellspacing='0'>";
			foreach ($selected_app->modules as $module)
			{
        		if (!$_SESSION["wa_current_user"]->check_module_access($module))
        			continue;
				// image
				echo "<tr>";
				// values
				echo "<td valign='top' class='menu_group'>";
				echo "<table border=0 width='100%'>";
				echo "<tr><td class='menu_group' colspan='2'>";
				echo $module->name;
				echo "</td></tr><tr>";
				echo "<td class='menu_group_items'>";

				foreach ($module->lappfunctions as $appfunction)
				{
					$img = $this->get_icon($appfunction->category);
					if ($appfunction->label == "")
						echo "&nbsp;<br>";
					elseif ($_SESSION["wa_current_user"]->can_access_page($appfunction->access)) 
					{
							echo $img.menu_link($appfunction->link, $appfunction->label)."<br>\n";
					}
					elseif (!$_SESSION["wa_current_user"]->hide_inaccessible_menu_items())
					{
							echo $img.'<span class="inactive">'
								.access_string($appfunction->label, true)
								."</span><br>\n";
					}
				}
				echo "</td>";
				if (sizeof($module->rappfunctions) > 0)
				{
					echo "<td width='50%' class='menu_group_items'>";
					foreach ($module->rappfunctions as $appfunction)
					{
						$img = $this->get_icon($appfunction->category);
						if ($appfunction->label == "")
							echo "&nbsp;<br>";
						elseif ($_SESSION["wa_current_user"]->can_access_page($appfunction->access)) 
						{
								echo $img.menu_link($appfunction->link, $appfunction->label)."<br>\n";
						}
						elseif (!$_SESSION["wa_current_user"]->hide_inaccessible_menu_items())
						{
								echo $img.'<span class="inactive">'
									.access_string($appfunction->label, true)
									."</span><br>\n";
						}
					}
					echo "</td>";
				}

				echo "</tr></table></td></tr>";
			}
			echo "</table>";
  		}
	}

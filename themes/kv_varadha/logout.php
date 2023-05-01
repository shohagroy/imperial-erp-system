<?php 
page_header(_("Logout"), true, false, '');

echo "<table width='100%' border='0'>
  
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><div align='center'><font size=2>";
echo _("Thank you for using") . " ";

echo "<strong>$app_title $version</strong>";

echo "</font></div></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><div align='center'>";
echo "<a href='$path_to_root/index.php'><b>" . _("Click here to Login Again.") . "</b></a>";
echo "</div></td>
  </tr>
</table>
<br>\n";
end_page(false, true);
session_unset();
@session_destroy();

?>
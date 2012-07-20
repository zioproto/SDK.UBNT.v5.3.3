#!/sbin/cgi
<?
include("lib/settings.inc");
if (fileinode($cfg_file_bak) != -1) {
$cfg = @cfg_load($cfg_file_bak);
} else {
$cfg = @cfg_load($cfg_file);
}
include("lib/l10n.inc");
include("lib/misc.inc");
>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title><? echo get_title($cfg, dict_translate("DHCP Client Information")); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
<script type="text/javascript" language="javascript" src="jsl10n.cgi"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/dhcpc.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/js/jquery.utils.js"></script>
</head>

<body class="popup">
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/dhcpc.js"></script>
<script type="text/javascript" language="javascript">
//<!--
function dhcpcRefresh() {
	dhcpc.fetch();
	if (typeof reloadStatus == 'function') {
		reloadStatus();
	}
	return false;
}

$(document).ready(function() {
	$('.msg').hide();
	$('.data').hide();
	dhcpc.fetch(1);
});
//-->
</script>
<br>
<form enctype="multipart/form-data" action="<? echo $PHP_SELF; ?>" method="POST">
<table cellspacing="0" cellpadding="0" align="center">
<tr><td colspan="2">
<table cellspacing="0" cellpadding="0" class="listhead" id="dhcpcinfo">
	<tr><th colspan="2"><? echo dict_translate("DHCP Client Information"); ></th></tr>
	<tr class="msg initial_hide"><td colspan="2" valign="top"><span id="status_msg"></span></td></tr>
	<tr class="data initial_hide" id="dhcp0"><td valign="top">
	<table width="50%">
	<tr><td class="f"><? echo dict_translate("IP Address:");></td><td><span class="ipaddr"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Netmask:");></td><td><span class="mask"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Gateway:");></td><td><span class="gw"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Primary DNS IP:");></td><td><span class="dns1"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Secondary DNS IP:");></td><td><span class="dns2"></span></td></tr>
	</table>
	</td><td valign="top">
	<table width="50%">
	<tr><td class="f"><? echo dict_translate("DHCP Server:");></td><td><span class="serverid"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Domain:");></td><td><span class="domain"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Total Lease Time:");></td><td><span class="leasetime"></span></td></tr>
	<tr><td class="f"><? echo dict_translate("Remaining Lease Time:");></td><td><span class="leasetime_left"></span></td></tr>
	</table>
	</td></tr>
</table>
</td></tr>
<tr><td>&nbsp;</td><td class="change">
<input type="button" id="ctrl_renew" class="data ctrl" onClick="return dhcpc.renew();" value=" <? echo dict_translate("Renew"); > ">
<input type="button" id="ctrl_release" class="data ctrl" onClick="return dhcpc.release();" value=" <? echo dict_translate("Release"); > ">
<input type="button" id="ctrl_refresh" class="ctrl" onClick="return dhcpcRefresh();" value=" <? echo dict_translate("Refresh"); > ">
</td>
</tr>
</table>
</form>
  
</body>
</html>


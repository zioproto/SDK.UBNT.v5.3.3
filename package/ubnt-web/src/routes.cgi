#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/link.inc");

/*
Bridge:
ebtables FORWARD -i [LAN|WLAN] -p 0x0800 --ip-src [!] address[/mask] --ip-dst [!] address[/mask] --ip-protocol [6(TCP)|17(UDP)|1(ICMP)|0(IP)] --ip-sport [!] port1[:port2] --ip-dport [!] port1[:port2] -j [DROP|ACCEPT|CONTINUE]

Router
iptables FORWARD -i [LAN|WLAN|PPP] [--protocol [6(TCP)|17(UDP)|1(ICMP)|0(IP)] | -m ipp2p --ipp2p]  --src [!] address[/mask] --dst [!] address[/mask] --sport [!] port1[:port2] --dport [!] port1[:port2] -j [REJECT|DROP|ACCEPT]
*/

$var_names[] = "ip";
$var_names[] = "netmask";
$var_names[] = "gateway";
/* devname currently isn't used for non default routes */
/* $var_names[] = "devname"; */
$var_names[] = "comment";
$var_names[] = "status";
$var_names_count = count($var_names);

$i = 0;
while ($i < 20)
{
	$i++;
	$j = 0;
	$route_cfg = cfg_get_route_cfg($cfg, $i, $var_names);
	while ($j < $var_names_count)
	{
		$var = $var_names[$j] + $i;
		if ($REQUEST_METHOD != "POST" && !isset($$var))
		{
			$$var = $route_cfg[$j];
		}
		/* echo "" + $i + ": " + $var + " -> " + $$var + "<br>\n"; */
		$j++;
	}
}

$saved = 0;
if ($REQUEST_METHOD == "POST" && $action == "routestore")
{
	$i = 0;
	while ($i < 20)
	{
		$i++;
		$j = 0;
		while ($j < $var_names_count)
		{
			$var = $var_names[$j] + $i;
			$values[$j] = $$var;
			$j++;
		}
		set_route_cfg($cfg, $i, $var_names, $values);
	}
	cfg_save($cfg, $cfg_file);
	cfg_set_modified($cfg_file);
	$saved = 1;
}

><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
 "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title><? echo get_title($cfg, dict_translate("Static Routes")); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
<script type="text/javascript" language="javascript" src="jsl10n.cgi"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/jsval.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/network.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/util.js"></script>
<script type="text/javascript" language="javascript">
//<!--
<? if ($saved) { >
if (window.opener && !window.opener.closed && window.opener.doSubmit)
	window.opener.doSubmit();
window.close();
<? } >

function updateEnabled(checkbox, hidden)
{
	if (checkbox.checked)
	{
		hidden.value = "enabled";
	}
	else
	{
		hidden.value = "disabled";
	}
}

var line_chain = new Array("ip", "netmask", "gateway");

function validateLine(name, idx, value)
{
	var i;
	var dep;

	if (value != '')
	{
		/* Not empty - OK */
		return true;
	}

	dep = document.getElementById("status_check" + idx);
	if (dep.checked)
	{
		/* Cannot be empty when enabled */
		return false;
	}

	for (i = 0; i < line_chain.length; ++i)
	{
		if (name != line_chain[i])
		{
			dep = document.getElementById(line_chain[i] + idx);
			if (dep)
			{
				if (dep.value.replace(/^\s+|\s+$/g, '') != '')
				{
					/* Cannot be empty when others are not empty. */
					return false;
				}
			}
		}
	}
	return true;
}

function convert_ip_to_integer(value)
{
	var quad = value.split('.');
	return 16777216 * parseInt(quad[0]) + 65536 * parseInt(quad[1]) +
		256 * parseInt(quad[2]) + parseInt(quad[3]);
}

function validateRouteTargetNetwork(ip, netmask)
{
	var ip_int;
	var bitmask;

	ip_int = convert_ip_to_integer(ip);
	bitmask = convert_ip_to_integer(netmask);

	/* masked network ip should be equal to non masked */
	return ((ip_int & bitmask) == (ip_int & ip_int));
}

function validateRouteTargetIP(idx, value)
{
	var mask;
	var mask_value;

	if (!_validateNonZeroIP(value))
	{
		return false;
	}
	mask = document.getElementById("netmask" + idx);
	if (!mask)
	{
		/* no mask field - do not allow this */
		return false;
	}
	mask_value = mask.value.replace(/^\s+|\s+$/g, '');
	if (!_validateNetmask(mask_value))
	{
		/* If netmask is invalid - let it go, netmask field
		 * validator will fail */
		return true;
	}
	/* check the network */
	return validateRouteTargetNetwork(value, mask_value);
}

function validateRouteLine(id, name, value)
{
	var col_name;
	var idx;

	col_name = id.replace(/^([^\d]+)(\d+)$/, "$1");
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");

	value = value.replace(/^\s+|\s+$/g, '');
	if (!validateLine(col_name, idx, value))
	{
		return false;
	}
	if (value == '')
	{
		/* This was allowed by validateLine() */
		return true;
	}
	if (col_name == 'ip')
	{
		return validateRouteTargetIP(idx, value);
	}
	else if (col_name == 'netmask')
	{
		return _validateNetmask(value);
	}
	else if (col_name == 'gateway')
	{
		return _validateNonZeroIP(value);
	}
	return false;
}

//-->
</script>
</head>
<body class="popup">
  <form name="routes" enctype="multipart/form-data" action="<? echo $PHP_SELF;>"
  method="POST" onSubmit="return validateStandard(this, 'error');">
	<table cellspacing="0" cellpadding="0" align="center" class="popup">
	  <tr><th colspan="3"><? echo dict_translate("Static Routes"); ></th></tr>
	  <tr>
		<td colspan="3">
		  <? include("lib/error.tmpl");>
		  <br>
		  <table cellspacing="0" cellpadding="0">
			<tr>
			  <td>&nbsp;</td>
			  <td><? echo dict_translate("Target Network IP"); >&nbsp;</td>
			  <td><? echo dict_translate("Netmask"); >&nbsp;</td>
			  <td><? echo dict_translate("Gateway IP"); >&nbsp;</td>
			  <td><? echo dict_translate("Comment"); >&nbsp;</td>
			  <td><? echo dict_translate("On"); ></td>
			</tr>
<?
$i = 0;
while ($i < 20)
{
	$i++;
	$j = 0;
	while ($j < $var_names_count)
	{
		$var = $var_names[$j] + $i;
		$var_name = $var_names[$j];
		$$var_name = $$var;
		$j++;
	}
>			<tr>
			  <td class="h"><? echo $i;>.</td>

			  <td><input type="text" style="width: 145px;" name="ip<? echo $i;>"
			  id="ip<? echo $i;>" value="<? echo $ip;>" size="16" maxlength="15"
			  req="1" callback="validateRouteLine"
			  realname="<? echo dict_translate("Target Network IP"); >"></td>
			  <td><input type="text" style="width: 145px;" name="netmask<? echo $i;>"
			  id="netmask<? echo $i;>" value="<? echo $netmask;>" size="16" maxlength="15"
			  req="1" callback="validateRouteLine"
			  realname="<? echo dict_translate("Netmask"); >"></td>
			  <td><input type="text" style="width: 145px;" name="gateway<? echo $i;>"
			  id="gateway<? echo $i;>" value="<? echo $gateway;>" size="16" maxlength="15"
			  req="1" callback="validateRouteLine"
			  realname="<? echo dict_translate("Target Network IP"); >"></td>
			  <td><input type="text" style="width: 145px;" name="comment<? echo $i;>"
			  id="comment<? echo $i;>" value="<? echo $comment;>" size="9"></td>

			  <td><input type="hidden" name="status<? echo $i;>"
			  id="status<? echo $i;>" value="<?echo $status>">
			  <input type="checkbox" name="status_check<? echo $i;>"
			  id="status_check<? echo $i;>"
			  onClick="updateEnabled(this, this.form.status<? echo $i;>);"
			  <? if ($status == "enabled") {echo "checked";}> ></td>
			</tr>
<?
}
>
			<tr>
			  <td colspan="6">&nbsp;</td>
			</tr>
			<tr>
			  <td colspan="6" class="centered">&nbsp;<input type="hidden" name="action"
			  value="routestore">
				<input type="submit" name="route_submit"
				value="<? echo dict_translate("Save")>">
				<input type="button" name="cancel"
				value="<? echo dict_translate("Cancel")>"
				onClick="window.close()">
			  </td>
			</tr>
		  </table>
		</td>
	  </tr>
	</table>
  </form>
</body>
</html>

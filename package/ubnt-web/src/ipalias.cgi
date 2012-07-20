#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/link.inc");

$var_names[] = "ip";
$var_names[] = "netmask";
$var_names[] = "enabled";
$var_names[] = "comment";
$var_names_count = count($var_names);

$i = 0;
while ($i < $ipaliases_max)
{
	$i++;
	$j = 0;
	$ipalias_cfg = cfg_get_ipalias_cfg($cfg, $iface, $i, $var_names);
	while ($j < $var_names_count)
	{
		$var = $var_names[$j] + $i;
		if (!isset($$var))
		{
			$$var = $ipalias_cfg[$j];
		}
		/* echo "" + $i + ": " + $var + " -> " + $$var + "<br>\n"; */
		$j++;
	}
}

$saved = 0;
if ($REQUEST_METHOD == "POST" && $action == "iastore")
{
	$i = 0;
	while ($i < $ipaliases_max)
	{
		$i++;
		$j = 0;
		while ($j < $var_names_count)
		{
			$var = $var_names[$j] + $i;
			$values[$j] = $$var;
			$j++;
		}
		set_ipalias_cfg($cfg, $iface, $i, $var_names, $values);
	}
	cfg_save($cfg, $cfg_file);
	cfg_set_modified($cfg_file);
	$saved = 1;
}

if ($iface == "ath0")
{
	$title = dict_translate("WLAN IP Aliases");
}
elseif ($iface == "eth0")
{
	$title = dict_translate("LAN IP Aliases");
}
elseif ($iface == "eth1")
{
	if (1) { /* TODO check netmode */
		$title = dict_translate("LAN2 IP Aliases");
        } else {
		$title = dict_translate("WAN IP Aliases");
        }
}
elseif ($iface == "br0")
{
	$title = dict_translate("Bridge IP Aliases");
}
else
{
	$title = dict_translate("IP Aliases");
}

>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title><? echo get_title($cfg, $title); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
<script type="text/javascript" language="javascript" src="jsl10n.cgi"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/jsval.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/network.js"></script>
<script type="text/javascript" language="javascript">
//<!--
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

<? if ($saved) { >
if (window.opener && !window.opener.closed && window.opener.doSubmit)
	window.opener.doSubmit();
window.close();
<? } >

var line_chain = new Array("ip", "netmask");

function validateLine(id, value)
{
	var name;
	var idx;
	var i;
	var dep;

	if (value != '')
	{
		return true;
	}

	name = id.replace(/^([^\d]+)(\d+)$/, "$1");
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");

	dep = document.getElementById("active" + idx);
	if (dep.checked)
	{
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
					return false;
				}
			}
		}
	}
	return true;
}

function validateIAIP(id, name, value)
{
	value = value.replace(/^\s+|\s+$/g, '');
	if (value == '' || _validateIP(value))
	{
		return validateLine(name, value);
	}
	return false;
}

function validateIANetmask(id, name, value)
{
	value = value.replace(/^\s+|\s+$/g, '');
	if (value == '' || _validateNetmask(value))
	{
		return validateLine(name, value);
	}
	return false;
}
//-->
</script>
</head>
<body class="popup">
	<form name="ipalias" enctype="multipart/form-data" action="ipalias.cgi"
	method="POST" onSubmit="return validateStandard(this, 'error');">
		<table cellspacing="0" cellpadding="0" align="center" class="popup">
			<tr><th colspan="3"><? echo $title; ></th></tr>
    		<tr>
		<td colspan="3">
    <? include("lib/error.tmpl");>
    <br>
		  <table cellspacing="0" cellpadding="0">
		  <tr>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("IP"); ></td>
		  <td><? echo dict_translate("Netmask"); ></td>
		  <td><? echo dict_translate("Comment"); ></td>
		  <td><? echo dict_translate("Enabled"); ></td>
		  </tr>
<?
$i = 0;
while ($i < $ipaliases_max)
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

>
		  <tr>
		  <td class="h"><? echo $i;>.</td>
		  <td><input type="text" style="width: 145px;" name="ip<? echo $i;>" id="ip<? echo $i;>"
		  value="<? echo $ip;>" size="16" maxlength="15"
		  req="1" callback="validateIAIP" realname="<? echo dict_translate("IP"); >"></td>
		  <td><input type="text" style="width: 145px;" name="netmask<? echo $i;>" id="netmask<? echo $i;>"
		  value="<? echo $netmask;>" size="16" maxlength="15"
		  req="1" callback="validateIANetmask" realname="<? echo dict_translate("Netmask"); >"></td>
		  <td><input type="text"  style="width: 145px;"name="comment<? echo $i;>" id="comment<? echo $i;>" value="<? echo $comment;>" size="16"></td>
		  <td>
		  	<input type="hidden" name="enabled<? echo $i;>"
		  		id="enabled<? echo $i;>" value="<?echo $enabled>">
		  	<input type="checkbox" name="active<? echo $i;>"
		  	id="active<? echo $i;>" onClick="updateEnabled(this, this.form.enabled<? echo $i;>);"
		  	<? if ($enabled == "enabled") {echo "checked";}> >
                  </td>
		  </tr>
<?
}
>
		  <tr>
		    <td colspan="5">&nbsp;</td>
		  </tr>
		  <tr>
		    <td colspan="2">&nbsp;<input type="hidden" name="action" value="iastore"><input type="hidden" name="iface" value="<? echo $iface;>"></td>
		    <td colspan="2">
		    <input type="submit" name="ia_submit" value="<? echo dict_translate("Save")>">
		    <input type="button" name="cancel" value="<? echo dict_translate("Cancel")>"
		    onClick="window.close()">
		    </td>
		    <td>&nbsp;</td>
		  </tr>
		  </table>
		</td>
		</table>
	</form>
</body>
</html>

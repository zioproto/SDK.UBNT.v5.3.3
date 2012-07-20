#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/link.inc");

$var_names[] = "src_ip";
$var_names[] = "src_port";
$var_names[] = "dst_port";
$var_names[] = "dst_ip";
$var_names[] = "protocol";
$var_names[] = "enabled";
$var_names[] = "comment";
$var_names_count = count($var_names);

$i = 0;
while ($i < 20)
{
	$i++;
	$j = 0;
	$port_forward_cfg = cfg_get_port_forward_cfg($cfg, $i, $var_names);
	while ($j < $var_names_count)
	{
		$var = $var_names[$j] + $i;
		if (!isset($$var))
		{
			$$var = $port_forward_cfg[$j];
		}
		/* echo "" + $i + ": " + $var + " -> " + $$var + "<br>\n"; */
		$j++;
	}
}

$saved = 0;
if ($REQUEST_METHOD == "POST" && $action == "pfstore")
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
		set_port_forward_cfg($cfg, $i, $var_names, $values);
	}
	cfg_save($cfg, $cfg_file);
	cfg_set_modified($cfg_file);
	$saved = 1;
}

>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title><? echo get_title($cfg, dict_translate("Port Forwarding")); ></title>
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

var line_chain = new Array("dst_ip", "dst_port", "src_port");

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

function isValidPort(port) {
	var portNum = parseInt(port);
	if ((portNum < 1) || (portNum > 65535))
		return false;
	return true;
}

function validatePFPort1(id, name, value)
{
	return validatePFPort(id, name, value, ':');
}

function validatePFPort2(id, name, value) {
	if (!value)
		return true;
	value = value.replace(/^\s+|\s+$/g, '');
	if (value.length == 0)
		return true;
	return isValidPort(value);
}

function validatePFPort(id, name, value, delim)
{
	/* trim */
	value = value.replace(/^\s+|\s+$/g, '');
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");
	if (value != '')
	{
                if (delim && (pos = value.indexOf(delim)) >= 0) {
                        from = value.substr(0, pos);
                        to = value.substr(pos+1);
                        if (isNaN(from) || isNaN(to))
                                return false;
                        intfrom = parseInt(from);
                        intto = parseInt(to);
                        if (intfrom != from || intto != to ||
                            intfrom < 1 || intfrom > 65535 ||
                            intto < 1 || intto > 65535 ||
                            intfrom > intto)
				return false;
                }
                else if (isNaN(value) || parseInt(value) != value || parseInt(value) < 1 || parseInt(value) > 65535) {
			return false;
                }
	}
	return validateLine(name, value);
}

function validatePFIP(id, name, value)
{
	value = value.replace(/^\s+|\s+$/g, '');
	if (value == '' || _validateIP(value))
	{
		return validateLine(name, value);
	}
	return false;
}

function validateSourceIP(id, name, value)
{
	value = value.replace(/^\s+|\s+$/g, '');
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");

	if (value == '')
	{
		dep = document.getElementById("active" + idx);
		if (dep.checked)
		{
			dep = document.getElementById(name);
			dep.value = "0.0.0.0/0";
		}
		return true;
	} else {
		if ((pos = value.indexOf('/')) >= 0) {
			mask = value.substr(pos+1);
			if (isNaN(mask) || parseInt(mask) != mask || parseInt(mask) < 0 ||
				parseInt(mask) > 32) {
				return false;
			}
			value = value.substr(0, pos);
		}
		if (_validateIP(value)) {
			return true;
		}
	}
	return false;
}

//-->
</script>
</head>
<body class="popup">
	<form name="traceroute" enctype="multipart/form-data" action="port_forward.cgi"
	method="POST" onSubmit="return validateStandard(this, 'error');">
		<table cellspacing="0" cellpadding="0" align="center" class="popup">
			<tr><th colspan="3"><? echo dict_translate("Port Forwarding"); ></th></tr>
    		<tr>
		<td colspan="3">
    <? include("lib/error.tmpl");>
    <br>
		  <table cellspacing="0" cellpadding="0">
		  <tr>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Private IP"); ></td>
		  <td><? echo dict_translate("Private Port"); ></td>
		  <td><? echo dict_translate("Type"); ></td>
		  <td><? echo dict_translate("Source IP/mask"); ></td>
		  <td><? echo dict_translate("Public Port"); ></td>
		  <td><? echo dict_translate("Comment"); ></td>
		  <td><? echo dict_translate("Enabled"); ></td>
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

>
		  <tr>
		  <td class="h"><? echo $i;>.</td>
		  <td><input type="text" class="std_width" name="dst_ip<? echo $i;>" id="dst_ip<? echo $i;>"
		  value="<? echo $dst_ip;>" size="11" maxlength="15"
		  req="1" callback="validatePFIP" realname="<? echo dict_translate("Private IP"); >"></td>
		  <td><input type="text" class="std_width" name="dst_port<? echo $i;>" id="dst_port<? echo $i;>"
		  value="<? echo $dst_port;>" size="6" maxlength="11"
		  req="1" callback="validatePFPort2" realname="<? echo dict_translate("Private Port [1 - 65535]"); >"></td>
		  <td><select class="std_width" name="protocol<? echo $i;>" id="protocol<? echo $i;>">
		  	<option value="tcp" <? if ($protocol == "tcp") { echo "selected"; }> ><? echo dict_translate("TCP"); ></option>
		  	<option value="udp" <? if ($protocol == "udp") { echo "selected"; }> ><? echo dict_translate("UDP"); ></option>
		  	  </select>
		  </td>
		  <td><input type="text" class="std_width" name="src_ip<? echo $i;>" id="src_ip<? echo $i;>"
		  	value="<? echo $src_ip;>" size="16" maxlength="31"
	       	  	req="1" callback="validateSourceIP" realname="<? echo dict_translate("Source IP/mask"); >">
                  </td>
		  <td><input type="text" class="std_width" name="src_port<? echo $i;>" id="src_port<? echo $i;>"
		  	value="<? echo $src_port;>" size="6" maxlength="11"
		  	req="1" callback="validatePFPort1" realname="<? echo dict_translate("Public Port (port or portX:portY)"); >" ></td>
		  	<td><input type="text" style="width: 145px;" name="comment<? echo $i;>" id="comment<? echo $i;>" value="<? echo $comment;>" size="9"></td>
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
		    <td colspan="7">&nbsp;</td>
		  </tr>
		  <tr>
		    <td colspan="3">&nbsp;<input type="hidden" name="action" value="pfstore"></td>
		    <td colspan="3">
		    <input type="submit" name="pf_submit" value="<? echo dict_translate("Save")>">
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

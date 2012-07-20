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
iptables FORWARD -i [WAN|LAN|WLAN|PPP] [--protocol [6(TCP)|17(UDP)|1(ICMP)|0(IP)] | -m ipp2p --ipp2p]  --src [!] address[/mask] --dst [!] address[/mask] --sport [!] port1[:port2] --dport [!] port1[:port2] -j [DROP|ACCEPT|REJECT]
*/

$var_names[] = "act";
$var_names[] = "input_ifc";
$var_names[] = "src_ip";
$var_names[] = "not_src_ip";
$var_names[] = "dst_ip";
$var_names[] = "not_dst_ip";
$var_names[] = "src_port";
$var_names[] = "not_src_port";
$var_names[] = "dst_port";
$var_names[] = "not_dst_port";
$var_names[] = "protocol";
$var_names[] = "enabled";
$var_names[] = "comment";
$var_names_count = count($var_names);

if (strlen($netmode) == 0) {
	$netmode = cfg_get_def($cfg, "netmode", "bridge");
}

$i = 0;
while ($i < 20)
{
	$i++;
	$j = 0;
	$firewall_cfg = cfg_get_firewall_cfg($cfg, $i, $var_names, $netmode);
	while ($j < $var_names_count)
	{
		$var = $var_names[$j] + $i;
		if ($REQUEST_METHOD != "POST" && !isset($$var))
		{
			$$var = $firewall_cfg[$j];
		}
		/* echo "" + $i + ": " + $var + " -> " + $$var + "<br>\n"; */
		$j++;
	}
}

$saved = 0;
if ($REQUEST_METHOD == "POST" && $action == "firewallstore")
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
		set_firewall_cfg($cfg, $i, $var_names, $values, $netmode);
	}
	cfg_save($cfg, $cfg_file);
	cfg_set_modified($cfg_file);
	$saved = 1;
}

><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
 "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title><? echo get_title($cfg, dict_translate("Firewall")); ></title>
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

function updateNot(checkbox, hidden)
{
	if (checkbox.checked)
	{
		hidden.value = "!";
	}
	else
	{
		hidden.value = "";
	}
}

function updatePorts(id, value)
{
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");
	setDisabled(document.getElementById("src_port" + idx), value < 2);
	setDisabled(document.getElementById("dst_port" + idx), value < 2);
	setDisabled(document.getElementById("not_src_port" + idx), value < 2);
	setDisabled(document.getElementById("not_dst_port" + idx), value < 2);
	setDisabled(document.getElementById("src_port_chk" + idx), value < 2);
	setDisabled(document.getElementById("dst_port_chk" + idx), value < 2);
	setDisabled(document.getElementById("not_src_port_chk" + idx), value < 2);
	setDisabled(document.getElementById("not_dst_port_chk" + idx), value < 2);
}

<? if ($saved) { >
if (window.opener && !window.opener.closed && window.opener.doSubmit)
	window.opener.doSubmit();
window.close();
<? } >

function validateFirewallPort(id, name, value)
{
	/* trim */
	value = value.replace(/^\s+|\s+$/g, '');
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");
	if (value != '')
	{
		if ((pos = value.indexOf(':')) >= 0)
		{
			from = value.substr(0, pos);
			to = value.substr(pos+1);
			if (isNaN(from) || isNaN(to))
				return false;
			intfrom = parseInt(from);
			intto = parseInt(to);
			if (intfrom != from || intto != to || intfrom < 1 ||
				intfrom > 65535 || intto < 1 || intto > 65535 ||
				intfrom > intto)
				return false;
		}
		else if (isNaN(value) || parseInt(value) != value ||
					parseInt(value) < 1 || parseInt(value) > 65535)
		{
			return false;
		}
	}
	return true;
}

function validateFirewallIP(id, name, value)
{
	value = value.replace(/^\s+|\s+$/g, '');
	idx = id.replace(/^([^\d]+)(\d+)$/, "$2");

	if (value == '0.0.0.0/0')
	{
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
		dep = document.getElementById("active" + idx);
		if (dep.checked) {
			return _validateIP(value);
		} else {
			return true;
		}
	}
	return false;
}

//-->
</script>
</head>
<body class="popup">
	<form name="firewall" enctype="multipart/form-data" action="firewall.cgi"
	method="POST" onSubmit="return validateStandard(this, 'error');">
		<table cellspacing="0" cellpadding="0" align="center" class="popup">
			<tr><th colspan="3"><? echo dict_translate("Firewall"); ></th></tr>
    		<tr>
		<td colspan="3">
    <? include("lib/error.tmpl");>
    <br>
		  <table cellspacing="0" cellpadding="0">
		  <tr>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Action"); >&nbsp;</td>
		  <td><? echo dict_translate("Interface"); >&nbsp;</td>
		  <td><? echo dict_translate("IP Type"); >&nbsp;</td>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Not"); >&nbsp;</td>
		  <td><? echo dict_translate("Source IP/Mask"); >&nbsp;</td>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Not"); >&nbsp;</td>
		  <td><? echo dict_translate("Src Port"); >&nbsp;</td>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Not"); >&nbsp;</td>
		  <td><? echo dict_translate("Destination IP/Mask"); >&nbsp;</td>
		  <td>&nbsp;</td>
		  <td><? echo dict_translate("Not"); >&nbsp;</td>
		  <td><? echo dict_translate("Dst Port"); >&nbsp;</td>
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
>		  <tr>
		  <td class="h"><? echo $i;>.</td>

                  <td><select class="std_width" name="act<? echo $i;>" id="act<? echo $i;>">
		  	<option value="DROP" <? if (strlen($act) == 0 || $act == "DROP") { echo "selected"; }> ><? echo dict_translate("DROP"); ></option>
		  	<option value="ACCEPT" <? if ($act == "ACCEPT") { echo "selected"; }> ><? echo dict_translate("ACCEPT"); ></option>
		      </select>
                  </td>
                  
		  <td><select class="std_width" name="input_ifc<? echo $i;>" id="input_ifc<? echo $i;>">
		  	<option value="" <? if (strlen($input_ifc) == 0) { echo "selected"; }> ><? echo dict_translate("ANY"); ></option>
                        <? if ($netmode == "soho") { >
		  	<option value="eth0" <? if ($input_ifc == "eth0") { echo "selected"; }> ><? echo dict_translate("LAN"); ></option>
		  	<option value="eth1" <? if ($input_ifc == "eth1") { echo "selected"; }> ><? echo dict_translate("WAN"); ></option>
                        <? } else { >
		  	<option value="ath0" <? if ($input_ifc == "ath0") { echo "selected"; }> ><? echo dict_translate("WLAN"); ></option>
		  	<option value="eth+" <? if ($input_ifc == "eth+") { echo "selected"; }> ><? echo dict_translate("LAN"); ></option>
                        <? } >
                        <? if ($netmode != "bridge") { >
		  	<option value="ppp+" <? if ($input_ifc == "ppp+") { echo "selected"; }> ><? echo dict_translate("PPP"); ></option>
                        <? } >
		      </select>
		  </td>
                  
		  <td><select class="std_width" name="protocol<? echo $i;>" id="protocol<? echo $i;>" onChange="updatePorts('protocol<? echo $i;>', this.value);">
		  	<option value="0" <? if (strlen($protocol) == 0 || $protocol == "0") { echo "selected"; }> ><? echo dict_translate("IP"); ></option>
		  	<option value="1" <? if ($protocol == "1") { echo "selected"; }> ><? echo dict_translate("ICMP"); ></option>
		  	<option value="6" <? if ($protocol == "6") { echo "selected"; }> ><? echo dict_translate("TCP"); ></option>
		  	<option value="17" <? if ($protocol == "17") { echo "selected"; }> ><? echo dict_translate("UDP"); ></option>
                        <? if ($netmode != "bridge") { >
                        <option value="-1" <? if ($protocol == "-1") { echo "selected"; }> ><? echo dict_translate("P2P"); ></option>
                        <? } >
			</select></td><td>&nbsp;
		  </td>

                  <td><input type="hidden" name="not_src_ip<? echo $i;>" id="not_src_ip<? echo $i;>" value="<? echo $not_src_ip;>">
                  <input type="checkbox" name="not_src_ip_chk<? echo $i;>" id="not_src_ip_chk<? echo $i;>" value="!"
                  <?if ($not_src_ip == "!") {echo "checked";}> onClick="updateNot(this, this.form.not_src_ip<? echo $i;>);"></td>

		  <td><input type="text" style="width: 145px;" name="src_ip<? echo $i;>" id="src_ip<? echo $i;>"
		  value="<? echo $src_ip;>" size="16" maxlength="18"
		  req="1" callback="validateFirewallIP" realname="<? echo dict_translate("Source IP/Mask"); >"></td><td>&nbsp;</td>

                  <td><input type="hidden" name="not_src_port<? echo $i;>" id="not_src_port<? echo $i;>" value="<? echo $not_src_port;>"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled"; }> >
                  <input type="checkbox" name="not_src_port_chk<? echo $i;>" id="not_src_port_chk<? echo $i;>" value="!"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled"; }
                    if ($not_src_port == "!") {echo "checked";}> onClick="updateNot(this, this.form.not_src_port<? echo $i;>);"></td>

		  <td><input type="text" class="std_width" name="src_port<? echo $i;>" id="src_port<? echo $i;>"
		  value="<? echo $src_port;>" size="6" maxlength="11"
		  req="1" callback="validateFirewallPort" realname="<? echo dict_translate("Src Port"); >"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled style='backgroundColor: #d0d0d0;'"; }> ></td><td>&nbsp;</td>

                  <td><input type="hidden" name="not_dst_ip<? echo $i;>" id="not_dst_ip<? echo $i;>" value="<? echo $not_dst_ip;>">
                  <input type="checkbox" name="not_dst_ip_chk<? echo $i;>" id="not_dst_ip_chk<? echo $i;>" value="!"
                  <?if ($not_dst_ip == "!") {echo "checked";}> onClick="updateNot(this, this.form.not_dst_ip<? echo $i;>);"></td>

		  <td><input type="text" style="width: 145px;" name="dst_ip<? echo $i;>" id="dst_ip<? echo $i;>"
		  value="<? echo $dst_ip;>" size="16" maxlength="18"
		  req="1" callback="validateFirewallIP" realname="<? echo dict_translate("Destination IP/Mask"); >"></td><td>&nbsp;</td>

                  <td><input type="hidden" name="not_dst_port<? echo $i;>" id="not_dst_port<? echo $i;>" value="<? echo $not_dst_port;>"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled"; }> >
                  <input type="checkbox" name="not_dst_port_chk<? echo $i;>" id="not_dst_port_chk<? echo $i;>" value="!"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled"; }
                    if ($not_dst_port == "!") {echo "checked";}> onClick="updateNot(this, this.form.not_dst_port<? echo $i;>);"></td>

		  <td><input type="text" class="std_width" name="dst_port<? echo $i;>" id="dst_port<? echo $i;>"
		  value="<? echo $dst_port;>" size="6" maxlength="11"
		  req="1" callback="validateFirewallPort" realname="<? echo dict_translate("Dst Port"); >"
                  <?if (strlen($protocol) == 0 || intVal($protocol) < 2) { echo "disabled style='backgroundColor: #d0d0d0;'"; }> ></td>

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
		  	<td colspan="17">&nbsp;</td>
		  </tr>
		  <tr>
		    <td colspan="4">&nbsp;<input type="hidden" name="action" value="firewallstore"></td>
		    <td colspan="4">&nbsp;<input type="hidden" name="netmode" value="<? echo $netmode>"></td>
		    <td colspan="4">
		    <input type="submit" name="firewall_submit" value="<? echo dict_translate("Save")>">
		    <input type="button" name="cancel" value="<? echo dict_translate("Cancel")>"
		    onClick="window.close()">
		    </td>
		    <td colspan="5">&nbsp;</td>
		  </tr>
		  </table>
		</td>
		</table>
	</form>
</body>
</html>

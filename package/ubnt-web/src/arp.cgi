#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
$arp_regexp="([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]*$";
include("lib/arp_head.tmpl");
flush();

$fp = @fopen("/proc/net/arp", "r");
if ($fp > -1) {
$line=fgets($fp,255);
while(!feof($fp)) {
	$line=fgets($fp,255);
	if (ereg($arp_regexp,$line,$res) && $res[4] != "00:00:00:00:00:00") {
		$f=$res[6];
		if ($f==$br_iface){$f=dict_translate("BRIDGE");}
		elseif ($f==$eth0_iface){$f=dict_translate("LAN");}
		elseif ($f==$wan_iface){$f=dict_translate("WAN");}
       		elseif (substr($f,0,3)=="ppp"){$f=dict_translate("PPP");}
       		else {$f=dict_translate("WLAN");}
		echo "<tr><td class=\"str\">" + $res[1] + "</td><td class=\"str\">" + $res[4] + "</td><td>" + $f + "</td></tr>\n";
	}
}
fclose($fp);
}
include("lib/arp_tail.tmpl");
>

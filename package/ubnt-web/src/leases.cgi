#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
$leasedir="/tmp";
$fregexp="^leases.([[:digit:]]+).([^[:space:]]+)$";
$lease_regexp="^([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+).*$";
$page_title=dict_translate("DHCP Leases");
include("lib/ptable_head.tmpl");
>
<tr>
<th><? echo dict_translate("MAC Address"); ></th><th><? echo dict_translate("IP Address"); ></th><th><? echo dict_translate("Remaining Lease"); ></th><th><? echo dict_translate("Hostname"); ></th><th><? echo dict_translate("Interface"); ></th>
</tr>
<?
flush();

Exec("/sbin/route -n", $lines, $result);

@opendir($leasedir);
$f=@readdir();
while (strlen($f) != 0) {
	if (ereg($fregexp,$f,$r)) {
		$ifc=$r[2];
		if ($ifc==$br_iface){$ifc=dict_translate("BRIDGE");}
		elseif ($ifc==$eth0_iface){$ifc=dict_translate("LAN");}
       		elseif ($ifc==$wan_iface){$ifcf=dict_translate("WAN");}
		elseif ($ifc==$wlan_iface){$ifc=dict_translate("WLAN");}

		$fp = @fopen($leasedir+"/"+$f,"r");
		while (!feof($fp)) {
			$line=fgets($fp,255);
			if (ereg($lease_regexp,$line,$res)) {
				$x=intval($res[1]);
				$hostname=ereg_replace("\*", " ", $res[4]);
				$now=time();
				if ($now > $x) {
					$left="expired";
				} else {
					$t = $x - $now;
					$h = $t / 3600;
					$m = $t / 60;
					$left = sprintf("%02u:%02u:%02u", $h,$m%60,$t%60);
				}
				echo "<tr><td class=\"str\">" + strtoupper($res[2]) + "</td><td class=\"str\">" + $res[3] + "</td>";
				echo "<td class=\"str\">" + $left + "</td>";
				echo "<td class=\"str\">" + $hostname + "</td>";
				echo "<td>" + $ifc + "</td></tr>\n";
			}
		}
	}
	$f=readdir();
}
closedir();

include("lib/arp_tail.tmpl");
>

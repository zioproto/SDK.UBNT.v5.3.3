#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
$brmacs_regexp="[[:space:]]*([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]*$";
include("lib/brmacs_head.tmpl");
flush();

Exec($cmd_brctl + " showmacs br0", $arr, $result);

if ($result == 0) {
	$i = 1;
	$size = count($arr);
	while ($i < $size) {
		if (ereg($brmacs_regexp, $arr[$i], $res)) {
			if ($res[3] != "yes"){
			$port = $res[1];
			if ($port == "1") { $port = "LAN"; } else { $port = "WLAN"; }
			echo "<tr><td class=\"str\">" + strtoupper($res[2]) + "</td><td class=\"str\">" + $port + "</td><td class=\"num\">" + $res[4] + "</td></tr>\n";
			}
		}

		$i++;
	}
}

include("lib/arp_tail.tmpl");
>

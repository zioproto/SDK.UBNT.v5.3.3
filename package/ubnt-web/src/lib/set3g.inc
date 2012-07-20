<?
$dhcpc = "disabled";
$pppoe = "disabled";
$gsm = "enabled";
$natifc = "ppp+";
set_vlan($cfg, $vlan_iface, $vlan_status, $vlan_tag, $vlan_ifc);
set_ipv4($cfg, $wlan_iface, "", "");
set_ipv4($cfg, $wan_iface, $wlanip, $wlanmask);
set_ipv4($cfg, $lan_iface, $lanip, $lanmask);
set_ipv4gw($cfg, $wan_iface, $wangw);
cfg_set_hwaddr($cfg, $wan_iface, $hwaddr_status, $hwaddr);
set_gsm($cfg, $wan_iface, $gsmuser, $gsmpass, $gsmapn, $gsm);
set_dhcpc($cfg, $wan_iface, $dhcpc);
set_pppoe($cfg, $wan_iface, $wanppoeuser, $wanppoepass, $wanpppoemppe, $pppoe, $pppoemtu, $pppoemru);
set_dhcpd($cfg, $lan_iface, $dhcpd_start, $dhcpd_end, $dhcpd_netmask, $dhcpd_lease, $dhcpd_status, $dhcpd_dnsproxy_status, $dhcpd_dns1, $dhcpd_dns2);
set_dns($cfg, $dns1, $dns2);
if (strlen($natifc) == 0) {
	$natifc = $wan_iface;
}
set_nat($cfg, $natifc, $nat_status);
include("lib/setnatmodules.inc");
set_DMZ($cfg, $natifc, $dmz_status, $dmz_ip, $dmz_mgmt);
set_fallback($cfg, $wan_iface, $dhcpc, $fallback_ip, $fallback_netmask);
set_port_forward($cfg, $natifc, $port_forward_status);
set_firewall($cfg, $firewall_status, "soho");
if ($nat_status=="on" || $nat_status=="enabled" ||
	$dmz_status=="on" || $dmz_status=="enabled" ||
	$port_forward_status=="enabled" || $pppoe=="enabled" ||
        $firewall_status=="enabled")
{
	cfg_set($cfg, "iptables.status", "enabled");
}
else {
	cfg_set($cfg, "iptables.status", "disabled");
}
if ($igmp_status=="on" || $igmp_status=="enabled")
{	
	cfg_set($cfg, "igmpproxy.status", "enabled");
        if ($igmp_upstream == "lan") {
	        $lan_stream = "upstream";
        	$wan_stream = "1.downstream";
        } else {
	        $wan_stream = "upstream";
        	$lan_stream = "1.downstream";
        }
	if ($pppoe != "enabled") {
		cfg_set($cfg, "igmpproxy."+$wan_stream+".devname", $wan_iface);
	} else {
		cfg_set($cfg, "igmpproxy."+$wan_stream+".devname", "ppp0");
	}
	cfg_set($cfg, "igmpproxy."+$lan_stream+".devname", $lan_iface);
}
else {
	cfg_set($cfg, "igmpproxy.status", "disabled");
}
cfg_set($cfg, "netmode", $netmode);
cfg_set($cfg, "bridge.status", "disabled");
cfg_set($cfg, "ebtables.status", "disabled");
cfg_set($cfg, "netconf.3.status", "disabled");
cfg_set($cfg, "wireless.1.macclone", "disabled");
$autoip_lan_status = set_autoip($cfg, $lan_iface, $autoip_lan_status);
$autoip_wan_status = set_autoip($cfg, $wan_iface, $autoip_wan_status);
disable_ipaliases($cfg, $br_iface, $ipaliases_max);
cfg_disable_hwaddrs($cfg, 4);
cfg_set($cfg, "tshaper.in.1.devname", $lan_iface);
cfg_set($cfg, "tshaper.out.1.devname", $wan_iface);

cfg_set($cfg, "netconf.1.mtu", $lan_mtu);
cfg_set($cfg, "netconf.2.mtu", $lan_mtu);
cfg_set($cfg, "netconf.4.mtu", $wan_mtu);
cfg_set($cfg, "netconf.5.mtu", $wan_mtu);

include("lib/setdef.inc");
>

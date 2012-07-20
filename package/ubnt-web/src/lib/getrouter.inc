<?
$is_ap = 0;
if ($wlanmode == "ap" || $wlanmode == "apwds")
{
	$wan_iface = $eth0_iface;
	$lan_iface = $wlan_iface;
	$is_ap = 1;
}
else
{
	$lan_iface = $eth0_iface;
	$wan_iface = $wlan_iface;
}
$vlan_status = cfg_get_vlan_status($cfg, $wan_iface, $vlan_status);
$vlan_tag = cfg_get_vlan_tag($cfg, $wan_iface, $vlan_tag);
$vlan_ifc = cfg_get_vlan_ifc($cfg, $wan_iface, $vlan_ifc);
if ($vlan_status=="enabled")
{
	if ($vlan_ifc == "lan") {
		$vlan_iface = $lan_iface;
		$lan_iface = $vlan_iface + "." + $vlan_tag;
	} else {
		$vlan_iface = $wan_iface;
		$wan_iface = $vlan_iface + "." + $vlan_tag;
	}
}
$dhcpc = cfg_get_dhcpc($cfg, $wan_iface, $dhcpc);
$pppoe = cfg_get_pppoe($cfg, $wan_iface, $pppoe);
$wlanipmode = 0;
if ($dhcpc == "enabled")
{
	$wlanipmode = 1;
}
elseif ($pppoe == "enabled")
{
	$wlanipmode = 2;
}
$wanppoeuser = cfg_get_pppoe_user($cfg, $wan_iface, $wanppoeuser);
$wanppoepass = cfg_get_pppoe_pass($cfg, $wan_iface, $wanppoepass);
$wanpppoemppe = cfg_get_pppoe_mppe($cfg, $wan_iface, $wanpppoemppe);
$pppoemtu = cfg_get_pppoe_mtu($cfg, $wan_iface, $pppoemtu);
$pppoemru = cfg_get_pppoe_mru($cfg, $wan_iface, $pppoemru);
if ($is_ap == 1) {
	$wlanip = cfg_get_ipv4addr($cfg, $lan_iface, $wlanip);
	$wlanmask = cfg_get_ipv4mask($cfg, $lan_iface, $wlanmask);
	$lanip = cfg_get_ipv4addr($cfg, $wan_iface, $lanip);
	$lanmask = cfg_get_ipv4mask($cfg, $wan_iface, $lanmask);
} else {
	$wlanip = cfg_get_ipv4addr($cfg, $wan_iface, $wlanip);
	$wlanmask = cfg_get_ipv4mask($cfg, $wan_iface, $wlanmask);
	$lanip = cfg_get_ipv4addr($cfg, $lan_iface, $lanip);
	$lanmask = cfg_get_ipv4mask($cfg, $lan_iface, $lanmask);
}
$wangw = cfg_get_ipv4gw($cfg, $wan_iface, $wangw);
$hwaddr_status = cfg_get_hwaddr_status($cfg, $wan_iface);
$hwaddr = cfg_get_hwaddr($cfg, $wan_iface);
$dhcpd_status = cfg_get_dhcpd($cfg, $lan_iface, $dhcpd_status);
$dhcpd_start = cfg_get_dhcpd_start($cfg, $lan_iface, $dhcpd_start);
$dhcpd_end = cfg_get_dhcpd_end($cfg, $lan_iface, $dhcpd_end);
$dhcpd_netmask = cfg_get_dhcpd_mask($cfg, $lan_iface, $dhcpd_netmask);
$dhcpd_lease = cfg_get_dhcpd_lease($cfg, $lan_iface, $dhcpd_lease);
$dhcpd_dnsproxy_status = cfg_get_dhcpd_dnsproxy($cfg, $lan_iface, $dhcpd_dnsproxy_status);
$dhcpd_dns1 = cfg_get_dhcpd_dns($cfg, $lan_iface, $dhcpd_dns1, 1);
$dhcpd_dns2 = cfg_get_dhcpd_dns($cfg, $lan_iface, $dhcpd_dns2, 2);
$dns1 = cfg_get_dns($cfg, $dns1, 1);
$dns2 = cfg_get_dns($cfg, $dns2, 2);
$nat_status = cfg_get_nat($cfg, $wan_iface, $nat_status);
$dmz_status = cfg_get_DMZ_status($cfg, $wan_iface, $dmz_status);
$dmz_ip = cfg_get_DMZ_ip($cfg, $wan_iface, $dmz_ip);
$dmz_mgmt = cfg_get_DMZ_mgmt($cfg, $wan_iface, $dmz_mgmt);
$fallback_ip = cfg_get_fallback_ip($cfg, $wan_iface, $fallback_ip);
$fallback_netmask = cfg_get_fallback_netmask($cfg, $wan_iface, $fallback_netmask);
$port_forward_status = cfg_get_port_forward($cfg, $wan_iface, $port_forward_status);
$igmp_status = cfg_get_igmp_status($cfg, $wan_iface, $igmp_status);
$igmp_upstream = cfg_get_igmp_upstream($cfg, $wan_iface);
$autoip_lan_status = cfg_get_autoip($cfg, $lan_iface, $autoip_lan_status);
$autoip_wan_status = cfg_get_autoip($cfg, $wan_iface, $autoip_wan_status);
if ($autoip_lan_status == $autoip_wan_status) {
	$autoip_wan_status = "disabled";
}
$firewall_status = cfg_get_firewall($cfg, $firewall_status, $netmode);
if (strlen($lan_mtu) == 0) {
	$lan_mtu = 1500;
}
$lan_mtu = cfg_get_def($cfg, "netconf.1.mtu", $lan_mtu);
if (strlen($wlan_mtu) == 0) {
	$wlan_mtu = 1500;
}
$wlan_mtu = cfg_get_def($cfg, "netconf.2.mtu", $wlan_mtu);
include("lib/getnatmodules.inc");
>

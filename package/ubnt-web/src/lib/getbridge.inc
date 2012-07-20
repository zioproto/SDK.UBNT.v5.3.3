<?
	$vlan_status = cfg_get_vlan_status($cfg, $br_iface, $vlan_status);
	$vlan_tag = cfg_get_vlan_tag($cfg, $br_iface, $vlan_tag);
	$vlan_ifc = cfg_get_vlan_ifc($cfg, $br_iface, $vlan_ifc);
	$dhcpc = cfg_get_dhcpc($cfg, $br_iface, $dhcpc);
	$brip = cfg_get_ipv4addr($cfg, $br_iface, $brip);
	$brmask = cfg_get_ipv4mask($cfg, $br_iface, $brmask);
	$brgw = cfg_get_ipv4gw($cfg, $br_iface, $brgw);
	$dns1 = cfg_get_dns($cfg, $dns1, 1);
	$dns2 = cfg_get_dns($cfg, $dns2, 2);
	$fallback_ip = cfg_get_fallback_ip($cfg, $br_iface, $fallback_ip);
        $fallback_netmask = cfg_get_fallback_netmask($cfg, $br_iface, $fallback_netmask);
	$stp = cfg_get_def($cfg, "bridge.1.stp.status", $stp);
	$wlanipmode = 0;
	if ($dhcpc == "enabled")
	{
		$wlanipmode = 1;
	}
	$autoip_br_status = cfg_get_autoip($cfg, $br_iface, $autoip_br_status);
	$firewall_status = cfg_get_firewall($cfg, $firewall_status, $netmode);
        if (strlen($lan_mtu) == 0) {
        	$lan_mtu = 1500;
        }
        $lan_mtu = cfg_get_def($cfg, "netconf.3.mtu", $lan_mtu);
>

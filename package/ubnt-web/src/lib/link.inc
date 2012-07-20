<?
Function get_iface_index $iface
(
	global $br_iface, $wlan_iface, $wusb_iface, $eth0_iface, $eth1_iface;
	$index = 0; /* not found */
	if ($iface == $eth0_iface) {
		$index = "1";
	}
	elseif ($iface == $wlan_iface) {
		$index = "2";
	}
	elseif ($iface == $br_iface) {
		$index = "3";
	}
	elseif ($iface == $eth1_iface) {
		$index = "4";
	}
	elseif ($iface == $wusb_iface) {
		$index = "5";
	}
	elseif (substr($iface, 4, 1) == ".") {
		$index = "6";
	}
	return $index;
);

Function get_wlan_index $iface
(
	global $wlan_iface, $wusb_iface;
	$index = 0;
	if ($iface == $wlan_iface) {
		$index = "1";
	}
	elseif ($iface == $wusb_iface) {
		$index = "2";
	}
	return $index;
);

Function get_phyname $index
(
	return "wifi0";
);

Function set_wmode $cfg, $wlan_iface, $wmode, $ch_freq, $macclone (
	$netmode = cfg_get_def($cfg, "netmode", "bridge");
	$wds = "disabled";
	$ebtables = "disabled";
	$wireless = "managed";
	if ($macclone == "enabled" || $macclone == "on") {
		$macclone = "enabled";
	}
	else {
		$macclone = "disabled";
	}

	switch ($wmode)
	{
	case "apwds";
		$wireless = "master";
		$macclone = "disabled";
	case "stawds";
		$wds = "enabled";
		$macclone = "disabled";
		break;
	case "ap";
		$wireless = "master";
		$macclone = "disabled";
	case "sta";
	default;
		$wds = "disabled";
		break;
	};

	if ($netmode == "bridge" && $wireless != "master") {
		$st = "enabled";
		$ebtables = "enabled";
		if ($wds == "enabled" || $macclone == "enabled") {
			$st = "disabled";
		}
		cfg_set($cfg, "ebtables.1.status", $st);
		cfg_set($cfg, "ebtables.1.cmd", "-t nat -A PREROUTING --in-interface " + $wlan_iface +" -j arpnat --arpnat-target ACCEPT");
		cfg_set($cfg, "ebtables.2.status", $st);
		cfg_set($cfg, "ebtables.2.cmd", "-t nat -A POSTROUTING --out-interface " + $wlan_iface + " -j arpnat --arpnat-target ACCEPT");
		cfg_set($cfg, "ebtables.3.status", "enabled");
		cfg_set($cfg, "ebtables.3.cmd", "-t broute -A BROUTING --protocol 0x888e --in-interface " + $wlan_iface + " -j DROP");
	}
	$idx = get_wlan_index($wlan_iface);
	cfg_set($cfg, "wireless.$idx.wds", $wds);
	cfg_set($cfg, "radio.$idx.mode", $wireless);
	if (strlen($ch_freq) != 0 && $wireless == "master") {
		cfg_set($cfg, "radio.$idx.freq", $ch_freq);
	}
	cfg_set($cfg, "wireless.$idx.macclone", $macclone);
);

Function cfg_get_wmode $cfg, $wlan_iface (
	$idx = get_wlan_index($wlan_iface);
	$mode = strtolower(cfg_get_def($cfg, "radio.$idx.mode", "managed"));
	$wds = cfg_get_def($cfg, "wireless.$idx.wds", "disabled");
	if ($wds == "enabled") {
		if ($mode == "master") {
			$result = "apwds";
		}
		else {
			$result =  "stawds";
		}
	}
	else {
		if ($mode == "master") {
			$result = "ap";
		}
		else {
			$result = "sta";
		}
	}
	return $result;
);

Function cfg_get_wpa_auth $cfg, $idx, $wmode, $wpa_auth (
	if (strlen($wpa_auth) == 0) {
		$wpa_auth = "WPA-PSK";
	}
	if ($wmode == "ap" || $wmode == "apwds") {
		$wpa_auth = cfg_get_def($cfg, "aaa.$idx.wpa.key.1.mgmt", $wpa_auth);
	}
	else {
		$wpa_auth = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.key_mgmt.1.name", $wpa_auth);
	}
	return $wpa_auth;
);

Function cfg_get_wpa_eap $cfg, $idx, $wmode, $wpa_eap (
	if (strlen($wpa_eap) == 0) {
		$wpa_eap = "TTLS";
	}
	if ($wmode != "ap" && $wmode != "apwds") {
		$wpa_eap = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.eap.1.name", $wpa_eap);
	}
	return $wpa_eap;
);

Function cfg_get_wpa_inner $cfg, $idx, $wpa_inner (
	if (strlen($wpa_inner) == 0) {
		$wpa_inner = "auth=MSCHAPV2";
	}
	return  cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.phase2", $wpa_inner);
);

Function cfg_get_wpa_ident $cfg, $idx, $wpa_ident (
	return  cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.identity", $wpa_ident);
);

Function cfg_get_wpa_user $cfg, $idx, $wpa_user (
	if (strlen($wpa_user) == 0) {
		$wpa_user = "anonymous@myisp.com";
	}
	return  cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.anonymous_identity", $wpa_user);
);

Function cfg_get_wpa_passwd $cfg, $idx, $wpa_passwd (
	return  cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.password", $wpa_passwd);
);

Function get_wmode_type $wmode (
	if ($wmode == "stawds" || $wmode == "sta") {
		$type = 1;
	}
	else {
		if ($wmode == "ap" || $wmode == "apwds") {
			$type = 2;
		}
		else {
			$type = 3;
		}
	}
	return $type;
);

Function set_essid $cfg, $idx, $essid (
	return cfg_set($cfg, "wireless.$idx.ssid", $essid);
);

Function set_hide_ssid $cfg, $idx, $hidessid (
	if ($hidessid == "on") {
		$hidessid = "enabled";
	}
	elseif ($hidessid != "enabled") {
		$hidessid = "disabled";
	}
	return cfg_set($cfg, "wireless.$idx.hide_ssid", $hidessid);
);

Function cfg_get_hide_ssid $cfg, $idx, $def_hide (
	return cfg_get_def($cfg, "wireless.$idx.hide_ssid", $def_hide);
);

Function set_apmac $cfg, $idx, $apmac (
	return cfg_set($cfg, "wireless.$idx.ap", $apmac);
);

Function set_txpower $cfg, $idx, $power (
	return cfg_set($cfg, "radio.$idx.txpower", $power);
);

Function set_obey $cfg, $idx, $obey (
	cfg_del($cfg, "radio.$idx.regulatory");
	cfg_set($cfg, "radio.$idx.reg_obey", $obey);
	return cfg_set($cfg, "radio.$idx.obey", $obey);
);

Function set_country $cfg, $idx, $country, $subsystemid (
	$result = $cfg;
	if (strlen($country)) {
		if ($idx == 1) {
			cfg_set($cfg, "radio.countrycode", $country);
		}
		cfg_set($cfg, "radio.$idx.countrycode", $country);
		$result = cfg_set($cfg, "radio.$idx.subsystemid", $subsystemid);
	}
	return $result;
);

Function set_def_wep_key_id $cfg, $idx, $wep_key_id(
	return cfg_set($cfg, "wireless.$idx.security.default_key", $wep_key_id);
);

Function set_wep_key $cfg, $idx, $wep_key_id, $wep_key, $wep_key_type (
	if ($wep_key_type == 2) {
		$wep_key = "s:" + $wep_key;
	}
	return cfg_set($cfg, "wireless.$idx.security." + $wep_key_id + ".key", $wep_key);
);

Function set_security $cfg, $iface, $idx, $security, $wep_key_len, $wmode (
	global $br_iface;

	switch ($security)
	{
	case "wpa";
	case "wpa2";
		$pairwise="TKIP CCMP";
		break;
	case "wpatkip";
	case "wpa2tkip";
		$pairwise="TKIP";
		break;
	case "wpaaes";
	case "wpa2aes";
		$pairwise="CCMP";
		break;
	}

	switch ($security)
	{
	case "wep64";
	case "wep128";
		cfg_set($cfg, "wireless.$idx.security", $security);
		cfg_set($cfg, "wpasupplicant.device.$idx.status", "disabled");
		cfg_set($cfg, "wpasupplicant.status", get_wpasupplicant_status($cfg));
		cfg_set($cfg, "aaa.$idx.status", "disabled");
		cfg_set($cfg, "aaa.status", get_aaa_status($cfg));
		break;
	case "wep";
		cfg_set($cfg, "wireless.$idx.security", $wep_key_len);
		cfg_set($cfg, "wpasupplicant.device.$idx.status", "disabled");
		cfg_set($cfg, "wpasupplicant.status", get_wpasupplicant_status($cfg));
		cfg_set($cfg, "aaa.$idx.status", "disabled");
		cfg_set($cfg, "aaa.status", get_aaa_status($cfg));
		break;
	case "wpa";
	case "wpatkip";
	case "wpaaes";
	case "wpa2";
	case "wpa2tkip";
	case "wpa2aes";
		cfg_set($cfg, "ebtables.3.status", "enabled");
		cfg_set($cfg, "ebtables.3.cmd", "-t broute -A BROUTING --protocol 0x888e --in-interface " + $iface + " -j DROP");
		if ($wmode == "ap" || $wmode == "apwds")
		{
			cfg_set($cfg, "wpasupplicant.device.$idx.status", "disabled");
			cfg_set($cfg, "wpasupplicant.status", get_wpasupplicant_status($cfg));
			cfg_set($cfg, "wireless.$idx.security", "none");
			cfg_set($cfg, "aaa.status", "enabled");
			cfg_set($cfg, "aaa.$idx.status", "enabled");
			if (substr($security, 0, 4) == "wpa2") {
				cfg_set($cfg, "aaa.$idx.wpa", 2);
			}
			else {
				cfg_set($cfg, "aaa.$idx.wpa", 1);
			}
			cfg_set($cfg, "aaa.$idx.wpa.1.pairwise", $pairwise);
			cfg_set($cfg, "aaa.$idx.driver", "madwifi");
			cfg_set($cfg, "aaa.$idx.devname", $iface);
			cfg_set($cfg, "aaa.$idx.br.devname", $br_iface);
			cfg_set($cfg, "aaa.$idx.ssid", cfg_get_essid($cfg, $idx, ""));
		}
		else {
			cfg_set($cfg, "aaa.$idx.status", "disabled");
			cfg_set($cfg, "aaa.status", get_aaa_status($cfg));
			cfg_set($cfg, "wireless.$idx.security", "none");
			cfg_set($cfg, "wpasupplicant.status", "enabled");
			cfg_set($cfg, "wpasupplicant.device.$idx.status", "enabled");
			cfg_set($cfg, "wpasupplicant.device.$idx.devname", $iface);
			cfg_set($cfg, "wpasupplicant.device.$idx.driver", "madwifi");
			if (substr($security, 0, 4) == "wpa2") {
				cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.proto.1.name", "RSN");
			}
			else {
				cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.proto.1.name", "WPA");
			}
			cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.pairwise.1.name", $pairwise);
			cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.ssid", cfg_get_essid($cfg, $idx, ""));
		}
		break;
	default;
		cfg_set($cfg, "wireless.$idx.security", "none");
		cfg_set($cfg, "aaa.$idx.status", "disabled");
		cfg_set($cfg, "aaa.status", get_aaa_status($cfg));
		cfg_set($cfg, "wpasupplicant.device.$idx.status", "disabled");
		cfg_set($cfg, "wpasupplicant.status", get_wpasupplicant_status($cfg));
		cfg_set($cfg, "ebtables.3.status", "disabled");
		break;
	}
);

Function set_wpa_ap $cfg, $idx, $wpa_auth, $wpa_key, $radius_auth_ip,
	$radius_auth_port, $radius_auth_secret
(
	if (!strlen($wpa_auth)) {
		$$wpa_auth = "WPA-PSK";
	}

	cfg_set($cfg, "aaa.$idx.wpa.key.1.mgmt", $wpa_auth);
	if ($wpa_auth == "WPA-PSK") {
		cfg_set($cfg, "aaa.$idx.radius.auth.1.status", "disabled");
		cfg_set($cfg, "aaa.$idx.wpa.psk", $wpa_key);
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.psk", $wpa_key);
	}
	else {
		cfg_set($cfg, "aaa.$idx.radius.auth.1.status", "enabled");
		cfg_set($cfg, "aaa.$idx.radius.auth.1.ip", $radius_auth_ip);
		cfg_set($cfg, "aaa.$idx.radius.auth.1.port", $radius_auth_port);
		cfg_set($cfg, "aaa.$idx.radius.auth.1.secret", $radius_auth_secret);
	}
);

Function cfg_get_radius_auth_ip $cfg, $idx, $radius_auth_ip (
	return cfg_get_def($cfg, "aaa.$idx.radius.auth.1.ip", $radius_auth_ip);
);

Function cfg_get_radius_auth_port $cfg, $idx, $radius_auth_port (
	return cfg_get_def($cfg, "aaa.$idx.radius.auth.1.port", $radius_auth_port);
);

Function cfg_get_radius_auth_secret $cfg, $idx, $radius_auth_secret (
	return cfg_get_def($cfg, "aaa.$idx.radius.auth.1.secret", $radius_auth_secret);
);

Function set_wpa_sta $cfg, $idx,
	$wpa_auth, $wpa_key, $wpa_eap, $wpa_inner, $wpa_ident,
	$wpa_user, $wpa_passwd, $apmac (
	if (!strlen($wpa_auth)) {
		$$wpa_auth = "WPA-PSK";
	}

	cfg_set($cfg, "wpasupplicant.device.$idx.profile", $wpa_auth);
	cfg_set($cfg, "wpasupplicant.profile.$idx.name", $wpa_auth);
	cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.key_mgmt.1.name", $wpa_auth);
	cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.bssid", $apmac);
	if (strlen($wpa_key) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.psk", $wpa_key);
		cfg_set($cfg, "aaa.$idx.wpa.psk", $wpa_key);
	}
	if (strlen($wpa_eap) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.eap.1.status", "enabled");
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.eap.1.name", $wpa_eap);
	}
	else {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.eap.1.status", "disabled");
	}
	if (strlen($wpa_ident) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.identity", $wpa_ident);
	}
	if (strlen($wpa_user) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.anonymous_identity", $wpa_user);
	}
	if (strlen($wpa_passwd) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.password", $wpa_passwd);
	}
	if (strlen($wpa_inner) != 0) {
		cfg_set($cfg, "wpasupplicant.profile.$idx.network.1.phase2", $wpa_inner);
	}
	return 1;
);

Function set_authtype $cfg, $idx, $type (
	if (!strlen($type)) {
		$type = "1";
	}
	return cfg_set($cfg, "wireless.$idx.authmode", $type);
);

Function set_rate $cfg, $idx, $rate, $auto (
	if (!strlen($auto)) {
		$auto = "disabled";
	}
	cfg_set($cfg, "radio.$idx.rate.auto", $auto);
	if ($radio1_legacy == 1) {
		return cfg_set($cfg, "radio.$idx.rate.max", $rate);
	}
	else {
		return cfg_set($cfg, "radio.$idx.rate.mcs", $rate);
	}
);

Function set_clksel $cfg, $idx, $clksel (
	if ($clksel == "2") {
		$clksel = 4;
	}
	elseif ($clksel == "1") {
		$clksel = 2;
	}
	else {
		$clksel = 1;
	}
	return cfg_set($cfg, "radio.$idx.clksel", $clksel);
);

Function set_chanshift $cfg, $idx, $chanshift (
	return cfg_set($cfg, "radio.$idx.chanshift", $chanshift);
);

Function set_ieee_mode $cfg, $idx, $mode (
	return cfg_set($cfg, "radio.$idx.ieee_mode", $mode);
);

Function set_rts_treshold $cfg, $wlan_iface, $rts, $rtsoff (
	if (strlen($rtsoff)) {
		$rts = $rtsoff;
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "radio.$idx.rts", $rts);
);

Function set_frag_treshold $cfg, $wlan_iface, $frag, $fragoff (
	if (strlen($fragoff)) {
		$frag = $fragoff;
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "radio.$idx.frag", $frag);
);

Function set_ack_distance $cfg, $wlan_iface, $ackdistance, $ieee_mode (
	$idx = get_wlan_index($wlan_iface);
	cfg_set($cfg, "radio.$idx.ackdistance", $ackdistance);
	/* backwards compatibility */
	if ($ieee_mode == "b" || $ieee_mode == "2") {
		$minack = 20 * 2 + 3;
	}
	else {
		$minack = 9 * 2 + 3;
	}
	$ack = cfg_set($cfg, "radio.$idx.acktimeout",
		$minack + (intVal($ackdistance) / 150)); /* 150 - speed of light (m/us)/2 */
);

Function set_autoack $cfg, $wlan_iface, $autoack (
	if ($autoack == "on") {
		$autoack = "enabled";
	} elseif ($autoack != "enabled") {
		$autoack = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "radio.$idx.ack.auto", $autoack);
);

Function set_fast_frame $cfg, $wlan_iface, $fast_frame (
	if (!strlen($fast_frame)) {
		$fast_frame = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "wireless.$idx.fastframes", $fast_frame);
);

Function set_bursting $cfg, $wlan_iface, $burst (
	if (!strlen($burst)) {
		$burst = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "wireless.$idx.frameburst", $burst);
);

Function set_compression $cfg, $wlan_iface, $compression (
	if (!strlen($compression)) {
		$compression = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_set($cfg, "wireless.$idx.compression", $compression);
);

Function set_dhcpc $cfg, $iface, $dhcpc (
	if ($dhcpc == "on" || $dhcpc == "enabled") {
		$dhcpc = "enabled";
		cfg_set($cfg, "dhcpc.1.devname", $iface);
	}
	else {
		$dhcpc = "disabled";
	}
	cfg_set($cfg, "dhcpc.status", $dhcpc);
	return cfg_set($cfg, "dhcpc.1.status", $dhcpc);
);

Function set_ipv4gw $cfg, $iface, $gw (
	cfg_set($cfg, "route.status", "enabled");
	if ($gw != "" && $gw != "0.0.0.0") {
		cfg_set($cfg, "route.1.status", "enabled");
		cfg_set($cfg, "route.1.ip", "0.0.0.0");
		cfg_set($cfg, "route.1.netmask", "0");
		cfg_set($cfg, "route.1.gateway", $gw);
		cfg_set($cfg, "route.1.devname", $iface);
	}
	else {
		cfg_set($cfg, "route.1.status", "disabled");
	}
	return $cfg;
);

Function set_ipv4 $cfg, $iface, $ip, $mask (
	global $wan_iface;

	if (!strlen($ip)) { $ip = "0.0.0.0"; }
	if (!strlen($mask)) { $mask = "255.255.255.0"; }

	$idx = get_iface_index($iface);
	cfg_set($cfg, "netconf.$idx.ip", $ip);
	cfg_set($cfg, "netconf.$idx.netmask", $mask);
	if ($iface == $wan_iface || substr($iface, 4, 1) == ".") {
		cfg_set($cfg, "netconf.$idx.devname", $iface);
	}

	return $cfg;
);

Function set_autoip $cfg, $iface, $status
(
	if (!strlen($status)) { $status = "disabled"; }

	$idx = get_iface_index($iface);
	cfg_set($cfg, "netconf.$idx.autoip.status", $status);
	return $status;
);

Function set_all_autoip_off $cfg
(
	global $br_iface, $wlan_iface, $wusb_iface, $eth0_iface, $eth1_iface;
	set_autoip($cfg, $br_iface, "disabled");
        set_autoip($cfg, $wlan_iface, "disabled");
        set_autoip($cfg, $wusb_iface, "disabled");
        set_autoip($cfg, $eth0_iface, "disabled");
	set_autoip($cfg, $eth1_iface, "disabled");
);

Function set_dns $cfg, $dns, $dns2 (
	$status = "enabled";
	if (!strlen($dns)) {
		$status = "disabled";
	}
	else {
		cfg_set($cfg, "resolv.nameserver.1.ip", $dns);
	}
	cfg_set($cfg, "resolv.nameserver.1.status", $status);

	$status = "enabled";
	if (!strlen($dns2)) {
		$status = "disabled";
	}
	else {
		cfg_set($cfg, "resolv.nameserver.2.ip", $dns2);
	}
	cfg_set($cfg, "resolv.nameserver.2.status", $status);
	return cfg_set($cfg, "resolv.status", "enabled");
);

Function cfg_get_essid $cfg, $idx, $essid (
	if (!strlen($essid)) {
		$essid = "UBNT";
	}
	return cfg_get_def($cfg, "wireless.$idx.ssid", $essid);
);

Function cfg_get_apmac $cfg, $idx, $apmac (
	return cfg_get_def($cfg, "wireless.$idx.ap", $apmac);
);

Function cfg_get_country $cfg, $wlan_iface, $country (
	global $radio;
	if (count($radio) < 2) {
		init_board_inc($wlan_iface);
	}

	if ($radio["ccode_locked"] == 1 || $radio["ccode_fixed"] == 1) {
		$ret = $radio["ccode"];
	} else {
		if (!strlen($country)) {
			$country = $radio["ccode"];
			if (!strlen($country)) {
				$country = "840";
			}
		}
		$idx = get_wlan_index($wlan_iface);
		if ($idx == 1) {
			$key = "radio.countrycode";
		}
		else {
			$key = "radio.$idx.countrycode";
		}
		$ret = cfg_get_def($cfg, $key, $country);
	}

	return $ret;
);

Function cfg_get_txpower $cfg, $idx, $txpower (
	if (!strlen($txpower)) {
		$txpower = w_get_txpower($wlan_iface);
	}
	return cfg_get_def($cfg, "radio.$idx.txpower", $txpower);
);

Function cfg_get_obey $cfg, $wlan_iface, $obey (
	$idx = get_wlan_index($wlan_iface);
	$obey_value = cfg_get_def($cfg, "radio.$idx.reg_obey", "disabled");
	if ($obey_value == "enabled") {
		$obey_value = cfg_get_def($cfg, "radio.$idx.obey", $obey);
	}
	return $obey_value;
);

Function cfg_get_rate $cfg, $wlan_iface, $rate (
	$idx = get_wlan_index($wlan_iface);
	if ($radio1_legacy == 1) {
		if (!strlen($rate)) {
			$rate = w_get_rate($wlan_iface);
		}
		return cfg_get_def($cfg, "radio.$idx.rate.max", $rate);
	} else {
		if (!strlen($rate)) {
			$rate = 15;
		}
		return cfg_get_def($cfg, "radio.$idx.rate.mcs", $rate);
	}
);

Function cfg_get_channel_scan_list $cfg, $idx, $channel_scan_list
(
	if (!strlen($channel_scan_list))
	{
		$channel_scan_list = "disabled";
	}
	return cfg_get_def($cfg, "wireless.$idx.scan_list.status", $channel_scan_list);
);

Function set_channel_scan_list $cfg, $idx, $channel_scan_list
(
	if ($channel_scan_list != "enabled") {
		$channel_scan_list = "disabled";
	}
	return cfg_set($cfg, "wireless.$idx.scan_list.status", $channel_scan_list);
);

Function cfg_get_scan_channels $cfg, $idx, $scan_channels
(
	return cfg_get_def($cfg, "wireless.$idx.scan_list.channels", $scan_channels);
);

Function set_scan_channels $cfg, $idx, $scan_channels
(
	return cfg_set($cfg, "wireless.$idx.scan_list.channels", $scan_channels);
);

Function cfg_get_mac_acl $cfg, $idx, $mac_acl
(
	if (!strlen($mac_acl)) {
		$mac_acl = "disabled";
	}
	return cfg_get_def($cfg, "wireless.$idx.mac_acl.status", $mac_acl);
);

Function set_mac_acl $cfg, $idx, $mac_acl_status
(
	if ($mac_acl_status != "enabled") {
		$mac_acl_status = "disabled";
	}
	return cfg_set($cfg, "wireless.$idx.mac_acl.status", $mac_acl_status);
);

Function set_mac_acl_policy $cfg, $idx, $mac_acl_policy
(
	if ($mac_acl_policy != "deny") {
		$mac_acl_policy = "allow";
	}
	return cfg_set($cfg, "wireless.$idx.mac_acl.policy", $mac_acl_policy);
);

Function set_mac_acl_list $cfg, $idx, $mac_acl_list, $mac_acl_max
(
	$i = 0;
	$n = count($mac_acl_list);
	while ($i < $mac_acl_max) {
		if ($i < $n && strlen($mac_acl_list[$i]) > 0) {
			$status = "enabled";
			$value = $mac_acl_list[$i];
		}
		else {
			$status = "disabled";
			$value = "";
		}
		$i++;
		cfg_set($cfg, "wireless.$idx.mac_acl."+$i+".status", $status);
		cfg_set($cfg, "wireless.$idx.mac_acl."+$i+".mac", $value);
	}
);

Function cfg_get_mac_acl_policy $cfg, $idx, $policy
(
	if (!strlen($policy)) {
		$policy = "allow";
	}
	return cfg_get_def($cfg, "wireless.$idx.mac_acl.policy", $policy);
);

Function cfg_get_mac_acl_list $cfg, $idx, $mac_acl_max
(
	$i = 0;
	while ($i < $mac_acl_max) {
		$i++;
		$enabled = cfg_get_def($cfg, "wireless.$idx.mac_acl.$i.status", "disabled");
		if ($enabled == "enabled") {
			$mac = cfg_get_def($cfg, "wireless.$idx.mac_acl.$i.mac", "");
			if (strlen($mac)) {
				$result[] = $mac;
			}
		}
	}
	return $result;
);

Function cfg_get_clksel $cfg, $wlan_iface, $clksel (
	$idx = get_wlan_index($wlan_iface);
	$clksel = cfg_get_def($cfg, "radio.$idx.clksel", $clksel);
	if ($clksel == "4") {
		$clksel = 2;
	}
	elseif ($clksel == "2") {
		$clksel = 1;
	}
	else {
		$clksel = 0;
	}
	return $clksel;
);

Function cfg_get_chanshift $cfg, $wlan_iface, $chanshift (
	if (!strlen($chanshift)) { $chanshift = 0; }
	$idx = get_wlan_index($wlan_iface);
	$ret = cfg_get_def($cfg, "radio.$idx.chanshift", $chanshift);
	return $ret;
);

Function cfg_get_ieee_mode $cfg, $wlan_iface, $mode (
	global $radio;
	if (!IsSet($radio["name"])) {
		init_board_inc($wlan_iface);
	}

	if ($radio["ccode_fixed"] == 1  &&
            $radio["caps"] & $radio_cap_11n_no_ht40) {
		if ($radio["ieee_mode_a"] == 1) {
			$ret = "11naht";
		}
		else {
			$ret = "11nght";
		}
	}
	else {
		if (!strlen($mode)) {
			$mode = "11nght";
		}
		$idx = get_wlan_index($wlan_iface);
		$ret = cfg_get_def($cfg, "radio.$idx.ieee_mode", $mode);
	}
	return $ret;
);

Function cfg_get_security $cfg, $wlan_iface, $security, $wmode (
	$idx = get_wlan_index($wlan_iface);
	$sec = cfg_get_def($cfg, "wireless.$idx.security", "none");
	$newsec = substr($sec, 0, 3);
	if ($newsec == "wep") {
		$sec = "wep";
	}
	else {
		$newsec = "";
		switch ($wmode)
		{
		case "apwds";
		case "ap";
			$w_status = cfg_get_def($cfg, "aaa.status", "disabled");
			$w_dev_status = cfg_get_def($cfg, "aaa.$idx.status", "enabled");
			$newsec = cfg_get_def($cfg, "aaa.$idx.wpa", 1 );
			$pkey = cfg_get_def($cfg, "aaa.$idx.wpa.1.pairwise", "TKIP CCMP" );
			break;
		case "stawds";
		case "sta";
		default;
			$w_status = cfg_get_def($cfg, "wpasupplicant.status", "disabled");
			$w_dev_status = cfg_get_def($cfg, "wpasupplicant.device.$idx.status", "enabled");
			$newsec = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.proto.1.name", "WPA" );
			$pkey = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.pairwise.1.name", "TKIP CCMP" );
			break;
		};
		if ($w_status == "enabled" && $w_dev_status == "enabled") {
			$sec = "wpa";
			if ($newsec == "RSN" || $newsec == 2) {
				$sec = "wpa2";
			}
			switch ($pkey)
			{
			case "TKIP";
				$sec = $sec + "tkip";
				break;
			case "CCMP";
				$sec = $sec + "aes";
				break;
			}
		}
	}
	return $sec;
);

Function cfg_get_def_wep_id $cfg, $idx, $wep_key_id (
	if (!strlen($wep_key_id)) {
		$wep_key_id = 1;
	}
	return cfg_get_def($cfg, "wireless.$idx.security.default_key", $wep_key_id);
);

Function cfg_get_wep_key $cfg, $idx, $wep_key_id, $wep_key (
	if (!strlen($wep_key_id)) {
		$wep_key_id = 1;
	}
	return cfg_get_def($cfg, "wireless.$idx.security.$wep_key_id.key", $wep_key);
);

Function cfg_get_wep_key_length $cfg, $idx, $len (
	$key_len = cfg_get_def($cfg, "wireless.$idx.security", "wep64");
	if ($key_len == "none") {
		$key_len = "wep64";
	}
	return $key_len;
);

Function cfg_get_wpa_key $cfg, $idx, $wmode, $wpa_key (
	if ($wmode == "ap" || $wmode == "apwds") {
		$wpa_key = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.psk", $wpa_key);
		$wpa_key = cfg_get_def($cfg, "aaa.$idx.wpa.psk", $wpa_key);
	} else {
		$wpa_key = cfg_get_def($cfg, "aaa.$idx.wpa.psk", $wpa_key);
		$wpa_key = cfg_get_def($cfg, "wpasupplicant.profile.$idx.network.1.psk", $wpa_key);
	}
	return $wpa_key;
);

Function cfg_get_authtype $cfg, $idx, $type (
	if (!strlen($type)) {
		$type = 1;
	}
	return cfg_get_def($cfg, "wireless.$idx.authmode", $type);
);

Function cfg_get_ackdistance $cfg, $wlan_iface, $sltconst (
	$idx = get_wlan_index($wlan_iface);
	$distance = cfg_get_def($cfg, "radio.$idx.ackdistance", "");
	if (!strlen($distance)) {
		$ack = cfg_get_def($cfg, "radio.$idx.acktimeout", 0);
		$ack = intVal($ack);
		$ack -= $sltconst * 2 + 3;
		if ($ack > 0) {
			$distance = $ack * 150; /* speed of light (m/us)/2 */
		}
		else {
			$distance = 0;
		}
	}
	return intVal($distance);
);

Function cfg_get_autoack $cfg, $wlan_iface, $autoack (
	if (!strlen($autoack)) {
		$autoack = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_get_def($cfg, "radio.$idx.ack.auto", $autoack);
);

Function cfg_get_fast_frame $cfg, $wlan_iface, $fast_frame (
	if (!strlen($fast_frame)) {
		$fast_frame = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_get_def($cfg, "wireless.$idx.fastframes", $fast_frame);
);

Function cfg_get_bursting $cfg, $wlan_iface, $burst (
	if (!strlen($burst)) {
		$burst = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_get_def($cfg, "wireless.$idx.frameburst", $burst);
);

Function cfg_get_compression $cfg, $wlan_iface, $compression (
	if (!strlen($compression)) {
		$compression = "disabled";
	}
	$idx = get_wlan_index($wlan_iface);
	return cfg_get_def($cfg, "wireless.$idx.compression", $compression);
);

Function cfg_get_dhcpc $cfg, $iface, $dhcpc (
	if (!strlen($dhcpc)) {
		$dhcpc = "disabled";
	}
	$enabled = cfg_get_def($cfg, "dhcpc.status", $dhcpc);
	$dhcpc = cfg_get_def($cfg, "dhcpc.1.status", $dhcpc);

	if ($enabled == "enabled" && $dhcpc == "enabled") {
		$result = $dhcpc;
	}
	else {
		$result = "disabled";
	}
	return $result;
);

Function cfg_get_autoip $cfg, $iface, $autoip_status
(
	$status = "enabled";
	if (strlen($autoip_status)) {
		$status = $autoip_status;
	}
	else {
		$idx = get_iface_index($iface);
		$status = cfg_get_def($cfg, "netconf.$idx.autoip.status", $status);
	}
	return $status;
);

Function cfg_get_ipv4addr $cfg, $iface, $ip (
	if (!strlen($ip)) {
		$ip = if_get_ipaddr($iface);
	}
	$idx = get_iface_index($iface);
	$ip = cfg_get_def($cfg, "netconf.$idx.ip", $ip);
	return $ip;
);

Function cfg_get_ipv4mask $cfg, $iface, $mask (
	if (!strlen($mask)) {
		$mask = "255.255.255.0";
	}
	$idx = get_iface_index($iface);
	$mask = cfg_get_def($cfg, "netconf.$idx.netmask", $mask);
	return $mask;
);

Function cfg_get_ipv4gw $cfg, $iface, $gw (
	if (!strlen($gw)) {
		$gw = "0.0.0.0";
	}
	return cfg_get_def($cfg, "route.1.gateway", $gw);
);

Function cfg_get_dns $cfg, $dns, $no (
	if (cfg_get_def($cfg, "resolv.nameserver."+$no+".status", "disabled") == "enabled") {
		$dns = cfg_get_def($cfg, "resolv.nameserver."+$no+".ip", $dns);
	}
	return $dns;
);

Function set_gsm $cfg, $iface, $user, $pass, $apn, $gsm (
	if ($gsm == "on" || $gsm == "enabled") {
		$gsm = "enabled";
		cfg_set($cfg, "3g.1.user", $user);
		cfg_set($cfg, "3g.1.password", $pass);
		cfg_set($cfg, "3g.1.apn", $apn);
	}
	else {
		$gsm = "disabled";
	}
	cfg_set($cfg, "3g.status", $gsm);
	return cfg_set($cfg, "3g.1.status", $gsm);
);

Function set_gsm_pin_code $cfg, $iccid, $pin (
	cfg_set($cfg, "3g.1.pin.status", "enabled");
	cfg_set($cfg, "3g.1.pin.iccid", $iccid);
	return cfg_set($cfg, "3g.1.pin.code", $pin);
);

Function cfg_get_gsm_pin_status $cfg (
       return cfg_get_def($cfg, "3g.1.pin.status", "disabled");
);

Function cfg_get_gsm $cfg, $iface, $gsm (
	if ($gsm == "on" || $gsm == "enabled") {
		$gsm = "enabled";
	}
	else {
		$gsm = "disabled";
	}
	$enabled = cfg_get_def($cfg, "3g.status", $gsm);
	$gsm = cfg_get_def($cfg, "3g.1.status", $gsm);

	if ($enabled != "enabled" || $gsm != "enabled") {
		$gsm = "disabled";
	}
	return $gsm;
);

Function cfg_get_gsm_pin $cfg, $iface, $pin (
	return cfg_get_def($cfg, "3g.1.pin.code", "");
);

Function cfg_get_gsm_user $cfg, $iface, $user (
	return cfg_get_def($cfg, "3g.1.user", $user);
);

Function cfg_get_gsm_pass $cfg, $iface, $pass (
	return cfg_get_def($cfg, "3g.1.password", $pass);
);

Function cfg_get_gsm_apn $cfg, $iface, $apn (
	return cfg_get_def($cfg, "3g.1.apn", $apn);
);

Function validate_pincode $device, $pin (
	global $simpinok_file;
	$cmd = "PINCODE=" + $pin + " gcom -d " + $device + " -s /usr/etc/gcom/setpin.gcom && touch " + $simpinok_file;

	exec($cmd, $lines, $res);
	$error_msg = "";
	if ($res != 0) {
		$error_msg = dict_translate("PIN code incorrect. Three wrong entry of PIN code will result in SIM blocking.");
	}

	return $error_msg;
);

Function set_pppoe $cfg, $iface, $user, $pass, $mppe, $pppoe, $mtu, $mru (
	if ($pppoe == "on" || $pppoe == "enabled") {
		$pppoe = "enabled";
		cfg_set($cfg, "ppp.1.devname", $iface);
		cfg_set($cfg, "ppp.1.name", $user);
		cfg_set($cfg, "ppp.1.password", $pass);
		if ($mppe == "on" || $mppe == "enabled") {
			cfg_set($cfg, "ppp.1.require.mppe128", "enabled");
		} else {
			cfg_set($cfg, "ppp.1.require.mppe128", "disabled");
		}
		cfg_set($cfg, "iptables.3.cmd","-A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu");
		cfg_set($cfg, "ppp.1.mtu", $mtu);
		cfg_set($cfg, "ppp.1.mru", $mru);
	}
	else {
		$pppoe = "disabled";
	}
	cfg_set($cfg, "ppp.status", $pppoe);
	cfg_set($cfg, "iptables.3.status", $pppoe);
	return cfg_set($cfg, "ppp.1.status", $pppoe);
);

Function cfg_get_pppoe $cfg, $iface, $pppoe (
	if ($pppoe == "on" || $pppoe == "enabled") {
		$pppoe = "enabled";
	}
	else {
		$pppoe = "disabled";
	}
	$enabled = cfg_get_def($cfg, "ppp.status", $pppoe);
	$pppoe = cfg_get_def($cfg, "ppp.1.status", $pppoe);

	if ($enabled != "enabled" || $pppoe != "enabled") {
		$pppoe = "disabled";
	}
	return $pppoe;
);

Function cfg_get_pppoe_pass $cfg, $iface, $pass (
	return cfg_get_def($cfg, "ppp.1.password", $pass);
);

Function cfg_get_pppoe_mppe $cfg, $iface, $mppe (
	$mppe = cfg_get_def($cfg, "ppp.1.require.mppe128", $mppe);
	if ($mppe == "on" || $mppe == "enabled") {
		$mppe = "enabled";
	}
	else {
		$mppe = "disabled";
	}
	return $mppe;
);

Function cfg_get_pppoe_user $cfg, $iface, $user (
	return cfg_get_def($cfg, "ppp.1.name", $user);
);

Function cfg_get_pppoe_mtu $cfg, $iface, $mtu (
	if (strlen($mtu) == 0) {
		$mtu = 1492;
	}
	return cfg_get_def($cfg, "ppp.1.mtu", $mtu);
);

Function cfg_get_pppoe_mru $cfg, $iface, $mru (
	if (strlen($mru) == 0) {
		$mru = 1492;
	}
	return cfg_get_def($cfg, "ppp.1.mru", $mru);
);

Function set_dhcpd $cfg, $iface, $start, $end, $mask, $lease, $dhcpd, $dnsproxy, $dns1, $dns2 (
	if ($dhcpd == "on" || $dhcpd == "enabled") {
		$dnsmasq = "disabled";
		$dhcpd = "enabled";
		cfg_set($cfg, "dhcpd.1.devname", $iface);
		cfg_set($cfg, "dhcpd.1.start", $start);
		cfg_set($cfg, "dhcpd.1.end", $end);
		cfg_set($cfg, "dhcpd.1.netmask", $mask);
		cfg_set($cfg, "dhcpd.1.lease_time", $lease);
		if (!strlen($dnsproxy)) {
			$dnsproxy = "disabled";
		}
		cfg_set($cfg, "dhcpd.1.dnsproxy", $dnsproxy);
		$status = "disabled";
		if (strlen($dns1)) {
			$status = "enabled";
		}
		cfg_set($cfg, "dhcpd.1.dns.1.status", $status);
		cfg_set($cfg, "dhcpd.1.dns.1.server", $dns1);
		$status = "disabled";
		if (strlen($dns2)) {
			$status = "enabled";
		}
		cfg_set($cfg, "dhcpd.1.dns.2.status", $status);
		cfg_set($cfg, "dhcpd.1.dns.2.server", $dns2);
	}
	else {
		$dnsmasq = "enabled";
		$dhcpd = "disabled";
	}
	cfg_set($cfg, "dnsmasq.status", $dnsmasq);
	cfg_set($cfg, "dnsmasq.1.devname", $iface);
	cfg_set($cfg, "dnsmasq.1.status", $dnsmasq);
	cfg_set($cfg, "dhcpd.status", $dhcpd);
	return cfg_set($cfg, "dhcpd.1.status", $dhcpd);
);

Function cfg_get_dhcpd $cfg, $iface, $dhcpd (
	if ($dhcpd == "on" || $dhcpd == "enabled") {
		$dhcpd = "enabled";
	}
	else {
		$dhcpd = "disabled";
	}
	$enabled = cfg_get_def($cfg, "dhcpd.status", $dhcpd);
	$dhcpd = cfg_get_def($cfg, "dhcpd.1.status", $dhcpd);

	if ($enabled != "enabled" || $dhcpd != "enabled") {
		$dhcpd = "disabled";
	}
	return $dhcpd;
);

Function cfg_get_dhcpd_dnsproxy $cfg, $iface, $dhcpd_dns_proxy (
	if (!strlen($dhcpd_dns_proxy)) {
		$dhcpd_dns_proxy = "enabled";
	}
	$dhcpd_dns_proxy = cfg_get_def($cfg, "dhcpd.1.dnsproxy", $dhcpd_dns_proxy);
	return $dhcpd_dns_proxy;
);

Function cfg_get_dhcpd_lease $cfg, $iface, $lease (
	if (!strlen($lease)) {
		$lease = "3600";
	}
	return cfg_get_def($cfg, "dhcpd.1.lease_time", $lease);
);

Function cfg_get_dhcpd_start $cfg, $iface, $start (
	return cfg_get_def($cfg, "dhcpd.1.start", $start);
);

Function cfg_get_dhcpd_end $cfg, $iface, $end (
	return cfg_get_def($cfg, "dhcpd.1.end", $end);
);

Function cfg_get_dhcpd_mask $cfg, $iface, $mask (
	if (!strlen($mask)) {
		$mask = "255.255.255.0";
	}
	return cfg_get_def($cfg, "dhcpd.1.netmask", $mask);
);

Function cfg_get_dhcpd_dns $cfg, $iface, $dns, $no (
	if (!strlen($no)) {
        	$no = "1";
        }
	if (!strlen($dns)) {
        	$dns = cfg_get_dns($cfg, $dns, $no);
        }
        if (cfg_get_def($cfg, "dhcpd.1.dns."+$no+".status", "disabled") == "enabled") {
        	$dns = cfg_get_def($cfg, "dhcpd.1.dns."+$no+".server", $dns);
        }
	return $dns;
);

Function set_route_cfg $cfg, $idx, $variable_order, $cfg_line
(
	/* adjust offset: 1 - default route
	   actual routes starts from 2 */
	$idx = 1 + $idx;
	$i = 0;
	while ($i < count($variable_order)) {
		$var_name = $variable_order[$i];
		$$var_name = $cfg_line[$i];
		$i++;
	}
	cfg_set($cfg, "route."+$idx+".ip", $ip);
	cfg_set($cfg, "route."+$idx+".netmask", $netmask);
	cfg_set($cfg, "route."+$idx+".gateway", $gateway);
	/* devname currently isn't used for non default routes */
	/* cfg_set($cfg, "route."+$idx+".devname", $devname); */
	cfg_set($cfg, "route."+$idx+".comment", $comment);
	cfg_set($cfg, "route."+$idx+".status", $status);
);

Function cfg_get_route_cfg $cfg, $idx, $variable_order
(
	/* adjust offset: 1 - default route
	   actual routes starts from 2 */
	$idx = 1 + $idx;

	$def_status = "enabled";
	$ip = cfg_get_def($cfg, "route."+$idx+".ip", "");
	if (strlen($ip) == 0) {
		$def_status = "disabled";
	}
	$status = cfg_get_def($cfg, "route."+$idx+".status", $def_status);
	$netmask = cfg_get_def($cfg, "route."+$idx+".netmask", "");
	$gateway = cfg_get_def($cfg, "route."+$idx+".gateway", "");
	/* devname currently isn't used for non default routes */
	/* $devname = cfg_get_def($cfg, "route."+$idx+".devname", ""); */
	$comment = cfg_get_def($cfg, "route."+$idx+".comment", "");

	$i = 0;
	while ($i < count($variable_order)) {
		$name = $variable_order[$i];
		$result[$i] = $$name;
		$i++;
	}
	return $result;
);

Function set_port_forward_cfg $cfg, $idx, $variable_order, $cfg_line
(
	/* adjust offset: 1 - nat, 2 - dmz, 3 - ppp mtu fix, 4,5 - port forward state
	   actual rules starts from 6 */
	$idx = 5 + $idx;
	$i = 0;
	while ($i < count($variable_order)) {
		$var_name = $variable_order[$i];
		$$var_name = $cfg_line[$i];
		$i++;
	}

	/* trim */
	$src_port = ereg_replace("(^\s+)", '', $src_port);
	$src_port = ereg_replace("\s+$", '', $src_port);
	$dst_port = ereg_replace("(^\s+)", '', $dst_port);
	$dst_port = ereg_replace("\s+$", '', $dst_port);
	$cmd = "";

	if (strlen($src_port) != 0) {
        	if (strlen($src_ip) == 0) {
                   $src_ip = "0.0.0.0/0";
                }
		$cmd = "-t nat -A PORTFORWARD -p " + $protocol + " -s " + $src_ip
			+ " --dport " + $src_port + " -j DNAT --to " + $dst_ip;
		if (strlen($dst_port) != 0) {
			$cmd = $cmd + ":" + $dst_port;
		}
	}
	cfg_set($cfg, "iptables."+$idx+".cmd", $cmd);
	cfg_set($cfg, "iptables."+$idx+".comment", $comment);
	cfg_set($cfg, "iptables."+$idx+".status", $enabled);
);

Function cfg_get_port_forward_cfg $cfg, $idx, $variable_order
(
	/* adjust offset: 1 - nat, 2 - dmz, 3 - ppp mtu fix, 4,5 - port forward state
	   actual rules starts from 6 */
	$idx = 5 + $idx;
	$enabled = cfg_get_def($cfg, "iptables."+$idx+".status", "disabled");
	$comment = cfg_get_def($cfg, "iptables."+$idx+".comment", "");
	$cmd = cfg_get_def($cfg, "iptables."+$idx+".cmd", "");
	$variable = "";
	$delim = " ";
	$tok = strtok($cmd, $delim);
	while ($tok) {
		if ($tok == "-p") {
			$variable = "protocol";
		}
		elseif ($tok == "-s")
		{
			$variable = "src_ip";
		}
		elseif ($tok == "--dport") {
			$variable = "src_port";
		}
		elseif ($tok == "--to") {
			$variable = "dst_ip";
		}
		elseif (strlen($variable) != 0) {
			$$variable = $tok;
			if ($variable == "dst_ip") {
				$variable = "dst_port";
			}
			elseif ($variable == "src_port") {
				$delim = " :";
				$variable = "";
			}
			else {
				$variable = "";
			}
		}
		$tok = strtok($delim);
	}
	$i = 0;
	while ($i < count($variable_order)) {
		$name = $variable_order[$i];
		$result[$i] = $$name;
		$i++;
	}
	return $result;
);

Function cfg_get_ipalias_cfg $cfg, $iface, $idx, $variable_order
(
	$iface_idx = get_iface_index($iface);
	if ($iface_idx != 0) {
		$prefix = "netconf."+$iface_idx+".alias."+$idx;
		$enabled = cfg_get_def($cfg, ""+$prefix+".status", "disabled");
		$ip = cfg_get_def($cfg, ""+$prefix+".ip", "");
		$netmask = cfg_get_def($cfg, ""+$prefix+".netmask", "");
		$comment = cfg_get_def($cfg, ""+$prefix+".comment", "");
	}

	$i = 0;
	while ($i < count($variable_order)) {
		$name = $variable_order[$i];
		$result[$i] = $$name;
		$i++;
	}
	return $result;
);

Function set_ipalias_cfg $cfg, $iface, $idx, $variable_order, $cfg_line
(
	$i = 0;
	while ($i < count($variable_order)) {
		$var_name = $variable_order[$i];
		$$var_name = $cfg_line[$i];
		$i++;
	}

	$iface_idx = get_iface_index($iface);
	if ($iface_idx != 0)
	{
		$prefix = "netconf."+$iface_idx+".alias."+$idx;
		cfg_set($cfg, ""+$prefix+".ip", $ip);
		cfg_set($cfg, ""+$prefix+".netmask", $netmask);
		cfg_set($cfg, ""+$prefix+".comment", $comment);
		cfg_set($cfg, ""+$prefix+".status", $enabled);
	}
);

Function disable_ipaliases $cfg, $iface, $max_aliases
(
	$iface_idx = get_iface_index($iface);
	if ($iface_idx) {
		$i = 0;
		$prefix = "netconf."+$iface_idx+".alias.";
		while ($i < $max_aliases) {
			$i++;
			cfg_set($cfg, ""+$prefix+$i+".status", "disabled");
		}
	}
);

Function copy_ipaliases $cfg, $from_iface, $to_iface, $max_aliases
(
	$var_names[] = "ip";
	$var_names[] = "netmask";
	$var_names[] = "enabled";
	$var_names[] = "comment";

	$i = 0;
	while ($i < $max_aliases) {
		$i++;
		$ipalias_cfg = cfg_get_ipalias_cfg($cfg, $from_iface, $i, $var_names);
		set_ipalias_cfg($cfg, $to_iface, $i, $var_names, $ipalias_cfg);
	}	
);

Function cfg_get_port_forward $cfg, $iface, $port_forward
(
	$iptables = cfg_get_def($cfg, "iptables.status", $port_forward);
	$port_forward_chain = cfg_get_def($cfg, "iptables.4.status", $port_forward);
	$port_forward_rule = cfg_get_def($cfg, "iptables.5.status", $port_forward);

	if ($iptables == "enabled" && $port_forward_chain == "enabled" &&
		$port_forward_rule == "enabled") {
		$port_forward = "enabled";
	}
	else {
		$port_forward = "disabled";
	}
	return $port_forward;
);

Function set_port_forward $cfg, $iface, $port_forward
(
	if ($port_forward == "enabled") {
		cfg_set($cfg, "iptables.4.cmd", "-t nat -N PORTFORWARD");
		cfg_set($cfg, "iptables.5.cmd", "-t nat -I PREROUTING -i $iface"
			+ " -j PORTFORWARD");
		cfg_set($cfg, "iptables.status", "enabled");
	}
	else {
		$port_forward = "disabled";
	}

	cfg_set($cfg, "iptables.4.status", $port_forward);
	return cfg_set($cfg, "iptables.5.status", $port_forward);
);

Function set_firewall_cfg $cfg, $idx, $variable_order, $cfg_line, $netmode
(
	/* adjust offset: 1 - nat, 2 - dmz, 3 - ppp mtu fix, 4,5 - port forward state, 50,51,52 - firewall state
	   actual rules starts from 52 */
	$tbl = "ebtables";
	$prefix = " --ip";
	$p = " -p 0x0800";
	if ($netmode != "bridge") {
		$tbl = "iptables";
		$prefix = " -";
		$p = "";
	}
	$idx = 52 + $idx;
	$i = 0;
	while ($i < count($variable_order)) {
		$var_name = $variable_order[$i];
		$$var_name = $cfg_line[$i];
		$i++;
	}
	if (strlen($src_ip) != 0) {
		$cmd = "-A FIREWALL";
		if (strlen($input_ifc) != 0) {
			$cmd = $cmd + " -i "+ $input_ifc;
		}
		$cmd = $cmd + $p;
		if ($protocol == "-1") {
			$cmd = $cmd + " -m ipp2p --ipp2p";
		}
		else {
                	if ($protocol != 0 || $tbl != "ebtables") {
				$cmd = $cmd + $prefix + "-protocol "+$protocol;
                        }
		}
		$cmd = $cmd + $prefix+"-src ";
		if (strlen($not_src_ip) != 0) {
			$cmd = $cmd + "! ";
		}
		$cmd = $cmd + $src_ip;
		if (strlen($src_port) != 0 && $protocol != 0) {
			$cmd = $cmd + $prefix+"-sport ";
			if (strlen($not_src_port) != 0) {
				$cmd = $cmd + "! ";
			}
			$cmd = $cmd+$src_port;
		}
		$cmd = $cmd + $prefix+"-dst ";
		if (strlen($not_dst_ip) != 0) {
			$cmd = $cmd + "! ";
		}
		$cmd = $cmd+$dst_ip;
		if (strlen($dst_port) != 0 && $protocol != 0) {
			$cmd = $cmd + $prefix+"-dport ";
			if (strlen($not_dst_port) != 0) {
				$cmd = $cmd + "! ";
			}
			$cmd = $cmd+$dst_port;
		}
		if (strlen($act) != 0) {
			$cmd = $cmd + " -j " + $act;
		} else {
			$cmd = $cmd + " -j DROP";
		}

		cfg_set($cfg, $tbl+"."+$idx+".cmd", $cmd);
	}
	else {
		cfg_set($cfg, $tbl+"."+$idx+".cmd", "");
	}
	cfg_set($cfg, $tbl+"."+$idx+".comment", $comment);
	cfg_set($cfg, $tbl+"."+$idx+".status", $enabled);
);


Function cfg_get_firewall_cfg $cfg, $idx, $variable_order, $netmode
(
	/* adjust offset: 1 - nat, 2 - dmz, 3 - ppp mtu fix, 4,5 - port forward state, 50,51,52 - firewall state
	   actual rules starts from 53 */
	$tbl = "ebtables";
	if ($netmode != "bridge") {
		$tbl = "iptables";
	}
	$idx = 52 + $idx;
	$enabled = cfg_get_def($cfg, $tbl+"."+$idx+".status", "disabled");
	$comment = cfg_get_def($cfg, $tbl+"."+$idx+".comment", "");
	$cmd = cfg_get_def($cfg, $tbl+"."+$idx+".cmd", "");
	$variable = "";
	$tok = strtok($cmd, " ");
	while ($tok) {
		if ($tok == "-i") {
			$variable = "input_ifc";
		}
		elseif (strlen(strstr($tok, "-protocol")) != 0) {
			$variable = "protocol";
		}
		elseif ($tok == "--ipp2p") {
			$protocol = "-1";
			$variable = "";
		}
		elseif (strlen(strstr($tok, "-dport")) != 0) {
			$variable = "dst_port";
		}
		elseif (strlen(strstr($tok, "-sport")) != 0) {
			$variable = "src_port";
		}
		elseif (strlen(strstr($tok, "-dst")) != 0) {
			$variable = "dst_ip";
		}
		elseif (strlen(strstr($tok, "-src")) != 0) {
			$variable = "src_ip";
		}
		elseif (strlen(strstr($tok, "-j")) != 0) {
			$variable = "act";
		}
		elseif (strlen($variable) != 0) {
			if ($tok == "!") {
				$not_variable = "not_"+$variable;
				$$not_variable = $tok;
			}
			else {
				$$variable = $tok;
				$variable = "";
			}
		}
		$tok = strtok(" ");
	}
	$i = 0;
	while ($i < count($variable_order)) {
		$name = $variable_order[$i];
		$result[$i] = $$name;
		$i++;
	}
	return $result;
);

Function set_firewall $cfg, $firewall, $netmode
(
	$tbl = "ebtables";
	$oth_tbl = "iptables";
	if ($netmode != "bridge") {
		$tbl = "iptables";
		$oth_tbl = "ebtables";
	}

	cfg_set($cfg, "$oth_tbl.50.status", "disabled");
	cfg_set($cfg, "$oth_tbl.51.status", "disabled");
	cfg_set($cfg, "$oth_tbl.52.status", "disabled");

	if ($firewall == "enabled") {
		cfg_set($cfg, $tbl+".50.cmd", "-N FIREWALL");
		cfg_set($cfg, $tbl+".51.cmd", "-A INPUT -j FIREWALL");
		cfg_set($cfg, $tbl+".52.cmd", "-A FORWARD -j FIREWALL");
		cfg_set($cfg, $tbl+".status", "enabled");
	} else {
		$firewall = "disabled";
	}

	cfg_set($cfg, $tbl+".50.status", $firewall);
	cfg_set($cfg, $tbl+".51.status", $firewall);
	return cfg_set($cfg, $tbl+".52.status", $firewall);
);

Function cfg_get_firewall $cfg, $firewall, $netmode
(
	$tbl = "ebtables";
	if ($netmode != "bridge") {
		$tbl = "iptables";
	}
	$f = cfg_get_def($cfg, $tbl+".status", $firewall);
	$firewall_chain = cfg_get_def($cfg, $tbl+".50.status", $firewall);
	$firewall_rule = cfg_get_def($cfg, $tbl+".51.status", $firewall);

	if ($f == "enabled" && $firewall_chain == "enabled" && $firewall_rule == "enabled")
	{
		$firewall = "enabled";
	}
	else {
		$firewall = "disabled";
	}
	return $firewall;
);

Function cfg_get_nat $cfg, $iface, $nat (
	if ($nat == "on" || $nat == "enabled") {
		$nat = "enabled";
	}
	else {
		$nat = "disabled";
	}
	$enabled = cfg_get_def($cfg, "iptables.status", $nat);
	$nat = cfg_get_def($cfg, "iptables.1.status", $nat);

	if ($enabled != "enabled" || $nat != "enabled") {
		$nat = "disabled";
	}
	return $nat;
);

Function cfg_get_DMZ_status $cfg, $iface, $dmz (
	$iptables = cfg_get_def($cfg, "iptables.status", $dmz);
	$dmz = cfg_get_def($cfg, "iptables.2.status", $dmz);
	if ($iptables != "enabled" && $dmz != "enabled") {
		$dmz = "disbled";
	}
	return $dmz;
);

Function cfg_get_DMZ_ip $cfg, $iface, $ip (
	$v = cfg_get_def($cfg, "iptables.2.cmd", "-t nat -A PREROUTING -i "+$iface+" -j DNAT --to-destination "+$ip);
	$ip = strrchr($v, " ");
	if ($ip == " DMZ_MGMT") {
      		$i=0;
                $idx = 200;
       		while($i < 11);
			$v = cfg_get_def($cfg, "iptables."+$idx+".cmd", "-t nat -A DMZ_MGMT -j DNAT --to-destination "+$ip);
                        if (strlen(strstr($v, "--to-destination")) != 0) {
                        	$i = 11; /* break */
                        }
               		$idx = $idx + 1;
       		        $i++;
       		endwhile;
		$ip = strrchr($v, " ");
	} elseif ($ip == " ACCEPT") {
		$v = cfg_get_def($cfg, "iptables.200.cmd", "-t nat -A PREROUTING -i "+$iface+" -j DNAT --to-destination "+$ip);
		$ip = strrchr($v, " ");
        }
	if (substr($ip, 1, 2) == "--") {
		$ip = "";
	}
	else {
		$ip = substr($ip, 1, strlen($ip));
	}
	return $ip;
);

Function cfg_get_DMZ_iface $cfg, $iface, $ip (
	$v = cfg_get_def($cfg, "iptables.2.cmd", "-t nat -A PREROUTING -i "+$iface+" -j DNAT --to-destination "+$ip);
        $v = strstr($v, "-i");
        $v = substr($v, 3, strlen($v));
        return strtok($v, " ");
);

Function cfg_get_DMZ_mgmt $cfg, $iface, $mgmt (
	$ret = "enabled";
	$v = cfg_get_def($cfg, "iptables.2.cmd", "");
	if (strlen(strstr($v, "DMZ_MGMT")) != 0 || strlen(strstr($v, "!")) != 0 || strlen(strstr($v, "ACCEPT")) != 0)
	{
		$ret = "disabled";
	}
	return $ret;
);

Function cfg_update_dmz_mgmt $cfg (
	$netmode = cfg_get_def($cfg, "netmode", "bridge");
        if ($netmode != "bridge" &&
            cfg_get_DMZ_status($cfg, "", "disbled") == "enabled") {
        	$iface = cfg_get_DMZ_iface($cfg, "", "");
        	if (cfg_get_DMZ_mgmt($cfg, $iface, "disabled") == "disabled")
                {
                        $ip = cfg_get_DMZ_ip($cfg, $iface, "");
                        set_DMZ($cfg, $iface, "enabled", $ip, "disabled");
                }
        }
);

Function cfg_get_fallback_ip $cfg, $iface, $ip (
	$fallback = "192.168.1.20";
	$i = 1;
	while ($i < 5) {
		$key = "dhcpc." + $i + ".";
		$ifc = cfg_get_def($cfg, $key + "devname", "");
		if ($iface == $ifc) {
			$i = 16;
		}
		$i = $i + 1;
	}
	if ($i == 16) {
		$key += "fallback";
	}
	else {
		$key = "dhcpc.1.fallback";
	}
	return cfg_get_def($cfg, $key, $fallback);
);

Function cfg_get_fallback_netmask $cfg, $iface, $netmask (
	$nm = "255.255.255.0";
	$i = 1;
	while ($i < 5) {
		$key = "dhcpc." + $i + ".";
		$ifc = cfg_get_def($cfg, $key + "devname", "");
		if ($iface == $ifc) {
			$i = 16;
		}
		$i = $i + 1;
	}
	if ($i == 16) {
		$key += "fallback_netmask";
	}
	else {
		$key = "dhcpc.1.fallback_netmask";
	}
	return cfg_get_def($cfg, $key, $nm);
);

Function cfg_get_igmp_status $cfg, $iface, $igmp (
	$igmp = cfg_get_def($cfg, "igmpproxy.status", $igmp);
	return $igmp;
);

Function cfg_get_igmp_upstream $cfg, $iface (
	$upstream = cfg_get_def($cfg, "igmpproxy.upstream.devname", $iface);
	if (($upstream == $iface) || ($upstream == "ppp0")) {
		return "wan";
	else {
		return "lan";
	}
);

Function set_nat $cfg, $iface, $nat (
	if ($nat == "on" || $nat == "enabled") {
		$nat = "enabled";
		cfg_set($cfg, "iptables.1.cmd", "-t nat -I POSTROUTING -o "+$iface+" -j MASQUERADE");
		cfg_set($cfg, "iptables.status", $nat);
	}
	else {
		$nat = "disabled";
	}
	return cfg_set($cfg, "iptables.1.status", $nat);
);

Function get_mgmt_info $cfg (
	$res["http"] = cfg_get_def($cfg, "httpd.port", "80");
	$res["airview"] = cfg_get_def($cfg, "airview.tcp_port", "18888");
	$res["icmp"] = "8";
	if (cfg_get_def($cfg, "sshd.status", "enabled") == "enabled") {
		$res["ssh"] = cfg_get_def($cfg, "sshd.port", "22");
	}
	if (cfg_get_def($cfg, "httpd.https.status", "disabled") == "enabled") {
		$res["https"] = cfg_get_def($cfg, "httpd.https.port", "443");
	}
	if (cfg_get_def($cfg, "telnetd.status", "disabled") == "enabled") {
		$res["telnet"] = cfg_get_def($cfg, "telnetd.port", "23");
	}        
        if (cfg_get_def($cfg, "snmp.status", "disabled") == "enabled") {
        	$res["snmp"] = "161";
        }
        if (cfg_get_def($cfg, "discovery.status", "enabled") == "enabled") {
        	$res["discover"] = "1001";
        }
	return $res;
);

Function set_DMZ $cfg, $iface, $dmz, $ip, $mgmt (
	if ($dmz == "on" || $dmz == "enabled") {
		$dmz = "enabled";
		$cmd = "-t nat -A PREROUTING -i "+$iface;
		if (strlen($mgmt) == 0 || $mgmt == "disabled") {
                        $cmd = $cmd + " -j DMZ_MGMT";
                        $cmd = "-t nat -N DMZ_MGMT; iptables "+$cmd;
			$idx = 200;
                        $ports = get_mgmt_info($cfg);
                        Reset($ports);
			$i=0;
			while($i < count($ports));
                        	$key = key($ports);
                        	$proto = "TCP";
	                        if ($key == "snmp") {
        	                	$proto = "UDP";
                	        }
                                $port = $ports[$key];
                                if ($key == "icmp") {
                                	$proto = "ICMP";
                                	cfg_set($cfg, "iptables."+$idx+".cmd", "-t nat -A DMZ_MGMT -p "+$proto+" --icmp-type "+$port+" -j RETURN");
                                } else {
					cfg_set($cfg, "iptables."+$idx+".cmd", "-t nat -A DMZ_MGMT -p "+$proto+" --dport "+$port+" -j RETURN");
                                }
				cfg_set($cfg, "iptables."+$idx+".status", $dmz);
                		$idx = $idx + 1;
			        next($ports);
			        $i++;
			endwhile;
                        /* 10 posisions for MGMT ports */
                        while($i < 10);
                        	cfg_set($cfg, "iptables."+$idx+".status", "disabled");
                                cfg_set($cfg, "iptables."+$idx+".cmd", "");
                		$idx = $idx + 1;
			        $i++;
                        endwhile;

			cfg_set($cfg, "iptables."+$idx+".cmd", "-t nat -A DMZ_MGMT -j DNAT --to-destination "+$ip);
			cfg_set($cfg, "iptables."+$idx+".status", $dmz);


		} else {
			cfg_set($cfg, "iptables.200.status", "disabled");
			$cmd = $cmd + " -j DNAT --to-destination "+$ip;
		}
		cfg_set($cfg, "iptables.2.cmd", $cmd);
		cfg_set($cfg, "iptables.2.status", $dmz);
		cfg_set($cfg, "iptables.status", $dmz);
	}
	else {
		$dmz = "disabled";
		cfg_set($cfg, "iptables.2.status", $dmz);
		cfg_set($cfg, "iptables.200.status", $dmz);
	}
	return 1;
);

Function set_fallback $cfg, $iface, $dhcpc, $ip, $netmask (
	$result = $cfg;
	if ($dhcpc == "on" || $dhcpc == "enabled") {
		$i = 1;
		while ($i < 5) {
			$key = "dhcpc." + $i + ".";
			$ifc = cfg_get_def($cfg, $key + "devname", "");
			if ($iface == $ifc) {
				$i = 16;
			}
			$i = $i + 1;
		}
		if ($i == 16) {
			$result = cfg_set($cfg, $key + "fallback", $ip);
			$result = cfg_set($cfg, $key + "fallback_netmask", $netmask);
		}
		else {
			$result = cfg_set($cfg, "dhcpc.1.fallback", $ip);
			$result = cfg_set($cfg, "dhcpc.1.fallback_netmask", $netmask);
		}
	}
	return $result;
);

Function set_antenna $cfg, $idx, $div (
	return cfg_set($cfg, "radio.$idx.antenna", $div);
);

Function get_antenna_diversity_old $cfg, $idx, $div (
	$tx = cfg_get_def($cfg, "radio.$idx.tx_antenna", 0);
	$div = cfg_get_def($cfg, "radio.$idx.tx_antenna_diversity", $div);
	$ext = cfg_get_def($cfg, "radio.$idx.ext_antenna", 0);
	if ($ext != 0) {
		$div = 5;
	}
	else {
		if ($div == "enabled" || $tx == 0) {
			$div = 4;
		}
		else {
			$div = $tx;
		}
	}
	return $div;
);

Function get_antenna_diversity $cfg, $idx, $div (
	$div = cfg_get_def($cfg, "radio.$idx.antenna", -1);
	if ($div == -1) {
		$div = get_antenna_diversity_old($cfg, $idx, $div);
	}
	return $div;
);

Function get_manual_antenna_gain $cfg, $idx, $antenna_gain (
	if (!strlen($antenna_gain)) { $antenna_gain = 0; }
	return cfg_get_def($cfg, "radio.$idx.antenna.gain", $antenna_gain);
);

Function get_cable_loss $cfg, $idx, $cable_loss (
	if (!strlen($cable_loss)) { $cable_loss = 0; }
	return cfg_get_def($cfg, "radio.$idx.cable.loss", $cable_loss);
);

Function set_antenna_gain $cfg, $idx, $antenna_gain, $cable_loss (
	cfg_set($cfg, "radio.$idx.antenna.gain", $antenna_gain);
	cfg_set($cfg, "radio.$idx.cable.loss", $cable_loss);
);

Function has_builtin_antenna (
	global $radio;
	$i = 0; $has_builtin = 0;
	while ($i < $radio["ant_count"] && $has_builtin == 0) {
		$has_builtin = $radio["ant_builtin_$i"];
		$i++;
	}
	return $has_builtin;
);

Function get_antenna_index $ant_id (
	global $radio;
	$i = 0; $idx = 0;
	while ($i < $radio["ant_count"]) {
		if ($radio["ant_id_$i"] == $ant_id) {
			$idx = $i;
		}
		$i++;
	}
	return $idx;
);

Function generate_js_regdomain $cc, $name, $ieee_mode_a, $ieee_mode_bg
(
	global $cmd_regdomain;
	$modestring = "";
	$chanshift_flag = "";
	if ($ieee_mode_a) {
		$modestring += " -A -T ";
		$chanshift_flag = " -s 5";
	}
	if ($ieee_mode_bg) {
		$modestring += " -B -G -S ";
		$chanshift_flag = " -s 2";
	}
	$cc = intval($cc);
	echo ""+$name+" = [\n";
	PassThru($cmd_regdomain + $cc + $modestring + " -r -a -j -w 3" + $chanshift_flag);
	/* workaround for explorer array termination (,) problem: one extra element at the end. */
	echo "\"\"];\n";
);

Function set_wds_info $cfg, $idx, $auto, $peer1, $peer2, $peer3, $peer4, $peer5, $peer6 (
	if ($auto == "on" || $auto == "enabled") {
		$auto = "enabled";
	} else {
		$auto = "disabled";
	}
	cfg_set($cfg, "wireless.$idx.autowds", $auto);
	cfg_set($cfg, "wireless.$idx.wds.1.peer", $peer1);
	cfg_set($cfg, "wireless.$idx.wds.2.peer", $peer2);
	cfg_set($cfg, "wireless.$idx.wds.3.peer", $peer3);
	cfg_set($cfg, "wireless.$idx.wds.4.peer", $peer4);
	cfg_set($cfg, "wireless.$idx.wds.5.peer", $peer5);
	return cfg_set($cfg, "wireless.$idx.wds.6.peer", $peer6);
);

Function get_wds_info $cfg, $idx (
	$auto = cfg_get($cfg, "wireless.$idx.autowds");
	$peer1 = cfg_get($cfg, "wireless.$idx.wds.1.peer");
	$peer2 = cfg_get($cfg, "wireless.$idx.wds.2.peer");
	$peer3 = cfg_get($cfg, "wireless.$idx.wds.3.peer");
	$peer4 = cfg_get($cfg, "wireless.$idx.wds.4.peer");
	$peer5 = cfg_get($cfg, "wireless.$idx.wds.5.peer");
	$peer6 = cfg_get($cfg, "wireless.$idx.wds.6.peer");
	$info[0] = $auto;
	$info[1] = $peer1;
	$info[2] = $peer2;
	$info[3] = $peer3;
	$info[4] = $peer4;
	$info[5] = $peer5;
	$info[6] = $peer6;
	return $info;
);

Function module_loaded $module
(
	global $proc_modules;
	$lines = @file($proc_modules);
	if ($lines == "-1" || count($lines) == 0 || strlen($module) == 0) {
		$result = -1;
	}
	else {
		$result = 0;
		$c = count($lines);
		$l = strlen($module);
		while ($c) {
			$c--;
			if (substr($lines[$c], 0, $l) == $module) {
				$result = 1;
				$c = 0;
			}
		}
	}
	return $result;
);

Function is_test
(
	global $test_lock_file;
	$result = 1;
	if (fileinode($test_lock_file) == -1) {
		$result = 0;
	} else {
		$fp = @fopen($test_lock_file, "r");
		if ($fp != -1) {
			$result = @fgets($fp, 10);
			$result = intval($result);
			@fclose($fp);
		}
	}
	return $result;
);

Function is_testmode_active
(
	global $test_lock_file;
	$result = 0;
	if (substr(sprintf("%o",filePerms($test_lock_file)), 3, 3) == "755") {
		$result = 1;
	}
	return $result;
);

Function cfg_get_hwaddr_status $cfg, $iface
(
	$status = "disabled";
	$idx = get_iface_index($iface);
	if ($idx != 0) {
		$status = cfg_get_def($cfg, "netconf.$idx.hwaddr.status", $status);
	}
	return $status;
);

Function cfg_get_hwaddr $cfg, $iface
(
	$hwaddr = "";
	$idx = get_iface_index($iface);
	if ($idx != 0) {
		$hwaddr = cfg_get_def($cfg, "netconf.$idx.hwaddr", $hwaddr);
	}
	return $hwaddr;
);

Function cfg_set_hwaddr $cfg, $iface, $status, $hwaddr
(
	$idx = get_iface_index($iface);
	if ($idx != 0) {
		if (strlen($status) == 0) {
			$status = "disabled";
		}
		cfg_set($cfg, "netconf.$idx.hwaddr.status", $status);
		cfg_set($cfg, "netconf.$idx.hwaddr", $hwaddr);
	}
);

Function cfg_disable_hwaddrs $cfg, $excl_idx
(
	$if_count = 4;
	$idx = 1;
	while ($idx <= $if_count) {
		if ($idx != $excl_idx) {
			cfg_set($cfg, "netconf.$idx.hwaddr.status", "disabled");
		}
		$idx++;
	}
);

Function get_client_hwaddr
(
	$cmd = "grep $REMOTE_ADDR /proc/net/arp | sed 's/[[:space:],]\+/ /g' | cut -d ' ' -f4";

	$hwaddr = "";
	exec($cmd, $lines, $res);
	if (count($lines) == 1 && strlen($lines[0]) == 17) {
		$hwaddr = $lines[0];
	}

	return $hwaddr;
);

Function cfg_get_vlan_status $cfg, $iface, $vlan_status (
	if ($vlan_status == "on" || $vlan_status == "enabled")
	{
		$vlan_status = "enabled";
	}
	else
	{
		$vlan_status = "disabled";
	}
	$enabled = cfg_get_def($cfg, "vlan.status", $vlan_status);
	$vlan_status = cfg_get_def($cfg, "vlan.1.status", $vlan_status);

	if ($enabled != "enabled" || $vlan_status != "enabled")
	{
		$vlan_status = "disabled";
	}
	return $vlan_status;
);

Function cfg_get_vlan_tag $cfg, $iface, $vlan_tag (
	return cfg_get_def($cfg, "vlan.1.id", $vlan_tag);
);

Function cfg_get_vlan_ifc $cfg, $iface, $vlan_ifc (
	global $wlan_iface;

	$netmode = cfg_get_def($cfg, "netmode", "bridge");
	$ifc = cfg_get_def($cfg, "vlan.1.devname", $iface);
	if ($netmode == "bridge") {
		if ($ifc == $wlan_iface) {
			return "wlan";
		} else {
			return "lan";
		}
	} else {
		if ($ifc == $iface) {
			return "wan";
		} else {
			return "lan";
		}
	}
);

Function set_vlan $cfg, $iface, $vlan_status, $vlan_tag, $vlan_ifc (
	global $eth0_iface;
	global $wlan_iface;
	global $ipaliases_max;

	$netmode = cfg_get_def($cfg, "netmode", "bridge");
	$vlan_status2 = "disabled";
	$vlan_netconf_status = "disabled";
	if ($vlan_status == "on" || $vlan_status == "enabled") {
		$vlan_status = "enabled";
		if ($netmode == "bridge") {
			if ($vlan_ifc == "lan") {
				$vlan_status2 = "enabled";
				cfg_set($cfg, "vlan.1.devname", $eth0_iface);
				cfg_set($cfg, "vlan.1.id", $vlan_tag);
				cfg_set($cfg, "vlan.2.devname", "eth1");
				cfg_set($cfg, "vlan.2.id", $vlan_tag);
				cfg_set($cfg, "bridge.1.port.1.status", "disabled");
				cfg_set($cfg, "bridge.1.port.3.status", "disabled");
				cfg_set($cfg, "bridge.1.port.4.devname", $eth0_iface + "." + $vlan_tag);
				cfg_set($cfg, "bridge.1.port.4.prio", "10");
				cfg_set($cfg, "bridge.1.port.4.status", "enabled");
				cfg_set($cfg, "bridge.1.port.5.devname", "eth1." + $vlan_tag);
				cfg_set($cfg, "bridge.1.port.5.prio", "20");
				cfg_set($cfg, "bridge.1.port.5.status", "enabled");
			} else {
				cfg_set($cfg, "vlan.1.devname", $wlan_iface);
				cfg_set($cfg, "vlan.1.id", $vlan_tag);
				cfg_set($cfg, "bridge.1.port.2.status", "disabled");
				cfg_set($cfg, "bridge.1.port.4.devname", $wlan_iface + "." + $vlan_tag);
				cfg_set($cfg, "bridge.1.port.4.prio", "20");
				cfg_set($cfg, "bridge.1.port.4.status", "enabled");
				cfg_set($cfg, "bridge.1.port.5.status", "disabled");
			}
		} else {
			cfg_set($cfg, "vlan.1.devname", $iface);
			cfg_set($cfg, "vlan.1.id", $vlan_tag);
			$vlan_netconf_status = "enabled";
                        $old_vlan = cfg_get_vlan_status($cfg,  $iface, $vlan_status);
			if (($old_vlan != "enabled") && strlen($iface) && strlen($vlan_tag)) {
	       			copy_ipaliases($cfg, $iface, $iface + "." + $vlan_tag, $ipaliases_max);
                        }
			disable_ipaliases($cfg, $iface, $ipaliases_max);
			set_ipv4($cfg, $iface, "", "");
			cfg_set($cfg, "bridge.1.port.4.status", "disabled");
			cfg_set($cfg, "bridge.1.port.5.status", "disabled");
		}
	}
	else
	{
		$vlan_status = "disabled";
		if ($netmode != "bridge") {
			$iface = cfg_get_def($cfg, "vlan.1.devname", $iface);
			$vlan_tag = cfg_get_def($cfg, "vlan.1.id", $vlan_tag);
                        $old_vlan = cfg_get_vlan_status($cfg,  $iface, $vlan_status);
			if (($old_vlan != "disabled") && strlen($iface) && strlen($vlan_tag)) {
				copy_ipaliases($cfg, $iface + "." + $vlan_tag, $iface, $ipaliases_max);
			}
		}
	}
	cfg_set($cfg, "vlan.status", $vlan_status);
	cfg_set($cfg, "vlan.1.status", $vlan_status);
	cfg_set($cfg, "vlan.2.status", $vlan_status2);
	cfg_set($cfg, "netconf.6.status", $vlan_netconf_status);
);

Function init_board_inc $iface
(
	global $radio;
	$keys[] = "name";
	$keys[] = "shortname";
	$keys[] = "bus";
	$keys[] = "ccode";
	$keys[] = "ccode_locked";
	$keys[] = "ccode_fixed";
	$keys[] = "subsystemid";
	$keys[] = "subvendorid";
	$keys[] = "txpower_max";
	$keys[] = "txpower_offset";
	$keys[] = "low_txpower_atten";
	$keys[] = "low_txpower_limit";
	$keys[] = "ieee_modes";
	$keys[] = "ieee_mode";
	$keys[] = "ieee_mode_a";
	$keys[] = "ieee_mode_bg";
	$keys[] = "ant_count";
	$keys[] = "def_antenna";
	$keys[] = "caps";
	$keys[] = "distance_limit_km";
	$keys[] = "chains";
	$keys[] = "ieee_mode_bg";
	$keys[] = "ieee_mode";
	$keys[] = "low_txpower_mode";
	$keys[] = "legacy";
	$keys[] = "antennas";

	$idx_keys[] = "ant_id";
	$idx_keys[] = "ant_name";
	$idx_keys[] = "ant_gain";
	$idx_keys[] = "ant_builtin";
	$idx_keys[] = "ant_chains";

	$idx = get_wlan_index($iface);
	$radio["index"] = $idx;

	$i = 0;
	while ($i < count($keys)) {
		$cfg_var = "radio$idx" + "_$keys[$i]";
		global $$cfg_var;
		$radio[$keys[$i]] = $$cfg_var;
		$i++;
	}

	$i = 0;
	while ($i < count($idx_keys)) {
		$cfg_var = "radio$idx" + "_$idx_keys[$i]";
		global $$cfg_var; $arr = $$cfg_var;
		$j = 0;
		while ($j < count($$cfg_var)) {
			$key_name = $idx_keys[$i]+ "_$j";
			$radio[$key_name] = $arr[$j];
			$j++;
		}
		$i++;
	}
);

Function get_wpasupplicant_status $cfg
(
	$max_radio = 2;
	$i = 1; $status = "disabled";
	while ($i <= $max_radio && $status == "disabled") {
		$device_status = cfg_get_def($cfg, "wpasupplicant.device.$i.status", "disabled");
		if ($device_status == "enabled") {
			$status = "enabled";
		}
		$i++;
	}
	return $status;
);

Function get_aaa_status $cfg
(
	$max_radio = 2;
	$i = 1; $status = "disabled";
	while ($i <= $max_radio && $status == "disabled") {
		$aaa_status = cfg_get_def($cfg, "aaa.$i.status", "disabled");
		if ($aaa_status == "enabled") {
			$status = "enabled";
		}
		$i++;
	}
	return $status;
);

Function get_status $status_value
(
	$status = "disabled";
	if (strlen($status_value)) {
		$status = "enabled";
	}
	return $status;
);

Function cfg_get_obey_default $cfg, $wlan_iface
(
	global $radio;
	if (($radio["ccode_fixed"] == 1) || (has_builtin_antenna() != 1)) {
		$obey_default = "disabled";
	} else {
		$obey_default = "enabled";
	}
	return $obey_default;
);

Function cfg_get_dfs $cfg, $wlan_iface
(
	$dummy = "dummy";
	$idx = get_wlan_index($wlan_iface);
	$dfs = cfg_get_def($cfg, "radio.$idx.dfs.status", $dummy);
	if ($dfs == $dummy || strlen($dfs) == 0) {
		$def_obey = cfg_get_obey_default($cfg, $wlan_iface);
		$dfs = cfg_get_obey($cfg, $wlan_iface, $def_obey);
	}
	return $dfs;
);

>

#!/sbin/cgi
<?
  SecureVar("cmd*");
  SecureVar("lines");
  include("lib/settings.inc");
  $cfg = @cfg_load($cfg_file);
  include("lib/l10n.inc");
  include("lib/link.inc");
  include("lib/misc.inc");

  if ($cfg == -1) {
	  include("lib/busy.tmpl");
	  exit;
  }

  if ($radio_count < 1) {
	include("lib/linknoradio.tmpl");
	exit;
  }

  $netmode = cfg_get_def($cfg, "netmode", "bridge");

  $curr_iface = $wlan_iface;
  $curr_ifidx = get_wlan_index($curr_iface); 
  init_board_inc($curr_iface);
  
  $polling = cfg_get_def($cfg, "radio.$curr_ifidx.polling", "disabled");
  $polling_fh = cfg_get_def($cfg, "radio.$curr_ifidx.polling_fh", "0");

  if (strlen($wep_key_type) == 0) {
	$wep_key_type = 1;
  }

  $oldwmode = cfg_get_wmode($cfg, $curr_iface);
  if (strlen($oldwmode) == 0) {
	  $oldwmode = "sta";
  }

  if (strlen($wmode) == 0) {
	  $wmode = $oldwmode;
  }

	if ($radio["antennas"] == 1 || has_builtin_antenna() == 1) {
		if (strlen($antenna) == 0) {
			$antenna = get_antenna_diversity($cfg, $curr_ifidx, "enabled");
		}
		$antenna_idx = get_antenna_index($antenna);
		if ($radio["ant_builtin_$antenna_idx"] == 1) {
			$antenna_gain = $radio["ant_gain_$antenna_idx"];
			$cable_loss = 0;
		}
	}
	else {
		$antenna_idx = 0;
	}

	if ($radio["ant_chains_$antenna_idx"] > 0) {
		$radio_chains = $radio["ant_chains_$antenna_idx"];
	}
	else {
		$radio_chains = $radio["chains"];
	}

  if (strlen($country) == 0)
  {
  	if (strlen($old_country) == 0) {
		$country = cfg_get_country($cfg, $curr_iface, $country);
		$old_country = $country;
        } else {
		$country = $old_country;
	}
  }
  $eirp_status = cfg_get_def($cfg, "system.eirp.status", "");
  $eirp_first_run = 0;
  if (strlen($eirp_status) == 0) {
  	$eirp_first_run = 1;
	if (has_builtin_antenna() != 1) {
       		$eirp_status = "enabled";
  	} else {
       		$eirp_status = "disabled";
        }
  }

	if ($REQUEST_METHOD == "POST")
	{
		if ($wmode == "ap" || $wmode == "apwds")
		{
                	if (!isset($mac_acl_policy)) {
                        	$mac_acl_policy = cfg_get_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
                        }
			if (isset($mac_acl_mac_0))
			{
				$i = 0;
				while ($i < $mac_acl_max)
				{
					$name = "mac_acl_mac_" + $i;
					if (strlen($$name))
					{
						$mac_acl_list[] = $$name;
					}
					$i++;
				}
			}
			else
			{
				$mac_acl_list = cfg_get_mac_acl_list($cfg, $curr_ifidx, $mac_acl_max);
			}
		}

		if ($radio["legacy"] != 1)
		{
			if (strlen($ieee_mode) == 6)
			{
				if ($clksel == "E")
				{
					$ieee_mode += "40";
					if (strstr($wmode, "ap") > 0)
					{
						$ieee_mode += $extchannel;
					}
				}
				else
				{
					$ieee_mode += "20";
				}
			}
		}

		if ($cc != "changed")
		{
			$netwarning = 0;
			if ($netmode == "router" &&
				  (get_wmode_type($oldwmode) != get_wmode_type($wmode)))
			{
				$netwarning = 1;
			}

			/* common variables */
			set_wmode($cfg, $curr_iface, $wmode, $chan_freq, $macclone);

			set_essid($cfg, $curr_ifidx, $essid);
			set_country($cfg, $curr_ifidx, $country, $radio["subsystemid"]);

			if (strlen($obey_regulatory_status)) {
				$obey_regulatory_status = "enabled";
			} else {
				$obey_regulatory_status = "disabled";
			}
			set_obey($cfg, $curr_ifidx, $obey_regulatory_status);
		        cfg_set($cfg, "system.eirp.status", $eirp_status);
                        $eirp_first_run = 0;
			$rtxpower = intval($txpower);
			$chain_dbm   = 0;
			$min_txpower = 0;
			if($radio_chains == 2) {
				$chain_dbm = 3;
			} elseif ($radio_chains == 3) {
				$chain_dbm = 5;
			}
			if ($radio["low_txpower_mode"]) {
				$min_txpower = ($radio["txpower_max"] - $radio["low_txpower_atten"] - $radio["low_txpower_limit"]);
				if ( $min_txpower < ($radio["txpower_offset"] + $chain_dbm - $radio["low_txpower_atten"])) {
					$min_txpower = ($radio["txpower_offset"] + $chain_dbm - $radio["low_txpower_atten"]);
				} 
			} else {
				$min_txpower = ($radio["txpower_offset"] + $chain_dbm);
			}
			if($rtxpower < $min_txpower) {
				$rtxpower = $min_txpower;
			}
			set_txpower($cfg, $curr_ifidx, $rtxpower);

			if ($radio["legacy"] != 1)
			{
				if (strstr($ieee_mode, "11ng") > 0)
				{
					$forbiasauto = 1;
				}
				else
				{
					$forbiasauto = 0;
				}
				if (strstr($ieee_mode, "ht40") > 0)
				{
					if (strstr($wmode, "sta") > 0)
					{
						$cwm_mode = 1;
					}
					else
					{
						$cwm_mode = 2;
					}
				}
				else
				{
					$cwm_mode = 0;
				}
				$cwm_enable = 0;
				cfg_set($cfg, "radio.$curr_ifidx.cwm.mode", $cwm_mode);
				cfg_set($cfg, "radio.$curr_ifidx.forbiasauto", $forbiasauto)
				cfg_set($cfg, "radio.$curr_ifidx.cwm.enable", $cwm_enable);
			}
			set_ieee_mode($cfg, $curr_ifidx, $ieee_mode);
			if ($radio["ieee_mode_a"]) {
				cfg_set($cfg, "radio.$curr_ifidx.dfs.status", get_status($dfs));
			}
			set_ack_distance($cfg, $curr_iface, $ackdistance, $ieee_mode);
			set_rate($cfg, $curr_ifidx, $rate, $rate_auto);
			cfg_set($cfg, "radio.$curr_ifidx.mcastrate", $mcast_rate);
			set_clksel($cfg, $curr_ifidx, $clksel);
			set_chanshift($cfg, $curr_ifidx, $chanshift);
			set_authtype($cfg, $curr_ifidx, $authtype);

			set_antenna($cfg, $curr_ifidx, $antenna);
			if ($radio["ant_builtin_$antenna_idx"] != 1 && strlen($antenna_gain) && strlen($cable_loss)) {
				set_antenna_gain($cfg, $curr_ifidx, $antenna_gain, $cable_loss);
			}

			set_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
			if (IsSet($scan_channels))
			{
				set_scan_channels($cfg, $curr_ifidx, $scan_channels);
			}

			/* mode specific */
			if ($wmode == "sta" || $wmode == "stawds")
			{
				set_apmac($cfg, $curr_ifidx, $apmac);
				set_hide_ssid($cfg, $curr_ifidx, "");
			}
			else
			{
				$apmac = "";
				set_apmac($cfg, $curr_ifidx, $apmac);
				set_hide_ssid($cfg, $curr_ifidx, $hidessid);
				if ($wmode == "apwds")
				{
					set_wds_info($cfg, $curr_ifidx, $wds_auto, $peer1, $peer2, $peer3, $peer4, $peer5, $peer6);
				}
				set_mac_acl($cfg, $curr_ifidx, $mac_acl_status);
				set_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
				set_mac_acl_list($cfg, $curr_ifidx, $mac_acl_list, $mac_acl_max);
			}

			set_security($cfg, $curr_iface, $curr_ifidx, $security, $wep_key_length, $wmode);
			if ($security == "wep")
			{
				set_def_wep_key_id($cfg, $curr_ifidx, $wep_key_id);
				set_wep_key($cfg, $curr_ifidx, $wep_key_id, $wep_key, $wep_key_type);
			}
			elseif (substr($security, 0, 3) == "wpa")
			{
				if ($wmode == "ap" || $wmode == "apwds")
				{
					set_wpa_ap($cfg, $curr_ifidx, $wpa_auth, $wpa_key,
						$radius_auth_ip, $radius_auth_port, $radius_auth_secret);
				}
				else
				{
					set_wpa_sta($cfg, $curr_ifidx, $wpa_auth, $wpa_key, $wpa_eap,
						$wpa_inner, $wpa_ident, $wpa_user, $wpa_passwd, $apmac);
				}
			}

			$wlanmode = $wmode;
			if ($netmode == "bridge")
			{
				include("lib/getbridge.inc");
				include("lib/setbridge.inc");
			}
			elseif ($netmode == "router")
			{
				include("lib/getrouter.inc");
				include("lib/setrouter.inc");
			}
			elseif ($netmode == "soho")
			{
				include("lib/getsoho.inc");
				include("lib/setsoho.inc");
			}
			elseif ($netmode == "3g")
			{
				include("lib/get3g.inc");
				include("lib/set3g.inc");
			}

			cfg_save($cfg, $cfg_file);
			cfg_set_modified($cfg_file);
			$message = dict_translate("Configuration saved");
		}
		else
		{
			$txpower = cfg_get_txpower($cfg, $curr_ifidx, $txpower);
			if (strlen($obey_regulatory_status)) {
				$obey_regulatory_status = "enabled";
			} else {
				$obey_regulatory_status = "disabled";
			}
			if ($old_country != $country)
			{
				if ($country == 511) {
					$obey_regulatory_status = "disabled";
				}
			}

			if ($radio["ant_builtin_$antenna_idx"] != 1)
			{
				$antenna_gain = get_manual_antenna_gain($cfg, $curr_ifidx, $antenna_gain);
				$cable_loss = get_cable_loss($cfg, $curr_ifidx, $cable_loss);
			}

			if ($radio["ieee_mode_a"]) {
				$dfs = cfg_get_dfs($cfg, $curr_iface);
			}

			$channel_scan_list = cfg_get_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
			if (!IsSet($scan_channels))
			{
				$scan_channels = cfg_get_scan_channels($cfg, $curr_ifidx, $scan_channels);
			}
                        
			$essid = htmlspecialchars($essid);
			$wpa_key = htmlspecialchars($wpa_key);
			$wpa_ident = htmlspecialchars($wpa_ident);
			$wpa_user = htmlspecialchars($wpa_user);
			$wpa_passwd = htmlspecialchars($wpa_passwd);
			$radius_auth_secret = htmlspecialchars($radius_auth_secret);

			if ($wmode == "ap" || $wmode == "apwds")
			{
				if ($feature_ap == 1) {
					$include_page="ap";
					include("lib/linkap.tmpl");
					exit;
				}
			}
			if ($wmode == "sta" || $wmode == "stawds" || $feature_ap == 0)
			{
				$include_page="sta";
				include("lib/linksta.tmpl");
				exit;
			}

		}
	}

  /* retrieve common variables */
  $essid = cfg_get_essid($cfg, $curr_ifidx, $essid);
  $hidessid = cfg_get_hide_ssid($cfg, $curr_ifidx, $hidessid);
  $ieee_mode = cfg_get_ieee_mode($cfg, $curr_iface, $ieee_mode);
  if ($radio["ieee_mode_a"]) {
	  $dfs = cfg_get_dfs($cfg, $curr_iface);
  }

	if ($radio["ant_builtin_$antenna_idx"] != 1)
 	{
		$antenna_gain = get_manual_antenna_gain($cfg, $curr_ifidx, $antenna_gain);
		$cable_loss = get_cable_loss($cfg, $curr_ifidx, $cable_loss);
	}
  $country = cfg_get_country($cfg, $curr_iface, $country);
  $old_country = $country;
  $txpower = cfg_get_txpower($cfg, $curr_ifidx, $txpower);
  $rtxpower = intval($txpower);
  if ($rtxpower <= $radio["txpower_max"]) {
	  $txpower = $rtxpower;
  } else {
	  $txpower = $radio["txpower_max"];
  }
  $obey_default = cfg_get_obey_default($cfg, $curr_iface);
  $obey_regulatory_status = cfg_get_obey($cfg, $curr_iface, $obey_default);
  if (($eirp_first_run != 0) && (has_builtin_antenna() != 1)) {
  	$obey_regulatory_status = "disabled";
  }
  $rate_auto = cfg_get_def($cfg, "radio.$curr_ifidx.rate.auto", "enabled");
  $rate = cfg_get_rate($cfg, $curr_iface, $rate);
  $mcast_rate = cfg_get_def($cfg, "radio.$curr_ifidx.mcastrate", $mcast_rate);
  $clksel = cfg_get_clksel($cfg, $curr_iface, $clksel);
  $timings = get_timings($ieee_mode, $clksel, $radio["caps"] & $radio_cap_fast_clock);
  $sltconst = $timings[0];
  $ackdistance = cfg_get_ackdistance($cfg, $curr_iface, $sltconst);

  $chanshift = cfg_get_chanshift($cfg, $curr_iface, $chanshift);
  $authtype = cfg_get_authtype($cfg, $curr_ifidx, $authtype);
  $wep_key_id = cfg_get_def_wep_id($cfg, $curr_ifidx, $wep_key_id);
  $wep_key = cfg_get_wep_key($cfg, $curr_ifidx, $wep_key_id, $wep_key);
  $wep_key_length = cfg_get_wep_key_length($cfg, $curr_ifidx, $wep_key_length);
  $wep_key_type = 1;
  if (strlen($wep_key) > 2 && "s:" == substr($wep_key, 0, 2)) {
        $wep_key_type = 2;
        $wep_key = substr($wep_key, 2, strlen($wep_key) - 2);
  }

  $essid = htmlspecialchars($essid);
  $wpa_key = htmlspecialchars(cfg_get_wpa_key($cfg, $curr_ifidx, $wmode, $wpa_key));

  $wpa_auth = cfg_get_wpa_auth($cfg, $curr_ifidx, $wmode, $wpa_auth);
  $wpa_eap = cfg_get_wpa_eap($cfg, $curr_ifidx, $wmode, $wpa_eap);
  $wpa_inner = cfg_get_wpa_inner($cfg, $curr_ifidx, $wpa_inner);
  $wpa_ident = htmlspecialchars(cfg_get_wpa_ident($cfg, $curr_ifidx, $wpa_ident));
  $wpa_user = htmlspecialchars(cfg_get_wpa_user($cfg, $curr_ifidx, $wpa_user));
  $wpa_passwd = htmlspecialchars(cfg_get_wpa_passwd($cfg, $curr_ifidx, $wpa_passwd));

  $radius_auth_ip = cfg_get_radius_auth_ip($cfg, $curr_ifidx, $radius_auth_ip);
  $radius_auth_port = cfg_get_radius_auth_port($cfg, $curr_ifidx, "1812");
  $radius_auth_secret = htmlspecialchars(cfg_get_radius_auth_secret($cfg, $curr_ifidx, "secret"));

  $security = cfg_get_security($cfg, $curr_iface, $security, $wmode);

  $channel_scan_list = cfg_get_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
  $scan_channels = cfg_get_scan_channels($cfg, $curr_ifidx, $scan_channels);

  if ($wmode == "sta" || $wmode == "stawds" || $feature_ap == 0) {
	  $apmac = cfg_get_apmac($cfg, $curr_ifidx, $apmac);
	  $macclone = cfg_get_def($cfg, "wireless.$curr_ifidx.macclone", $macclone);
	  $include_page="sta";
	  include("lib/linksta.tmpl");
  } else {
		if ($wmode == "apwds") {
			$info = get_wds_info($cfg, $curr_ifidx);
	        $wds_auto = $info[0];
	        $peer1 = $info[1];
	        $peer2 = $info[2];
	        $peer3 = $info[3];
	        $peer4 = $info[4];
	        $peer5 = $info[5];
	        $peer6 = $info[6];
		}
		$chan_freq = cfg_get_def($cfg, "radio.$curr_ifidx.freq", $chan_freq);
		$mac_acl_status = cfg_get_mac_acl($cfg, $curr_ifidx, $mac_acl_status);
		$mac_acl_policy = cfg_get_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
		$mac_acl_list = cfg_get_mac_acl_list($cfg, $curr_ifidx, $mac_acl_max);
		$include_page="ap";
		include("lib/linkap.tmpl");
  }
>

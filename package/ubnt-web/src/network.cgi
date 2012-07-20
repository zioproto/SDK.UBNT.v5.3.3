#!/sbin/cgi
<?
  SecureVar("cmd*");
  SecureVar("lines");
  include("lib/settings.inc");
  $cfg = @cfg_load($cfg_file);
  include("lib/l10n.inc");
  include("lib/link.inc");
  
  $threeg_inc = "/etc/3g.inc";
  if (fileSize($threeg_inc) > 0) {
	include($threeg_inc);
  }

  include("lib/network_head.tmpl");

  if ($cfg == -1) {
	  include("lib/busy.tmpl");
	  exit;
  }

  $netmode_cfg = cfg_get_def($cfg, "netmode", "bridge");
  if (strlen($netmode)==0) {
	$netmode = $netmode_cfg;
  }
  $wlanmode = cfg_get_wmode($cfg, $wlan_iface);

  $client_hwaddr = get_client_hwaddr();

  if ($REQUEST_METHOD == "POST") {
       	  cfg_set($cfg, "netconf.1.up", "enabled");
       	  cfg_set($cfg, "netconf.2.up", "enabled");
       	  cfg_set($cfg, "netconf.4.up", "enabled");
       	  cfg_set($cfg, "bridge.1.port.1.status", "enabled");
       	  cfg_set($cfg, "bridge.1.port.2.status", "enabled");
       	  cfg_set($cfg, "bridge.1.port.3.status", "enabled");
  	  if ((strlen($ifcdisable) != 0) && ($ifcdisable != "0")) {
                  if (($netmode == "soho" || $netmode == "3g") && $ifcdisable == "1") {
	          	cfg_set($cfg, "netconf.2.up", "disabled");
                  } else {
                  	cfg_set($cfg, "netconf.2.up", "enabled");
                  }
          	  cfg_set($cfg, "netconf." + $ifcdisable + ".up", "disabled");
                  if ($ifcdisable == 1) {
                  	/* work around for bad default config */
                  	if (cfg_get_def($cfg, "bridge.1.port.1.devname", $eth0_iface) == $wlan_iface) {
	       	  		cfg_set($cfg, "bridge.1.port.1.devname", $eth0_iface);
                                cfg_set($cfg, "bridge.1.port.2.devname", $wlan_iface);
                        }
                        cfg_set($cfg, "bridge.1.port.3.status", "disabled");
                  }
		  cfg_set($cfg, "bridge.1.port." + $ifcdisable + ".status", "disabled");
          }
	  if ($netmode == "router") {
               	include("lib/setrouter.inc");
	  } elseif ($netmode == "soho") {
          	include("lib/setsoho.inc");
	  } elseif ($netmode == "3g") {
		if ($threeg_simstatus == 2 && cfg_get_gsm_pin_status($cfg) != "enabled") {
			$error_msg = validate_pincode($threeg_device, $gsmpin);
			if (strlen($error_msg) == 0) {
				set_gsm_pin_code($cfg, $threeg_iccid, $gsmpin);	
			}
		}
          	include("lib/set3g.inc");
          } else {
       	  	include("lib/setbridge.inc");
          }
	  if (strlen($error_msg) == 0) {
	      cfg_save($cfg, $cfg_file);
	      cfg_set_modified($cfg_file);
	      $message = dict_translate("Configuration saved");
	      $netmode_cfg = $netmode;
	  }
  } 

  $ifcdisable = 0;
  if (cfg_get_def($cfg, "netconf.1.up", "enabled") == "disabled") {
  	  $ifcdisable = 1;
  } elseif (cfg_get_def($cfg, "netconf.2.up", "enabled") == "disabled") {
  	  $ifcdisable = 2;
  } elseif (cfg_get_def($cfg, "netconf.4.up", "enabled") == "disabled") {
  	  $ifcdisable = 4;
  }

  $firewall_status = cfg_get_firewall($cfg, $firewall_status, $netmode);
  if ($netmode != "bridge") {
          if ($netmode == "soho") {
		include("lib/getsoho.inc");
          } elseif ($netmode == "3g") {
		include("lib/get3g.inc");
          } else {
  	  	include("lib/getrouter.inc");
          }
	  if ($is_ap) {
		  include("lib/netrouterap.tmpl");
	  } else {
		  include("lib/netroutersta.tmpl");
	  }
  } else {
	include("lib/getbridge.inc");
	include("lib/netbridge.tmpl");
  }
>

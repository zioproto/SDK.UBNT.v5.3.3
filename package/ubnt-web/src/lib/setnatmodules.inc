<?

if (IsSet($nat_update_cfg) && $nat_update_cfg=="1") {

$nat_bk_status="disabled";
if ($nat_sip_check!="enabled" && $nat_sip_check!="on") {
	cfg_set($cfg, "system.modules.blacklist.1.expr", "ip_.*_sip");
        $nat_sip_status="enabled";
        $nat_bk_status="enabled";
} else {
	$nat_sip_status="disabled";
}
cfg_set($cfg, "system.modules.blacklist.1.status", $nat_sip_status);
if ($nat_pptp_check!="enabled" && $nat_pptp_check!="on") {
	cfg_set($cfg, "system.modules.blacklist.2.expr", "ip_.*_pptp");
        $nat_pptp_status="enabled";
        $nat_bk_status="enabled";
} else {
	$nat_pptp_status="disabled";
}
cfg_set($cfg, "system.modules.blacklist.2.status", $nat_pptp_status);
if ($nat_ftp_check!="enabled" && $nat_ftp_check!="on") {
	cfg_set($cfg, "system.modules.blacklist.3.expr", "ip_.*_ftp");
        $nat_ftp_status="enabled";
        $nat_bk_status="enabled";
} else {
	$nat_ftp_status="disabled";
}
cfg_set($cfg, "system.modules.blacklist.3.status", $nat_ftp_status);
if ($nat_rtsp_check!="enabled" && $nat_rtsp_check!="on") {
	cfg_set($cfg, "system.modules.blacklist.4.expr", "ip_.*_rtsp");
        $nat_rtsp_status="enabled";
        $nat_bk_status="enabled";
} else {
	$nat_rtsp_status="disabled";
}
cfg_set($cfg, "system.modules.blacklist.4.status", $nat_rtsp_status);

cfg_set($cfg, "system.modules.blacklist.status", $nat_bk_status);

} # if (IsSet($nat_save_config)...

>

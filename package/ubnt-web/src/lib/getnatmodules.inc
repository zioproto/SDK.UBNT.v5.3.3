<?
$nat_bk_status=cfg_get_def($cfg, "system.modules.blacklist.status", "disabled");
$nat_sip_status=cfg_get_def($cfg, "system.modules.blacklist.1.status", $nat_bk_status);
$nat_pptp_status=cfg_get_def($cfg, "system.modules.blacklist.2.status", $nat_bk_status);
$nat_ftp_status=cfg_get_def($cfg, "system.modules.blacklist.3.status", $nat_bk_status);
$nat_rtsp_status=cfg_get_def($cfg, "system.modules.blacklist.4.status", $nat_bk_status);
>
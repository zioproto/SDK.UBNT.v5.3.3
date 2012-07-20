#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
  if ($REQUEST_METHOD == "GET") {
      include("lib/system.inc");
      $fwversion = fw_get_version();
      @unlink($firmware_file);
      cleanup_dir($upload_dir);
      include("lib/fwupload.tmpl");
  } elseif ($REQUEST_METHOD == "POST") {
     include("lib/system.inc");
     $fwversion = fw_get_version();
      
      if (!fw_validate($fwfile)) {
      	 $error_msg = dict_translate("msg_bad_fwimage|Bad firmware update image.");
         @unlink($fwfile);
         @unlink($firmware_file);
         include("lib/fwupload.tmpl");
      } else {
         $newfwversion = fw_extract_version($firmware_file);
	 if (fw_is_thirdparty($firmware_file)) {
		 $error_msg = dict_translate("warn_third_party_firmware|WARNING: Uploaded firmware is third-party, make sure you're familiar with recovery procedure!");
	 }
         include("lib/fwflash.tmpl");
      }
  }
>

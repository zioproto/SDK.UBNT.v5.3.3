#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/system.inc");
include("lib/link.inc");

$fwversion = fw_get_version();
$fwbuild= fw_get_build();
if ($cfg == -1)
{
	include("lib/busy.tmpl");
	exit;
}
$message = "";
$modified = 0;
if ($REQUEST_METHOD=="POST")
{
	if (strlen($cfgupload) > 0)
	{
		if (strlen($cfgfile) > 0)
		{
			$error_msg = check_uploaded_file($cfgfile, $cfgfile_size,
				dict_translate("configuration"), 65535);
			if (strlen($error_msg) == 0)
			{
				$tcfg = @cfg_load($cfgfile);
				if ($tcfg == -1)
				{
					$error_msg = dict_translate("msg_invalid_conf_file|Invalid configuration file");
				}
				else
				{
					if (cfg_get($tcfg, "radio.status") != "enabled" || cfg_get($tcfg, "radio.1.status") != "enabled")
					{
						$error_msg = dict_translate("msg_invalid_conf_file_struct|Invalid configuration file structure");
					}
				}
			}

			if (strlen($error_msg) > 0)
			{
				@unlink($cfgfile);
			} else {
				cfg_save($tcfg, $cfg_file);
				cfg_set_modified($cfg_file);
			}
		}
	}
	elseif ($feature_logo == 1 && strlen($chlogo) > 0)
	{
		if ($logoStatus == "on")
		{
			$logoStatus = "enabled";
		}
		else
		{
			$logoStatus = "disabled";
		}

		if (strlen($logo_file) > 0)
		{
			$error_msg = check_uploaded_file($logo_file,
				$logo_file_size, dict_translate("logo"), 51200);
			if (strlen($error_msg) > 0)
			{
				@unlink($logo_file);
				include("lib/system.tmpl");
				exit;
			}

			@mkdir("/etc/persistent/www/", 0755);
			@rename($logo_file, "/etc/persistent/www/logo.gif");
		}
		elseif ($logoStatus == "enabled")
		{
			if (fileinode("/etc/persistent/www/logo.gif") == -1)
			{
				@mkdir("/etc/persistent/www/", 0755);
				exec("cp /usr/www/images/ulogo.gif /etc/persistent/www/logo.gif");
			}
		}
		if ($logoStatus == "enabled") {
			cfg_set($cfg, "ls_logo.url", $logoURL);
		}
		cfg_set($cfg, "ls_logo.status", $logoStatus);
		cfg_save($cfg, $cfg_file);
		cfg_set_modified($cfg_file);
		$logo_status = $logoStatus;
		$logo_url = $logoURL;
	}
	elseif(strlen($change))
	{
		if ((strlen($OldPassword) != 0) || (strlen($NewPassword) != 0) ||
			(strlen($NewPassword2) != 0))
		{
			if ($NewPassword != $NewPassword2)
			{
				$error_msg = dict_translate("msg_passwords_dont_match|New passwords do not match!");
			}
			elseif ((strlen($NewPassword) == 0) || (strlen($NewPassword2) == 0)) {
				$error_msg = dict_translate("msg_password_empty|New password cannot be empty!");
			}
			else
			{
				$passwd = cfg_get($cfg, "users.1.password");
				if ($passwd == "")
				{
					$passwd = "oHSl3yqR.t1uQ";
				}

				$crypted = crypt($OldPassword, $passwd);
				if ($passwd != $crypted)
				{
					$error_msg = dict_translate("msg_curr_passwd_wrong|Current password is wrong.");
				}
				else
				{
					$crypted = crypt($NewPassword);
					cfg_set($cfg, "users.1.password", $crypted);
					cfg_set($cfg, "users.1.status", "enabled");
				}
			}
		}
		if (strlen($error_msg) == 0)
		{
			$old_username = cfg_get_def($cfg, "users.1.name", "ubnt");
			cfg_set($cfg, "gui.language", $active_language);
			if (strlen($username) == 0)
			{
				$username = $old_username;
			}
			cfg_set($cfg, "users.1.name", $username);
			if ($ro_status == "enabled")
			{
				cfg_set($cfg, "users.2.status", "enabled");
				$crypted = cfg_get_def($cfg, "users.2.password", "");
				if (strlen($hasRoPassword)) {
					if (strlen($roPassword)) {
						$crypted = crypt($roPassword);
					} else {
						$crypted = "";
					}
				}
				cfg_set($cfg, "users.2.password", $crypted);
				$old_username = cfg_get_def($cfg, "users.2.name", "guest");
				if (strlen($rousername) == 0)
				{
					$rousername = $old_username;
				}
				cfg_set($cfg, "users.2.name", $rousername);
				cfg_set($cfg, "users.2.gid", 100);
				cfg_set($cfg, "users.2.uid", 100);
				cfg_set($cfg, "users.2.shell", "/bin/false");
			}
			else
			{
				cfg_set($cfg, "users.2.status", "disabled");
			}
			cfg_set($cfg, "resolv.host.1.status", "enabled");
			cfg_set($cfg, "resolv.host.1.name", $hostname);
			if ($date_status != "enabled") {
				$date_status = "disabled";
			}
			cfg_set($cfg, "system.date.status", $date_status);
			if (isset($systemdate)) {
				cfg_set($cfg, "system.date", $systemdate);
			}
			cfg_set($cfg, "system.timezone", $timezone);
			if ($resetb_status != "enabled") {
				$resetb_status = "disabled";
			}
			cfg_set($cfg, "system.button.reset", $resetb_status);
			if ($radio_outdoor == 0) {
				if ($advmode_status != "enabled") {
					$advmode_status = "disabled";
				}
				cfg_set($cfg, "system.advanced.mode", $advmode_status);
			}

			cfg_set($cfg, "system.latitude", $latitude);
			cfg_set($cfg, "system.longitude", $longitude);
			cfg_save($cfg, $cfg_file);
			cfg_set_modified($cfg_file);
			$modified = 1;
		}
	}
}

if ($modified || !isset($hostname))
{
	$hostname = cfg_get_def($cfg, "resolv.host.1.name", "UBNT");
}
if ($modified || !isset($username))
{
	$username = cfg_get_def($cfg, "users.1.name", $username);
}
if ($modified || !isset($rousername))
{
	$rousername = cfg_get_def($cfg, "users.2.name", $rousername);
}
if ($modified || !isset($ro_status))
{
	$ro_status = cfg_get_def($cfg, "users.2.status", $ro_status);
}

if ($modified || !isset($date_status))
{
	$date_status = cfg_get_def($cfg, "system.date.status", "disabled");
}

if ($modified || !isset($systemdate))
{
	$systemdate = cfg_get_def($cfg, "system.date", $systemdate);
}

if ($modified || !isset($timezone))
{
	$timezone = cfg_get_def($cfg, "system.timezone", "GMT");
}

if ($modified || !isset($resetb_status)) {
	$resetb_status = cfg_get_def($cfg, "system.button.reset", "enabled");
}

if ($radio_outdoor == 0) {
	if ($modified || !isset($advmode_status)) {
		$advmode_status = cfg_get_def($cfg, "system.advanced.mode", "disabled");
	}
}

if ($modified)
{
	$OldPassword = "";
	$NewPassword = "";
	$NewPassword2 = "";
	$roPassword = "";
}

if ($feature_logo == 1)
{
	$logo_url = cfg_get_def($cfg, "ls_logo.url", "http://");
	$logo_status = cfg_get_def($cfg, "ls_logo.status", "disabled");
}

$hostname = htmlspecialchars($hostname);
$username = htmlspecialchars($username);

$latitude = cfg_get_def($cfg, "system.latitude", $latitude);
$longitude = cfg_get_def($cfg, "system.longitude", $longitude);

include("lib/system.tmpl");
>

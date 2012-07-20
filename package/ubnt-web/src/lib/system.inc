<?
$firmware_file = "/tmp/fwupdate.bin";
$upload_dir = "/tmp/upload";

Function fw_get_version (
	$version = @file("/etc/version");
	if ($version == "-1" || count($version) == 0)
	{
		$version = "unknown";
	}
	else
	{
		$version = $version[0];
	}
    return $version;
);

Function fw_get_build (
	$version = @file("/usr/lib/version");
	if ($version == "-1" || count($version) == 0)
	{
		$version = "unknown";
	}
	else
	{
		$version = $version[0];
	}
	if (ereg("[.]([[:digit:]]{1,4})[.][[:digit:]]{6}[.][[:digit:]]{4}", $version, $regs))
	{
		$build = $regs[1];
	}
	else
	{
		$build = "unknown";
	}
    return $build;
);

Function fw_validate $file (
	global $firmware_file;
	if (fileinode($file) == -1)
	{
		$result = 0;
	}
	else
	{
		if ($file != $firmware_file && rename($file, $firmware_file) != 0)
		{
			$result = 0;
		}
		else
		{
			exec("/sbin/fwupdate -c", $lines, $res);
			if ($res == 0)
			{
				$result = 1;
			}
			else
			{
				$result = 0;
			}
		}
	}
    return $result;
);

	Function check_uploaded_file $file_name, $file_size, $file_description, $max_size (
		$error_msg = "";
		if (strlen($file_name) > 0) {
			if ($file_size > $max_size) {
				$error_msg = sprintf(dict_translate("msg_upl_file_too_big|Uploaded %s file is too big. Maximum allowed size is %d bytes."), $file_description, $max_size);
			} elseif ($logo_file_size == 0) {
				$error_msg = sprintf(dict_translate("msg_upl_file_empty|Uploaded %s file is empty!"), $file_description);
			}
		}
		return $error_msg;
	);

  Function fw_extract_version $file (
    $fp = @fopen($file, "r");
    if ($fp != -1) {
	    $head = fgets($fp, 5);
	    $version = fgets($fp, 128);
	    fclose($fp);
    }
    if (ereg("^([^\.]*)\.([^\.]*)\.([^\.]*)\.([^\.]*)\.(.*)$", $version, $regs)) {
	if (strchr($version, '-')) {
		return "$regs[1].$regs[3].$regs[4].$regs[5]";
	} else {
                $len = strlen($regs[5]);
	        if ($len > 17) {
                        return "$regs[1].$regs[3].$regs[4]." + substr($regs[5], 0, $len - 17);
                }
		return "$regs[1].$regs[3].$regs[4]";
	}
    } else {
        return $version;
    }
  );

Function fw_is_thirdparty $file (
	$fp = @fopen($file, "r");
	if ($fp == -1)
	{
		$result = 1;
	}
	else
	{
		$head = fgets($fp, 5);
		fclose($fp);
		if ($head == "OPEN")
		{
			$result = 1;
		}
		else
		{
			$result = 0;
		}
	}
	return $result;
);

Function cleanup_dir $d (
	if (fileinode($d) != -1)
	{
		@openDir($d);
		$f = readDir();
		while (strlen($f) != 0) {
			if ($f != "." && $f != "..") {
				@unlink("$d/$f");
			}
			$f = readDir();
		}
		@closeDir();
        }
);

>

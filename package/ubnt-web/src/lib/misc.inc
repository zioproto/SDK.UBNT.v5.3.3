<?
$wmm_names[0] = dict_translate("No QoS");
$wmm_names[1] = dict_translate("Auto Priority");
$wmm_names[2] = dict_translate("Video Priority");
$wmm_names[3] = dict_translate("Voice Priority");
$wmm_values[0] = -1;
$wmm_values[1] = 0;
$wmm_values[2] = 4;
$wmm_values[3] = 8;

Function wmm_get_status $wmm_level (
   if ($wmm_level == "-1") {
       $status = "disabled";
   }
   else
   {
	   $level = intval($wmm_level);
	   $status = "disabled";
	   switch ($level) {
	   case 0;
	   case 4;
	   case 8;
	       $status = "enabled";
	       break;
	   }
   }

   return $status;
);

Function wmm_get_levelname $wmm_status, $wmm_level (
   global $wmm_names;
   global $wmm_values;
   $levelname = $wmm_names[0];
   if ($wmm_status == "enabled" || $wmm_status == 1) {
       $i = 0;
       while ($i < count($wmm_values)) {
           if ($wmm_values[$i] == $wmm_level) {
               $levelname = $wmm_names[$i];
               $i = count($wmm_values) - 1;
           }
           $i++;
       }
   }

   return $levelname;
);

Function wmm_generate_options $wmm_status, $wmm_level (
   global $wmm_names;
   global $wmm_values;

   $selectedName = wmm_get_levelname($wmm_status, $wmm_level);
    
   $i = 0;
   while ($i < count($wmm_values)) {
        $options += "<option value=\"" + $wmm_values[$i] + "\"";
        $name = $wmm_names[$i];
        if ($name == $selectedName) {
            $options += " selected";
        }
        $options += ">" + $name + "</option>";
        $i++;
   }

   return $options;
);

Function cfg_set_signal_leds $cfg, $led1, $led2, $led3, $led4 (
	cfg_set($cfg, "wireless.1.signal_led1", $led1);
	cfg_set($cfg, "wireless.1.signal_led2", $led2);
	cfg_set($cfg, "wireless.1.signal_led3", $led3);
	cfg_set($cfg, "wireless.1.signal_led4", $led4);
);

Function cfg_get_leds $cfg (
	$v[0] = cfg_get_def($cfg, "wireless.1.signal_led1", "94");
	$v[1] = cfg_get_def($cfg, "wireless.1.signal_led2", "80");
	$v[2] = cfg_get_def($cfg, "wireless.1.signal_led3", "73");
	$v[3] = cfg_get_def($cfg, "wireless.1.signal_led4", "65");

	return $v;
);

Function secsToCountdown $time
(
	$time = IntVal($time);
	$days = $time / 86400;
	$hours = ($time / 3600) % 24;
	$minutes = ($time / 60) % 60;
	$secs = $time % 60;

	if ($days > 0) {
		if ($days == 1) {
			$daystr = dict_translate("day");
		} else {
			$daystr = dict_translate("days");
		}
		$result = sprintf("%d %s %.2d:%.2d:%.2d",
			$days, $daystr, 
			$hours, $minutes, $secs);
	}
	else
	{
		$result = sprintf("%.2d:%.2d:%.2d", $hours, $minutes, $secs);
	}
	return $result;
);

Function fast_clock_mode $ieee_mode (
	switch ($ieee_mode) {
		case "11naht40minus";
		case "11naht40plus";
		case "11naht40";
			$fc_mode = "11nght40";
			break;
		case "1";
		case "a";
		case "11naht20";
			$fc_mode = "11nght20";
			break;
		default;
			$fc_mode = $ieee_mode;
			break;
	}
	return $fc_mode;
);

Function get_timings $ieee_mode, $clksel, $fastclock (
	$maxacktimeout = 204;
	$sltconst = 12;

	if (!$fastclock) {
		$timings_ieee_mode = $ieee_mode;
	} else {
		$timings_ieee_mode = fast_clock_mode($ieee_mode);
	}

	switch ($timings_ieee_mode)
	{
		case "11naht40minus";
		case "11naht40plus";
		case "11naht40";
			$sltconst = 12;
			$maxacktimeout = 204;
			break;
		case "1";
		case "a";
		case "11naht20";
			if ($clksel == 1)
			{
				$sltconst = 20;
				$maxacktimeout = 819; /* 20 half */
			}
			elseif ($clksel == 2)
			{
				$sltconst = 41;
				$maxacktimeout = 1638; /* 10 quarter */
			}
			else
			{
				$sltconst = 12;
				$maxacktimeout = 409; /* 40 */
			}
			break;
		case "2";
		case "b";
			$maxacktimeout = 744; /* 22 */
			$sltconst = 14;
			break;
		case "11nght40minus";
		case "11nght40plus";
		case "11nght40";
			$sltconst = 12;
			$maxacktimeout = 186;
			break;  
		case "3";
		case "gt";
		case "g";
		case "pureg";
		case "11nght20";
			if ($clksel == 1)
			{
				$sltconst = 20;
				$maxacktimeout = 744; /* 22 half */
			}
			elseif ($clksel == 2)
			{
				$sltconst = 40;
				$maxacktimeout = 1489; /* 11 quarter */
			}
			else
			{
				$sltconst = 12;
				$maxacktimeout = 372; /* 44 */
			}
			break;
		case "7";
		case "ast";
		case "5";
		case "at";
			$sltconst = 6;
			$maxacktimeout = 204; /* 80 */
			break;
	}

	$timings[0] = $sltconst;
	$timings[1] = $maxacktimeout;

	return $timings;
);

Function ack_to_distance $ack
(
	global $cfg, $wlan_iface, $ieee_mode, $clksel;

	$ieee_mode = strtolower(cfg_get_ieee_mode($cfg, $wlan_iface, $ieee_mode));
	$clksel = cfg_get_clksel($cfg, $wlan_iface, $clksel);
	$timings = get_timings($ieee_mode, $clksel);

	$sltconst = $timings[0];
	$minacktimeout = $sltconst * 2 + 3;

	if ($ack >= $minacktimeout) {
		$dist = DoubleVal($ack - $minacktimeout) * 150;
	}
	else {
		$dist = 0.0;
	}

	$distkm = $dist / 1000.0;
	$distmiles = $dist / 1609.344;

	return Sprintf("%d / %.1f miles (%.1f km)", $ack, $distmiles, $distkm);
);

Function bytes_to_human $bytes (
	$b = DoubleVal($bytes);
	$str = "";
	if ($b > 1073741824) {
		$str = sprintf("%.2f GBytes", $b / 1073741824);
	} else {
		if ($b > 1048576) {
			$str = sprintf("%.2f MBytes", $b / 1048576);
		} else {
			if ($b > 1024) {
				$str = sprintf("%.2f kBytes", $b / 1024);
			}
		}
	}
	return $str;
);

Function get_current_ack (
	$retval = "";
	$ret = Exec("iwpriv wifi0 get_acktimeout", $arr, $result);
	if ($result == 0) {
		if (ereg("([[:print:]]*):([[:print:]]*)$", $arr[0], $res)) {
			$retval = $res[2];
		}
	}

	return $retval;
);

>

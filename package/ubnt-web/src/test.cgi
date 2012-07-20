#!/sbin/cgi
<?
include("lib/settings.inc");

$rssi_test_duration = "15";
$progress_duration = "47";
$default_rssithresh = "13";
$default_testchain = 3;

$test_links = 1;

if ($radio1_chains > 1){
	$test_links = 2;
}

$chain_name[0] = "Chain 0";
$chain_name[1] = "Chain 1";

eval("\$essid_val=\$essid;");

if (strlen($essid_val) == 0) {
	$essid_val="LINKTEST";
}

if ($REQUEST_METHOD=="POST" && $action=="test") {
	eval("\$tchain=\$usechain;");
}

if (!isset($tchain)) {
	$tchain = $default_testchain;
}

if (!isset($rssithresh)) {
	$rssithresh = $default_rssithresh;
}
if (!isset($file_url)) {
	$file_url="ftp://192.168.1.254/test/test.zip";
}

if (!isset($channel)) {
	exec("athchans -i ath0 0");
	exec("athchans -i ath0 -g",$chanlist);
	ereg("^[0-9]+",$chanlist,$channel);
}


>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>Testing utility</title>
<style type="text/css">
body, td, th, table {
    font-family: Verdana, Arial, Helvetica, sans-serif;
    font-size: 12px;
}
th { border: 1px solid #aaa;
background-color: #eee;
color: #900;
text-align: left;
text-indent: 10px;
white-space: nowrap;
margin-left: 10px;
font-size: 14px;
font-weight: bold;
letter-spacing: 0.3em;
width: 100%;
}
td { padding-left: 20px; padding-right: 20px; padding-top: 5px }
div.errmsg {
color: red;
font-weight: bold;
border-width: 1px 1px 0px 1px;
background-color: #FFFCE2; 
border-style: solid;
border-color: #666; 

padding: 5px 5px 5px 5px;
}
div.err {
color: red;
font-weight: bold;
font-size: 32px;
border-width: 0px 1px 1px 1px;
background-color: #FFFCE2; 
border-style: solid;
border-color: #666; 
padding: 5px 5px 5px 5px;
}
div.msg {
font-weight: bold;
border-width: 1px 1px 0px 1px;
background-color: #FFFCE2; 
border-style: solid;
border-color: #666; 
padding: 5px 5px 5px 5px;
}
div.success {
color: #009D53;
font-weight: bold;
font-size: 32px;
border-width: 0px 1px 1px 1px;
background-color: #FFFCE2; 
border-style: solid;
border-color: #666; 
padding: 5px 5px 5px 5px;
}
input[type=submit], input[type=reset] {
background: #eee;
color: #222;
border: 1px outset #ccc;
padding: .1em .5em;
}
input[type=text], input[type=file]{ padding: .25em .5em }
input[type=text]:focus, input[type=password]:focus, input[type=file]:focus { 
border: 1px solid #886 
}
input[type=text], input[type=file]{
background: #fff;
color: #000;
border: 1px solid #d7d7d7;
}
table { border: 0px; width: 700px }
table.data { border: 0px; width: 600px }
table.data th { border: 0px; background-color: #fff; color: #333; font-size: 12px; font-weight: bold; width: 30%; letter-spacing: normal}
table.data input[type=submit] { margin-right:100px; width: 6em; }
div#progress {
display: block;
border: 1px solid grey;
text-align: left;
width: 600px;
}
</style>
<script type="text/javascript" language="javascript" src="jsl10n.cgi"></script>
<script language="javascript1.2" type="text/javascript" src="FULL_VERSION_LINK/progress.js"></script> 
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
<script type="text/javascript" language="javascript">
//<!--
var duration = <? echo $progress_duration;>;
var check_timer = false;


function init() {

<?	
	if ($REQUEST_METHOD=="POST" && $action=="test") {
		echo "duration="+intval($progress_duration)+";";
		if (strlen($do) != 0) {
			echo "duration=duration+10;";
		}
		if ($usechain == 3)	{
			echo "duration=duration*2;";
		}>
		toggleDownload();
		var progress = new Progress("progress", "FULL_VERSION_LINK/images/p.gif", 600);
		progress.run(duration * 1000, "shut(true)");
         	checkDone();
<?
	}
>
}

function checkDone() {
	// check if we are still being displayed at all
	jQuery.getJSON("testdone.cgi?"+(new Date().getTime()), function(d) {
		if (d.done) {
                	shut(true);
                        return false;
                }
       		if (check_timer)
       			clearTimeout(check_timer);
       		check_timer = setTimeout("checkDone()", 1000 * <?if(strlen($ethonly)==0){>5<?}else{>1<?}>);
	});
	return false;
}


function shut(reload) {
	if (reload) {
		var url = document.location.href;
		var idx = url.indexOf('?');
		if (idx > 0) {
			url = url.substring(0, idx);
		}
       		if (check_timer)
       			clearTimeout(check_timer);
		url += "?";
		url += "essid=" + "<?echo htmlspecialchars($essid_val)>" + "&";
		url += "tchain=" + "<?echo htmlspecialchars($tchain)>" + "&";
		url += "file_url="+"<?echo htmlspecialchars($file_url)>";
                url += "&do="+"<?echo $do>";
                url += "&ethonly="+"<?echo $ethonly>";
                url += "&rssithresh="+"<?echo $rssithresh>";
                url += "&channel="+"<?echo $channel>";
                url += "&chan_scan="+"<?echo $chan_scan>";
		document.location.href = url;
	}
}
function setEnabled(o, enabled) {
	if (!o) return false;
	o.disabled = !enabled;
	if (enabled) { o.style.backgroundColor = "#FFFFFF"; }
	else { o.style.backgroundColor = "#f0f0f0"; }
}

function toggleDownload() {
	var o = document.getElementById("file_url");
        var d = document.getElementById("do");
       	setEnabled(o, d.checked);
}

function toggleScanAll() {
	var o = document.getElementById("channel");
        var d = document.getElementById("chan_scan");
       	setEnabled(o, !d.checked);
}


function sync(cbox, text) {
        var o = document.getElementById(text);
      	var c = document.getElementById(cbox);
	setEnabled(o, c.checked);
        toggleDownload();
}

jQuery(document).ready(init);

// -->
</script>
</head>
<body bgcolor=white>
<a href="index.cgi">Main</a>
<table align="center">
<tr><th>Test Settings 
        <?                                                                    
                if ($radio1_ccode_locked == 1)                                
                        {                                                                                                                
                                        echo "[country locked to ";           
                                        if ($radio1_ccode == 840)             
                                                {echo "USA]";}                                         
                                        else                                                           
                                                {echo "unknown";}                                      
                        } elseif ($radio1_ccode_locked == 2) {                                         
                                        echo "[regdomain locked to ";                                  
                                        if ($radio1_ccode == 42)                                       
                                                {echo "42 (USA/Canada)]";}                                                                     
                                        else                                                                                                   
                                                {echo "unknown]";}                                                                             
                        }                                                                                                                      
                else { echo "[country unlocked]";}                                                                                             
        >                                                                                                                                      
	</th>
</tr>
<tr><td>
<form enctype="multipart/form-data" action="<?echo $PHP_SELF;>" method="POST">
<table class="data">
<tr>
<th>SSID to associate to:</th>
<td>
<input id="essid" name="essid" type="text" size="16" value="<?echo htmlspecialchars($essid_val)>"></td>
</tr>
<tr>
<th>Working channel:</th>
<td>
<input id="channel" name="channel"  type="text" size="4" value="<?echo htmlspecialchars($channel)>">
<input id="chan_scan" name="chan_scan" type="checkbox" <?if(strlen($chan_scan)!=0) { echo "checked";}> onClick="toggleScanAll();">Scan all</td>
</tr>

<? 
if ($radio1_chains > 1) { >
<tr>
<th>Test:</th>
<td><input id="usechain1" name="usechain" type="radio" value="1" <?if($tchain==1){ echo "Checked ";}>  >Chain 0</td>
</tr>
<tr>
<td>&nbsp;</td>
<td><input id="usechain2" name="usechain" type="radio" value="2" <?if($tchain==2){ echo "Checked ";}>  >Chain 1</td>
</tr>
<tr>
<td>&nbsp;</td>
<td><input id="usechain4" name="usechain" type="radio" value="3" <?if($tchain==3){ echo "Checked ";}>  > All tests</td>
</tr>
<?	} >

<tr>
<th>RSSI threshold:</th>
<td><input name="rssithresh" type="text" size="5" value="<?echo htmlspecialchars($rssithresh)>"></td>
</tr>
<tr>
<th>Include file upload/download test</th>
<td><input id="do" name="do" type="checkbox" <?if(strlen($do)!=0) { echo "checked";}> onClick="toggleDownload();"></td>
</tr>
<tr>
<th>URL to retrieve file from:</th>
<td><input id="file_url" name="file_url" type="text" size="64" value="<?echo htmlspecialchars($file_url)>"></td>
</tr>
<th>Test Ethernet only</th>
<td><input id="ethonly" name="ethonly" type="checkbox" <?if(strlen($ethonly)!=0) { echo "checked";}>></td>
</tr>

<? if ($REQUEST_METHOD=="POST" && $action=="test") {
    @unlink("/tmp/test-started");
    @unlink("/tmp/test-done");

	@unlink("/tmp/test-flash.dat");
	@unlink("/tmp/test-eth.dat");
    @unlink("/tmp/test.sh");
    

    $fp = fopen("/tmp/test.sh", "w");
    fputs($fp, "#!/bin/sh\n");
    fputs($fp, "echo Start > /tmp/test-started\n");

	fputs($fp, "/sbin/factorytest ethernet\n");
    
	if (strlen($ethonly)==0) {
    
	    fputs($fp, "/sbin/factorytest flash\n");
		fputs($fp, "/sbin/factorytest led\n");
	
	    $i = 0;
	
		$chains = intval(1);
	
		if ($radio1_chains == 1){
	
			$chains = intval(1);
			$usechain = intval(1);
		}
		else {
	
			$chains = intval($usechain);
			if ($usechain == 3) {
				$chains--;
			}
		}
	
	    while ($i < $chains) {
	    	$i = $i + 1;
	
	       	@unlink("/tmp/test-rssi"+$i+".dat");
	   		@unlink("/tmp/test-setup"+$i+".dat");
	       	@unlink("/tmp/test-throughput"+$i+".dat");
	
			if (($usechain == 3) || ($i == intval($usechain))){
				if (strlen($chan_scan)!=0) {
				fputs($fp, "/sbin/factorytest setup -i "+$wlan_iface+" -e "+$essid+" -n "+$i+" -o /tmp/test-setup"+$i+".dat\n");
				}
				else {
					fputs($fp, "/sbin/factorytest setup -i "+$wlan_iface+" -e "+$essid+" -n "+$i+" -f "+$channel+" -o /tmp/test-setup"+$i+".dat\n");
				}
				fputs($fp, "sleep 1\n");
				fputs($fp, "/sbin/factorytest rssi -c "+$rssi_test_duration+" -e "+$essid+" -i "+$wlan_iface+" -o /tmp/test-rssi"+$i+".dat\n");
				if (strlen($do)!=0) {
					fputs($fp, "sleep 1\n");
					fputs($fp, "/sbin/factorytest throughput -u "+$file_url+" -o /tmp/test-throughput"+$i+".dat\n");
				}
			}
	    }
	}
	
    fputs($fp, "echo Done > /tmp/test-done\n");
    fclose($fp);

    chmod("/tmp/test.sh", 755);
    $cmd="/tmp/test.sh";
    bgexec(1, $cmd);
>
<tr><td align="center" colspan="2">Test in progress...</td></tr>
<tr><td colspan="2"><div id="progress"></div></td></tr>
<?} else {>
<tr>
<td colspan="2" align="right">
<input name="action" type="hidden" value="test">
<input type="submit" value="GO!">
</td>
</tr>
<tr><td colspan="2">
<?

if (fileinode("/tmp/test-started") != -1) {
	$cmd="kill -9 `pidof test.sh` ";
	exec($cmd);
	$cmd="kill -INT `pidof factorytest`";
	exec($cmd);
	@unlink("/tmp/test-started");
	@unlink("/tmp/test-done");
	if ((fileinode("/tmp/test-flash.dat") != -1) && (strlen($ethonly)==0)) {
		$flashtestdat = @cfg_load("/tmp/test-flash.dat");
		$status = cfg_get_def($flashtestdat, "status", "-1");
		if (intval($status) != 0) {
			echo "<div class=\"errmsg\">\n";
			$msg = cfg_get_def($flashtestdat, "message", "Unknown error");
			echo $msg;
			echo "</div><div class=\"err\">FLASH TEST FAILED</div>\n";
		} else {
			echo "<div class=\"msg\">\n";
			echo "Done";
			echo "</div><div class=\"success\">FLASH TEST SUCCESS</div>\n";
		}
		echo "<br>";
	}

	if (strlen($ethonly)==0) {

	$i = 0;

	$avg_store[0] = 0;
	$avg_store[1] = 0;
	$avg_store[2] = 0;

	$assoc_st = 1;


	while ($i < $test_links) {

      	 	$i = $i + 1;

			if (fileinode("/tmp/test-setup"+$i+".dat") != -1) {
				$ch_s = 1;
				$setupdat = @cfg_load("/tmp/test-setup"+$i+".dat");
				$status = cfg_get_def($setupdat, "status", "-1");
				$msg = cfg_get_def($setupdat, "message", "Unknown error");
				if (intval($status) != 0) {					                	
					$assoc_st = 0;
					echo "<div class=\"errmsg\">\n";
					echo $msg;
					echo "</div><div class=\"err\">ASSOCIATION to '"+$essid+"' FAILED on "+$chain_name[$i-1]+"</div>\n";
				echo "<br>";
			}

            if (fileinode("/tmp/test-rssi"+$i+".dat") != -1) {
				$ch_s = 1;
				$rssidat = @cfg_load("/tmp/test-rssi"+$i+".dat");
        	                $status = cfg_get_def($rssidat, "status", "-1");


                	        $avg = cfg_get_def($rssidat, "avg[2]", "0");
							$avg_store[$i-1] = $avg;
                        	if (intval($status) < 0) {
					echo "<div class=\"errmsg\">\n";
                                        $msg = cfg_get_def($rssidat, "message", "Unknown error");
					echo $msg;
					echo "</div><div class=\"err\">RSSI TEST FAILED on "+$chain_name[$i-1]+"</div>\n";
	                        } else {


        	                	if (intval($avg) < intval($rssithresh)) {
						echo "<div class=\"errmsg\">\n";
						echo "FAILURE: RSSI average of %#0.1f is TOO LOW, expected at least %u\n" $avg, $rssithresh;
						echo "</div><div class=\"err\">RSSI TEST FAILED on "+$chain_name[$i-1]+"</div>\n";
	                	        } else {
						echo "<div class=\"msg\">\n";
	        	                	echo "RSSI TEST PASSED SUCCESSFULLY: RSSI average is %#0.1f\n" $avg;
						echo "</div><div class=\"success\">RSSI TEST SUCCESS on "+$chain_name[$i-1]+"</div>\n";
        	                	}
	                        }
				echo "<br>";
                        }
             if (fileinode("/tmp/test-throughput"+$i+".dat") != -1) {
				$ch_s = 1;
				$thrdat = @cfg_load("/tmp/test-throughput"+$i+".dat");
        	                $status = cfg_get_def($thrdat, "status", "-1");
                	        $tx = cfg_get_def($thrdat, "tx.speed", "0");
                	        $rx = cfg_get_def($thrdat, "rx.speed", "0");
                                $msg = cfg_get_def($thrdat, "message", "Unknown error");
                        	if (intval($status) < 0) {
					echo "<div class=\"errmsg\">\n";
        				echo $msg;
					echo "</div><div class=\"err\">THROUGHPUT TEST FAILED on "+$chain_name[$i-1]+"</div>\n";
	                        } else {
       					echo "<div class=\"msg\">\n";
               	                	echo "THROUGHPUT TEST ("+$msg+") on "+$chain_name[$i-1]+":\n";
                                        $tx = doubleval($tx) * 8 / (1024 * 1024);
                                        $rx = doubleval($rx) * 8 / (1024 * 1024);
					echo "Upload speed: %.3g Mbits/s\n" $tx;
					echo "Download speed: %.3g Mbits/s\n" $rx;
       					echo "</div>\n";
					echo "<div class=\"success\">THROUGHPUT TEST SUCCESS on "+$chain_name[$i-1]+"</div>\n";

                                }
				echo "<br>";
                }
			}

			@unlink("/tmp/test-rssi"+$i+".dat");
       		@unlink("/tmp/test-setup"+$i+".dat");
	       	@unlink("/tmp/test-throughput"+$i+".dat");
        }
        
			if (($radio1_chains != 1) && ($tchain ==3) && ($assoc_st == 1)){
				$diff=doubleval($avg_store[1])-doubleval($avg_store[0]);
				if (abs($diff) <= 6) {
					echo "<div class=\"msg\">\n";
					echo "RSSI differential is %#0.1fdB\n" $diff;
					echo "</div><div class=\"success\">RSSI DIFFERENTIAL TEST SUCCESS</div>\n";
				}
				else {
					echo "<div class=\"errmsg\">\n";
					echo "FAILURE: RSSI differential %#0.1f is TOO HIGH, expected +/- 6bB\n" $diff;
					echo "</div><div class=\"err\">RSSI DIFFERENTIAL TEST FAILED</div>\n";
				}
			}
			echo "<br>";
		}

		@unlink("/tmp/test-flash.dat");
        @unlink("/tmp/test.sh");
        
        $phycount=1;
        
		if (fileinode("/etc/board.info") != -1) {
			$phycntdat = @cfg_load("/etc/board.info");
			$phycount = cfg_get_def($phycntdat, "board.phycount", "-1");
		}
        
		if (fileinode("/tmp/test-eth.dat") != -1) {
			$ethdat = @cfg_load("/tmp/test-eth.dat");
			$testphycount = cfg_get_def($ethdat, "ethcnt", "-1");
			$foundphycount = cfg_get_def($ethdat, "ethfound", "-1");
			
			$i = 0;
			while  ($i < $phycount) {
				$link = cfg_get_def($ethdat, "port$i.link", "-1");
				$duplex = cfg_get_def($ethdat, "port$i.fullduplex", "-1");
				$speed = cfg_get_def($ethdat, "port$i.speed", "-1");
				$maxspeed = cfg_get_def($ethdat, "port$i.maxspeed", "-1");
				$success = cfg_get_def($ethdat, "port$i.success", "-1");
				$phyaddr = cfg_get_def($ethdat, "port$i.phy", "-1");
				$primary = cfg_get_def($ethdat, "port$i.primary", "-1");
					
				if ($primary == 1) {
					$eth_port = "PRIMARY";
						}
				elseif ($primary == 0) {
					$eth_port = "SECONDARY($phyaddr)";
						}
				else {
					$eth_port = "UNKNOWN";
						}
					
				if ($success == 1) {
					echo "<div class=\"msg\">ETHERNET TEST SUCCESS </div>\n";
					echo  "<div class=\"success\">ETH PORT $eth_port SUCCESS."; 
					echo "</div>\n";
				}
				else {
							$msg = "";
					if ($link == 0) {
								$msg += "Cable unplugged<br>";
							}
					elseif ($link == 1) {
								
						if ($duplex != 1) {
							$msg += "Duplex: HALF<br>";
								}
						if ($speed < $maxspeed) {
							$msg += "Speed: $speed<br>";
									$fail = 1;
								}
							}
							else {
						$msg +="Port not found";
							}
								echo "<div class=\"errmsg\">\n";
								echo "ETHERNET TEST FAILURE:<br>\n";
								echo "$msg\n";
								echo "</div>";
								echo "<div class=\"err\">";
								echo "ETH PORT $eth_port TEST FAILURE\n";
								echo "</div>\n";
							}
							echo "<br>";
				$i++;
			}
		
		}


        

		if ($feature_3g == 1) {
			if (fileinode("/etc/3g.inc") != -1) {
				include("/etc/3g.inc");
				echo "<div class=\"msg\">\n";
				echo "Done\n";
				echo "</div><div class=\"success\">3G MODEM TEST SUCCESS. Detected: $threeg_product</div>\n";
			}
			else {
				echo "<div class=\"errmsg\">\n";
				echo "FAILURE: no 3G modem detected\n";
				echo "</div><div class=\"err\">3G MODEM TEST FAILED</div>\n";
			}
		}
}>
</td></tr>
<?}>
</table>

</form>
</td></tr>
<tr><td>&nbsp;</td></tr>

</table>
</body>
</html>

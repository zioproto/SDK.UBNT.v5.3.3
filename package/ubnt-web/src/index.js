var reload_timeout = 0;

function refreshStatus(d, textStatus, xhr) {
	if (d == null) {
		return handleError(xhr, textStatus, null);
	}
	var is_ap = (d.wireless.mode == 'ap');
	setOperationMode(d);
	update_basic(d);
	update_polling(is_ap, d.wireless.polling, d.wireless);
	update_misc(d);
	update_wanlan(d.lan, d.wan);
	update_hwaddr(d.lan.hwaddr, d.wan.hwaddr, d.wlan.hwaddr);
	update_wlan(d.wlan);
	update_antenna(d.wireless.antenna);
	update_chains(d.wireless.chains);
	if (document.getElementById("threeg_info"))
		update_threeg(d.threeg);
}

function handleError(xhr, textStatus, errorThrown) {
	if (xhr && xhr.status != 200 && xhr.status != 0) {
		window.location.reload();
	}
}

function reloadStatus() {
	$.ajax({
		url: "status.cgi",
		cache: false,
		dataType: "json",
		success: refreshStatus,
		error: handleError,
		complete: function(xhr, status) {
				if (reload_timeout)
					clearTimeout(reload_timeout);
				reload_timeout = setTimeout(reloadStatus, 2000);
			}
	});
	return false;
}

function setOperationMode(status) {
	var wireless = status.wireless;
	var nmode = status.host.netrole;
	var lan = status.lan;
	var wan = status.wan;
	var services = status.services;
	var firewall = status.firewall;
	var airview = status.airview;

	var is_ap = (wireless.mode == 'ap');
	var mode_string = status.wlan.enabled ? getModeString(is_ap, wireless.wds) : l10n_get('Disabled');

	var airview_status;
	if (airview.enabled==1) {
		old_mode_string = mode_string;
		mode_string = l10n_get('Spectral Analyzer');
		if (airview.status.active_connections > 0) {
			airview_status = l10n_get('Active') + ' - ' + airview.status.active_connections + ' ' + l10n_get('clients');
		} else if (airview.watchdog.seconds_to_exit > 0) {
			airview_status = l10n_get('Idle for') + ' ' + airview.watchdog.seconds_idle + l10n_get('s') + ".  " + l10n_get('Back to') + ' ' + old_mode_string + ' ' + l10n_get('in') + ' ' + airview.watchdog.seconds_to_exit + l10n_get('s');
		} else {
			airview_status = l10n_get('Switching back to') + ' ' + old_mode_string;
		}
	} else {
		airview_status = "";
	}

	var is_soho = false, is_bridge = false;
	switch (nmode) {
	case 'bridge':
		netmodestr=l10n_lang['Bridge'];
		is_bridge = true;
		break;
	case 'router':
		netmodestr=l10n_lang['Router'];
		break;
	case 'soho':
		netmodestr=l10n_lang['SOHO Router'];
		is_soho = true;
		break;
	case '3g':
		netmodestr=l10n_lang['3G Router'];
		break;
	}
	$('#netmode').text(netmodestr);
	$('#wmode').text(mode_string);
	$('#astatus').text(airview_status);
	$('#astatusinfo').toggle(airview_status != "");

	var ethcount = lan.status.length + wan.status.length;
	$('.oneport').toggle(ethcount == 1);
	$('.twoport').toggle(ethcount == 2);
	if (ethcount == 2) {
		$('#lan2_label').toggle(!is_soho);
		$('#wanlan_label').toggle(is_soho);
	} else {
		$('#lan_label').toggle(!is_soho);
		$('#wan_label').toggle(is_soho);
	}
	$('.apinfo').toggle(is_ap);
	$('.stainfo').toggle(!is_ap);
	$('.bridge').toggle(is_bridge);
	$('.router').toggle(!is_bridge);
	$('#wanmac').toggle(is_soho);
	$('#dhcpc_info').toggle(1 == services.dhcpc);
	$('#dhcp_leases').toggle(1 == services.dhcpd);
	$('#ppp_info').toggle(1 == services.pppoe);

	$('#a_stainfo').attr('href','stainfo.cgi?ifname=' + global['wlan_iface'] + '&sta_mac='+wireless.apmac+'&mode=ap');
	$('#a_fw').attr('href', 'fw.cgi?netmode='+nmode);
	var wd_str = wireless.chwidth;
	if (wireless.cwmmode == 1) {
		if (wireless.rstatus == 1)
			wd_str = (global.has_ht40 ? l10n_get('Auto') + ' 20 / 40' : '20');
		else
			wd_str = (global.has_ht40 ? l10n_get('Auto') + ' ' + wireless.chwidth : '' + wireless.chwidth);
	}

	if (status.wlan.enabled) {
		$('#wd').text(wd_str + ' MHz');
		setExtendedChannel(wireless.opmode);
	}
	else {
		$('#wd').text('-');
		$('#ext_chan').hide();
	}

	var fwall_enabled = !is_bridge ? firewall.iptables : firewall.ebtables;
	$('#fwall').toggle(fwall_enabled == 1);
}

function refreshContent(uri, data) {
	reloadStatus();

	if (uri.indexOf("?") > 0)
		uri = uri + '&id=';
	else
		uri = uri + '?id=';
	$("#extraFrame").load(uri+(new Date().getTime()), data, function(r,s,xhr){
		if (xhr && xhr.status != 200 && xhr.status != 0) {
			window.location.reload();
			return;
		}
		$.ready();
	});
	return false;
}

function format_rate(rate) {
	return parseInt(rate) > 0 ? '' + rate + ' Mbps' : '-';
}

function format_ccq(ccq) {
	return parseInt(ccq) > 0 ? '' + ccq + ' %' : '-';
}

function strip_fwversion(fwversion) {
	if (fwversion.indexOf("-") > 0) {
		var ver = fwversion.split(".");
		if (ver.length > 3) {
			var v = "";
			for(i = 0; i < ver.length - 3; i++) {
				v += ver[i]+"."
			}
			v += ver[i];
			return v;
		}
	}
	return fwversion;
}

function update_basic(status) {
	var host = status.host;
	var wireless = status.wireless;

	$('#signalinfo .switchable').toggle(wireless.rstatus == 5);
	updateSignalLevel(wireless.signal, wireless.rssi, wireless.noisef,
			wireless.chwidth, wireless.rx_chainmask,
			wireless.chainrssi, wireless.chainrssiext);
	$('#hostname').text(host.hostname);
	$('#fwversion').text(strip_fwversion(host.fwversion));
	if (wireless.mode == 'ap' && wireless.hide_essid == 1)
		$('#essid_label').text(l10n_get('Hidden SSID'));
	$('#essid').text(status.wlan.enabled ? wireless.essid : '-');

	var show_mac = (wireless.apmac != "00:00:00:00:00:00") && (wireless.mode == 'ap' || wireless.rstatus == 5);
	$('#apmac').text(show_mac ? wireless.apmac : l10n_get('Not Associated'));
	$('#frequency').text(status.wlan.enabled ? wireless.frequency : '-');
	$('#channel').text(status.wlan.enabled ? wireless.channel : '-');
	$('#txrate').text(status.wlan.enabled ? format_rate(wireless.txrate) : '-');
	$('#rxrate').text(status.wlan.enabled ? format_rate(wireless.rxrate) : '-');
	$('#count').text(status.wlan.enabled ? " "+wireless.count : '-');
	update_ack(status);
}

function update_ack(status) {
	var wireless = status.wireless;
	if ((wireless.polling.enabled == 0 || wireless.polling.noack == 0) && status.wlan.enabled) {
		var dist = (wireless.distance < 150) ? 150 : wireless.distance;
		var dist_km = toFixed(dist / 1000.00, 1);
		var dist_mi = toFixed(dist / 1609.344, 1);
		var ack_val = [wireless.ack, '/', dist_mi, 'miles', '(' + dist_km, 'km)'];
		$('#ack').text(ack_val.join(' '));
	}
	else {
		// PtPNoAck or WLAN interface is disabled
		$('#ack').text('-');
	}
}

function update_polling(is_ap, polling, wireless) {
	if (polling.enabled > 0) {
		$('#polling').text(l10n_get('Enabled'));
		var chain_count = global['chain_count'];
		if (parseInt(chain_count) == 1 && polling.capacity <= 50)
			$('#amcborder').addClass('halfborder');
		$('.pollinfo').show();
	} else {
		$('#polling').text(is_ap ? l10n_get('Disabled') : '-');
		$(".pollinfo").hide();
	}
	/* Workaround for ubnt_poll.capacity = 100% when there is no connections */
	if (wireless.apmac == "00:00:00:00:00:00" || 0 == wireless.count) {
		polling.quality = 0;
		polling.capacity = 0;
	}
	$('#amq').text(polling.quality);
	$('#amc').text(polling.capacity);
	update_meter('amqbar', polling.quality, 100);
	update_meter('amcbar', polling.capacity, 100);
}

function update_hwaddr(lmac, wanmac, wmac) {
	$('#l_mac').text(lmac);
	$('#w_mac').text(wmac);
	$('#wan_mac').text(wanmac);
}

function translate_security(security) {
	var security_str = global['security'];
	if (security_str.length == 0) {
		security_str = security;
	}

	return l10n_get(security_str);
}

function update_misc(status, qos, uptime, ccq, date_time) {
	$('#security').text(status.wlan.enabled ? translate_security(status.wireless.security) : '-');
	$('#qos').text(status.wireless.qos);
	var uptime_str = secsToCountdown(status.host.uptime, l10n_get('day'), l10n_get('days'));
	$('#uptime').text(uptime_str);
	$('#ccq').text(format_ccq(parseInt(status.wireless.ccq) / 10));
	$('#date').text(status.host.time);
}

function get_eth_str(ethstat) {
	if (ethstat.plugged != 0) {
		if (ethstat.speed > 0) {
			var str = '' + ethstat.speed + 'Mbps';
			if (ethstat.duplex == 1)
				str += '-' + l10n_get('Full');
			else if (ethstat.duplex == 0)
				str += '-' + l10n_get('Half');
			return str;
		}
		else {
			return l10n_get('Plugged');
		}
	}
	else {
		return l10n_get('Unplugged');
	}
}

function update_wanlan(lan, wan) {
	$('#lan_ip').text(lan.ip);
	$('#wan_ip').text(wan.ip);
	if (lan.status.length == 2) {
		$('#lan1_cable').text(get_eth_str(lan.status[0]));
		$('#lan2_cable').text(get_eth_str(lan.status[1]));
		return 0;
	} else {
		if (wan.status.length == 1) {
			if (lan.status.length == 1) {
				$('#lan1_cable').text(get_eth_str(wan.status[0]));
				$('#lan2_cable').text(get_eth_str(lan.status[0]));
				return;
			} else {
				$('#lan_cable').text(get_eth_str(wan.status[0]));
				return;
			}
		}
	}
	$('#lan_cable').text(get_eth_str(lan.status[0]));
}

function update_wlan(wlan) {
	$('#wlan_ip').text(wlan.ip);
}

function update_antenna(value) {
	$('#antenna').text(l10n_get(value));
}

function update_chains(value) {
	$('#chains').text(value);
}

function showAction(select) {
	if (select.value == "")
		return;
	openPage(select.value, 700,200);
	select.selectedIndex = 0;
}

function getModeString(is_ap, is_wds) {
	var mode_string;
	if (is_ap)
		mode_string = is_wds ? l10n_get('Access Point WDS') : l10n_get('Access Point');
	else
		mode_string = is_wds ? l10n_get('Station WDS') : l10n_get('Station');

	return mode_string;
}

function setExtendedChannel(opmode) {
	var ext_ch = 0;
	if (ext_ch = /minus$/.test(opmode))
		$("#ext_chan").html('&nbsp;(' + l10n_get('Lower') + ')');
	else if (ext_ch = /plus$/.test(opmode))
		$("#ext_chan").html('&nbsp;(' + l10n_get('Upper') + ')');
	$('#ext_chan').toggle(ext_ch);
}

function update_threeg(threeg) {
	if (threeg != undefined) {
		$('#threeg_info').show();
		$('#threeg_product').html('<img src="FULL_VERSION_LINK/images/' + threeg.image + '" alt="' + threeg.product + '">');
		switch(threeg.sim_status) {
			case 0:
				$('#threeg_signal_row').show();
				$('#threeg_status_row').hide();
				break;
			case 1:
				$('#threeg_signal_row').hide();
				$('#threeg_status_row').show();
				$('#threeg_status').text(l10n_get('Please insert SIM card'));
				break;
			case 2:
				$('#threeg_signal_row').hide();
				$('#threeg_status_row').show();
				$('#threeg_status').text(l10n_get('SIM PIN required'));
				break;
			case 3:
				$('#threeg_signal_row').hide();
				$('#threeg_status_row').show();
				$('#threeg_status').text(l10n_get('SIM Card Blocked'));
				break;
		}
		update_meter('threeg_bar', threeg.signal, 100);
	}
	else {
		$('#threeg_status_row').hide();
		$('#threeg_signal_row').hide();
		if (global.is_3g_product) {
			$('#threeg_product').text(l10n_get('Not detected'));
		}
		else {
			$('#threeg_info').hide();
		}
	}
}



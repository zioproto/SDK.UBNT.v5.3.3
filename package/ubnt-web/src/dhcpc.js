var dhcpc = {
ifname : null,
error: function(xhr, textStatus, errorThrown) {
	if (xhr && xhr.status != 200 && xhr.status != 0) {
		window.location.reload();
	}
},
fetch : function(t) {
	dhcpc.fetch.success = function(data, textStatus, xhr) {
		var error_str = null;
		var can_release = true;
		var can_renew = true;
		if (data.dhcpc.status == 0) {
			var idx = 0;
			if (data.dhcpc.info[idx].status == 0) {
				od = data.dhcpc.info[idx];
				dhcpc.ifname = od.ifname;
				var p = $('#dhcp'+idx);
				p.find('.ipaddr').html(od.ip);
				p.find('.mask').html(od.netmask);
				p.find('.gw').html(od.gateway);
				p.find('.dns1').html(od.dns[0]);
				p.find('.dns2').html(od.dns[1]);
				p.find('.serverid').html(od.serverid);
				p.find('.domain').html(od.domain);
				p.find('.leasetime').html(od.leasetime_str);
				p.find('.leasetime_left').html(od.leasetime_left);
				$('.msg').hide();
				$('.data').show();
			} else {
				error_str = data.dhcpc.info[idx].error;
				can_release = false; can_renew = true;
			}
		} else {
			error_str = data.dhcpc.error;
			can_release = can_renew = false;
		}
		if (error_str && error_str.length > 0 ){
			$('#status_msg').html(error_str);
			$('.msg').show();
			$('.data').hide();
			$('#ctrl_renew').toggle(can_renew);
			$('#ctrl_release').toggle(can_release);
		}
	};
	$.ajax({
		url: '/dhcpcinfo.cgi',
		cache: false,
		async: t ? false : true,
		dataType: 'json',
		success: dhcpc.fetch.success,
		error: dhcpc.error
	});
	return false;
},
release : function() {
	dhcpc.release.success = function(data, textStatus, xhr) {
		dhcpc.fetch();
	};
	$.ajax({
		url: '/dhcpcinfo.cgi?action=release&ifname=' + dhcpc.ifname,
		cache: false,
		dataType: 'json',
		success: dhcpc.release.success,
		error: dhcpc.error
	});
	return false;
},
renew : function() {
	dhcpc.renew.success = function(data, textStatus, xhr) {
		dhcpc.fetch();
	};
	$.ajax({
		url: '/dhcpcinfo.cgi?action=renew&ifname=' + dhcpc.ifname,
		cache: false,
		dataType: 'json',
		success: dhcpc.renew.success,
		error: dhcpc.error
	});
	return false;
}
};

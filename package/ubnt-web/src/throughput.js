var ready = false;
var data_timer = 0;
var max_data = 20;
var lan_data = new PlotData(2, max_data);
var wlan_data = new PlotData(2, max_data);
var ppp_data = new PlotData(2, max_data);
var last_uptime = 0;
var dev_uptime = 0;
var plots = [];

function reloadData() {
	// check if we are still being displayed at all
	var x = document.getElementById('throughput');
	if (!x) {
		return;
	}
	jQuery.getJSON("status.cgi?"+(new Date().getTime()), function(d) {
		if (!d) {
			if (typeof reloadStatus == 'function')
				reloadStatus();
			else 
				window.location.reload();

			return;
		}
		if (isNaN(d.host.uptime)) {
			dev_uptime = (dev_uptime == 0 ? (new Date()).getTime() / 1000 : dev_uptime + 5); //for testing
		} else {
			dev_uptime = d.host.uptime;
		}
	       	update_all("lanCanvas", lan_data, 
			d.lan.rx.bytes, d.lan.tx.bytes);
                if (d.host.netrole == 'soho') {
                	jQuery("#wlangraphlabel").text("WAN");
			update_all("wlanCanvas", wlan_data,
		       		d.wan.rx.bytes, d.wan.tx.bytes);
                }
		else if (d.host.netrole == '3g') {
                	jQuery("#wlangraphlabel").text("WAN");
			update_all("wlanCanvas", wlan_data,
		       		d.wan.rx.bytes, d.wan.tx.bytes);
                }
                else {
                	jQuery("#wlangraphlabel").text("WLAN");
			update_all("wlanCanvas", wlan_data,
		       		d.wlan.rx.bytes, d.wlan.tx.bytes);
                }
                if (d.services.pppoe)
			update_all("pppCanvas", ppp_data,
				d.ppp.rx.bytes, d.ppp.tx.bytes);
		if (ready) {
			if (data_timer) 
				clearTimeout(data_timer);
			data_timer = setTimeout("reloadData()", 1000 * 5);
		}
		else {
			if (data_timer) 
				clearTimeout(data_timer);
			data_timer = setTimeout("reloadData()", 500);
		}
                ready = true;
		last_uptime = dev_uptime;
	});
	return false;
}

function update_all(id, data, rxbytes, txbytes) {
	data.add_values( [rxbytes, txbytes], dev_uptime - last_uptime);
	updateCanvas(id, data);
}

function normalize_max(val, ticks)
{
	var delta = val / ticks;
	var magn = Math.pow(10, Math.floor(Math.log(delta) / Math.LN10));
	var norm = delta / magn;
	var tick_size = 10;
	if(norm < 1.5) tick_size = 1;
	else if(norm < 2.25) tick_size = 2;
	else if(norm < 3) tick_size = 2.5;
	else if(norm < 7.5) tick_size = 5;
	tick_size *= magn;
	if (Math.floor(val / tick_size) * tick_size >= val)
	{
		return val;
	}
	return (Math.floor(val / tick_size) + 1) * tick_size;
}

function normalize_float(val) {
	if (!val)
		return 0;
	if (val > 100)
		return toFixed(val, 0);
	if (val > 10)
		return toFixed(val, 1);
	return toFixed(val, 2);
}

function formatBPS(value)
{
	var unit;
	var power;
	if (Math.round(value) < 1024)
	{
		unit = "bps";
		power = 0;
	}
	else if (Math.round(value / 1024) < 1024)
	{
		value = value / 1024;
		unit = "kbps";
		power = 1;
	}
	else
	{
		value = value / 1024 / 1024;
		unit = "Mbps";
		power = 2;		
	}
	value = normalize_float(value);
	return [value, unit, power, ""+value+unit];
}

function updateCanvas(id, data)
{
	if (!document.getElementById(id))
	{
		return;
	}
	var dp = data.get_plot_values();
	var rx = formatBPS(dp[3][0])[3];
        var tx = formatBPS(dp[3][1])[3];
        var d = [
			{data:dp[0][0], label:'RX: '+rx, lines: {show: true, lineWidth: 1}, points: {show: true, lineWidth: 1, radius: 1}, color: '#2389C6'},
			{data:dp[0][1], label:'TX: '+tx, lines: {show: true, lineWidth: 1}, points: {show: true, lineWidth: 1, radius: 1}, color: '#FF0000'}
		];
        var o = {
			xaxis:{
				noTicks: data.max_data,
				tickFormatter: function(n){ return ''; },
				min: 0,
				max: data.max_data
			},
			yaxis:{
				noTicks: 8,
				tickFormatter: function(n){	return n == 0 ? dp[1]+' 0' : (Math.round(n * 100) / 100); },
				min: 0,
				max: dp[2]
			},
			legend: {
				position: 'nw',
				backgroundOpacity: 0.4
			}
                };
        if (!plots[id]) {
                plots[id] = jQuery.plot(jQuery("#"+id), d, o);
                if (!jQuery.browser.msie)
                        plots[id] = undefined;
        } else {
                plots[id].parseOptions(o);
                plots[id].setData(d);
                plots[id].setupGrid();
                plots[id].draw();
        }
        d = undefined;
        o = undefined;
        dp = undefined;
        rx = undefined;
        tx = undefined;
}

function PlotData(series_count, max_data) 
{
	this.max_data = max_data;
	this.last = new Array(series_count);
	this.data = new Array(series_count);
	this.shifted = new Array(series_count);
	
	for (var i=0; i < series_count; ++i)
	{
		this.last[i] = 0;
		this.data[i] = new Array();
		this.shifted[i] = false;
	}

	this.add_values = function (values, sec)
	{
		var last;
		if (sec <= 0)
		{
			return;
		}
		for (var i = 0; i < this.data.length; ++i)
		{			
			if (this.data[i].length >= this.max_data)
			{
				this.data[i].shift();
			}
			if (values[i])
			{
				last = parseFloat(values[i]);
				if (this.data[i].length == 0)
					this.last[i] = last;
                                if (last < this.last[i]) {
                                	if (this.last[i] > 0x7FFFFFFF)
                                        	this.data[i].push((0xFFFFFFFF - this.last[i] + last) / sec * 8); // bps
                                        else {
                                        	if ((this.last[i] + last) < 0x8FFFFFFF) //handle restart
        	                        		this.data[i].push(last / sec * 8); // bps                                                
	                                        else
        	                        		this.data[i].push((0x7FFFFFFF - this.last[i] + last) / sec * 8); // bps
                                             }
                                }
                                else
					this.data[i].push((last - this.last[i]) / sec * 8); // bps
				this.last[i] = last;
			}
			else
			{
				this.data[i].push(0);
			}
			if (!this.shifted[i] && this.data[i].length == 2)
			{
				this.data[i].shift();
                                this.shifted[i] = true;
			}
		}
	}

	this.get_plot_values = function()
	{
		var result = new Array();
		var tmp;
		var max = 100; // 100 bps
		var format;
		var last = new Array();
		for (var i = 0; i < this.data.length; ++i)
		{
			result[i] = new Array();
			for (var j = 0; j < this.data[i].length; ++j)
			{
				tmp = this.data[i][j];
				if (tmp > max)
				{
					max = tmp;
				}
				result[i][j] = [j, tmp]
			}
			last[i] = this.data[i][j-1];
		}
		format = formatBPS(max);
		max = normalize_max(format[0], 8);
		if (format[2] > 0)
		{
			for (var i = 0; i < result.length; ++i)
			{
				for (var j = 0; j < result[i].length; ++j)
				{
					 result[i][j][1] /= Math.pow(1024, format[2]);
				}
			}
		}
		return [result, format[1], max, last]; // data, unit, normalized value
	}
}

jQuery(document).ready(function(){
	ready = true;
});

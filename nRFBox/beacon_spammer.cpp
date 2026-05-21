#include "config.h"
#include "icon.h"
#include <DNSServer.h>
#include <WebServer.h>

namespace BeaconSpammer {

// Web server for custom input
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer webServer(80);

int currentMenu = 0; // 0: Select Payload, 1: Select Mode, 2: Spamming, 3: Web Input
int payloadIndex = 0;
int modeIndex = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// Spam state
bool isSpamming = false;
unsigned long lastSpamTime = 0;
int marqueeIndex = 0;
unsigned long marqueeTimer = 0;

// Maximum SSIDs
const int MAX_SSIDS = 60;
String customSSIDs[MAX_SSIDS];
int customSSIDCount = 0;

String rickroll[] = {
  "01_Never", "02_Gonna", "03_Give", "04_You", "05_Up",
  "06_Never", "07_Gonna", "08_Let", "09_You", "10_Down",
  "11_Never", "12_Gonna", "13_Run", "14_Around", "15_And",
  "16_Desert", "17_You"
};

String panic[] = {
  "01_FREE_VIRUS", "02_SYSTEM_ERROR", "03_HACKED_NETWORK",
  "04_YOUR_PHONE_IS_INFECTED", "05_PLEASE_REBOOT", "06_DOWNLOAD_RAM"
};

// Raw 802.11 Beacon Frame template
uint8_t beacon_packet[128] = {
  0x80, 0x00, 0x00, 0x00, // Frame Control, Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination address (Broadcast)
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source address (MAC) - Will be randomized
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID - Will be randomized
  0xc0, 0x6c, // Seq-ctl
  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  0x64, 0x00, // Beacon interval
  0x01, 0x04, // Capability info
  // SSID parameter set will be appended dynamically
};

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3);

const char config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Spammer Config</title>
  <style>
    body { font-family: sans-serif; padding: 20px; background: #222; color: #fff; }
    textarea { width: 100%; height: 200px; padding: 10px; background: #333; color: #0f0; border: 1px solid #555; border-radius: 5px; box-sizing: border-box; }
    .checkbox-container { display: flex; align-items: center; margin-top: 15px; }
    .checkbox-container input { width: 20px; height: 20px; margin-right: 10px; cursor: pointer; }
    .checkbox-container label { font-size: 16px; cursor: pointer; user-select: none; }
    .btn { display: block; width: 100%; padding: 15px; background: #007bff; color: #fff; border: none; border-radius: 5px; margin-top: 15px; font-size: 18px; font-weight: bold; cursor: pointer; }
    h2 { margin-top: 0; }
    p { font-size: 14px; color: #aaa; }
  </style>
</head>
<body>
  <h2>Beacon Spammer Payload</h2>
  <p>Enter up to 60 SSIDs (one per line). Prepend numbers to sort them correctly.</p>
  <form action="/save" method="POST">
    <textarea name="ssids" placeholder="01_Hello&#10;02_World"></textarea>
    <div class="checkbox-container">
      <input type="checkbox" id="multiply" name="multiply" value="1">
      <label for="multiply">Multiply first line to max SSIDs (60)</label>
    </div>
    <button type="submit" class="btn">Start Attack</button>
  </form>
</body>
</html>
)rawliteral";

void handleRoot() {
  webServer.send(200, "text/html", config_html);
}

void handleSave() {
  if (webServer.hasArg("ssids")) {
    String payload = webServer.arg("ssids");
    customSSIDCount = 0;
    
    bool multiply = webServer.hasArg("multiply") && webServer.arg("multiply") == "1";
    
    if (multiply) {
      // Find first non-empty line
      String firstWord = "";
      int startIdx = 0;
      while (startIdx < payload.length()) {
        int endIdx = payload.indexOf('\n', startIdx);
        if (endIdx == -1) endIdx = payload.length();
        String line = payload.substring(startIdx, endIdx);
        line.trim();
        if (line.length() > 0) {
          firstWord = line;
          break;
        }
        startIdx = endIdx + 1;
      }
      
      // Strip leading numbering if present (e.g. "01_", "1.", "01 ")
      int stripLen = 0;
      while (stripLen < firstWord.length() && isdigit(firstWord.charAt(stripLen))) {
        stripLen++;
      }
      if (stripLen > 0 && stripLen < firstWord.length()) {
        char nextChar = firstWord.charAt(stripLen);
        if (nextChar == '_' || nextChar == '.' || nextChar == ' ' || nextChar == '-') {
          firstWord = firstWord.substring(stripLen + 1);
          firstWord.trim();
        }
      }
      
      firstWord.replace(" ", "_");
      if (firstWord.length() == 0) firstWord = "Spam";
      
      for (int i = 1; i <= MAX_SSIDS; i++) {
        char buf[35];
        snprintf(buf, sizeof(buf), "%02d_%s", i, firstWord.c_str());
        customSSIDs[customSSIDCount++] = String(buf);
      }
    } else {
      int startIndex = 0;
      while (startIndex < payload.length() && customSSIDCount < MAX_SSIDS) {
        int endIndex = payload.indexOf('\n', startIndex);
        if (endIndex == -1) endIndex = payload.length();
        
        String line = payload.substring(startIndex, endIndex);
        line.trim();
        
        // replace spaces with underscores for safety
        line.replace(" ", "_");
        
        if (line.length() > 0) {
          customSSIDs[customSSIDCount++] = line;
        }
        startIndex = endIndex + 1;
      }
    }
    
    webServer.send(200, "text/plain", "Payload Saved! Look at nRFBox Screen.");
    delay(500);
    currentMenu = 1; // move to mode selection
    WiFi.softAPdisconnect(true);
  }
}

void buildAndSendBeacon(String ssid, uint8_t mac[6], uint8_t channel) {
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  
  // Update MACs
  for(int i=0; i<6; i++) {
    beacon_packet[10 + i] = mac[i];
    beacon_packet[16 + i] = mac[i];
  }
  
  int ssidLen = ssid.length();
  if (ssidLen > 32) ssidLen = 32;
  
  // SSID Element ID and Length
  beacon_packet[36] = 0x00;
  beacon_packet[37] = ssidLen;
  
  // SSID Content
  for (int i = 0; i < ssidLen; i++) {
    beacon_packet[38 + i] = ssid.charAt(i);
  }
  
  // Add Supported Rates
  int offset = 38 + ssidLen;
  uint8_t supported_rates[] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};
  for(int i=0; i<sizeof(supported_rates); i++) {
     beacon_packet[offset++] = supported_rates[i];
  }
  
  // DS Parameter (Channel)
  beacon_packet[offset++] = 0x03;
  beacon_packet[offset++] = 0x01;
  beacon_packet[offset++] = channel;
  
  esp_wifi_80211_tx(WIFI_IF_AP, beacon_packet, offset, false);
}

void spammerSetup() {
  Serial.begin(115200);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  
  esp_bt_controller_deinit();
  WiFi.disconnect();
  
  currentMenu = 0;
  payloadIndex = 0;
  modeIndex = 0;
  isSpamming = false;
  customSSIDCount = 0;
  setNeoPixelColour("blue");
}

void drawMenuScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);
  
  if (currentMenu == 0) {
    u8g2.drawStr(0, 10, "Select Payload:");
    if(payloadIndex == 0) u8g2.drawStr(0, 23, "> 1. Rickroll"); else u8g2.drawStr(0, 23, "  1. Rickroll");
    if(payloadIndex == 1) u8g2.drawStr(0, 33, "> 2. Panic"); else u8g2.drawStr(0, 33, "  2. Panic");
    if(payloadIndex == 2) u8g2.drawStr(0, 43, "> 3. Custom (Web)"); else u8g2.drawStr(0, 43, "  3. Custom (Web)");
    u8g2.drawStr(0, 60, "R: Select | C: Exit");
  } 
  else if (currentMenu == 1) {
    u8g2.drawStr(0, 10, "Select Mode:");
    if(modeIndex == 0) u8g2.drawStr(0, 23, "> 1. Constant Flood"); else u8g2.drawStr(0, 23, "  1. Constant Flood");
    if(modeIndex == 1) u8g2.drawStr(0, 33, "> 2. Cycle Marquee"); else u8g2.drawStr(0, 33, "  2. Cycle Marquee");
    u8g2.drawStr(0, 60, "R: Select | L: Back | C: Exit");
  }
  else if (currentMenu == 3) {
    u8g2.drawStr(0, 10, "Web Config Mode");
    u8g2.drawStr(0, 22, "Connect to Wi-Fi:");
    u8g2.drawStr(0, 32, "SSID: nRFBox_Config");
    u8g2.drawStr(0, 42, "Open: 192.168.4.1");
    u8g2.drawStr(0, 60, "L: Stop/Back | C: Exit");
  }
  else if (currentMenu == 2) {
    u8g2.drawStr(0, 10, "SPAMMING...");
    if (modeIndex == 0) u8g2.drawStr(0, 22, "Mode: Constant Flood");
    if (modeIndex == 1) u8g2.drawStr(0, 22, "Mode: Cycle Marquee");
    
    if (modeIndex == 1) {
       // Display current marquee item
       String* targetList = (payloadIndex == 0) ? rickroll : (payloadIndex == 1) ? panic : customSSIDs;
       u8g2.drawStr(0, 35, targetList[marqueeIndex].c_str());
    }
    
    u8g2.drawStr(0, 60, "L: Stop/Back | C: Exit");
  }
  
  u8g2.sendBuffer();
}

void spammerLoop() {
  unsigned long currentMillis = millis();
  
  drawMenuScreen();
  
  if (currentMenu == 3) {
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  
  if (currentMenu == 2) {
    // SPAMMING LOGIC
    String* targetList = (payloadIndex == 0) ? rickroll : (payloadIndex == 1) ? panic : customSSIDs;
    int targetCount = (payloadIndex == 0) ? 17 : (payloadIndex == 1) ? 6 : customSSIDCount;
    
    if (targetCount == 0) {
      currentMenu = 0; // fallback
      return;
    }
    
    if (modeIndex == 0) {
      // CONSTANT FLOOD
      if (currentMillis - lastSpamTime > 100) { // burst every 100ms
         for(int c = 1; c <= 11; c+=5) { // Channels 1, 6, 11 (common)
           for(int i = 0; i < targetCount; i++) {
              uint8_t mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, (uint8_t)(i & 0xFF)}; // unique MAC per SSID
              buildAndSendBeacon(targetList[i], mac, c);
           }
         }
         lastSpamTime = currentMillis;
         
         static bool flash = false;
         setNeoPixelColour(flash ? "red" : "0");
         flash = !flash;
      }
    } else {
      // CYCLE MARQUEE
      if (currentMillis - marqueeTimer > 2000) { // change string every 2 seconds
         marqueeIndex = (marqueeIndex + 1) % targetCount;
         marqueeTimer = currentMillis;
      }
      
      if (currentMillis - lastSpamTime > 50) { // fast burst of single string
         uint8_t fixedMac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01}; // Keep MAC constant so phone updates text!
         for(int c = 1; c <= 11; c+=5) {
             buildAndSendBeacon(targetList[marqueeIndex], fixedMac, c);
         }
         lastSpamTime = currentMillis;
         
         static bool flash = false;
         setNeoPixelColour(flash ? "purple" : "0");
         flash = !flash;
      }
    }
  }

  // INPUT HANDLING
  if (currentMillis - lastDebounceTime > debounceDelay) {
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      if (currentMenu == 0) payloadIndex = (payloadIndex - 1 + 3) % 3;
      if (currentMenu == 1) modeIndex = (modeIndex - 1 + 2) % 2;
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (currentMenu == 0) payloadIndex = (payloadIndex + 1) % 3;
      if (currentMenu == 1) modeIndex = (modeIndex + 1) % 2;
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BTN_PIN_RIGHT) == LOW) {
      if (currentMenu == 0) {
        if (payloadIndex == 2) {
          // Launch Web Config
          WiFi.mode(WIFI_AP);
          WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
          WiFi.softAP("nRFBox_Config", "toblerone");
          dnsServer.start(DNS_PORT, "*", apIP);
          webServer.on("/", HTTP_GET, handleRoot);
          webServer.on("/save", HTTP_POST, handleSave);
          webServer.onNotFound(handleRoot);
          webServer.begin();
          currentMenu = 3;
        } else {
          currentMenu = 1;
        }
      } else if (currentMenu == 1) {
        // Start Spamming
        WiFi.mode(WIFI_AP); // required for esp_wifi_80211_tx
        WiFi.softAP("nRFBox_Spammer", NULL, 1, 1); // Hidden AP to avoid default "ESP_XXXXXX" SSID
        currentMenu = 2;
        marqueeIndex = 0;
        marqueeTimer = millis();
      }
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BTN_PIN_LEFT) == LOW) {
      if (currentMenu == 1) {
        currentMenu = 0;
      } else if (currentMenu == 2 || currentMenu == 3) {
        // Stop spamming / config
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        setNeoPixelColour("blue");
        currentMenu = 0;
      }
      lastDebounceTime = currentMillis;
    } else if (digitalRead(BUTTON_SELECT_PIN) == LOW) {
      // If we are spamming or in web config, clean up before exiting
      if (currentMenu == 2 || currentMenu == 3) {
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        setNeoPixelColour("0");
      }
      // Note: BUTTON_SELECT_PIN is intentionally not fully consumed/blocked here
      // so that the main nRFBox.ino loop catches the SELECT press to natively exit the module.
    }
  }
}

} // namespace BeaconSpammer

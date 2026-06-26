#include "wifi_setup.h"
#include "display_manager.h"
#include <WiFiManager.h>

#define LED_PIN 2

static WiFiManager wm;
static bool portalRunning = false;
static bool shownConnected = false;
static unsigned long lastBlink = 0;

static const char* portalSsid = "ESP32-Barni";
static const char* portalPass = "12345678";

static void onConfigPortal(WiFiManager *wmPtr) {
  portalRunning = true;

  String msg = String("WiFi Setup Portal\nSSID: ") + portalSsid +
               "\nPassword: " + portalPass;
  drawText(msg.c_str(), DisplayTextMode::CENTER, MultilineMode::HEADER);
}

void wifiBegin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  wm.setConfigPortalTimeout(180);
  wm.setConfigPortalBlocking(false);
  wm.setAPCallback(onConfigPortal);

  drawText("Connecting to WiFi...", DisplayTextMode::CENTER, MultilineMode::NORMAL);

  if (wm.autoConnect(portalSsid, portalPass)) {
    shownConnected = true;
    digitalWrite(LED_PIN, HIGH);

    String msg = "Connected to WiFi " + WiFi.SSID() +
                 "\nIP: " + WiFi.localIP().toString();
    drawText(msg.c_str(), DisplayTextMode::CENTER, MultilineMode::NORMAL);

    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    portalRunning = true;
  }
}

void wifiLoop() {
  wm.process();

  if (portalRunning) {
    if (millis() - lastBlink > 750) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastBlink = millis();
    }

    if (!shownConnected && WiFi.status() == WL_CONNECTED) {
      portalRunning = false;
      shownConnected = true;
      digitalWrite(LED_PIN, HIGH);

      String msg = "Connected to WiFi " + WiFi.SSID() +
                   "\nIP: " + WiFi.localIP().toString();
      drawText(msg.c_str(), DisplayTextMode::CENTER);

      Serial.print("Connected, IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void resetWiFiSettings() {
  wm.resetSettings();
  Serial.println("WiFi settings reset.");
  drawText("Resetting WiFi Settings...", DisplayTextMode::CENTER, MultilineMode::NORMAL);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  ESP.restart();
}
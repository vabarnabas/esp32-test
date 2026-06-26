#include "wifi_setup.h"
#include <WiFiManager.h>

#define LED_PIN 2

static WiFiManager wm;
static bool portalRunning = false;
static unsigned long lastBlink = 0;

void wifiBegin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  wm.setConfigPortalTimeout(180);
  wm.setConfigPortalBlocking(false);

  if (wm.autoConnect("ESP32-Barni", "ViCa1969")) {
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, HIGH);
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
    if (WiFi.status() == WL_CONNECTED) {
      portalRunning = false;
      digitalWrite(LED_PIN, HIGH);
      Serial.print("Connected, IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void resetWiFiSettings() {
  wm.resetSettings();
  Serial.println("WiFi settings reset.");
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  ESP.restart();
}
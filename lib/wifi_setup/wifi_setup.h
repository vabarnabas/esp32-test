#pragma once
#include <Arduino.h>

void wifiBegin();    // call once from setup()
void wifiLoop();     // call every loop() iteration
void resetWiFiSettings(); // call to reset WiFi settings and restart the ESP
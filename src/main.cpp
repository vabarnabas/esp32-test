#include <Arduino.h>

#include "universal_menu.h"
#include "wifi_setup.h"
#include "encoder_manager.h"
#include "display_manager.h"

extern MenuPage rootPage, settingsPage, wifiPage;

static MenuItem wifiItems[] = {
  { "Reset WiFi Settings", nullptr, &resetWiFiSettings},
  { "Back", nullptr, nullptr },
};
MenuPage wifiPage = { "WiFi Settings", wifiItems, 2, &settingsPage };

static MenuItem settingsItems[] = {
  { "WiFi Settings", &wifiPage, nullptr },
  { "Back", nullptr, nullptr },
};
MenuPage settingsPage = { "Settings", settingsItems, 2, &rootPage };

static MenuItem rootItems[] = {
  { "About", nullptr, nullptr },
  { "Settings", &settingsPage, nullptr },
};
MenuPage rootPage = { "Main Menu", rootItems, 2, nullptr };

static void renderSerial(const MenuView& v) {
  Serial.printf("\n== %s ==\n", v.title);
  for (int i = 0; i < v.itemCount; i++)
    Serial.printf("%s %s\n", (i == v.selectedItem ? ">" : " "), v.items[i].title);
}


void setup() {
  Serial.begin(115200);
  displayBegin();
  menuBegin(&rootPage, renderEInk);
  wifiBegin();
  encoderBegin();
}

void loop() {
  wifiLoop();

  int d = encoderTakeDelta();
  while (d > 0) { menuNavigateDown(); d--; };
  while (d < 0) { menuNavigateUp();   d++; };
  if (encoderPressed()) menuSelect();
}


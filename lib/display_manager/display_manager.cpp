#include "display_manager.h"
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define EPD_CS   5
#define EPD_DC   17
#define EPD_RST  16
#define EPD_BUSY 4

GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(
    GxEPD2_290_T94_V2(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

static int partialCount = 99;
static int partialCountMax = 100;

void displayBegin() {
  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
}

static void drawMenu(const MenuView& v) {
  const int TOP = 30, ROW_H = 22;
  int maxRows = (display.height() - TOP) / ROW_H;
  int first = 0;
  if (v.selectedItem >= maxRows) first = v.selectedItem - maxRows + 1;

  display.fillScreen(GxEPD_WHITE);

  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(6, 20);
  display.print(v.title);
  display.drawLine(0, 27, display.width(), 27, GxEPD_BLACK);

  display.setFont(&FreeSansBold9pt7b);
  for (int row = 0; row < maxRows; row++) {
    int i = first + row;
    if (i >= v.itemCount) break;

    int y = TOP + row * ROW_H;
    bool sel = (i == v.selectedItem);

    if (sel) {
      display.fillRect(0, y, display.width(), ROW_H, GxEPD_BLACK);
      display.setTextColor(GxEPD_WHITE);
    } else {
      display.setTextColor(GxEPD_BLACK);
    }

    display.setCursor(8, y + 16);
    display.print(v.items[i].title);
  }
}

void renderEInk(const MenuView& v) {
  bool full = (partialCount >= partialCountMax);
  if (full) {
    display.setFullWindow();
    partialCount = 0;
  } else {
    display.setPartialWindow(0, 0, display.width(), display.height());
    partialCount++;
  }

  display.firstPage();
  do {
    drawMenu(v);
  } while (display.nextPage());

  display.powerOff();
}
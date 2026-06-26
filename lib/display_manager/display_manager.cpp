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

static const GFXfont* getFontForLine(MultilineMode multilineMode, int lineIndex) {
  if (multilineMode == MultilineMode::HEADER && lineIndex == 0) {
    return &FreeSansBold12pt7b;
  }
  return &FreeSansBold9pt7b;
}

void displayBegin() {
  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
}

void drawText(const char* text, DisplayTextMode mode) {
  drawText(text, mode, MultilineMode::NORMAL);
}

void drawText(const char* text, DisplayTextMode mode, MultilineMode multilineMode) {
  const int16_t padX = 8;
  const int16_t padY = 8;
  const int16_t maxWidth = display.width() - padX * 2;

  String input(text ? text : "");
  String lines[20];
  int lineCount = 0;

  int paragraphStart = 0;
  while (paragraphStart <= input.length() && lineCount < 20) {
    int newline = input.indexOf('\n', paragraphStart);
    String paragraph = (newline == -1)
      ? input.substring(paragraphStart)
      : input.substring(paragraphStart, newline);

    int start = 0;
    while (start < paragraph.length() && lineCount < 20) {
      while (start < paragraph.length() && paragraph[start] == ' ') start++;
      if (start >= paragraph.length()) break;

      int end = paragraph.indexOf(' ', start);
      String word = (end == -1) ? paragraph.substring(start) : paragraph.substring(start, end);
      start = (end == -1) ? paragraph.length() : end + 1;

      display.setFont(getFontForLine(multilineMode, lineCount));

      if (lines[lineCount].length() == 0) {
        lines[lineCount] = word;
      } else {
        String candidate = lines[lineCount] + " " + word;

        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(candidate, 0, 0, &x1, &y1, &w, &h);

        if (w <= maxWidth) {
          lines[lineCount] = candidate;
        } else {
          lineCount++;
          if (lineCount < 20) {
            lines[lineCount] = word;
          }
        }
      }
    }

    if (lineCount < 20 && lines[lineCount].length() > 0) {
      lineCount++;
    }

    if (newline == -1) break;
    paragraphStart = newline + 1;
  }

  int16_t lineHeights[20];
  int16_t totalHeight = 0;

  for (int i = 0; i < lineCount; i++) {
    display.setFont(getFontForLine(multilineMode, i));

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(lines[i], 0, 0, &x1, &y1, &w, &h);

    lineHeights[i] = h + 6;
    totalHeight += lineHeights[i];
  }

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextWrap(false);

    if (lineCount > 0) {
      int16_t startY = (mode == DisplayTextMode::CENTER)
        ? padY + ((display.height() - padY * 2 - totalHeight) / 2)
        : padY;

      int16_t currentY = startY;

      for (int i = 0; i < lineCount; i++) {
        display.setFont(getFontForLine(multilineMode, i));

        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(lines[i], 0, 0, &x1, &y1, &w, &h);

        int16_t x = padX;
        if (mode == DisplayTextMode::CENTER) {
          x = padX + ((display.width() - padX * 2 - w) / 2) - x1;
        }

        int16_t y = currentY - y1;

        display.setCursor(x, y);
        display.print(lines[i]);

        currentY += lineHeights[i];
      }
    }

  } while (display.nextPage());

  display.powerOff();
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
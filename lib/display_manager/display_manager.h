#pragma once
#include "universal_menu.h"   // for MenuView

enum class DisplayTextMode {
  BASE,
  CENTER,
};

enum class MultilineMode {
  NORMAL,
  HEADER
};

void displayBegin();
void renderEInk(const MenuView& view);   // matches MenuRenderFn
void drawText(const char* text,
              DisplayTextMode mode,
              MultilineMode multilineMode = MultilineMode::NORMAL);
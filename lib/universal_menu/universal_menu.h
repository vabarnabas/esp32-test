#pragma once
#include <Arduino.h>

struct MenuPage;

struct MenuItem {
  const char* title;
  MenuPage* subMenu;
  void (*action)();
};

struct MenuPage {
  const char* title;
  MenuItem* items;
  int itemCount;
  MenuPage* parent;
};

struct MenuView {
  const char* title;
  MenuItem* items;
  int itemCount;
  int selectedItem;
};

typedef void (*MenuRenderFunction)(const MenuView& view);

void menuBegin(MenuPage* rootPage, MenuRenderFunction renderFunction);

void menuNavigateUp();
void menuNavigateDown();
void menuSelect();

void menuRender();
#include "universal_menu.h"

static MenuPage* currentPage = nullptr;
static int currentItemIndex = 0;
static MenuRenderFunction renderFunction = nullptr;
static bool dirty = true;

static void enterPage(MenuPage* p) {
  currentPage = p;
  currentItemIndex = 0;
  dirty = true;
}

void menuBegin(MenuPage* rootPage, MenuRenderFunction render) {
  currentPage = rootPage;
  currentItemIndex = 0;
  renderFunction = render;
  dirty = true;
}

void menuNavigateUp() {
  if (currentPage && currentItemIndex > 0) {
    currentItemIndex = (currentItemIndex - 1 + currentPage->itemCount) % currentPage->itemCount;
    dirty = true;
  }
}

void menuNavigateDown() {
  if (currentPage && currentItemIndex < currentPage->itemCount - 1) {
    currentItemIndex = (currentItemIndex + 1) % currentPage->itemCount;
    dirty = true;
  }
} 

void menuSelect() {
  if (!currentPage) return;

  MenuItem& it = currentPage->items[currentItemIndex];

  bool isBackItem = (!it.subMenu && !it.action && currentPage->parent != nullptr);
  if (isBackItem) { enterPage(currentPage->parent); return; }
  if (it.subMenu) { enterPage(it.subMenu); return; }
  if (it.action) { it.action(); dirty = true; return; }
}

void menuRender() {
  if (!dirty || !renderFunction || !currentPage) return;

  MenuView view;
  view.title = currentPage->title;
  view.items = currentPage->items;
  view.itemCount = currentPage->itemCount;
  view.selectedItem = currentItemIndex;

  renderFunction(view);
  dirty = false;
}
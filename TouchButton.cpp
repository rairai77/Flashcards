#include "TouchButton.h"

TouchButton::TouchButton(int _x, int _y, int _w, int _h, String _label)
  : x(_x), y(_y), w(_w), h(_h), label(_label), lastPressed(false) {}

void TouchButton::draw(M5GFX &gfx, bool filled) {
  gfx.setTextColor(filled ? TFT_WHITE : TFT_BLACK);
  if (filled) {
    gfx.fillRect(x, y, w, h, TFT_BLACK);
  } else {
    gfx.fillRect(x, y, w, h, TFT_WHITE);
    gfx.drawRect(x, y, w, h, TFT_BLACK);
  }

  gfx.drawCenterString(label, x + w / 2, y + h / 2);
}

bool TouchButton::isTouched(lgfx::touch_point_t *points, int count) {
  for (int i = 0; i < count; i++) {
    if (points[i].x >= x && points[i].x <= x + w && points[i].y >= y && points[i].y <= y + h) {
      if (!lastPressed) {
        lastPressed = true;
        return true;
      }
      return false;
    }
  }
  reset_touch();
  return false;
}

void TouchButton::reset_touch() {
  lastPressed = false;
}
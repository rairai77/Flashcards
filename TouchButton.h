#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#include <M5GFX.h>

class TouchButton {
public:
  int x, y, w, h;
  String label;
  bool lastPressed;

  TouchButton(int _x, int _y, int _w, int _h, String _label);

  void draw(M5GFX &gfx, bool filled = false);
  bool isTouched(lgfx::touch_point_t *points, int count);
  void reset_touch();
};

#endif // TOUCH_BUTTON_H
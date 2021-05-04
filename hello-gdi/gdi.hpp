#ifndef GDI_H
#define GDI_H

#include <windows.h>

namespace gdi {

  COLORREF DefaultColor = RGB(128, 128, 128);
  HPEN DefaultPen = nullptr;

  int pen(int style = PS_SOLID, int width = 1, COLORREF color = DefaultColor) {
    if (DefaultPen) DeleteObject(DefaultPen);
    // Pen style can be: PS_SOLID PS_DASH PS_DOT PS_DASHDOT PS_DASHDOTDOT PS_NULL PS_INSIDEFRAME
    DefaultPen = CreatePen(style, width, color);
    return 0;
  }

  inline COLORREF background(HDC hdc, COLORREF color = TRANSPARENT) {
    // If the function succeeds, the return value specifies the previous background color as a COLORREF value.
    // If the function fails, the return value is CLR_INVALID.
    return SetBkColor(hdc, color);
  }

  int stroke(HDC hdc, int x1, int y1, int x2, int y2, HPEN pen = DefaultPen) {
    SelectObject(hdc, pen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    return 0;
  }

  int text(HDC hdc, const char *text, int x, int y, COLORREF color = DefaultColor) {
    RECT r = { x, y, 0, 0 };
    SetTextColor(hdc, color);
    DrawText(hdc, text, -1, &r, DT_SINGLELINE | DT_NOCLIP);
    return 0;
  }

  // TODO: custom brushes
  int rect(HDC hdc, int x, int y, int w, int h) {
    SelectObject(hdc, GetStockObject(BLACK_BRUSH)); // fill
    SelectObject(hdc, GetStockObject(BLACK_PEN)); // stroke
    Rectangle(hdc, x, y, w, h);
    return 0;
  }

  int init() {
    DefaultPen = CreatePen(PS_SOLID, 1, DefaultColor);
    return 0;
  }

  int deinit() {
    if (DefaultPen) DeleteObject(DefaultPen);
    return 0;
  }
}

#endif // GDI_H

// I ended up the project that i started 3 year ago as restoring one of my oldest programs - pseudo-3d turn-based shooter
// with ascii-graphics rendering in windows promt console.

#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "gdi.hpp"
#include "game.hpp"

UINT GlobalTimerId = 0;
UINT GlobalTimerDelay = 50;
int CoordX = 500; // CW_USEDEFAULT;
int CoordY = 300; // CW_USEDEFAULT;
int Width = 400;
int Height = 300;
LPCTSTR Title = "Walls v0.20200614a";


inline int ondestroy(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  gdi::deinit();
  PostQuitMessage(0);
  return 0;
}


inline int onresize(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  Width = LOWORD(lparam);
  Height = HIWORD(lparam);
  return 0;
}


inline int ontimer(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  InvalidateRect(hwnd, NULL, FALSE); // forces onpaint calling
  return 0;
}


inline int onpaint(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);
  // Double buffering.
  HDC cdc = CreateCompatibleDC(hdc);
  HBITMAP bmp = CreateCompatibleBitmap(hdc, Width, Height);
  SelectObject(cdc, bmp);

  // Draw.
  gdi::background(cdc);

  game::hdc = cdc;
  game::demo();

  char text [50];
  sprintf(text, "%d", game::dir);
  gdi::text(cdc, text, 10, 10, RGB(255, 255, 255));

  // Completes drawing by copying bitmap from buffer into display.
  BitBlt(hdc, 0, 0, Width, Height, cdc, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
  // Frees memory.
  DeleteObject(bmp);
  DeleteDC(cdc);
  DeleteDC(hdc);
  return 0;
}

// see: https://docs.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
inline int onkeydown(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  if (VK_ESCAPE == wparam) {
    DestroyWindow(hwnd);
  }
  return 0;
}


inline int onkeyup(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  return 0;
}



LRESULT APIENTRY wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  switch (message) {
  case WM_DESTROY: return ondestroy(hwnd, message, wparam, lparam);
  case WM_SIZE: return onresize(hwnd, message, wparam, lparam);
  case WM_TIMER: return ontimer(hwnd, message, wparam, lparam);
  case WM_PAINT: return onpaint(hwnd, message, wparam, lparam);
  case WM_KEYDOWN: onkeydown(hwnd, message, wparam, lparam); break;
  case WM_KEYUP: onkeyup(hwnd, message, wparam, lparam); break;
  }
  return DefWindowProc(hwnd, message, wparam, lparam);
}

int main(int argc, const char* argv[]) { // int WinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
  WNDCLASSEX wc;
  wc.cbSize        = sizeof(wc);
  wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = wndproc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "AppClass";
  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
  RegisterClassEx(&wc);

  // HWND and HDC === HANDLE === PVOID === void*
  HWND hwnd = CreateWindow("AppClass", Title, WS_OVERLAPPEDWINDOW, CoordX, CoordY, Width, Height,
                          (HWND)NULL, (HMENU)NULL, (HINSTANCE)wc.hInstance, NULL);
  HDC hdc = GetDC(hwnd);

  SetTimer(hwnd, GlobalTimerId = 1, GlobalTimerDelay, NULL);
  // KillTimer(hwnd, GlobalTimerId);




  game::init();

  ShowWindow(hwnd, true);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }

  return msg.wParam;
}

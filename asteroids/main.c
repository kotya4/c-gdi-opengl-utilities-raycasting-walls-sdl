#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#include "asteroids.h"
#include "rocket.h"


UINT WINDOW_WIDTH  = 400;
UINT WINDOW_HEIGHT = 400;
UINT WINDOW_REFRESH_TIMER_DELAY = 100; // in milliseconds
UINT WINDOW_REFRESH_TIMER_ID    = 1;   // must be not zero
BOOL WINDOW_KEYBOARD[256];

int on_create();
int on_paint(HDC hdc);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
  const wchar_t CLASS_NAME[]    = L"Asteroids Window Class";
  const wchar_t WINDOW_TITLE[]  = L"Asteroids The Game";

  WNDCLASS wc = { };
  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = CLASS_NAME;
  RegisterClass(&wc);

  // Create the window.

  HWND hwnd = CreateWindowEx(
    0,                              // Optional window styles.
    CLASS_NAME,                     // Window class
    WINDOW_TITLE,                   // Window text
    WS_OVERLAPPEDWINDOW,            // Window style
    CW_USEDEFAULT,                  // X
    CW_USEDEFAULT,                  // Y
    (int)WINDOW_WIDTH,              // Width
    (int)WINDOW_HEIGHT,             // Height
    NULL,                           // Parent window
    NULL,                           // Menu
    hInstance,                      // Instance handle
    NULL                            // Additional application data
  );

  if (hwnd == NULL) {
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  // Run the message loop.

  MSG msg = { };
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_SIZE:
    WINDOW_WIDTH  = LOWORD(lParam);
    WINDOW_HEIGHT = HIWORD(lParam);
    return 0;

  case WM_SETFOCUS:
    SetTimer(hwnd, WINDOW_REFRESH_TIMER_ID, WINDOW_REFRESH_TIMER_DELAY, NULL);
    return 0;

  case WM_KILLFOCUS:
    KillTimer(hwnd, WINDOW_REFRESH_TIMER_ID);
    return 0;

  case WM_CREATE:
    on_create();
    return 0;

  case WM_TIMER:
    {
      if(wParam == WINDOW_REFRESH_TIMER_ID) {
        InvalidateRect(hwnd, NULL, FALSE);
      }
    }
    return 0;

  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      HBRUSH bgbrush = CreateSolidBrush((COLORREF)0x006b4f36);
      FillRect(hdc, &ps.rcPaint, bgbrush);
      DeleteObject(bgbrush);
      on_paint(hdc);
      EndPaint(hwnd, &ps);
    }
    return 0;

  case WM_KEYDOWN:
    WINDOW_KEYBOARD[wParam] = TRUE;
    {
      if (wParam == VK_ESCAPE) {
        DestroyWindow(hwnd);
      }
    }
    return 0;

  case WM_KEYUP:
    WINDOW_KEYBOARD[wParam] = FALSE;
    return 0;

  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



int on_create() {

  srand(time(NULL));

  init_rocket(WINDOW_WIDTH, WINDOW_HEIGHT);

  for (int i = 0; i < ASTEROIDS_NUM; ++i) {
    init_asteroid(&asteroids[i], WINDOW_WIDTH, WINDOW_HEIGHT);
  }
}


int on_paint(HDC hdc) {
  HPEN linepen = CreatePen(PS_SOLID, 1, (COLORREF)0x008551fc);
  HPEN oldpen  = (HPEN)SelectObject(hdc, linepen);


  // draw rocket (center)
  MoveToEx(
    hdc,
    (int)(rocket.x + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 0]),
    (int)(rocket.y + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 1]),
    NULL);
  for (int i = 0; i < ROCKET_EDGES_NUM; ++i) {
    LineTo(
      hdc,
      (int)(rocket.x + rocket.edges[(i << 1) + 0]),
      (int)(rocket.y + rocket.edges[(i << 1) + 1]));
  }
  // draw rocket (horisontal)
  const int roh = (rocket.x < (WINDOW_WIDTH >> 1)) ? +WINDOW_WIDTH : -WINDOW_WIDTH;
  MoveToEx(
    hdc,
    (int)(rocket.x + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 0] + roh),
    (int)(rocket.y + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 1]),
    NULL);
  for (int i = 0; i < ROCKET_EDGES_NUM; ++i) {
    LineTo(
      hdc,
      (int)(rocket.x + rocket.edges[(i << 1) + 0] + roh),
      (int)(rocket.y + rocket.edges[(i << 1) + 1]));
  }
  // draw rocket (vertical)
  const int rov = (rocket.y < (WINDOW_HEIGHT >> 1)) ? +WINDOW_HEIGHT : -WINDOW_HEIGHT;
  MoveToEx(
    hdc,
    (int)(rocket.x + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 0]),
    (int)(rocket.y + rocket.edges[(ROCKET_EDGES_NUM - 1 << 1) + 1] + rov),
    NULL);
  for (int i = 0; i < ROCKET_EDGES_NUM; ++i) {
    LineTo(
      hdc,
      (int)(rocket.x + rocket.edges[(i << 1) + 0]),
      (int)(rocket.y + rocket.edges[(i << 1) + 1] + rov));
  }

  float rocket_rot_dx = 0;
  float rocket_rot_dy = 0;
  if (WINDOW_KEYBOARD['W']) rocket_rot_dy = -1;
  if (WINDOW_KEYBOARD['S']) rocket_rot_dy = +1;
  if (WINDOW_KEYBOARD['A']) rocket_rot_dx = -1;
  if (WINDOW_KEYBOARD['D']) rocket_rot_dx = +1;
  if (rocket_rot_dx || rocket_rot_dy) rotate_rocket(rocket_rot_dx, rocket_rot_dy);

  float rocket_mov_speed = -1;
  if (WINDOW_KEYBOARD['Q']) rocket_mov_speed = +1;
  move_rocket(rocket_mov_speed, WINDOW_WIDTH, WINDOW_HEIGHT);


  for (int i = 0; i < ASTEROIDS_NUM; ++i) {
    asteroid_t *a = &asteroids[i];

    // if ((*a).is_dead) continue;

    // draw asteroid
    MoveToEx(
      hdc,
      (int)((*a).x + (*a).edges[(ASTEROID_EDGES_NUM - 1 << 1) + 0]),
      (int)((*a).y + (*a).edges[(ASTEROID_EDGES_NUM - 1 << 1) + 1]),
      NULL);
    for (int i = 0; i < ASTEROID_EDGES_NUM; ++i) {
      LineTo(
        hdc,
        (int)((*a).x + (*a).edges[(i << 1) + 0]),
        (int)((*a).y + (*a).edges[(i << 1) + 1]));
    }
    // DEBUG: line to center
    // LineTo(hdc, WINDOW_WIDTH >> 1, WINDOW_HEIGHT >> 1);


    proc_border_collision(i, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!out_of_border(i, WINDOW_WIDTH, WINDOW_HEIGHT)) {
      proc_asteroids_collision(i);
    }


    // move and rotate asteroid
    asteroids[i].x += asteroids[i].velx;
    asteroids[i].y += asteroids[i].vely;
    rotate_asteroid(&asteroids[i]);



  }


  SelectObject(hdc, oldpen);
  DeleteObject(linepen);

  return 0;
}

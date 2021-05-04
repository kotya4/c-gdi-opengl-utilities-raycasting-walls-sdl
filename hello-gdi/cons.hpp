#ifndef CONS_H
#define CONS_H

#include <windows.h>

namespace cons {

  inline void gotoxy(int x, int y) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){ x, y });
  }

  inline void color(int value) { // value === 0x[BACKGROUND:4bits][TEXT:4bits]
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), value);
  }

  inline void wait(int msec) {
    Sleep(msec);
  }

  inline void cls() {
    // TIP: Call "cls" only once before render cicle,
    //      for per-frame clearing use "gotoxy(0,0)".
    system("cls");
  }

}

#endif // CONS_H
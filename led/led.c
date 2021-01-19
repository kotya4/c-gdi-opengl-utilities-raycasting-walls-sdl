#define WINVER 0x0500

#include <windows.h>
#include <stdio.h>

int main() {
  printf("3 bit LED counter\n");

  // virtual key codes, source: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
  const int vkkeys [3] = { VK_SCROLL, VK_CAPITAL, VK_NUMLOCK };
   // hardware scan codes, source: http://www.philipstorr.id.au/pcbook/book3/scancode.htm (see table, not image above)
  const int hckeys [3] = { 0x46, 0x3a, 0x45 };

  int counter = 0;

  // inits counter according to initial keys states.
  for (int i = 0; i < 3; ++i) {
    if (GetKeyState(vkkeys[i]) & 1) { // reset key
      keybd_event(vkkeys[i], hckeys[i], KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(vkkeys[i], hckeys[i], KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    counter |= (GetKeyState(vkkeys[i]) & 1) << i;
  }

  // main loop
  while (1) {
    for (int i = 0; i < 3; ++i) {
      if ((counter >> i & 1) ^ (GetKeyState(vkkeys[i]) & 1)) {
        keybd_event(vkkeys[i], hckeys[i], KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event(vkkeys[i], hckeys[i], KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      }
    }
    Sleep(200);
    ++counter;
  }

  return 0;
}

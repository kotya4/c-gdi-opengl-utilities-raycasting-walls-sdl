#define _WIN32_WINNT 0x500
#include <windows.h>
#include <string.h>
#include <stdio.h>

typedef struct _CONSOLE_FONT_INFOEX {
    ULONG cbSize;
    DWORD nFont;
    COORD dwFontSize;
    UINT  FontFamily;
    UINT  FontWeight;
    WCHAR FaceName[LF_FACESIZE];
} CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;

#ifdef __cplusplus
extern "C" {
#endif
BOOL WINAPI SetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx);
#ifdef __cplusplus
}
#endif

// #ifdef __cplusplus
// extern "C" {
// #endif
// BOOL WINAPI GetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX lpConsoleCurrentFont);
// #ifdef __cplusplus
// }
// #endif

void change_font(const WCHAR *facename, int fontsize) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize       = sizeof cfi;
    cfi.nFont        = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = fontsize;
    cfi.FontFamily   = FF_DONTCARE;
    cfi.FontWeight   = FW_NORMAL;
    wcscpy(cfi.FaceName, facename);
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

int is_console_own() {
    HWND consoleWnd = GetConsoleWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId(consoleWnd, &dwProcessId);
    return (GetCurrentProcessId() == dwProcessId);
}

int main() {
    if (is_console_own()) {
        printf("Program runs in its own terminal process.\n");
    } else {
        printf("Program runs in parent terminal process.\n");
    }
    printf("Press any key to continue...\n");
    getchar();

    change_font(L"Consolas", 12);
    printf("Terminal font changed to Consolas 12px.\n");
    printf("Press any key to continue...\n");
    getchar();

    change_font(L"Lucida Console", 12);
    printf("Terminal font changed to Lucida Console 12px.\n");

    return 0;
}

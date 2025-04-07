#include <windows.h>
#include <stdio.h>

#include "cursor.h"

int saveCursor(CursorPos* pos) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Handle to the console
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // Get cursor position from the console screen buffer info
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        pos->row = csbi.dwCursorPosition.Y + 1;
        pos->col = csbi.dwCursorPosition.X + 1;
        return 0;
    }
    else {
        // Set default values and throw error if failure
        pos->row = -1;
        pos->col = -1;
        return 1;
    }
}

void moveCursor(const CursorPos* pos) {
    printf("\033[%d;%dH", pos->row, pos->col);
    fflush(stdout);
}
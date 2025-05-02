#pragma once

typedef struct {
    int row;
    int col;
} CursorPos;

int saveCursor(CursorPos* pos);

void moveCursor(const CursorPos pos);

CursorPos createCursor(const int row, const int col);

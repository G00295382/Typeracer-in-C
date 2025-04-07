#pragma once

typedef struct {
    int row;
    int col;
} CursorPos;

int saveCursor(CursorPos* pos);

void moveCursor(const CursorPos* pos);
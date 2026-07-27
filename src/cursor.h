#include <libgpu.h>
#ifndef CURSOR_H
#define CURSOR_H

typedef struct Cursor {
    TIM_IMAGE *sprite;
    POLY_FT4  poly;

    int x, y;
    int width, height;
} Cursor;

void CursorInit(Cursor* cursor, TIM_IMAGE* spritesheet);
void CursorSetPos(Cursor* cursor, int x, int y);

#endif
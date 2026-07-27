#include "cursor.h"

void CursorInit(Cursor* cursor, TIM_IMAGE* sprite)
{
    int mode = sprite->mode & 0x3;

    cursor->sprite = sprite;
    cursor->width = sprite->prect->w;
    cursor->height = sprite->prect->h;

    // bit depth shenanigans
    if (mode == 0)
        cursor->width *= 4;
    else if (mode == 1)
        cursor->width *= 2;

    SetPolyFT4(&cursor->poly);
    cursor->poly.tpage = GetTPage(mode, 0, sprite->prect->x, sprite->prect->y);
    cursor->poly.clut = GetClut(sprite->crect->x, sprite->crect->y);

    setUV4(&cursor->poly, 0, 0, cursor->width, 0, 0, cursor->height, cursor->width, cursor->height);
    setRGB0(&cursor->poly, 255, 255, 255);

    CursorSetPos(cursor, 160, 120);
}

void CursorSetPos(Cursor* cursor, int x, int y)
{
    cursor->x = x;
    cursor->y = y;

    setXY4(&cursor->poly,
           cursor->x, cursor->y,
           cursor->x + cursor->width, cursor->y,
           cursor->x, cursor->y + cursor->height,
           cursor->x + cursor->width, cursor->y + cursor->height);
}
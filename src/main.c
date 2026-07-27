#define _FNTPRINT_
#define _KANJIFNTPRINT_
#include <libetc.h>
#include <libgpu.h>
#include <libcd.h>
#include <libgte.h>
#include <stdlib.h>
#include <stdio.h>
#include "pet.h"
#include "neko.h"
#include "cursor.h"
#include "joypad.h"

#define CURSOR_SPEED 3

extern char __heap_start, __sp;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define OTSIZE 4096
#define SCREEN_Z 512
#define CUBESIZE 196

extern int FntPrint(const char *fmt, ...);

char *FileRead(char *filename, u_long *length)
{
    CdlFILE filepos;
    int numsectors;
    char *buffer;

    buffer = NULL;

    if (CdSearchFile(&filepos, filename) == NULL)
    {
        printf("%s file not found in the CD.\n", filename);
        *length = 0;
        return NULL;
    }

    printf("Found %s in the CD.\n", filename);
    numsectors = (filepos.size + 2047) / 2048;   // compute the number of sectors to read from the file
    buffer = (char *)malloc3(2048 * numsectors); // allocate buffer for the file
    if (!buffer)
    {
        printf("Error allocating %d sectors!\n", numsectors);
        *length = 0;
        return NULL;
    }

    CdControl(CdlSetloc, (u_char *)&filepos.pos, 0);    // set read target to the file
    CdRead(numsectors, (u_long *)buffer, 0); // start reading from the CD (normal speed)
    CdReadSync(0, 0);                                   // wait until the read is complete

    *length = filepos.size;

    return buffer;
}

typedef struct DrawBuffer
{
    DRAWENV draw;
    DISPENV disp;
    u_long ot[OTSIZE];
    POLY_FT4 t;
} DrawBuffer;


void LoadTIM(char *filename, TIM_IMAGE *tim)
{
    printf("Loading TIM file: %s\n", filename);
    u_long *bytes;
    u_long length = 0;

    bytes = (u_long *)FileRead(filename, &length);

    if (bytes == NULL)
    {
        printf("Error reading %s from the CD.\n", filename);
        return;
    }

    if (length >= 4)
    {
        printf("tim magic number: 0x%08X\n", bytes[0]);
        printf("4 more bytes: 0x%08X\n", bytes[1]);
    }

    OpenTIM(bytes);
    ReadTIM(tim);

    LoadImage(tim->prect, tim->paddr);
    if (tim->mode & 0x8)
        LoadImage(tim->crect, tim->caddr);  // remember to copy clut!!

    DrawSync(0);
}

int main(void)
{
    InitHeap3((unsigned long *)(&__heap_start), (&__sp - 0x5000) - &__heap_start);
    static DrawBuffer drawBuffer[2];
    DrawBuffer *currentDrawBuffer;
    SVECTOR rotation = {0};
    VECTOR translation = {0, 0, (SCREEN_Z * 3) / 2, 0};
    MATRIX transform;
    size_t i;
    u_char *bytes;
    TIM_IMAGE neko_tim, cursor_tim;

    int timer;

    ResetGraph(0);
    InitGeom();
    SetGraphDebug(0);

    FntLoad(960, 256);
    SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));

    SetGeomOffset(160, 120);
    SetGeomScreen(SCREEN_Z);

    SetDefDrawEnv(&drawBuffer[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&drawBuffer[0].disp, 0, 240, 320, 240);

    SetDefDrawEnv(&drawBuffer[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&drawBuffer[1].disp, 0, 0, 320, 240);

    srand(0);

    currentDrawBuffer = &drawBuffer[0];

    PutDrawEnv(&drawBuffer[0].draw);
    PutDispEnv(&drawBuffer[1].disp);
    SetDispMask(1);

    CdInit();
    printf("Loading sprites...");
    LoadTIM("\\NEKO.TIM;1", &neko_tim);
    LoadTIM("\\CURSOR.TIM;1", &cursor_tim);


    Neko neko;
    NekoInit(&neko, &neko_tim);

    Cursor cursor;
    CursorInit(&cursor, &cursor_tim);

    JoyPadInit();

    timer = 0;

    while (1)
    {
        currentDrawBuffer = (currentDrawBuffer == &drawBuffer[0]) ? &drawBuffer[1] : &drawBuffer[0];

        PutDrawEnv(&currentDrawBuffer->draw);
        PutDispEnv(&currentDrawBuffer->disp);

        JoyPadUpdate();

        int cursorNewX = cursor.x;
        int cursorNewY = cursor.y;
        if (JoyPadCheck(PAD1_UP))    cursorNewY -= CURSOR_SPEED;
        if (JoyPadCheck(PAD1_DOWN))  cursorNewY += CURSOR_SPEED;
        if (JoyPadCheck(PAD1_LEFT))  cursorNewX -= CURSOR_SPEED;
        if (JoyPadCheck(PAD1_RIGHT)) cursorNewX += CURSOR_SPEED;

        if (cursorNewX < 0) cursorNewX = 0;
        if (cursorNewY < 0) cursorNewY = 0;
        if (cursorNewX > 320 - cursor.width) cursorNewX = 320 - cursor.width;
        if (cursorNewY > 240 - cursor.height) cursorNewY = 240 - cursor.height;

        CursorSetPos(&cursor, cursorNewX, cursorNewY);

        timer++;
        if(timer >= 10)
        {
            timer = 0;
            printf("Updating Neko!");
            NekoUpdate(&neko, cursor.x, cursor.y);
        }

        rotation.vy += 16;
        rotation.vz += 16;

        RotMatrix(&rotation, &transform);
        TransMatrix(&transform, &translation);

        ClearOTagR(currentDrawBuffer->ot, OTSIZE);

        AddPrim(&currentDrawBuffer->ot[1], &neko.pet.poly);
        AddPrim(&currentDrawBuffer->ot[0], &cursor.poly);

        printf("STATE: %s", NekoGetStateName(neko.state));

        DrawSync(0);
        VSync(0);

        ClearImage(&currentDrawBuffer->draw.clip, 0, 128, 128);

        DrawOTag(&currentDrawBuffer->ot[OTSIZE - 1]);
        FntFlush(-1);
    }

    return 0;
}

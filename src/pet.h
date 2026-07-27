#ifndef PET_H
#define PET_H

#include "libgpu.h"
#include "utils.h"

#define SHEET_ROWS 5
#define SHEET_COLS 8
#define SHEET_SPR_SIZE 32

typedef struct BoundingBox
{
    int left;
    int top;
    int right;
    int bottom;
} BoundingBox;

typedef struct Pet {
    TIM_IMAGE *spritesheet;
    POLY_FT4  poly;

    int x, y;
    int old_x, old_y;
    int scale;
    BoundingBox boundingBox;   // this is actually the screen size

    int lastImage;
} Pet;

void PetInit(Pet* pet, TIM_IMAGE* spritesheet);

void PetMoveTo(Pet* pet, int x, int y);
void PetSetImage(Pet* pet, int image);
void PetSetScale(Pet* pet, int scale);
void PetSetImageAndMoveTo(Pet* pet, int image, int x, int y);

#endif
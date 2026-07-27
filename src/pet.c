#include "pet.h"

#include <stdio.h>

void PetInit(Pet* pet, TIM_IMAGE* spritesheet)
{
    // initialize primitive
    SetPolyFT4(&pet->poly);
    pet->poly.tpage = GetTPage(spritesheet->mode & 0x3, 0, spritesheet->prect->x, spritesheet->prect->y);
    pet->poly.clut = GetClut(spritesheet->crect->x, spritesheet->crect->y);

    //set initial and old position
    pet->x = 20;
    pet->y = 20;
    pet->old_x = -1;
    pet->old_y = -1;
    pet->scale = 1;
    
    // set poly position and color
    setXY4(&pet->poly,
           pet->x, pet->y,
           pet->x + SHEET_SPR_SIZE, pet->y,
           pet->x, pet->y + SHEET_SPR_SIZE,
           pet->x + SHEET_SPR_SIZE, pet->y + SHEET_SPR_SIZE);
    setRGB0(&pet->poly, 255, 255, 255);
    
    pet->lastImage = -1;
    PetSetImage(pet, 0);
    
    pet->boundingBox.left = 0;
    pet->boundingBox.top = 0;
    pet->boundingBox.right = 320;
    pet->boundingBox.bottom = 240;
}

void PetMoveTo(Pet* pet, int x, int y)
{
    if(pet->old_x == -1)
    {
        pet->old_x = x;
        pet->old_y = y;
    } else {
        pet->old_x = pet->x;
        pet->old_y = pet->y;
    }
    pet->x = x;
    pet->y = y;

    setXY4(&pet->poly,
           pet->x, pet->y,
           pet->x + SHEET_SPR_SIZE, pet->y,
           pet->x, pet->y + SHEET_SPR_SIZE,
           pet->x + SHEET_SPR_SIZE, pet->y + SHEET_SPR_SIZE);
}

void PetSetImage(Pet* pet, int image)
{
    if(image != pet->lastImage)
    {
        int x = INDEX_1D_TO_COL(image, SHEET_COLS) * SHEET_SPR_SIZE;
        int y = INDEX_1D_TO_ROW(image, SHEET_COLS) * SHEET_SPR_SIZE;
        // u1/v1 are stored as u_char (0-255): clamp instead of letting the
        // last column/row (e.g. x+32 == 256) overflow and wrap to 0.
        int u1 = x + SHEET_SPR_SIZE > 255 ? 255 : x + SHEET_SPR_SIZE;
        int v1 = y + SHEET_SPR_SIZE > 255 ? 255 : y + SHEET_SPR_SIZE;
        setUV4(&pet->poly, x, y, u1, y, x, v1, u1, v1);
        pet->lastImage = image;
    }
}

void PetSetScale(Pet* pet, int scale)
{
    if(scale != 0)
        pet->scale = scale;
    else
        pet->scale = 0;
}

void PetSetImageAndMoveTo(Pet* pet, int image, int x, int y)
{
    PetSetImage(pet, image);
    PetMoveTo(pet, x, y);
}
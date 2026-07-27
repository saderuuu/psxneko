#ifndef NEKO_H
#define NEKO_H

#include <stdlib.h>
#include <stdio.h>

#include "libgpu.h"
#include "pet.h"

// neko states
// had to rename because DR_MOVE is used by psx libgpu
enum State
{
    STATE_STOP = 0,
    STATE_WASH = 1,
    STATE_SCRATCH = 2,
    STATE_YAWN = 3,
    STATE_SLEEP = 4,
    STATE_AWAKE = 5,
    STATE_U_MOVE = 6,
    STATE_D_MOVE = 7,
    STATE_L_MOVE = 8,
    STATE_R_MOVE = 9,
    STATE_UL_MOVE = 10,
    STATE_UR_MOVE = 11,
    STATE_DL_MOVE = 12,
    STATE_DR_MOVE = 13,
    STATE_U_CLAW = 14,
    STATE_D_CLAW = 15,
    STATE_L_CLAW = 16,
    STATE_R_CLAW = 17
};

#define NEKO_NUM_ANIM_FRAMES 32
#define NEKO_NUM_FOOT_FRAMES 8

// array containing animation frames
static int animation[][2] = {
    {28, 28},  /* NEKO_STOP */     
    {25, 28},  /* NEKO_WASH */     
    {26, 27},  /* NEKO_SCRATCH */  
    {29, 29},  /* NEKO_YAWN */     
    {30, 31},  /* NEKO_SLEEP */    
    {0,   0},  /* NEKO_AWAKE */    
    {1,   2},  /* NEKO_U_MOVE */   
    {9,  10},  /* NEKO_D_MOVE */   
    {13, 14},  /* NEKO_L_MOVE */   
    {5,   6},  /* NEKO_R_MOVE */   
    {15, 16},  /* NEKO_UL_MOVE */  
    {3,   4},  /* NEKO_UR_MOVE */  
    {11, 12},  /* NEKO_DL_MOVE */  
    {7,   8},  /* NEKO_DR_MOVE */  
    {17, 18},  /* NEKO_U_CLAW */   
    {23, 24},  /* NEKO_D_CLAW */   
    {21, 22},  /* NEKO_L_CLAW */   
    {19, 20}   /* NEKO_R_CLAW */   
};

// neko state times
#define NEKO_STOP_TIME      4
#define NEKO_WASH_TIME      10
#define NEKO_SCRATCH_TIME   4
#define NEKO_YAWN_TIME      3
#define NEKO_AWAKE_TIME     3
#define NEKO_CLAW_TIME      10
#define NEKO_MAX_NAME       64

// neko state name
#define CHASE_MOUSE             0
#define RUN_AWAY_FROM_MOUSE     1
#define RUN_AROUND_RANDOMLY     2
#define PACE_AROUND_SCREEN      3
#define RUN_AROUND              4

#define MAX_TICK 9999

typedef struct Neko {
    Pet pet;

    //position info
    int dx, dy;
    int oldToX, oldToY;
    int toX, toY;

    // size info
    SVECTOR m_dwScale;
    uint tickCount, stateCount;
    enum State state;

    //action info
    int action;
    int actionCount;
    int actionX, actionY, actionDX, actionDY;

    //attributes
    int speed;
    int idleSpace;
    char m_szLibname[255];
    char m_szFootprintLibname[255];
    int footprints;

    char m_szName[NEKO_MAX_NAME];
} Neko;

void NekoInit(Neko *neko, TIM_IMAGE* tim);
void NekoSetState(Neko *neko, enum State state);
bool NekoMoveStart(Neko *neko);
void NekoCalcDirection(Neko *neko);
void NekoRunTowards(Neko *neko, int x, int y);
int NekoGetStateAnimationFrameIndex(Neko* neko);
void NekoUpdate(Neko* neko, int mouseX, int mouseY);
const char* NekoGetStateName(int nState);

/*TODO: */

	//calculate new sleep time
	//g_dwSleepTime = 100;
	//if( g_nNumCats <= 20 ) g_dwSleepTime = 200 - ( 5 * g_nNumCats );

#endif
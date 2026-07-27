#include "neko.h"

void NekoInit(Neko *neko, TIM_IMAGE* tim)
{
    PetInit(&neko->pet, tim);

    neko->dx = 0;
    neko->dy = 0;
    neko->speed = 16;
    neko->idleSpace = 6;
    neko->action = CHASE_MOUSE;
    neko->actionCount = 0;
    neko->footprints = 0;

    NekoSetState(neko, STATE_STOP);

    // action starting point
    neko->actionX = neko->pet.boundingBox.left + (rand() % (neko->pet.boundingBox.right - (neko->speed * 8)));
    neko->actionY = neko->pet.boundingBox.top + (rand() % (neko->pet.boundingBox.bottom - (neko->speed * 8)));

    // action end point
    neko->actionDX = (((rand() % 2) ? 1 : -1) * (neko->speed / 2)) + 1;
    neko->actionDY = (((rand() % 2) ? 1 : -1) * (neko->speed / 2)) + 1;

    // remember to initialize rest of stuff!!
    neko->toX = neko->oldToX = neko->pet.x;
    neko->toY = neko->oldToY = neko->pet.y;
}

bool NekoMoveStart(Neko *neko)
{
    return (!((neko->oldToX >= neko->toX - (int)neko->idleSpace) &&
              (neko->oldToX <= neko->toX + (int)neko->idleSpace) &&
              (neko->oldToY >= neko->toY - (int)neko->idleSpace) &&
              (neko->oldToY <= neko->toY + (int)neko->idleSpace)));
}

void NekoCalcDirection(Neko *neko)
{
    enum State newState;
    long angle;

    if (neko->dx == 0 && (neko->dy == 0))
    {
        newState = STATE_STOP;
    }
    else
    {
        // evil fixed point hacking
        angle = ratan2((long)(-neko->dy), (long)neko->dx); 

        if (angle > 1792 || angle <= -1792)
            newState = STATE_L_MOVE;
        else if (angle > 1280)
            newState = STATE_UL_MOVE;
        else if (angle > 768)
            newState = STATE_U_MOVE;
        else if (angle > 256)
            newState = STATE_UR_MOVE;
        else if (angle > -256)
            newState = STATE_R_MOVE;
        else if (angle > -768)
            newState = STATE_DR_MOVE;
        else if (angle > -1280)
            newState = STATE_D_MOVE;
        else
            newState = STATE_DL_MOVE;
    }

    if (neko->state != newState)
        NekoSetState(neko, newState);
}

void NekoRunTowards(Neko *neko, int x, int y)
{
    neko->oldToX = neko->toX;
    neko->oldToY = neko->toY;

    neko->toX = x;
    neko->toY = y;

    int largeX, largeY, doubleLength;
    long length;
    largeX = neko->toX - neko->pet.x - SHEET_SPR_SIZE >> 2;
    largeY = neko->toY - neko->pet.y - SHEET_SPR_SIZE + 1;
    doubleLength = largeX * largeX + largeY * largeY;

    if (doubleLength != 0)
    {
        length = SquareRoot0(doubleLength);

        if (length <= (long)neko->speed)
        {
            neko->dx = largeX;
            neko->dy = largeY;
        }
        else
        {
            neko->dx = ((long)neko->speed * largeX) / length;
            neko->dy = ((long)neko->speed * largeY) / length;
        }
    }
    else
    {
        neko->dx = 0;
        neko->dy = 0;
    }

    neko->tickCount++;
    if (neko->tickCount >= MAX_TICK)
    {
        neko->tickCount = 0;
    }

    if (neko->tickCount % 2 == 0)
    {
        if (neko->stateCount < MAX_TICK)
        {
            neko->stateCount++;
        }
    }

    switch (neko->state)
    {
    case STATE_STOP:
        if (NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        else if (neko->stateCount >= NEKO_STOP_TIME)
        {
            if (neko->dx < 0 && neko->pet.x <= 0)
            {
                NekoSetState(neko, STATE_L_CLAW);
            }
            else if (neko->dx > 0 && neko->pet.x >= (neko->pet.boundingBox.right - neko->pet.boundingBox.left) - SHEET_SPR_SIZE)
            {
                NekoSetState(neko, STATE_R_CLAW);
            }
            else if (neko->dy < 0 && neko->pet.y <= 0)
            {
                NekoSetState(neko, STATE_U_CLAW);
            }
            else if (neko->dy > 0 && neko->pet.y >= (neko->pet.boundingBox.bottom - neko->pet.boundingBox.top) - SHEET_SPR_SIZE)
            {
                NekoSetState(neko, STATE_D_CLAW);
            }
            else
                NekoSetState(neko, STATE_WASH);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;

    case STATE_WASH:
        if (NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        else if (neko->stateCount >= NEKO_WASH_TIME)
        {
            NekoSetState(neko, STATE_SCRATCH);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;

    case STATE_SCRATCH:
        if (NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        else if (neko->stateCount >= NEKO_SCRATCH_TIME)
        {
            NekoSetState(neko, STATE_YAWN);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;

    case STATE_YAWN:
        if (NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        else if (neko->stateCount >= NEKO_YAWN_TIME)
        {
            NekoSetState(neko, STATE_SLEEP);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;

    case STATE_SLEEP:
        if (NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;

    case STATE_AWAKE:
        if (neko->stateCount >= NEKO_AWAKE_TIME + (rand() % 20))
        {
            NekoCalcDirection(neko);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;
    case STATE_U_MOVE:
    case STATE_D_MOVE:
    case STATE_L_MOVE:
    case STATE_R_MOVE:
    case STATE_UL_MOVE:
    case STATE_UR_MOVE:
    case STATE_DL_MOVE:
    case STATE_DR_MOVE:
    {
        int curX = neko->pet.x;
        int curY = neko->pet.y;
        int newX = curX + neko->dx;
        int newY = curY + neko->dy;
        int nWidth = (neko->pet.boundingBox.right - neko->pet.boundingBox.left) - SHEET_SPR_SIZE;
        int nHeight = (neko->pet.boundingBox.bottom - neko->pet.boundingBox.top) - SHEET_SPR_SIZE;
        bool outside = (newX <= 0 || newX >= nWidth || newY <= 0 || newY >= nHeight);

        NekoCalcDirection(neko);

        if (newX < 0)
        {
            newX = 0;
        }
        else if (newX > nWidth)
        {
            newX = nWidth;
        }

        if (newY < 0)
        {
            newY = 0;
        }
        else if (newY > nHeight)
        {
            newY = nHeight;
        }
        bool notMoved = (newX == curX) && (newY == curY);

        if (outside && notMoved)
        {
            NekoSetState(neko, STATE_STOP);
        }
        else
        {
            PetSetImageAndMoveTo(&neko->pet, NekoGetStateAnimationFrameIndex(neko), newX, newY);

            if (neko->footprints)
            {
                int fpAnim = -1;
                switch (neko->state)
                {
                case STATE_U_MOVE:
                    fpAnim = 0;
                    break;
                case STATE_D_MOVE:
                    fpAnim = 4;
                    break;
                case STATE_L_MOVE:
                    fpAnim = 6;
                    break;
                case STATE_R_MOVE:
                    fpAnim = 2;
                    break;
                case STATE_UL_MOVE:
                    fpAnim = 7;
                    break;
                case STATE_UR_MOVE:
                    fpAnim = 1;
                    break;
                case STATE_DL_MOVE:
                    fpAnim = 5;
                    break;
                case STATE_DR_MOVE:
                    fpAnim = 3;
                    break;
                }

                if(fpAnim != -1)
                {
                    if(neko->tickCount & 1)
                    {
                        // TODO: multiple sprite support
                        // m_pPet->DrawOnTarget(nX - (m_nDY / 2), nY, m_hIconFootprints[iFpAnim]);
                    } else {
                        // TODO: multiple sprite support
                        // m_pPet->DrawOnTarget(nX, nY - (m_nDX / 2), m_hIconFootprints[iFpAnim]);
                    }
                }
            }
        }
        break;
    }

    case STATE_U_CLAW:
    case STATE_D_CLAW:
    case STATE_L_CLAW:
    case STATE_R_CLAW:
        if(NekoMoveStart(neko))
        {
            NekoSetState(neko, STATE_AWAKE);
        }
        else if(neko->stateCount >= NEKO_CLAW_TIME)
        {
            NekoSetState(neko, STATE_SCRATCH);
        }
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;
    
    default:
        // something went wrong!
        printf("something went wrong!");
        NekoSetState(neko, STATE_STOP);
        PetSetImage(&neko->pet, NekoGetStateAnimationFrameIndex(neko));
        break;
    }
}

void NekoUpdate(Neko* neko, int mouseX, int mouseY)
{
    switch(neko->action)
    {
        default:
        case CHASE_MOUSE:
        {
            NekoRunTowards(neko, mouseX, mouseY);
            break;
        }

        case RUN_AWAY_FROM_MOUSE:
        {
            int xdiff, ydiff;
            int dwLimit = neko->idleSpace * 16;

            xdiff = (neko->pet.x + (SHEET_SPR_SIZE >> 2)) - mouseX;
            ydiff = (neko->pet.y + (SHEET_SPR_SIZE >> 2)) - mouseY;

            if(abs(xdiff) < dwLimit && abs(ydiff) < dwLimit)
            {
                int x, y;
                long doubleLength = (long)xdiff * xdiff + (long)ydiff * ydiff;
                if(doubleLength != 0)
                {
                    long length = SquareRoot0(xdiff * xdiff + ydiff * ydiff);

                    long fxDirX = ((long)xdiff << FX_SHIFT) / length;
                    long fxDirY = ((long)ydiff << FX_SHIFT) / length;

                    x = neko->pet.x + (int)((fxDirX * (long)dwLimit) >> FX_SHIFT);
                    y = neko->pet.y + (int)((fxDirY * (long)dwLimit) >> FX_SHIFT);
                }
                else
                {
                    x = 32;
                    y = 32;
                }

                NekoRunTowards(neko, x, y);

                if(neko->state == STATE_AWAKE)
                    NekoCalcDirection(neko);
            }
            else
            {
                NekoRunTowards(neko, neko->toX, neko->toY);
            }
            break;
        }

        case RUN_AROUND_RANDOMLY:
        {
            if(neko->state == STATE_SLEEP)
            {
                neko->actionCount++;
            }

            if(neko->actionCount > neko->idleSpace * 10)
            {
                neko->actionCount = 0;
                // TODO: we can save width in boundingbox
                // maybe factor this into a "getrandompositioninsidebox"
                NekoRunTowards(neko, neko->pet.boundingBox.left + (rand() % (neko->pet.boundingBox.right - neko->pet.boundingBox.left)),
                                     neko->pet.boundingBox.top + (rand() % (neko->pet.boundingBox.bottom - neko->pet.boundingBox.top)));
            }
            else
            {
                NekoRunTowards(neko, neko->toX, neko->toY);
            }
            break;
        }

        case PACE_AROUND_SCREEN:
        {
            if(neko->dx == 0 && neko->dy == 0)
            {
                neko->actionCount = (neko->actionCount + 1) % 4;
            }

            switch (neko->actionCount)
            {
            case 0:
                NekoRunTowards(neko, neko->pet.boundingBox.left + SHEET_SPR_SIZE, neko->pet.boundingBox.top + SHEET_SPR_SIZE);
                break;
            case 1:
                NekoRunTowards(neko, neko->pet.boundingBox.left + SHEET_SPR_SIZE, neko->pet.boundingBox.bottom - SHEET_SPR_SIZE);
                break;
            case 2:
                NekoRunTowards(neko, neko->pet.boundingBox.right - SHEET_SPR_SIZE, neko->pet.boundingBox.bottom - SHEET_SPR_SIZE);
                break;
            case 3:
                NekoRunTowards(neko, neko->pet.boundingBox.right - SHEET_SPR_SIZE, neko->pet.boundingBox.top + SHEET_SPR_SIZE);
                break;
            default:
                break;
            }
            break;
        }

        case RUN_AROUND:
        {
            int boundingBox = neko->speed * 8;

            neko->actionX += neko->actionDX;
            neko->actionY += neko->actionDY;

            if(neko->actionX < neko->pet.boundingBox.left + boundingBox)
            {
                if(neko->actionX > neko->pet.boundingBox.left)
                {
                    neko->actionDX++;
                }
                else
                {
                    neko->actionDX = -neko->actionDX;
                }
            }
            else if(neko->actionX < neko->pet.boundingBox.right - boundingBox)
            {
                if(neko->actionX < neko->pet.boundingBox.right)
                {
                    neko->actionDX--;
                }
                else
                {
                    neko->actionDX = -neko->actionDX;
                }
            }

            if(neko->actionY < neko->pet.boundingBox.top + boundingBox)
            {
                if(neko->actionY > neko->pet.boundingBox.top)
                {
                    neko->actionDY++;
                }
                else
                {
                    neko->actionDY = -neko->actionDY;
                }
            }
            else if(neko->actionY < neko->pet.boundingBox.bottom - boundingBox)
            {
                if(neko->actionY < neko->pet.boundingBox.bottom)
                {
                    neko->actionDY--;
                }
                else
                {
                    neko->actionDY = -neko->actionDY;
                }
            }
            NekoRunTowards(neko, neko->actionX, neko->actionY);
            break;
        }
    }
    // TODO: sounds
}

void NekoSetState(Neko *neko, enum State state)
{
    // reset animation counters
    neko->tickCount = 0;
    neko->stateCount = 0;

    // update state
    neko->state = state;
}

int NekoGetStateAnimationFrameIndex(Neko *neko)
{
    if (neko->state != STATE_SLEEP)
        return animation[neko->state][neko->tickCount & 0x1];
    else
        return animation[neko->state][(neko->tickCount >> 2) & 0x1];
}

const char* NekoGetStateName( int nState )
{
    static const char* const s_StateNames[] =
    {
        "STATE_STOP",
        "STATE_WASH",
        "STATE_SCRATCH",
        "STATE_YAWN",
        "STATE_SLEEP",
        "STATE_AWAKE",
        "STATE_U_MOVE",
        "STATE_D_MOVE",
        "STATE_L_MOVE",
        "STATE_R_MOVE",
        "STATE_UL_MOVE",
        "STATE_UR_MOVE",
        "STATE_DL_MOVE",
        "STATE_DR_MOVE",
        "STATE_U_CLAW",
        "STATE_D_CLAW",
        "STATE_L_CLAW",
        "STATE_R_CLAW"
    };

    if( nState < 0 || nState >= (int)(sizeof(s_StateNames) / sizeof(s_StateNames[0])) )
        return "STATE_UNKNOWN";

    return s_StateNames[nState];
}
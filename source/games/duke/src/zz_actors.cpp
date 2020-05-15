//-------------------------------------------------------------------------
/*
Copyright (C) 2016 EDuke32 developers and contributors

This file is part of EDuke32.

EDuke32 is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------

#include "ns.h"	// Must come before everything else!

#define actors_c_

#include "global.h"

BEGIN_DUKE_NS

#if KRANDDEBUG
# define ACTOR_STATIC
#else
# define ACTOR_STATIC static
#endif

#define DELETE_SPRITE_AND_CONTINUE(KX) do { A_DeleteSprite(KX); goto next_sprite; } while (0)

void G_ClearCameraView(DukePlayer_t *ps)
{
    ps->newowner = -1;
    ps->pos = ps->opos;
    ps->q16ang = ps->oq16ang;

    updatesector(ps->pos.x, ps->pos.y, &ps->cursectnum);
    P_UpdateScreenPal(ps);

    for (bssize_t SPRITES_OF(STAT_ACTOR, k))
        if (sprite[k].picnum==TILE_CAMERA1)
            sprite[k].yvel = 0;
}

int32_t A_MoveSprite(int32_t spriteNum, vec3_t const * const change, uint32_t clipType)
{

    return fi.movesprite(spriteNum, change->x, change->y, change->z, clipType);
}

int32_t block_deletesprite = 0;


// deletesprite() game wrapper
void A_DeleteSprite(int spriteNum)
{
    if (EDUKE32_PREDICT_FALSE(block_deletesprite))
    {
        Printf(TEXTCOLOR_RED "A_DeleteSprite(): tried to remove sprite %d in EVENT_EGS\n", spriteNum);
        return;
    }

#ifdef POLYMER
    if (actor[spriteNum].lightptr != NULL && videoGetRenderMode() == REND_POLYMER)
        A_DeleteLight(spriteNum);
#endif

    // AMBIENT_SFX_PLAYING
    if (sprite[spriteNum].picnum == TILE_MUSICANDSFX && actor[spriteNum].t_data[0] == 1)
        S_StopEnvSound(sprite[spriteNum].lotag, spriteNum);

    deletesprite(spriteNum);
}

void insertspriteq(int i);

void A_AddToDeleteQueue(int spriteNum)
{
    insertspriteq(spriteNum);
}

void A_SpawnMultiple(int spriteNum, int tileNum, int spawnCnt)
{
    spritetype *pSprite = &sprite[spriteNum];

    for (; spawnCnt>0; spawnCnt--)
    {
        int32_t const r1 = krand2(), r2 = krand2();
        int const j = A_InsertSprite(pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z - (r2 % (47 << 8)), tileNum, -32, 8,
                               8, r1 & 2047, 0, 0, spriteNum, 5);
        //fi.spawn(-1, j);
        sprite[j].cstat = krand2()&12;
    }
}

void A_DoGuts(int spriteNum, int tileNum, int spawnCnt)
{
    fi.guts(&sprite[spriteNum], tileNum, spawnCnt, 0);
}

void A_DoGutsDir(int spriteNum, int tileNum, int spawnCnt)
{
    fi.gutsdir(&sprite[spriteNum], tileNum, spawnCnt, 0);
}

static int32_t G_ToggleWallInterpolation(int32_t wallNum, int32_t setInterpolation)
{
    if (setInterpolation)
    {
        return G_SetInterpolation(&wall[wallNum].x) || G_SetInterpolation(&wall[wallNum].y);
    }
    else
    {
        G_StopInterpolation(&wall[wallNum].x);
        G_StopInterpolation(&wall[wallNum].y);
        return 0;
    }
}

void Sect_ToggleInterpolation(int sectNum, int setInterpolation)
{
    for (bssize_t j = sector[sectNum].wallptr, endwall = sector[sectNum].wallptr + sector[sectNum].wallnum; j < endwall; j++)
    {
        G_ToggleWallInterpolation(j, setInterpolation);

        int const nextWall = wall[j].nextwall;

        if (nextWall >= 0)
        {
            G_ToggleWallInterpolation(nextWall, setInterpolation);
            G_ToggleWallInterpolation(wall[nextWall].point2, setInterpolation);
        }
    }
}

void ms(short i);

void A_MoveSector(int spriteNum)
{
    ms(spriteNum);
}


int g_canSeePlayer = 0;

int G_WakeUp(spritetype *const pSprite, int const playerNum)
{
    DukePlayer_t *const pPlayer = g_player[playerNum].ps;
    if (!pPlayer->make_noise)
        return 0;
    int const radius = pPlayer->noise_radius;

    if (pSprite->pal == 30 || pSprite->pal == 32 || pSprite->pal == 33 || (RRRA && pSprite->pal == 8))
        return 0;

    return (pPlayer->noise_x - radius < pSprite->x && pPlayer->noise_x + radius > pSprite->x
        && pPlayer->noise_y - radius < pSprite->y && pPlayer->noise_y + radius > pSprite->y);
}


// sleeping monsters, etc

 int A_FindLocator(int const tag, int const sectNum)
{
    for (bssize_t SPRITES_OF(STAT_LOCATOR, spriteNum))
    {
        if ((sectNum == -1 || sectNum == SECT(spriteNum)) && tag == SLT(spriteNum))
            return spriteNum;
    }

    return -1;
}




void A_PlayAlertSound(int spriteNum)
{
    if (DEER)
        return;
    if (RR)
    {
        if (sprite[spriteNum].extra > 0)
        {
            switch (DYNAMICTILEMAP(PN(spriteNum)))
            {
                case COOT__STATICRR: if (!RRRA || (krand2()&3) == 2) A_PlaySound(PRED_RECOG, spriteNum); break;
                case LTH__STATICRR: break;
                case BILLYCOCK__STATICRR:
                case BILLYRAY__STATICRR:
                case BRAYSNIPER__STATICRR: A_PlaySound(PIG_RECOG, spriteNum); break;
                case DOGRUN__STATICRR:
                case HULK__STATICRR:
                case HEN__STATICRR:
                case DRONE__STATICRR:
                case PIG__STATICRR:
                case RECON__STATICRR:
                case MINION__STATICRR:
                case COW__STATICRR:
                case VIXEN__STATICRR:
                case RABBIT__STATICRR: break;
            }
        }
        return;
    }
    if (sprite[spriteNum].extra > 0)
    {
        switch (DYNAMICTILEMAP(PN(spriteNum)))
        {
            case LIZTROOPONTOILET__STATIC:
            case LIZTROOPJUSTSIT__STATIC:
            case LIZTROOPSHOOT__STATIC:
            case LIZTROOPJETPACK__STATIC:
            case LIZTROOPDUCKING__STATIC:
            case LIZTROOPRUNNING__STATIC:
            case LIZTROOP__STATIC:         A_PlaySound(PRED_RECOG, spriteNum); break;
            case LIZMAN__STATIC:
            case LIZMANSPITTING__STATIC:
            case LIZMANFEEDING__STATIC:
            case LIZMANJUMP__STATIC:       A_PlaySound(CAPT_RECOG, spriteNum); break;
            case PIGCOP__STATIC:
            case PIGCOPDIVE__STATIC:       A_PlaySound(PIG_RECOG, spriteNum); break;
            case RECON__STATIC:            A_PlaySound(RECO_RECOG, spriteNum); break;
            case DRONE__STATIC:            A_PlaySound(DRON_RECOG, spriteNum); break;
            case COMMANDER__STATIC:
            case COMMANDERSTAYPUT__STATIC: A_PlaySound(COMM_RECOG, spriteNum); break;
            case ORGANTIC__STATIC:         A_PlaySound(TURR_RECOG, spriteNum); break;
            case OCTABRAIN__STATIC:
            case OCTABRAINSTAYPUT__STATIC: A_PlaySound(OCTA_RECOG, spriteNum); break;
            case BOSS1__STATIC:            S_PlaySound(BOS1_RECOG); break;
            case BOSS2__STATIC:            S_PlaySound((sprite[spriteNum].pal == 1) ? BOS2_RECOG : WHIPYOURASS); break;
            case BOSS3__STATIC:            S_PlaySound((sprite[spriteNum].pal == 1) ? BOS3_RECOG : RIPHEADNECK); break;
            case BOSS4__STATIC:
            case BOSS4STAYPUT__STATIC:     if (sprite[spriteNum].pal == 1) S_PlaySound(BOS4_RECOG); S_PlaySound(BOSS4_FIRSTSEE); break;
            case GREENSLIME__STATIC:       A_PlaySound(SLIM_RECOG, spriteNum); break;
        }
    }
}

int A_CheckSwitchTile(int spriteNum)
{
    // picnum 0 would oob in the switch below,

    if (PN(spriteNum) <= 0)
        return 0;

    // TILE_MULTISWITCH has 4 states so deal with it separately,
    // TILE_ACCESSSWITCH and TILE_ACCESSSWITCH2 are only active in one state so deal with
    // them separately.

    if ((PN(spriteNum) >= TILE_MULTISWITCH && PN(spriteNum) <= TILE_MULTISWITCH + 3) || (PN(spriteNum) == TILE_ACCESSSWITCH || PN(spriteNum) == TILE_ACCESSSWITCH2))
        return 1;

    if (RRRA && PN(spriteNum) >= TILE_MULTISWITCH2 && PN(spriteNum) <= TILE_MULTISWITCH2 + 3)
        return 1;

    // Loop to catch both states of switches.
    for (bssize_t j=1; j>=0; j--)
    {
        switch (DYNAMICTILEMAP(PN(spriteNum)-j))
        {
        case RRTILE8464__STATICRR:
            if (RRRA) return 1;
            break;
        case NUKEBUTTON__STATIC:
            if (RR) return 1;
            break;
        case HANDPRINTSWITCH__STATIC:
        case ALIENSWITCH__STATIC:
        case MULTISWITCH__STATIC:
        case PULLSWITCH__STATIC:
        case HANDSWITCH__STATIC:
        case SLOTDOOR__STATIC:
        case LIGHTSWITCH__STATIC:
        case SPACELIGHTSWITCH__STATIC:
        case SPACEDOORSWITCH__STATIC:
        case FRANKENSTINESWITCH__STATIC:
        case LIGHTSWITCH2__STATIC:
        case POWERSWITCH1__STATIC:
        case LOCKSWITCH1__STATIC:
        case POWERSWITCH2__STATIC:
        case DIPSWITCH__STATIC:
        case DIPSWITCH2__STATIC:
        case TECHSWITCH__STATIC:
        case DIPSWITCH3__STATIC:
            return 1;
        }
    }

    return 0;
}


void movefta_d(void);
void movefallers_d();
void movestandables_d();
void moveweapons_d();
void movetransports_d(void);
void moveactors_d();
void moveexplosions_d();
void moveeffectors_d();

void movefta_r(void);
void moveplayers();
void movefx();
void movefallers_r();
void movestandables_r();
void moveweapons_r();
void movetransports_r(void);
void moveactors_r();
void thunder();
void moveexplosions_r();
void moveeffectors_r();

void doanimations(void);

void G_MoveWorld_d(void)
{
    extern double g_moveActorsTime, g_moveWorldTime;
    const double worldTime = timerGetHiTicks();

    movefta_d();     //ST 2
    moveweapons_d();          //ST 4
    movetransports_d();       //ST 9

    moveplayers();          //ST 10
    movefallers_d();          //ST 12
    moveexplosions_d();             //ST 5

    const double actorsTime = timerGetHiTicks();

    moveactors_d();           //ST 1

    g_moveActorsTime = (1-0.033)*g_moveActorsTime + 0.033*(timerGetHiTicks()-actorsTime);

    // XXX: Has to be before effectors, in particular movers?
    // TODO: lights in moving sectors ought to be interpolated
    //G_DoEffectorLights();
    moveeffectors_d();        //ST 3
    movestandables_d();       //ST 6

    //G_RefreshLights();
    doanimations();
    movefx();               //ST 11

    g_moveWorldTime = (1-0.033)*g_moveWorldTime + 0.033*(timerGetHiTicks()-worldTime);
}

void G_MoveWorld_r(void)
{
    extern double g_moveActorsTime, g_moveWorldTime;
    const double worldTime = timerGetHiTicks();

    if (!DEER)
    {
        movefta_r();     //ST 2
        moveweapons_r();          //ST 4
        movetransports_r();       //ST 9
    }

    moveplayers();          //ST 10
    movefallers_r();          //ST 12
    if (!DEER)
        moveexplosions_r();             //ST 5

    const double actorsTime = timerGetHiTicks();

    moveactors_r();           //ST 1

    g_moveActorsTime = (1 - 0.033) * g_moveActorsTime + 0.033 * (timerGetHiTicks() - actorsTime);

    if (DEER)
    {
        sub_56EA8();
        ghtarget_move();
        gharrow_move();
        ghdeploy_move();
        sub_519E8(ud.level_number);
        sub_5524C();
    }

    // XXX: Has to be before effectors, in particular movers?
    // TODO: lights in moving sectors ought to be interpolated
    // G_DoEffectorLights();
    if (!DEER)
    {
        moveeffectors_r();        //ST 3
        movestandables_r();       //ST 6
    }

    //G_RefreshLights();
    doanimations();
    if (!DEER)
        movefx();               //ST 11

    if (numplayers < 2 && g_thunderOn)
        thunder();

    g_moveWorldTime = (1 - 0.033) * g_moveWorldTime + 0.033 * (timerGetHiTicks() - worldTime);
}

void G_MoveWorld(void)
{
    if (!isRR()) G_MoveWorld_d();
    else G_MoveWorld_r();
}

END_DUKE_NS


//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
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

#include "compat.h"
#include "build.h"

#include "blood.h"

BEGIN_BLD_NS

static void burnThinkSearch(DBloodActor*);
static void burnThinkGoto(DBloodActor*);
static void burnThinkChase(DBloodActor*);

AISTATE cultistBurnIdle = { kAiStateIdle, 3, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE cultistBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE cultistBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &cultistBurnSearch };
AISTATE cultistBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, &cultistBurnSearch };
AISTATE cultistBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &cultistBurnChase };

AISTATE zombieABurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE zombieABurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &zombieABurnSearch };
AISTATE zombieABurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, NULL };
AISTATE zombieABurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &zombieABurnChase };

AISTATE zombieFBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE zombieFBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &zombieFBurnSearch };
AISTATE zombieFBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, NULL };
AISTATE zombieFBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &zombieFBurnChase };

AISTATE innocentBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE innocentBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &zombieFBurnSearch };
AISTATE innocentBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, NULL };
AISTATE innocentBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &zombieFBurnChase };

AISTATE beastBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE beastBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &beastBurnSearch };
AISTATE beastBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, &beastBurnSearch };
AISTATE beastBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &beastBurnChase };

AISTATE tinycalebBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE tinycalebBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &tinycalebBurnSearch };
AISTATE tinycalebBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, &tinycalebBurnSearch };
AISTATE tinycalebBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &tinycalebBurnChase };

AISTATE genDudeBurnIdle = { kAiStateIdle, 3, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE genDudeBurnChase = { kAiStateChase, 3, -1, 0, NULL, aiMoveForward, burnThinkChase, NULL };
AISTATE genDudeBurnGoto = { kAiStateMove, 3, -1, 3600, NULL, aiMoveForward, burnThinkGoto, &genDudeBurnSearch };
AISTATE genDudeBurnSearch = { kAiStateSearch, 3, -1, 3600, NULL, aiMoveForward, burnThinkSearch, &genDudeBurnSearch };
AISTATE genDudeBurnAttack = { kAiStateChase, 3, nBurnClient, 120, NULL, NULL, NULL, &genDudeBurnChase };

void BurnSeqCallback(int, DBloodActor*)
{
}

static void burnThinkSearch(DBloodActor* actor)
{
	auto pXSprite = &actor->x();
	auto pSprite = &actor->s();
	aiChooseDirection(actor, pXSprite->goalAng);
	aiThinkTarget(actor);
}

static void burnThinkGoto(DBloodActor* actor)
{
	auto pXSprite = &actor->x();
	auto pSprite = &actor->s();
	assert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax);
	DUDEINFO* pDudeInfo = getDudeInfo(pSprite->type);
	int dx = pXSprite->targetX - pSprite->x;
	int dy = pXSprite->targetY - pSprite->y;
	int nAngle = getangle(dx, dy);
	int nDist = approxDist(dx, dy);
	aiChooseDirection(actor, nAngle);
	if (nDist < 512 && abs(pSprite->ang - nAngle) < pDudeInfo->periphery)
	{
		switch (pSprite->type)
		{
		case kDudeBurningCultist:
			aiNewState(actor, &cultistBurnSearch);
			break;
		case kDudeBurningZombieAxe:
			aiNewState(actor, &zombieABurnSearch);
			break;
		case kDudeBurningZombieButcher:
			aiNewState(actor, &zombieFBurnSearch);
			break;
		case kDudeBurningInnocent:
			aiNewState(actor, &innocentBurnSearch);
			break;
		case kDudeBurningBeast:
			aiNewState(actor, &beastBurnSearch);
			break;
		case kDudeBurningTinyCaleb:
			aiNewState(actor, &tinycalebBurnSearch);
			break;
#ifdef NOONE_EXTENSIONS
		case kDudeModernCustomBurning:
			aiNewState(actor, &genDudeBurnSearch);
			break;
#endif
		}
	}
	aiThinkTarget(actor);
}

static void burnThinkChase(DBloodActor* actor)
{
	auto pXSprite = &actor->x();
	auto pSprite = &actor->s();
	if (actor->GetTarget() == nullptr)
	{
		switch (pSprite->type)
		{
		case kDudeBurningCultist:
			aiNewState(actor, &cultistBurnGoto);
			break;
		case kDudeBurningZombieAxe:
			aiNewState(actor, &zombieABurnGoto);
			break;
		case kDudeBurningZombieButcher:
			aiNewState(actor, &zombieFBurnGoto);
			break;
		case kDudeBurningInnocent:
			aiNewState(actor, &innocentBurnGoto);
			break;
		case kDudeBurningBeast:
			aiNewState(actor, &beastBurnGoto);
			break;
		case kDudeBurningTinyCaleb:
			aiNewState(actor, &tinycalebBurnGoto);
			break;
#ifdef NOONE_EXTENSIONS
		case kDudeModernCustomBurning:
			aiNewState(actor, &genDudeBurnGoto);
			break;
#endif
		}
		return;
	}
	assert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax);
	DUDEINFO* pDudeInfo = getDudeInfo(pSprite->type);
	spritetype* pTarget = &actor->GetTarget()->s();
	XSPRITE* pXTarget = &actor->GetTarget()->x();
	int dx = pTarget->x - pSprite->x;
	int dy = pTarget->y - pSprite->y;
	aiChooseDirection(actor, getangle(dx, dy));
	if (pXTarget->health == 0)
	{
		switch (pSprite->type)
		{
		case kDudeBurningCultist:
			aiNewState(actor, &cultistBurnSearch);
			break;
		case kDudeBurningZombieAxe:
			aiNewState(actor, &zombieABurnSearch);
			break;
		case kDudeBurningZombieButcher:
			aiNewState(actor, &zombieFBurnSearch);
			break;
		case kDudeBurningInnocent:
			aiNewState(actor, &innocentBurnSearch);
			break;
		case kDudeBurningBeast:
			aiNewState(actor, &beastBurnSearch);
			break;
		case kDudeBurningTinyCaleb:
			aiNewState(actor, &tinycalebBurnSearch);
			break;
#ifdef NOONE_EXTENSIONS
		case kDudeModernCustomBurning:
			aiNewState(actor, &genDudeBurnSearch);
			break;
#endif
		}
		return;
	}
	int nDist = approxDist(dx, dy);
	if (nDist <= pDudeInfo->seeDist)
	{
		int nDeltaAngle = ((getangle(dx, dy) + 1024 - pSprite->ang) & 2047) - 1024;
		int height = (pDudeInfo->eyeHeight * pSprite->yrepeat) << 2;
		if (cansee(pTarget->x, pTarget->y, pTarget->z, pTarget->sectnum, pSprite->x, pSprite->y, pSprite->z - height, pSprite->sectnum))
		{
			if (nDist < pDudeInfo->seeDist && abs(nDeltaAngle) <= pDudeInfo->periphery)
			{
				aiSetTarget(actor, actor->GetTarget());
				if (nDist < 0x333 && abs(nDeltaAngle) < 85)
				{
					switch (pSprite->type)
					{
					case kDudeBurningCultist:
						aiNewState(actor, &cultistBurnAttack);
						break;
					case kDudeBurningZombieAxe:
						aiNewState(actor, &zombieABurnAttack);
						break;
					case kDudeBurningZombieButcher:
						aiNewState(actor, &zombieFBurnAttack);
						break;
					case kDudeBurningInnocent:
						aiNewState(actor, &innocentBurnAttack);
						break;
					case kDudeBurningBeast:
						aiNewState(actor, &beastBurnAttack);
						break;
					case kDudeBurningTinyCaleb:
						aiNewState(actor, &tinycalebBurnAttack);
						break;
#ifdef NOONE_EXTENSIONS
					case kDudeModernCustomBurning:
						aiNewState(actor, &genDudeBurnSearch);
						break;
#endif
					}
				}
				return;
			}
		}
	}

	switch (pSprite->type)
	{
	case kDudeBurningCultist:
		aiNewState(actor, &cultistBurnGoto);
		break;
	case kDudeBurningZombieAxe:
		aiNewState(actor, &zombieABurnGoto);
		break;
	case 242:
		aiNewState(actor, &zombieFBurnGoto);
		break;
	case kDudeBurningInnocent:
		aiNewState(actor, &innocentBurnGoto);
		break;
	case kDudeBurningBeast:
		aiNewState(actor, &beastBurnGoto);
		break;
	case kDudeBurningTinyCaleb:
		aiNewState(actor, &tinycalebBurnGoto);
		break;
#ifdef NOONE_EXTENSIONS
	case kDudeModernCustomBurning:
		aiNewState(actor, &genDudeBurnSearch);
		break;
#endif
	}
	actor->SetTarget(nullptr);
}

END_BLD_NS

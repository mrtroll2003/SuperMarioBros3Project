#include <algorithm>
#include <typeinfo>
#include "debug.h"



#include "Brick.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Koopa.h"
#include "Firetrap.h"
#include "Coin.h"
#include "PowerUps.h"

#include "Collision.h"

extern list<LPGAMEOBJECT> objects;
extern CMushroom* mr;
extern CBlockCoin* bc;
extern CTanookiLeaf* tl;
extern CCoin* cgb;

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (abs(vx) > abs(maxVx)) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if ((flick == 1) && GetTickCount64() - tailflick_start > MARIO_TAILFLICK_TIME)
	{
		flick = 0;
		SetState(MARIO_STATE_IDLE);
	}

	isOnPlatform = false;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else 
	if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}
	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithQuestionBrick(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CMushroom*>(e->obj))
		OnCollisionWithMushroom(e);
	else if (dynamic_cast<CTanookiLeaf*>(e->obj))
		OnCollisionWithLeaf(e);
	else if (dynamic_cast<CFiretrap*>(e->obj))
		OnCollisionWithHostile(e);
	else if (dynamic_cast<CFireball*>(e->obj))
		OnCollisionWithHostile(e);
	else if (dynamic_cast<CButton*>(e->obj))
		OnCollisionWithButton(e);
	else if (dynamic_cast<CParaGoomba*>(e->obj))
		OnCollisionWithParaGoomba(e);
	else if (dynamic_cast<CParaKoopa*>(e->obj))
		OnCollisionWithParaKoopa(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
}

void CMario::OnCollisionWithHostile(LPCOLLISIONEVENT e)
{
	if (untouchable == 0)
	{
		if (level > MARIO_LEVEL_BIG)
		{
			level = MARIO_LEVEL_BIG;
			StartUntouchable();
		}
		else if (level > MARIO_LEVEL_SMALL)
		{
			level = MARIO_LEVEL_SMALL;
			StartUntouchable();
		}
		else
		{
			SetState(MARIO_STATE_DIE);
		}
	}
}
void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if ((e->ny < 0) || (flick == 1))
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			DebugOut(L"A Goomba killed");
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				OnCollisionWithHostile(e);
			}
		}
	}
}
void CMario::OnCollisionWithParaGoomba(LPCOLLISIONEVENT e)
{
	CParaGoomba* goomba = dynamic_cast<CParaGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if ((e->ny < 0) || (flick == 1))
	{
		switch (goomba->GetState())
		{
		case GOOMBA_STATE_WALKING:
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case GOOMBA_STATE_PARA_WALKING:
			goomba->SetState(GOOMBA_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case GOOMBA_STATE_PARA_SHORT_JUMPING:
			goomba->SetState(GOOMBA_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case GOOMBA_STATE_PARA_JUMPING:
			goomba->SetState(GOOMBA_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				OnCollisionWithHostile(e);
			}
		}
	}
}
void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
	if ((e->ny < 0) || (flick == 1))
	{
		switch (koopa->GetState())
		{
			case KOOPA_STATE_WALKING: 
				koopa->SetState(KOOPA_STATE_SHELL);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				break;
			case KOOPA_STATE_SPINNING:
				koopa->SetState(KOOPA_STATE_SHELL);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				break;
		}	
	}
	else
	{
		if (untouchable == 0)
		{
			switch (koopa->GetState())
			{
			case KOOPA_STATE_WALKING:
				OnCollisionWithHostile(e);
				break;
			case KOOPA_STATE_SPINNING:
				OnCollisionWithHostile(e);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			}
		}
	}
}
void CMario::OnCollisionWithParaKoopa(LPCOLLISIONEVENT e)
{
	CParaKoopa* koopa = dynamic_cast<CParaKoopa*>(e->obj);
	if ((e->ny < 0) || (flick == 1))
	{
		switch (koopa->GetState())
		{
		case KOOPA_STATE_PARA:
			koopa->SetState(KOOPA_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case KOOPA_STATE_WALKING:
			koopa->SetState(KOOPA_STATE_SHELL);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case KOOPA_STATE_SHELL:
			koopa->SetState(KOOPA_STATE_SPINNING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case KOOPA_STATE_SHAKING:
			koopa->SetState(KOOPA_STATE_SPINNING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		case KOOPA_STATE_SPINNING:
			koopa->SetState(KOOPA_STATE_SHELL);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			break;
		}
	}
	else
	{
		if (untouchable == 0)
		{
			switch (koopa->GetState())
			{
			case KOOPA_STATE_WALKING:
				OnCollisionWithHostile(e);
				break;
			case KOOPA_STATE_PARA:
				OnCollisionWithHostile(e);
				break;
			case KOOPA_STATE_SPINNING:
				OnCollisionWithHostile(e);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			}
		}
	}
}
void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}
void CMario::OnCollisionWithButton(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	for(auto i : objects)
		if (typeid(*static_cast<CGoldBrick*>(i)) == typeid(CGoldBrick))
		{
			float x = i->GetX();
			float y = i->GetY();
			i->Delete();
			cgb = new CCoin(x, y);
			objects.push_front(cgb);
		}
}
void CMario::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
	CQuestionBrick* qb = dynamic_cast<CQuestionBrick*>(e->obj);
	if (e->ny > 0)
		if (qb->GetQuesID() == ID_ITEM_MUSHROOM)
		{
			mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
			objects.push_back(mr);
			qb->Delete();
		}
		else if (qb->GetQuesID() == ID_ITEM_COIN)
		{
			bc = new CBlockCoin(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
			objects.push_back(bc);
			coin++;
			qb->Delete();
		}
		else if (qb->GetQuesID() == ID_ITEM_TANOOKI)
		{
			if (level >= MARIO_LEVEL_BIG)
			{
				tl = new CTanookiLeaf(qb->GetX() + 16, qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(tl);
			}
			else
			{
				mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(mr);
			}
				
			qb->Delete();
		}
		
}
void CMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
	SetLevel(MARIO_LEVEL_BIG);
	e->obj->Delete();
}
void CMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
	SetLevel(MARIO_LEVEL_TANOOKI);
	e->obj->Delete();
}
void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* pt = dynamic_cast<CPortal*>(e->obj);
	if (((pt->GetInput() == PORTAL_INPUT_SIT) && (vy >= 0)) || ((pt->GetInput() == PORTAL_INPUT_JUMP) && (state == MARIO_STATE_JUMP)))
		SetPosition(pt->GetDesX(), pt->GetDesY());
}
//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = ID_ANI_MARIO_BRACE_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = ID_ANI_MARIO_BRACE_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return aniId;
}
int CMario::GetAniIdTanooki()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (ay == 0)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TANOOKI_FLY_RIGHT;
			else
				aniId = ID_ANI_MARIO_TANOOKI_FLY_LEFT;
		}
		else if ((ay > 0) && (ay < MARIO_GRAVITY))
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TANOOKI_SLOWDROP_RIGHT;
			else
				aniId = ID_ANI_MARIO_TANOOKI_SLOWDROP_LEFT;
		}
		else if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TANOOKI_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_TANOOKI_JUMP_RUN_LEFT;
		}
		else if (state == MARIO_STATE_TAILFLICK)
			aniId = ID_ANI_MARIO_TANOOKI_TAILFLICK;
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TANOOKI_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_TANOOKI_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_TANOOKI_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_TANOOKI_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_TANOOKI_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_TANOOKI_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = ID_ANI_MARIO_TANOOKI_BRACE_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_TANOOKI_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_TANOOKI_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = ID_ANI_MARIO_TANOOKI_BRACE_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_TANOOKI_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_TANOOKI_WALKING_LEFT;
			}
	if (flick == 1) aniId = ID_ANI_MARIO_TANOOKI_TAILFLICK;
	if (aniId == -1) aniId = ID_ANI_MARIO_TANOOKI_IDLE_RIGHT;

	return aniId;
}

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	else if (level == MARIO_LEVEL_TANOOKI)
		aniId = GetAniIdTanooki();

	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return; 

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		if (run_start == -1) run_start = GetTickCount64();
		maxVx = MARIO_RUNNING_SPEED;
		ax = MARIO_ACCEL_RUN_X;
		nx = 1;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		if (run_start == -1) run_start = GetTickCount64();
		maxVx = -MARIO_RUNNING_SPEED;
		ax = -MARIO_ACCEL_RUN_X;
		nx = -1;
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		run_start = -1;
		maxVx = MARIO_WALKING_SPEED;
		ax = MARIO_ACCEL_WALK_X;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		run_start = -1;
		maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCEL_WALK_X;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isSitting) break;
		run_start = -1;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		DebugOut(L"[INFO]TIME: %d", GetTickCount64() - run_start);
		break;
	
	case MARIO_STATE_RELEASE_JUMP:
		//if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		ay = MARIO_GRAVITY;
		break;
	case MARIO_STATE_SLOWDROP:
		ay = MARIO_GRAVITY / 10;
		break;
	case MARIO_STATE_SIT:
		run_start = -1;
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y +=MARIO_SIT_HEIGHT_ADJUST;
		}
		break;
	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;
	case MARIO_STATE_TAILFLICK:
		tailflick_start = GetTickCount64();
		flick = 1;
	case MARIO_STATE_IDLE:
		run_start = -1;
		ax = 0.0f;
		vx = 0.0f;
		break;
	case MARIO_STATE_FLY:
		if (isOnPlatform)
		{
			vy = MARIO_FLY_SPEED;
			ay = 0;
			maxVx = MARIO_WALKING_SPEED;
			ax = MARIO_ACCEL_WALK_X;
			nx = this->nx;
		}
		
		break;
	case MARIO_STATE_DIE:
		run_start = -1;
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	}

	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (level>=MARIO_LEVEL_BIG)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else 
		{
			left = x - MARIO_BIG_BBOX_WIDTH/2;
			top = y - MARIO_BIG_BBOX_HEIGHT/2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH/2;
		top = y - MARIO_SMALL_BBOX_HEIGHT/2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = l;
}


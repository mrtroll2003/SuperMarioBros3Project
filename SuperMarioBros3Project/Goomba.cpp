#include <iostream>
#include "Goomba.h"
#include "Brick.h"
#include <algorithm>
#include "debug.h"
CGoomba::CGoomba(float x, float y):CGameObject(x, y)
{
	this->ax = 0;
	this->ay = GOOMBA_GRAVITY;
	die_start = -1;
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (state == GOOMBA_STATE_DIE)
	{
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{ 
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT;
	}
}

void CGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CGoomba*>(e->obj)) return;
	
	if (e->ny != 0 )
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if ( (state==GOOMBA_STATE_DIE) && (GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT) )
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CGoomba::Render()
{
	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == GOOMBA_STATE_DIE) 
	{
		aniId = ID_ANI_GOOMBA_DIE;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x,y);
	RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			die_start = GetTickCount64();
			y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE)/2;
			vx = 0;
			vy = 0;
			ay = 0; 
			break;
		case GOOMBA_STATE_WALKING: 
			vx = -GOOMBA_WALKING_SPEED;
			break;
	}
}

CParaGoomba::CParaGoomba(float x, float y) : CGameObject(x, y)
{
	this->ax = 0;
	this->vx = GOOMBA_WALKING_SPEED;
	this->ay = GOOMBA_GRAVITY/2;
	timer = GetTickCount64();
	SetState(GOOMBA_STATE_PARA_WALKING);
}
void CParaGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == GOOMBA_STATE_DIE)
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else if(state == GOOMBA_STATE_WALKING)
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT;
	}
	else
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - 18 / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + 18;
	}
}
void CParaGoomba::Render()
{
	int aniId = ID_ANI_PARAGOOMBA_WALKING;

	switch (state)
	{
	case GOOMBA_STATE_WALKING:
		aniId = ID_ANI_GOOMBA_WALKING;
		break;
	case GOOMBA_STATE_DIE:
		aniId = ID_ANI_GOOMBA_DIE;
		break;
	case GOOMBA_STATE_PARA_JUMPING:
		aniId = ID_ANI_PARAGOOMBA_AIR;
		break;
	case GOOMBA_STATE_PARA_SHORT_JUMPING:
		aniId = ID_ANI_PARAGOOMBA_AIR;
		break;
	default:
		aniId = ID_ANI_PARAGOOMBA_WALKING;
		break;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CParaGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	if ((state == GOOMBA_STATE_PARA_WALKING) && (GetTickCount64() - timer > GOOMBA_PARA_READY))
	{
		SetState(GOOMBA_STATE_PARA_SHORT_JUMPING);
		timer = GetTickCount64();
		jump_count++;
	}
	if ((state == GOOMBA_STATE_PARA_SHORT_JUMPING) && (GetTickCount64() - timer > GOOMBA_SJ_READY) && (isOnPlatform) && (jump_count < 3))
	{
		vy = -GOOMBA_SHORT_JUMP_DIST;
		jump_count++;
	}
	if ((state == GOOMBA_STATE_PARA_SHORT_JUMPING) && (isOnPlatform) && (jump_count == 3))
	{
		SetState(GOOMBA_STATE_PARA_JUMPING);
	}
	if ((state == GOOMBA_STATE_PARA_JUMPING) && (isOnPlatform == true))
	{
		SetState(GOOMBA_STATE_PARA_WALKING);
		timer = GetTickCount64();
	}
	if ((state == GOOMBA_STATE_DIE) && (GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}
	if (isOnPlatform)
		DebugOut(L"on_");
	else DebugOut(L"off_");
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
void CParaGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	if (vy < 0)
	{
		isOnPlatform = false;
	}
}
void CParaGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	isOnPlatform = false;
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CGoomba*>(e->obj)) return;
	if (e->ny != 0)
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}

}
void CParaGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_PARA_JUMPING:
		//y -= 10;
		jump_count = 0;
		vy = -GOOMBA_JUMP_DIST;
		ay = GOOMBA_GRAVITY/2;
		DebugOut(L"Jump_");
		break;
	case GOOMBA_STATE_PARA_SHORT_JUMPING:
		//y -= 20;
		vy = -GOOMBA_SHORT_JUMP_DIST;
		ay = GOOMBA_GRAVITY/2;
		DebugOut(L"SJump_");
		break;
	case GOOMBA_STATE_PARA_WALKING:
		//y -= 10;
		DebugOut(L"ParaWalk_");
		break;
	case GOOMBA_STATE_DIE:
		die_start = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		DebugOut(L"Walk_");
		break;
	}
}
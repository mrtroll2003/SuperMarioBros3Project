#include "Firetrap.h"
#include <algorithm>
#include "debug.h"
#include "Brick.h"
CFiretrap::CFiretrap(float x, float y) : CGameObject(x,y)
{
	timer_start = GetTickCount64();
	SetState(FIRETRAP_STATE_RISING);
}
void CFiretrap::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FIRETRAP_BBOX_WIDTH / 2;
	top = y - FIRETRAP_BBOX_HEIGHT / 2;
	right = left + FIRETRAP_BBOX_WIDTH;
	bottom = top + FIRETRAP_BBOX_HEIGHT;
}
void CFiretrap::Render()
{
	int aniId = ID_ANI_FIRETRAP_AIMING_UP_LEFT;
	switch (state)
	{
	case FIRETRAP_STATE_AIMING_UP_LEFT:
		aniId = ID_ANI_FIRETRAP_AIMING_UP_LEFT;
		break;
	case FIRETRAP_STATE_AIMING_DOWN_LEFT:
		aniId = ID_ANI_FIRETRAP_AIMING_DOWN_LEFT;
		break;
	case FIRETRAP_STATE_AIMING_UP_RIGHT:
		aniId = ID_ANI_FIRETRAP_AIMING_UP_RIGHT;
		break;
	case FIRETRAP_STATE_AIMING_DOWN_RIGHT:
		aniId = ID_ANI_FIRETRAP_AIMING_DOWN_RIGHT;
		break;
	default:
		break;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
void CFiretrap::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	float disX = GetX() - mario->GetX();
	float disY = GetY() - 8 - mario->GetY();
	if ((state == FIRETRAP_STATE_RISING) && (GetTickCount64() - timer_start > FIRETRAP_MOVE_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_AIMING_DOWN_LEFT);
	}
	if ((state == FIRETRAP_STATE_FALLING) && (GetTickCount64() - timer_start > FIRETRAP_MOVE_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_IDLE);
	}
	if ((state == FIRETRAP_STATE_IDLE)&& (GetTickCount64() - timer_start > FIRETRAP_IDLE_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_RISING);
	}
	if((state == FIRETRAP_STATE_AIMING_DOWN_LEFT) || (state == FIRETRAP_STATE_AIMING_DOWN_RIGHT) || (state == FIRETRAP_STATE_AIMING_UP_LEFT) || (state == FIRETRAP_STATE_AIMING_UP_RIGHT))
	{
		if (disX >= 0 && disY <  0)
			SetState(FIRETRAP_STATE_AIMING_DOWN_LEFT);
		if (disX >= 0 && disY >= 0)
			SetState(FIRETRAP_STATE_AIMING_UP_LEFT);
		if (disX < 0 && disY < 0)
			SetState(FIRETRAP_STATE_AIMING_DOWN_RIGHT);
		if (disX < 0 && disY >= 0)
			SetState(FIRETRAP_STATE_AIMING_UP_RIGHT);
		if (GetTickCount64() - timer_start > FIRETRAP_AIM_TIMEOUT)
		{
			timer_start = GetTickCount64();
			SetState(FIRETRAP_STATE_FALLING);
		}
			
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFiretrap::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}
void CFiretrap::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CPipe*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}
void CFiretrap::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FIRETRAP_STATE_RISING:
		vy = -FIRETRAP_MOVING_SPEED;
		DebugOut(L"Rising");
		break;
	case FIRETRAP_STATE_FALLING:
		vy = FIRETRAP_MOVING_SPEED;
		DebugOut(L"Falling");
		break;
	case FIRETRAP_STATE_IDLE:
		vy = 0;
		DebugOut(L"Idle");
		break;
	case FIRETRAP_STATE_AIMING_DOWN_LEFT:
		vy = 0;
		DebugOut(L"DL_");
		break;
	case FIRETRAP_STATE_AIMING_DOWN_RIGHT:
		vy = 0;
		DebugOut(L"DR_");
		break;
	case FIRETRAP_STATE_AIMING_UP_LEFT:
		vy = 0;
		DebugOut(L"UL_");
		break;
	case FIRETRAP_STATE_AIMING_UP_RIGHT:
		vy = 0;
		DebugOut(L"UR_");
		break;
	}
}
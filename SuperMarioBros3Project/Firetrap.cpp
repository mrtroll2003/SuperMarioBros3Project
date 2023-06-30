#include "Firetrap.h"
#include <algorithm>
#include "debug.h"
#include "Brick.h"
CFireball* fire;
extern list<LPGAMEOBJECT> objects;
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
		fire_start = GetTickCount64();
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
		if (GetTickCount64() - fire_start > (FIRETRAP_AIM_TIMEOUT / 2) && count == 0)
		{
			switch (state)
			{
			case FIRETRAP_STATE_AIMING_DOWN_LEFT:
				if (disY < -32)
					fire = new CFireball(GetX() , GetY() - 8, FIREBALL_STATE_LEFT3);
				else 
					fire = new CFireball(GetX() , GetY() - 8, FIREBALL_STATE_LEFT4);
				break;
			case FIRETRAP_STATE_AIMING_UP_LEFT:
				if (disY < 32)
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT1);
				else
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT2);
				break;
			case FIRETRAP_STATE_AIMING_UP_RIGHT:
				if (disY < 32)
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT1);
				else
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT1);
				break;
			case FIRETRAP_STATE_AIMING_DOWN_RIGHT:
				if (disY < -32)
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT1);
				else
					fire = new CFireball(GetX(), GetY() - 8, FIREBALL_STATE_LEFT1);
				break;
			}
			objects.push_back(fire);
			count++;
		}
		if (GetTickCount64() - timer_start > FIRETRAP_AIM_TIMEOUT)
		{
			timer_start = GetTickCount64();
			SetState(FIRETRAP_STATE_FALLING);
			count = 0;
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

void CGreenFiretrap::Render()
{
	int aniId = ID_ANI_GREEN_FIRETRAP_AIMING_UP_LEFT;
	switch (state)
	{
	case FIRETRAP_STATE_AIMING_UP_LEFT:
		aniId = ID_ANI_GREEN_FIRETRAP_AIMING_UP_LEFT;
		break;
	case FIRETRAP_STATE_AIMING_DOWN_LEFT:
		aniId = ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_LEFT;
		break;
	case FIRETRAP_STATE_AIMING_UP_RIGHT:
		aniId = ID_ANI_GREEN_FIRETRAP_AIMING_UP_RIGHT;
		break;
	case FIRETRAP_STATE_AIMING_DOWN_RIGHT:
		aniId = ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_RIGHT;
		break;
	default:
		break;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

CFireball::CFireball(float x, float y, int state) : CGameObject(x, y)
{
	timer = GetTickCount64();
	SetState(state);
}
void CFireball::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_FIREBALL)->Render(x, y);
}
void CFireball::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}
void CFireball::OnCollisionWith(LPCOLLISIONEVENT e)
{
	return;
}
void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (GetTickCount64() - timer > FIREBALL_TIMEOUT)
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
void CFireball::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FIREBALL_BBOX_WIDTH / 2;
	top = y - FIREBALL_BBOX_HEIGHT / 2;
	right = left + FIREBALL_BBOX_WIDTH;
	bottom = top + FIREBALL_BBOX_HEIGHT;
}
void CFireball::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FIREBALL_STATE_LEFT1:
		vx = -0.05f;
		vy = -0.12f;
		DebugOut(L"left1");
		break;
	case FIREBALL_STATE_LEFT2:
		vx = -0.12f;
		vy = -0.05f;
		DebugOut(L"left2");
		break;
	case FIREBALL_STATE_LEFT3:
		vx = -0.12f;
		vy = 0.05f;
		DebugOut(L"left3");
		break;
	case FIREBALL_STATE_LEFT4:
		vx = -0.05f;
		vy = 0.12f;
		DebugOut(L"left4");
		break;
	case FIREBALL_STATE_RIGHT1:
		vx = 0.05f;
		vy = -0.12f;
		DebugOut(L"R1");
		break;
	case FIREBALL_STATE_RIGHT2:
		vx = 0.12f;
		vy = -0.05f;
		DebugOut(L"R2");
		break;
	case FIREBALL_STATE_RIGHT3:
		vx = 0.12f;
		vy = 0.05f;
		DebugOut(L"R3");
		break;
	case FIREBALL_STATE_RIGHT4:
		vx = 0.05f;
		vy = 0.12f;
		DebugOut(L"R4");
		break;
	}
}

void CGreenFlytrap::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_GREEN_FLYTRAP)->Render(x, y);
	RenderBoundingBox();
}
void CGreenFlytrap::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if ((state == FIRETRAP_STATE_RISING) && (GetTickCount64() - timer_start > FIRETRAP_MOVE_TIMEOUT))
	{
		fire_start = GetTickCount64();
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_AIMING_DOWN_LEFT);
	}
	if ((state == FIRETRAP_STATE_FALLING) && (GetTickCount64() - timer_start > FIRETRAP_MOVE_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_IDLE);
	}
	if ((state == FIRETRAP_STATE_IDLE) && (GetTickCount64() - timer_start > FIRETRAP_IDLE_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_RISING);
	}
	if ((state == FIRETRAP_STATE_AIMING_DOWN_LEFT) && (GetTickCount64() - timer_start > FIRETRAP_AIM_TIMEOUT))
	{
		timer_start = GetTickCount64();
		SetState(FIRETRAP_STATE_FALLING);
	}
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
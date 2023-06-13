#include "PowerUps.h"
void CPowerUps::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - SHROOM_BBOX_WIDTH / 2;
	top = y - SHROOM_BBOX_HEIGHT / 2;
	right = left + SHROOM_BBOX_WIDTH;
	bottom = top + SHROOM_BBOX_HEIGHT;
}
void CMushroom::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}
void CMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}
CMushroom::CMushroom(float x, float y) : CPowerUps(x, y)
{
	this->ax = 0;
	this->ay = GRAVITY;
	timeout_start = GetTickCount64();
	SetState(SHROOM_STATE_RISING);
}
void CMushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - SHROOM_BBOX_WIDTH / 2;
	top = y - SHROOM_BBOX_HEIGHT / 2;
	right = left + SHROOM_BBOX_WIDTH;
	bottom = top + SHROOM_BBOX_HEIGHT;
}
void CMushroom::Render()
{
	int aniId = ID_ANI_SHROOM_RISING;
	if (state == SHROOM_STATE_MOVING)
	{
		aniId = ID_ANI_SHROOM_MOVING;
	}
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if ((state == SHROOM_STATE_RISING) && (GetTickCount64() - timeout_start > SHROOM_RISE_TIMEOUT))
	{
		SetState(SHROOM_STATE_MOVING);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
void CMushroom::SetState(int state) 
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SHROOM_STATE_RISING:
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case SHROOM_STATE_MOVING:
		vx = SHROOM_SPEED;
		ay = GRAVITY;
		break;
	}
}
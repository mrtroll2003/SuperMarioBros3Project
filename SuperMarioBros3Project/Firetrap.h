#pragma once
#define PI 3.14159265

#define FIRETRAP_MOVING_SPEED 0.06f
#define FIRETRAP_MOVE_TIMEOUT 670
#define FIRETRAP_IDLE_TIMEOUT 2500
#define FIRETRAP_AIM_TIMEOUT 2000


#define FIRETRAP_BBOX_WIDTH 17.0f
#define FIRETRAP_BBOX_HEIGHT 33.0f

#define FIREBALL_BBOX_WIDTH 9.0f
#define FIREBALL_BBOX_HEIGHT 9.0f
#define FIREBALL_TIMEOUT 2000

#define FIRETRAP_STATE_RISING 100
#define FIRETRAP_STATE_AIMING_UP_LEFT 200
#define FIRETRAP_STATE_AIMING_DOWN_LEFT 300
#define FIRETRAP_STATE_AIMING_UP_RIGHT 400
#define FIRETRAP_STATE_AIMING_DOWN_RIGHT 500
#define FIRETRAP_STATE_FALLING 600
#define FIRETRAP_STATE_IDLE 700

#define FIREBALL_STATE_LEFT1 100
#define FIREBALL_STATE_LEFT2 200
#define FIREBALL_STATE_LEFT3 300
#define FIREBALL_STATE_LEFT4 400
#define FIREBALL_STATE_RIGHT1 500
#define FIREBALL_STATE_RIGHT2 600
#define FIREBALL_STATE_RIGHT3 700
#define FIREBALL_STATE_RIGHT4 800

#define ID_ANI_FIRETRAP_AIMING_UP_LEFT 7001
#define ID_ANI_FIRETRAP_AIMING_DOWN_LEFT 7002
#define ID_ANI_FIRETRAP_AIMING_UP_RIGHT 7003
#define ID_ANI_FIRETRAP_AIMING_DOWN_RIGHT 7004

#define ID_ANI_GREEN_FIRETRAP_AIMING_UP_LEFT 7005
#define ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_LEFT 7006
#define ID_ANI_GREEN_FIRETRAP_AIMING_UP_RIGHT 7007
#define ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_RIGHT 7008

#define ID_ANI_GREEN_FLYTRAP 7009

#define ID_ANI_FIREBALL 7100

#include "GameObject.h"
#include <cmath>
#include "Mario.h"

extern CMario* mario;
class CFireball : public CGameObject
{
protected:
	ULONGLONG timer;
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	int IsBlocking() { return 0; }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CFireball(float x, float y, int state);
	void SetState(int state);
};
class CFiretrap : public CGameObject
{
protected:
	ULONGLONG timer_start;
	ULONGLONG fire_start;
	int count;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CFiretrap(float x, float y);
	virtual void SetState(int state);
	float GetX() { return x; }
	float GetY() { return y; }
};
class CGreenFiretrap : public CFiretrap
{
protected:
	void Render();
public:
	CGreenFiretrap(float x, float y) : CFiretrap(x, y) {}

};
class CGreenFlytrap : public CFiretrap
{
protected:
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
public:
	CGreenFlytrap(float x, float y) : CFiretrap(x, y) {}
};




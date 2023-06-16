#pragma once
#define PI 3.14159265

#define FIRETRAP_MOVING_SPEED 0.06f
#define FIRETRAP_MOVE_TIMEOUT 670
#define FIRETRAP_IDLE_TIMEOUT 2500
#define FIRETRAP_AIM_TIMEOUT 2000

#define FIRETRAP_BBOX_WIDTH 17
#define FIRETRAP_BBOX_HEIGHT 33

#define FIRETRAP_STATE_RISING 100
#define FIRETRAP_STATE_AIMING_UP_LEFT 200
#define FIRETRAP_STATE_AIMING_DOWN_LEFT 300
#define FIRETRAP_STATE_AIMING_UP_RIGHT 400
#define FIRETRAP_STATE_AIMING_DOWN_RIGHT 500
#define FIRETRAP_STATE_FALLING 600
#define FIRETRAP_STATE_IDLE 700

#define ID_ANI_FIRETRAP_AIMING_UP_LEFT 7001
#define ID_ANI_FIRETRAP_AIMING_DOWN_LEFT 7002
#define ID_ANI_FIRETRAP_AIMING_UP_RIGHT 7003
#define ID_ANI_FIRETRAP_AIMING_DOWN_RIGHT 7004

#define ID_ANI_FIREBALL 7100

#include "GameObject.h"
#include <cmath>
#include "Mario.h"

extern CMario* mario;
class CFiretrap : public CGameObject
{
protected:
	ULONGLONG timer_start;

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

class CFireball : public CGameObject
{
protected:
	void Render();
	void Update(DWORD dt);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }
	void OnNoCollision(DWORD dt);
public:
	CFireball(float x, float y, float vx);
};


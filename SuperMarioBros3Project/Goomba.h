#pragma once
#include "GameObject.h"

#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_SHORT_JUMP_DIST 0.12f
#define GOOMBA_JUMP_DIST 0.3f


#define GOOMBA_BBOX_WIDTH 17
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_DIE_TIMEOUT 500
#define GOOMBA_PARA_READY 2000
#define GOOMBA_SJ_READY 500

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200
#define GOOMBA_STATE_PARA_WALKING 300
#define GOOMBA_STATE_PARA_SHORT_JUMPING 400
#define GOOMBA_STATE_PARA_JUMPING 500
//#define GOOMBA_STATE_PARA_FALLING 500

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001
#define ID_ANI_PARAGOOMBA_WALKING 5002
#define ID_ANI_PARAGOOMBA_AIR 5003

class CGoomba : public CGameObject
{
protected:
	float ax;				
	float ay; 

	ULONGLONG die_start;

	void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	void Render();

	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }
	void OnNoCollision(DWORD dt);

	void OnCollisionWith(LPCOLLISIONEVENT e);

public: 	
	CGoomba(float x, float y);
	void SetState(int state);
};
class CParaGoomba : public CGameObject
{
protected:
	float ax;
	float ay;

	ULONGLONG die_start;

	int jump_count = 0;
	ULONGLONG timer;
	BOOLEAN isOnPlatform = false;

	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CParaGoomba(float x, float y);
	void SetState(int state);
};
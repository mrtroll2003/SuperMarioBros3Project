#pragma once
#include "GameObject.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.05f
#define KOOPA_SPINNING_SPEED 0.2f


#define KOOPA_BBOX_WIDTH 15
#define KOOPA_BBOX_HEIGHT 15
#define KOOPA_BBOX_HEIGHT_SHELL 12

#define KOOPA_DIE_TIMEOUT 500

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL 200
#define KOOPA_STATE_SHAKING 300
#define KOOPA_STATE_SPINNING 400

#define ID_ANI_KOOPA_WALKING 6000
#define ID_ANI_KOOPA_SHELL 6001
#define ID_ANI_KOOPA_SHAKING 6002
#define ID_ANI_KOOPA_SPINNING 6003
class CKoopa : public CGameObject
{
protected:
	float ax;
	float ay;

	ULONGLONG shell_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CKoopa(float x, float y);
	virtual void SetState(int state);
};


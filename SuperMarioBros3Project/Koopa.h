#pragma once
#include "GameObject.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.05f
#define KOOPA_SPINNING_SPEED 0.2f


#define KOOPA_BBOX_WIDTH 17
#define KOOPA_BBOX_HEIGHT 24
#define KOOPA_BBOX_HEIGHT_SHELL 14

#define KOOPA_SHELL_TIMEOUT 500

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL 200
#define KOOPA_STATE_SHAKING 300
#define KOOPA_STATE_SPINNING 400

#define ID_ANI_KOOPA_WALKING 6001
#define ID_ANI_KOOPA_SHELL 6002
#define ID_ANI_KOOPA_SHAKING 6003
#define ID_ANI_KOOPA_SPINNING 6004
class CKoopa : public CGameObject
{
protected:
	float ax;
	float ay;

	ULONGLONG shell_start;

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }
	void OnNoCollision(DWORD dt);

	void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CKoopa(float x, float y);
	void SetState(int state);
};


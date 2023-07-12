#pragma once
#include "GameObject.h"
#include <algorithm>
#include "debug.h"

#define SHROOM_SPEED 0.08f
#define COIN_SPEED 0.12f
#define GRAVITY 0.002f

#define SHROOM_RISE_TIMEOUT 850
#define COIN_ANI_TIMEOUT 750
#define LEAF_RISE_TIMEOUT 1250
#define SHROOM_BBOX_WIDTH 17
#define SHROOM_BBOX_HEIGHT 17
#define LEAF_BBOX_WIDTH 15
#define LEAF_BBOX_HEIGHT 13


#define SHROOM_STATE_RISING 100
#define SHROOM_STATE_MOVING 200

#define COIN_STATE_RISING 100
#define COIN_STATE_FALLING 200

#define ID_ANI_POWER_UPS 13000

#define ID_ANI_SHROOM (ID_ANI_POWER_UPS+100)
#define ID_ANI_SHROOM_RISING (ID_ANI_SHROOM + 1)
#define ID_ANI_SHROOM_MOVING (ID_ANI_SHROOM + 2)

#define ID_ANI_BLOCK_COIN (ID_ANI_POWER_UPS + 200)

#define ID_ANI_POPUP_COIN (ID_ANI_POWER_UPS+200)

#define ID_ANI_TANOOKI_LEAF (ID_ANI_POWER_UPS + 300)
class CPowerUps : public CGameObject
{
protected:
	float ax;
	float ay;
public:
	CPowerUps(float x, float y) : CGameObject(x, y) {}
	virtual int IsBlocking() { return 0; }
	virtual void Render() {};
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {};
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

class CMushroom : public CPowerUps
{
protected:
	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }
	ULONGLONG timeout_start;
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CMushroom(float x, float y);
	void SetState(int state);
};
class CBlockCoin : public CPowerUps
{
protected:
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	ULONGLONG timeout_start;
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void OnNoCollision(DWORD dt);
public:
	CBlockCoin(float x, float y);
	void SetState(int state);
};
class CTanookiLeaf : public CPowerUps
{
protected:
	int IsCollidable() { return 1; };
	int IsBlocking() { return 0; }
	ULONGLONG timeout_start;
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CTanookiLeaf(float x, float y);
};

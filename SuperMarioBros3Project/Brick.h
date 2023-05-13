#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_BRICK 10000
#define ID_ANI_GOLD_BRICK 10100
#define ID_ANI_INVI_BRICK 10200
#define BRICK_WIDTH 16
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16

class CBrick : public CGameObject {
public:
	CBrick(float x, float y) : CGameObject(x, y) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};
class CGoldBrick : public CBrick {
public:
	CGoldBrick(float x, float y) : CBrick(x,y) {}
	void Render();
	void Update(DWORD dt) {}
};
class CInviBrick : public CBrick {
public:
	CInviBrick(float x, float y) :CBrick(x, y) {}
	void Render();
	void Update(DWORD dt) {}
	
};

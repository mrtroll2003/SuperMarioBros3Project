#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#include "AssetIDs.h"

#define ID_ANI_BRICK 10000
#define ID_ANI_GOLD_BRICK 10100
#define ID_ANI_INVI_BRICK 10200
#define ID_ANI_BASE_BRICK 10300
#define ID_ANI_PIPE 10400
#define ID_ANI_QUESTION_BRICK 10500
#define ID_ANI_NULL_BRICK 10600
#define ID_ANI_STRIPE_BRICK 10700
#define ID_ANI_CLOUD_BRICK 10800
#define BRICK_WIDTH 16
#define BRICK_BBOX_WIDTH 16.0f
#define BRICK_BBOX_HEIGHT 16.0f
#define BASE_BRICK_BBOX_WIDTH 47.0f

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
class CQuestionBrick : public CBrick {
	int quesID;
public:
	int GetQuesID() { return quesID; }
	CQuestionBrick(float x, float y, int id) : CBrick(x, y) { quesID = id; };
	CQuestionBrick(float x, float y) : CBrick(x, y) { quesID = ID_ITEM_TANOOKI; };
	void Render();
	void Update(DWORD dt) {}
	float GetX() { return x; }
	float GetY() { return y; }
};
class CNullBrick : public CBrick {
public:
	CNullBrick(float x, float y) : CBrick(x,y) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};
class CInviBrick : public CBrick {
public:
	CInviBrick(float x, float y) :CBrick(x, y) {}
	void Render();
	void Update(DWORD dt) {}
	int IsDirectionColliable(float nx, float ny);
};
class CInviBase : public CInviBrick {
public:
	CInviBase(float x, float y) :CInviBrick(x, y) {}
	int IsDirectionColliable(float nx, float ny);
};
class CCloudBrick : public CBrick {
public:
	CCloudBrick(float x, float y) : CBrick(x, y) {}
	void Render();
	void Update(DWORD dt) {}
};
class CBaseBrick : public CBrick {
public:
	CBaseBrick(float x, float y) : CBrick(x,y) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};
class CPipe : public CBrick
{
public:
	CPipe(float x, float y) : CBrick(x, y) {}
	void Update(DWORD dt) {}
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};

class CStripeBrick : public CBrick
{
public:
	CStripeBrick(float x, float y) : CBrick(x, y) {}
	void Render();
	void Update(DWORD dt) {}
};
#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#define ID_ANI_ENVIRONMENT 60000
#define ID_ANI_WSB 60100
class CEnvironment: public CGameObject
{
public:
	CEnvironment(float x, float y) : CGameObject(x, y) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b) {}
	int IsBlocking() { return 0; }
};
class CWideSquaredBush : public CEnvironment
{
public:
	CWideSquaredBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};


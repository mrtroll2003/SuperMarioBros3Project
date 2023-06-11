#pragma once
#include "GameObject.h"
class CPowerUps : public CGameObject
{
public:
	CPowerUps(float x, float y) : CGameObject(x, y) {}
	int IsBlocking() { return 0; }
};


#pragma once
#include "GameObject.h"
class CEnvironment: public CGameObject
{
public:
	int IsBlocking() { return 0; }
};


#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#define ID_ANI_ENVIRONMENT 60000
#define ID_ANI_WSB 60100
#define ID_ANI_DCLOUD 60200
#define ID_ANI_BUSH 60300
#define ID_ANI_QUAD_PLAT 60500
#define ID_ANI_SCLOUD 60600
#define ID_ANI_LB 60700
#define ID_ANI_TRIP_PLAT 60800
class CEnvironment: public CGameObject
{
public:
	CEnvironment(float x, float y) : CGameObject(x, y) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b) {}
	int IsBlocking() { return 0; }
	int IsColliable() { return 0; }
};
class CWideSquaredBush : public CEnvironment
{
public:
	CWideSquaredBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CLowBush : public CEnvironment
{
public:
	CLowBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CSingleCloud : public CEnvironment
{
public:
	CSingleCloud(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CDoubleCloud : public CEnvironment
{
public:
	CDoubleCloud(float x, float y) : CEnvironment(x,y) {}
	void Render();
};
class CSingleBush : public CEnvironment
{
public:
	CSingleBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CDColorPlat : public CEnvironment
{
public: 
	CDColorPlat(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CTripPlat : public CEnvironment
{
public:
	CTripPlat(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CQuadPlat : public CEnvironment
{
public:
	CQuadPlat(float x, float y) : CEnvironment(x,y) {}
	void Render();
};

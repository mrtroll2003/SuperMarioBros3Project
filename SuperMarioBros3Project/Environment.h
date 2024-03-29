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
#define ID_ANI_HB 61000
#define ID_ANI_TRIP_PLAT 60800
#define ID_ANI_HIGH_PLAT 60900
#define ID_ANI_SKY_PLAT 61100
#define ID_ANI_SPIKE_BLACK 61200
#define ID_ANI_PLAIN_BLACK 61300
#define ID_ANI_BBB 61400
#define ID_ANI_SBB 61500
#define ID_ANI_MAIN 61600
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
class CHighBush : public CEnvironment
{
public:
	CHighBush(float x, float y) : CEnvironment(x, y) {}
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
class CHighPlat : public CEnvironment
{
public:
	CHighPlat(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CSkyPlat : public CEnvironment
{
public:
	CSkyPlat(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CSpikeBlackBG : public CEnvironment
{
public:
	CSpikeBlackBG(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CPlainBlackBG : public CEnvironment
{
public:
	CPlainBlackBG(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CBigBlackBush : public CEnvironment
{
public:
	CBigBlackBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CSmallBlackBush : public CEnvironment
{
public:
	CSmallBlackBush(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
class CMainScreen : public CEnvironment
{
public:
	CMainScreen(float x, float y) : CEnvironment(x, y) {}
	void Render();
};
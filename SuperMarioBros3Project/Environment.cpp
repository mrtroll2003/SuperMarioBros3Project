#include "Environment.h"
void CEnvironment::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_ENVIRONMENT)->Render(x, y);
}
void CWideSquaredBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_WSB)->Render(x, y);
}
void CLowBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_LB)->Render(x, y);
}
void CHighBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_HB)->Render(x, y);
}
void CSingleCloud::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SCLOUD)->Render(x, y);
}
void CDoubleCloud::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_DCLOUD)->Render(x, y);
}
void CSingleBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BUSH)->Render(x, y);
}
void CDColorPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_ENVIRONMENT + 400)->Render(x, y);
}
void CTripPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_TRIP_PLAT)->Render(x, y);
}
void CQuadPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_QUAD_PLAT)->Render(x, y);
}
void CHighPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_HIGH_PLAT)->Render(x, y);
}
void CSkyPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SKY_PLAT)->Render(x, y);
}
void CSpikeBlackBG::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SPIKE_BLACK)->Render(x, y);
}
void CPlainBlackBG::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_PLAIN_BLACK)->Render(x, y);
}
void CBigBlackBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BBB)->Render(x, y);
}
void CSmallBlackBush::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SBB)->Render(x, y);
}
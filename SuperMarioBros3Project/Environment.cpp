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
void CQuadPlat::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_QUAD_PLAT)->Render(x, y);
}
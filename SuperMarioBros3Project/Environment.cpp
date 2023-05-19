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
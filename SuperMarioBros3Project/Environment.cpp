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
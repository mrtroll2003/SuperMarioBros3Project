#include "Brick.h"
void CBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BRICK)->Render(x, y);
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x - BRICK_BBOX_WIDTH/2;
	t = y - BRICK_BBOX_HEIGHT/2;
	r = l + BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT;
}
void CGoldBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_GOLD_BRICK)->Render(x, y);
}
void CQuestionBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_QUESTION_BRICK)->Render(x, y);
}
void CNullBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_NULL_BRICK)->Render(x, y);
}
void CNullBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BRICK_BBOX_WIDTH / 2;
	t = y - BRICK_BBOX_HEIGHT / 2;
	r = l + BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT - 0.1;
}
void CInviBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_INVI_BRICK)->Render(x, y);
}
int CInviBrick::IsDirectionColliable(float nx, float ny)
{
	if ((nx == 0 && ny == -1) || (nx == 1 && ny == 0) || (nx == -1 && ny == 0))return 1;
	else return 0;
}
int CInviBase::IsDirectionColliable(float nx, float ny)
{
	if ((nx == 0 && ny == -1))return 1;
	else return 0;
}
void CCloudBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_CLOUD_BRICK)->Render(x, y);
}
void CBaseBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BASE_BRICK)->Render(x, y);
}
void CStripeBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_STRIPE_BRICK)->Render(x, y);
}
void CBaseBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BASE_BRICK_BBOX_WIDTH / 2;
	t = y - BRICK_BBOX_HEIGHT / 2;
	r = l + BASE_BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT;
}
void CPipe::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_PIPE)->Render(x, y);
}
void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - 33 / 2;
	t = y - 49 / 2;
	r = l + 33;
	b = t + 49;
}
void CBlackPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - 32 / 2;
	t = y - 32 / 2;
	r = l + 32;
	b = t + 32;
}
void CBlackPipe::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BLACK_PIPE)->Render(x, y);
}
void CSecretBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SECRET_BRICK)->Render(x, y);
}
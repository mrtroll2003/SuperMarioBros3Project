#include "Coin.h"

void CCoin::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_COIN)->Render(x, y);

	RenderBoundingBox();
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void CButton::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_BUTTON)->Render(x, y);

	RenderBoundingBox();
}
void CButton::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BUTTON_BBOX_WIDTH / 2;
	t = y - BUTTON_BBOX_HEIGHT / 2;
	r = l + BUTTON_BBOX_WIDTH;
	b = t + BUTTON_BBOX_HEIGHT;
}
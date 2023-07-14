#include <algorithm>
#include "debug.h"


#include "Koopa.h"
#include "Goomba.h"
#include "Brick.h"
#include "PowerUps.h"
#include "Mario.h"
extern CMushroom* mr;
extern CBlockCoin* bc;
extern CTanookiLeaf* tl;
extern CMario* mario;
extern list<LPGAMEOBJECT> objects;
CKoopa::CKoopa(float x, float y) : CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	shell_start = -1;
	SetState(KOOPA_STATE_WALKING);
}
void CKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPA_STATE_WALKING)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
	else
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT_SHELL / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT_SHELL;
	}
}
void CKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};
void CKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{

	if (!e->obj->IsBlocking()) return;
	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
	if (dynamic_cast<CKoopa*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
			switch (koopa->GetState())
			{
			case KOOPA_STATE_WALKING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SPINNING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			}
		}
	}
	else if (dynamic_cast<CGoomba*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				goomba->SetState(GOOMBA_STATE_DIE);
				DebugOut(L"A Goomba killed");
			}
		}
	}
	else if (dynamic_cast<CParaGoomba*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CParaGoomba* goomba = dynamic_cast<CParaGoomba*>(e->obj);
			switch (goomba->GetState())
			{
			case GOOMBA_STATE_WALKING:
				goomba->SetState(GOOMBA_STATE_DIE);
				break;
			case GOOMBA_STATE_PARA_WALKING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			case GOOMBA_STATE_PARA_SHORT_JUMPING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			case GOOMBA_STATE_PARA_JUMPING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			}
		}
	}
	else if (dynamic_cast<CParaKoopa*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CParaKoopa* koopa = dynamic_cast<CParaKoopa*>(e->obj);
			switch (koopa->GetState())
			{
			case KOOPA_STATE_PARA:
				koopa->SetState(KOOPA_STATE_WALKING);
				break;
			case KOOPA_STATE_WALKING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SPINNING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			}
		}
	}
	else if (dynamic_cast<CGoldBrick*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
			e->obj->Delete();
	}
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
	{
		CQuestionBrick* qb = dynamic_cast<CQuestionBrick*>(e->obj);
		if (state == KOOPA_STATE_SPINNING)
			if (qb->GetQuesID() == ID_ITEM_MUSHROOM)
			{
				mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(mr);
				qb->Delete();
			}
			else if (qb->GetQuesID() == ID_ITEM_COIN)
			{
				bc = new CBlockCoin(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(bc);
				mario->AddCoin();
				qb->Delete();
			}
			else if (qb->GetQuesID() == ID_ITEM_TANOOKI)
			{
				if (mario->GetLevel() >= MARIO_LEVEL_BIG)
				{
					tl = new CTanookiLeaf(qb->GetX() + 16, qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
					objects.push_back(tl);
				}
				else
				{
					mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
					objects.push_back(mr);
				}

				qb->Delete();
			}
	}
}
void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	
	if ((state == KOOPA_STATE_SHELL) && (GetTickCount64() - shell_start > KOOPA_SHELL_TIMEOUT))
	{
		SetState(KOOPA_STATE_SHAKING);
		shell_start = GetTickCount64();
	}
	if ((state == KOOPA_STATE_SHAKING) && (GetTickCount64() - shell_start > KOOPA_SHELL_TIMEOUT))
	{
		SetState(KOOPA_STATE_WALKING);
		shell_start = GetTickCount64();
	}
	
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
void CKoopa::Render()
{
	int aniId = -1;
	if (state == KOOPA_STATE_WALKING)
	{
		if (vx < 0)
			aniId = ID_ANI_KOOPA_WALKING;
		else aniId = ID_ANI_KOOPA_WALKING_RIGHT;
	}
	if (state == KOOPA_STATE_SHELL)
	{
		aniId = ID_ANI_KOOPA_SHELL;
	}
	if (state == KOOPA_STATE_SHAKING)
	{
		aniId = ID_ANI_KOOPA_SHAKING;
	}
	if (state == KOOPA_STATE_SPINNING)
	{
		aniId = ID_ANI_KOOPA_SPINNING;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
void CKoopa::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPA_STATE_WALKING:
		vx = KOOPA_WALKING_SPEED;
		y -= 10;
		DebugOut(L"Walking");
		break;
	case KOOPA_STATE_SPINNING:
		vx = KOOPA_SPINNING_SPEED;
		y -= 10;
		DebugOut(L"Spinning");
		break;
	case KOOPA_STATE_SHELL:
		shell_start = GetTickCount64();
		vx = 0;
		y -= 10;
		DebugOut(L"Shell");
		break;
	case KOOPA_STATE_SHAKING:
		shell_start = GetTickCount64();
		y -= 10;
		vx = 0;
		DebugOut(L"Shaking");
		break;
	}
}
CParaKoopa::CParaKoopa(float x, float y) : CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY/2;
	this->vx = KOOPA_WALKING_SPEED;
	jump_timer = GetTickCount64();
	shell_start = -1;
	SetState(KOOPA_STATE_PARA);
}
void CParaKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPA_STATE_WALKING || state == KOOPA_STATE_PARA)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
	else
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT_SHELL / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT_SHELL;
	}
}
void CParaKoopa::Render()
{
	int aniId = -1;
	if (state == KOOPA_STATE_PARA)
	{
		if (vx < 0)
			aniId = ID_ANI_KOOPA_PARA;
		else aniId = ID_ANI_KOOPA_PARA_RIGHT;
	}
	if (state == KOOPA_STATE_WALKING)
	{
		if (vx < 0)
			aniId = ID_ANI_KOOPA_WALKING;
		else aniId = ID_ANI_KOOPA_WALKING_RIGHT;
	}
	if (state == KOOPA_STATE_SHELL)
	{
		aniId = ID_ANI_KOOPA_SHELL;
	}
	if (state == KOOPA_STATE_SHAKING)
	{
		aniId = ID_ANI_KOOPA_SHAKING;
	}
	if (state == KOOPA_STATE_SPINNING)
	{
		aniId = ID_ANI_KOOPA_SPINNING;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
void CParaKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	if (vy < 0)
	{
		isOnPlatform = false;
	}
};
void CParaKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	isOnPlatform = false;
	if (!e->obj->IsBlocking()) return;
	if (e->ny != 0)
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
	if (dynamic_cast<CKoopa*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
			switch (koopa->GetState())
			{
			case KOOPA_STATE_WALKING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SPINNING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			}
		}
	}
	else if (dynamic_cast<CGoomba*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				goomba->SetState(GOOMBA_STATE_DIE);
				DebugOut(L"A Goomba killed");
			}
		}
	}
	else if (dynamic_cast<CParaGoomba*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CParaGoomba* goomba = dynamic_cast<CParaGoomba*>(e->obj);
			switch (goomba->GetState())
			{
			case GOOMBA_STATE_WALKING:
				goomba->SetState(GOOMBA_STATE_DIE);
				break;
			case GOOMBA_STATE_PARA_WALKING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			case GOOMBA_STATE_PARA_SHORT_JUMPING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			case GOOMBA_STATE_PARA_JUMPING:
				goomba->SetState(GOOMBA_STATE_WALKING);
				break;
			}
		}
	}
	else if (dynamic_cast<CParaKoopa*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
		{
			CParaKoopa* koopa = dynamic_cast<CParaKoopa*>(e->obj);
			switch (koopa->GetState())
			{
			case KOOPA_STATE_PARA:
				koopa->SetState(KOOPA_STATE_WALKING);
				break;
			case KOOPA_STATE_WALKING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			case KOOPA_STATE_SHELL:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SHAKING:
				koopa->SetState(KOOPA_STATE_SPINNING);
				break;
			case KOOPA_STATE_SPINNING:
				koopa->SetState(KOOPA_STATE_SHELL);
				break;
			}
		}
	}
	else if (dynamic_cast<CGoldBrick*>(e->obj))
	{
		if (state == KOOPA_STATE_SPINNING)
			e->obj->Delete();
	}
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
	{
		CQuestionBrick* qb = dynamic_cast<CQuestionBrick*>(e->obj);
		if (state == KOOPA_STATE_SPINNING)
			if (qb->GetQuesID() == ID_ITEM_MUSHROOM)
			{
				mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(mr);
				qb->Delete();
			}
			else if (qb->GetQuesID() == ID_ITEM_COIN)
			{
				bc = new CBlockCoin(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
				objects.push_back(bc);
				mario->AddCoin();
				qb->Delete();
			}
			else if (qb->GetQuesID() == ID_ITEM_TANOOKI)
			{
				if (mario->GetLevel() >= MARIO_LEVEL_BIG)
				{
					tl = new CTanookiLeaf(qb->GetX() + 16, qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
					objects.push_back(tl);
				}
				else
				{
					mr = new CMushroom(qb->GetX(), qb->GetY() - BRICK_BBOX_HEIGHT / 2 - SHROOM_BBOX_HEIGHT / 2 - 1.5f);
					objects.push_back(mr);
				}

				qb->Delete();
			}
	}
}
void CParaKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	if ((state == KOOPA_STATE_PARA) && (isOnPlatform) && (GetTickCount64() - jump_timer > KOOPA_JUMP_TIMEOUT))
	{
		vy = -KOOPA_PARA_SPEED;
	}
	if ((state == KOOPA_STATE_SHELL) && (GetTickCount64() - shell_start > KOOPA_SHELL_TIMEOUT))
	{
		SetState(KOOPA_STATE_SHAKING);
		shell_start = GetTickCount64();
	}
	if ((state == KOOPA_STATE_SHAKING) && (GetTickCount64() - shell_start > KOOPA_SHELL_TIMEOUT))
	{
		SetState(KOOPA_STATE_WALKING);
		shell_start = GetTickCount64();
	}
	
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
void CParaKoopa::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPA_STATE_PARA:
		vy = -KOOPA_PARA_SPEED;
		ay = KOOPA_GRAVITY/2;
		//y -= 10;
		DebugOut(L"Jumping_");
		break;
	case KOOPA_STATE_WALKING:
		vx = KOOPA_WALKING_SPEED;
		y -= 10;
		DebugOut(L"Walking");
		break;
	case KOOPA_STATE_SPINNING:
		vx = KOOPA_SPINNING_SPEED;
		y -= 10;
		DebugOut(L"Spinning");
		break;
	case KOOPA_STATE_SHELL:
		shell_start = GetTickCount64();
		vx = 0;
		y -= 10;
		DebugOut(L"Shell");
		break;
	case KOOPA_STATE_SHAKING:
		shell_start = GetTickCount64();
		y -= 10;
		vx = 0;
		DebugOut(L"Shaking");
		break;
	}
}
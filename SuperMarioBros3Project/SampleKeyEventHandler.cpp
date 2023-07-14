#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"

extern CMario* mario;
extern void Reload();

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT);
		break;
	case DIK_S:
		if (mario->GetLevel() == MARIO_LEVEL_TANOOKI)
		{
			if (!mario->onPlat())
				mario->SetState(MARIO_STATE_SLOWDROP);
			else if ((GetTickCount64() - mario->GetRuntime() > MARIO_KICKSTART_TIME) && (mario->GetRuntime() != -1))
				mario->SetState(MARIO_STATE_FLY);
			else
				mario->SetState(MARIO_STATE_JUMP);
		}
		else
			mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_1:
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		mario->SetLevel(MARIO_LEVEL_TANOOKI);
		break;
	case DIK_R: // reset
		Reload();
		break;
	
	case DIK_D:
		if (mario->GetLevel() == MARIO_LEVEL_TANOOKI)
			mario->SetState(MARIO_STATE_TAILFLICK);
		break;
	case DIK_SPACE:
		if (mario->GetX() >= 6000.0f)
			mario->SetPosition(20.0f, 10.0f);
		break;
	}
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	switch (KeyCode)
	{

	case DIK_S:
		mario->SetState(MARIO_STATE_RELEASE_JUMP);
		break;
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT_RELEASE);
		break;
	}
}

void CSampleKeyHandler::KeyState(BYTE *states)
{
	CGame* game = CGame::GetInstance();
	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (game->IsKeyDown(DIK_A))
			mario->SetState(MARIO_STATE_RUNNING_RIGHT);
		else
			mario->SetState(MARIO_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (game->IsKeyDown(DIK_A))
			mario->SetState(MARIO_STATE_RUNNING_LEFT);
		else
			mario->SetState(MARIO_STATE_WALKING_LEFT);
	}
	else
		mario->SetState(MARIO_STATE_IDLE);
}
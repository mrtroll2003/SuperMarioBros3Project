/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102
	
	SAMPLE 04 - COLLISION

	This sample illustrates how to:

		1/ Implement SweptAABB algorithm between moving objects
		2/ Implement a simple (yet effective) collision frame work, applying on Mario, Brick, Goomba & Coin

	Key functions: 
		CCollision::SweptAABB
		CCollision::SweptAABBEx
		CCollision::Scan
		CCollision::Filter
		CCollision::Process

		CGameObject::GetBoundingBox
		
================================================================ */

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <list>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "Animation.h"
#include "Animations.h"

#include "Mario.h"
#include "Brick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Coin.h"
#include "Platform.h"
#include "Environment.h"

#include "SampleKeyEventHandler.h"

#include "AssetIDs.h"

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"FinalProject"
#define WINDOW_ICON_PATH L"mario.ico"

#define BACKGROUND_COLOR D3DXCOLOR(146.0f/255, 237.0f/255, 235.0f/255, 0.0f)

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


#define TEXTURES_DIR L"textures"
#define TEXTURE_PATH_MARIO TEXTURES_DIR "\\mario_transparent.png"
#define TEXTURE_PATH_MISC TEXTURES_DIR "\\misc_transparent.png"
#define TEXTURE_PATH_ENEMY TEXTURES_DIR "\\enemies_transparent.png"
#define TEXTURE_PATH_ENVIRONMENT TEXTURES_DIR "\\environment_transparent.png"
#define TEXTURE_PATH_BBOX TEXTURES_DIR "\\bbox.png"

CGame *game;
CMario *mario;

list<LPGAMEOBJECT> objects;

CSampleKeyHandler * keyHandler; 

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
void LoadAssetsEnvironment()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnvi = textures->Get(ID_TEX_ENVI);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 1, 1, 78, 1 + 131, 78 + 67, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 2, 74, 1, 74 + 51, 1 + 24, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 3, 54, 6, 54 + 17, 6 + 17, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 4, 135, 69, 224, 150, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 5, 234, 37, 234 + 217, 37 + 113, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 6, 129, 1, 129 + 33, 25, texEnvi);
	LPANIMATION ani;

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 1);
	animations->Add(ID_ANI_WSB, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 2);
	animations->Add(ID_ANI_DCLOUD, ani);
	
	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 3);
	animations->Add(ID_ANI_BUSH, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 4);
	animations->Add(ID_ANI_ENVIRONMENT+400, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 5);
	animations->Add(ID_ANI_QUAD_PLAT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 6);
	animations->Add(ID_ANI_SCLOUD, ani);
}
void LoadAssetsMario()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMario = textures->Get(ID_TEX_MARIO);

	// IDLE
	sprites->Add(ID_SPRITE_MARIO_BIG_IDLE_RIGHT + 1, 246,154,259,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_IDLE_LEFT + 1, 186,154,199,181, texMario);

	// WALKING LEFT
	sprites->Add(ID_SPRITE_MARIO_BIG_WALKING_RIGHT + 2, 275,154,290,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_WALKING_RIGHT + 3, 304,154,321,181, texMario);

	// WALKING RIGHT
	sprites->Add(ID_SPRITE_MARIO_BIG_WALKING_LEFT + 2, 155,154,170,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_WALKING_LEFT + 3, 125,154,140,181, texMario);

	// RUNNING RIGHT 
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 1, 334,154,355,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 2, 334,154,355,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 3, 392,154,413,181, texMario);

	// RUNNING LEFT
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 1, 91,154,112,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 2, 65,154,86,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 3, 34,154,55,181, texMario);

	// JUMP WALK RIGHT & LEFT 
	sprites->Add(ID_SPRITE_MARIO_BIG_JUMP_WALK_RIGHT + 1, 395,275,412,302, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_JUMP_WALK_LEFT + 1, 35,275,52,302, texMario);

	// JUMP RUN RIGHT & LEFT 
	sprites->Add(ID_SPRITE_MARIO_BIG_JUMP_RUN_RIGHT + 1, 394,195,413,222, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_JUMP_RUN_LEFT + 1, 35,195,52,222, texMario);

	// SIT RIGHT/LEFT
	sprites->Add(ID_SPRITE_MARIO_BIG_SIT_RIGHT + 1, 426,239,441,256, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_SIT_LEFT + 1, 5,239,20,256, texMario);

	// BRACING RIGHT/LEFT
	sprites->Add(ID_SPRITE_MARIO_BIG_BRACE_RIGHT + 1, 425,154,442,181, texMario);
	sprites->Add(ID_SPRITE_MARIO_BIG_BRACE_LEFT + 1, 5,154,22,181, texMario);

	// MARIO DIE
	sprites->Add(ID_SPRITE_MARIO_DIE + 1, 215, 120, 231, 135, texMario);

	// SMALL MARIO 
	sprites->Add(ID_SPRITE_MARIO_SMALL_IDLE_RIGHT + 1, 247, 0, 247 + 15, 15, texMario);			
	sprites->Add(ID_SPRITE_MARIO_SMALL_IDLE_LEFT + 1, 187, 0, 187 + 15, 15, texMario);			

	sprites->Add(ID_SPRITE_MARIO_SMALL_WALKING_RIGHT + 2, 275, 0, 290, 15, texMario);			
	sprites->Add(ID_SPRITE_MARIO_SMALL_WALKING_RIGHT + 3, 306, 0, 321, 15, texMario); 

	sprites->Add(ID_SPRITE_MARIO_SMALL_WALKING_LEFT + 2, 155, 0, 170, 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_WALKING_LEFT + 3, 125, 0, 140, 15, texMario);

	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 1, 275, 0, 275 + 15, 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 2, 306, 0, 306 + 15, 15, texMario); 
	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 3, 335, 0, 335 + 15, 15, texMario);

	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 1, 155, 0, 155 + 15, 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 2, 125, 0, 125 + 15, 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 3, 95, 0, 95 + 15, 15, texMario);

	sprites->Add(ID_SPRITE_MARIO_SMALL_BRACE_LEFT + 1, 6, 0, 6 + 15, 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_BRACE_RIGHT + 1, 426, 0, 426 + 15, 15, texMario);

	sprites->Add(ID_SPRITE_MARIO_SMALL_JUMP_WALK_LEFT + 1, 35, 80, 35 + 15, 80 + 15, texMario);		
	sprites->Add(ID_SPRITE_MARIO_SMALL_JUMP_WALK_RIGHT + 1, 395, 80, 395 + 15, 80 + 15, texMario);

	sprites->Add(ID_SPRITE_MARIO_SMALL_JUMP_RUN_LEFT + 1, 65, 40, 65 + 15, 40 + 15, texMario);
	sprites->Add(ID_SPRITE_MARIO_SMALL_JUMP_RUN_RIGHT + 1, 365, 40, 365 + 15, 40 + 15, texMario);

	LPANIMATION ani;

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_IDLE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_IDLE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_IDLE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_IDLE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_IDLE_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_BIG_WALKING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_BIG_WALKING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_WALKING_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_IDLE_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_BIG_WALKING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_BIG_WALKING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_WALKING_LEFT, ani);

	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_RUNNING_RIGHT, ani);

	// Mario runs faster hence animation speed should be faster
	ani = new CAnimation(50);	
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_BIG_RUNNING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_RUNNING_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_JUMP_WALK_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_JUMP_WALK_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_JUMP_WALK_LEFT + 1);
	animations->Add(ID_ANI_MARIO_JUMP_WALK_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_JUMP_RUN_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_JUMP_RUN_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_JUMP_RUN_LEFT + 1);
	animations->Add(ID_ANI_MARIO_JUMP_RUN_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_SIT_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_SIT_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_SIT_LEFT + 1);
	animations->Add(ID_ANI_MARIO_SIT_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_BRACE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_BRACE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_BIG_BRACE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_BRACE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_DIE + 1);
	animations->Add(ID_ANI_MARIO_DIE, ani);

	//
	// SMALL MARIO 
	//
	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_IDLE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_IDLE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_IDLE_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_SMALL_WALKING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_SMALL_WALKING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_SMALL_WALKING_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_IDLE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_IDLE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_IDLE_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_SMALL_WALKING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_SMALL_WALKING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_SMALL_WALKING_LEFT, ani);

	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_SMALL_RUNNING_RIGHT, ani);

	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_SMALL_RUNNING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_SMALL_RUNNING_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_BRACE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_BRACE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_BRACE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_BRACE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_JUMP_WALK_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_JUMP_WALK_LEFT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_JUMP_RUN_LEFT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_SMALL_JUMP_RUN_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT, ani);
}

void LoadAssetsGoomba()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnemy = textures->Get(ID_TEX_ENEMY);

	sprites->Add(ID_SPRITE_GOOMBA_WALK + 1, 4, 13, 22, 30, texEnemy);  
	sprites->Add(ID_SPRITE_GOOMBA_WALK + 2, 24, 13, 42, 30, texEnemy); 

	sprites->Add(ID_SPRITE_GOOMBA_DIE + 1, 44, 19, 62, 30, texEnemy);

	LPANIMATION ani = new CAnimation(100);
	ani->Add(ID_SPRITE_GOOMBA_WALK + 1);
	ani->Add(ID_SPRITE_GOOMBA_WALK + 2);
	animations->Add(ID_ANI_GOOMBA_WALKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_GOOMBA_DIE + 1);
	animations->Add(ID_ANI_GOOMBA_DIE, ani);

}
void LoadAssetsKoopa()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnemy = textures->Get(ID_TEX_ENEMY);

	sprites->Add(ID_SPRITE_KOOPA_WALKING + 1, 5, 129, 5 + 17, 129 + 27, texEnemy);
	sprites->Add(ID_SPRITE_KOOPA_WALKING + 2, 27, 129, 27 + 17, 129 + 27, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_SHELL + 1, 49, 138, 49 + 17, 138 + 17, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 1, 49, 138, 49 + 17, 138 + 17, texEnemy);
	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 2, 49, 138, 49 + 17, 138 + 17, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_SHAKING + 1, 49, 138, 49 + 17, 138 + 17, texEnemy);
	sprites->Add(ID_SPRITE_KOOPA_SHAKING + 2, 49, 138, 49 + 17, 138 + 17, texEnemy);

	LPANIMATION ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_WALKING + 1);
	ani->Add(ID_SPRITE_KOOPA_WALKING + 2);
	animations->Add(ID_ANI_KOOPA_WALKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_SHELL + 1);
	animations->Add(ID_ANI_KOOPA_SHELL, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_SHAKING + 1);
	ani->Add(ID_SPRITE_KOOPA_SHAKING + 2);
	animations->Add(ID_ANI_KOOPA_SHAKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_SPINNING + 1);
	ani->Add(ID_SPRITE_KOOPA_SPINNING + 2);
	animations->Add(ID_ANI_KOOPA_SPINNING, ani);
}
void LoadAssetsBrick()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);
	LPTEXTURE texEnvi = textures->Get(ID_TEX_ENVI);
	sprites->Add(ID_SPRITE_BRICK + 1, 372, 153, 372 + 15, 153 + 15, texMisc);

	sprites->Add(ID_SPRITE_GOLD_BRICK + 1, 299, 134, 299 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 2, 317, 134, 317 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 3, 335, 134, 335 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 4, 353, 134, 353 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 5, 371, 134, 371 + 15, 134 + 15, texMisc);

	sprites->Add(ID_SPRITE_INVI_BRICK + 1, 283, 244, 283 + 15, 244 + 15, texMisc);

	sprites->Add(ID_SPRITE_PIPE + 1, 4, 27, 37, 76, texEnvi);
	//for debugging
	//sprites->Add(ID_SPRITE_INVI_BRICK + 1, 300, 135, 300 + 15, 135 + 15, texMisc);

	sprites->Add(ID_SPRITE_QUESTION_BRICK + 1, 300, 117, 300 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 2, 318, 117, 318 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 3, 336, 117, 336 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 4, 354, 117, 354 + 15, 117 + 15, texMisc);

	sprites->Add(ID_SPRITE_NULL_BRICK + 1, 205, 7, 205 + 15, 7 + 15, texEnvi);

	sprites->Add(ID_SPRITE_BASE_BRICK + 1, 4, 7, 4 + 49, 7 + 15, texEnvi);
	LPANIMATION ani;
	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_BRICK + 1);
	animations->Add(ID_ANI_BRICK, ani);

	ani = new CAnimation(100);

	ani->Add(ID_SPRITE_GOLD_BRICK + 1);
	ani->Add(ID_SPRITE_GOLD_BRICK + 2);
	ani->Add(ID_SPRITE_GOLD_BRICK + 3);
	ani->Add(ID_SPRITE_GOLD_BRICK + 4);
	ani->Add(ID_SPRITE_GOLD_BRICK + 5);
	animations->Add(ID_ANI_GOLD_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_INVI_BRICK + 1);
	animations->Add(ID_ANI_INVI_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_BASE_BRICK + 1);
	animations->Add(ID_ANI_BASE_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_PIPE + 1);
	animations->Add(ID_ANI_PIPE, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_QUESTION_BRICK + 1, 1000);
	ani->Add(ID_SPRITE_QUESTION_BRICK + 2);
	ani->Add(ID_SPRITE_QUESTION_BRICK + 3);
	ani->Add(ID_SPRITE_QUESTION_BRICK + 4);
	animations->Add(ID_ANI_QUESTION_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_NULL_BRICK + 1);
	animations->Add(ID_ANI_NULL_BRICK, ani);
}
void LoadAssetsCoin()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);

	sprites->Add(ID_SPRITE_COIN + 1, 303, 99, 303 + 9, 99 + 15, texMisc);
	sprites->Add(ID_SPRITE_COIN + 2, 321, 99, 321 + 9, 99 + 15, texMisc);
	sprites->Add(ID_SPRITE_COIN + 3, 338, 99, 338 + 9, 99 + 15, texMisc);

	LPANIMATION ani = new CAnimation(300);
	ani->Add(ID_SPRITE_COIN + 1);
	ani->Add(ID_SPRITE_COIN + 2);
	ani->Add(ID_SPRITE_COIN + 3);
	animations->Add(ID_ANI_COIN, ani);
}

void LoadAssetsOther()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(ID_SPRITE_CLOUD_BEGIN, 390, 117, 390 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_CLOUD_MIDDLE, 408, 117, 408 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_CLOUD_END, 426, 117, 426 + 15, 117 + 15, texMisc);

}

/*
	Load all game resources
	In this example: load textures, sprites, animations and mario object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures* textures = CTextures::GetInstance();

	textures->Add(ID_TEX_MARIO, TEXTURE_PATH_MARIO);
	textures->Add(ID_TEX_ENEMY, TEXTURE_PATH_ENEMY);
	textures->Add(ID_TEX_MISC, TEXTURE_PATH_MISC);
	textures->Add(ID_TEX_ENVI, TEXTURE_PATH_ENVIRONMENT);
	//textures->Add(ID_TEX_BBOX, TEXTURE_PATH_BBOX);

	LoadAssetsEnvironment();
	LoadAssetsMario();
	LoadAssetsGoomba();
	LoadAssetsKoopa();
	LoadAssetsBrick();
	LoadAssetsCoin();
	LoadAssetsOther();
}

void ClearScene()
{ 
	list<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		delete (*it);
	}
	objects.clear();
}

#define MARIO_START_X 20.0f
#define MARIO_START_Y 10.0f

#define BRICK_X 0.0f
#define GOOMBA_X 200.0f
#define COIN_X 100.0f

#define BRICK_Y GROUND_Y + 20.0f
#define NUM_BRICKS 15

/*
* Reload all objects of current scene 
* NOTE: super bad way to build a scene! We need to load a scene from data instead of hard-coding like this 
*/
void Reload()
{
	ClearScene();
	//Environment background
	CWideSquaredBush* b = new CWideSquaredBush(68.5f, BRICK_Y - 20.0f);
	objects.push_back(b);
	CDoubleCloud* c = new CDoubleCloud(152.5f, 50.0f);
	objects.push_back(c);
	for (int i = 1; i < 4; i++)
	{
		CSingleBush* sb = new CSingleBush(150.0f + i * 18.0f, BRICK_Y -6.0f);
		objects.push_back(sb);
	}
	CDColorPlat* d = new CDColorPlat(300.0f, BRICK_Y -38.0f);
	objects.push_back(d);
	for (int i = 0; i <3; i++)
	{
		CInviBase* b = new CInviBase(264.9f + i * BRICK_WIDTH, BRICK_Y - 37.5f);
		objects.push_back(b);
	}
	for (int i = 0; i < 3; i++)
	{
		CInviBase* b = new CInviBase(296.5f + i * BRICK_WIDTH, BRICK_Y - 69.5f);
		objects.push_back(b);
	}
	CPipe* p = new CPipe(400.0f, BRICK_Y - 21.0f);
	objects.push_back(p);
	CSingleCloud* sc1 = new CSingleCloud(350.0f, 50.0f);
	objects.push_back(sc1);
	//2nd color plat
	CQuadPlat* qp1 = new CQuadPlat(550.0f, BRICK_Y - 53.0f);
	objects.push_back(qp1);
	for (int i = 0; i < 5; i++)
	{
		CInviBase* b = new CInviBase((550.0f - 96.0f) + i * (BRICK_WIDTH-1), BRICK_Y - 36.5f);
		objects.push_back(b);
	}
	for (int i = 0; i < 4; i++)
	{
		CInviBase* b = new CInviBase((550.0f - 36.0f) + i * BRICK_WIDTH, BRICK_Y - 69.5f);
		objects.push_back(b);
	}
	for (int i = 0; i < 4; i++)
	{
		CInviBase* b = new CInviBase((550.0f + 15.0f) + i * (BRICK_WIDTH-1), BRICK_Y - 101.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 6; i++)
	{
		CInviBase* b = new CInviBase((550.0f + 15.5f) + i * (BRICK_WIDTH - 1), BRICK_Y - 20.5f);
		objects.push_back(b);
	}
	// Main ground
	for (int i = 0; i < NUM_BRICKS; i++)
	{
		CBaseBrick* b = new CBaseBrick(i * BRICK_WIDTH * 3.0f, BRICK_Y+10.0f);
		objects.push_back(b);
	}

	// World boundaries for Mario
	for (int i = 0; i < 10; i++)
	{
		CInviBrick* b = new CInviBrick(0, BRICK_Y - i * BRICK_WIDTH);
		objects.push_back(b);
	}

	mario = new CMario(MARIO_START_X, MARIO_START_Y);
	objects.push_back(mario);

	// COINS 
	for (int i = 0; i < 10; i++)
	{
		CCoin* c = new CCoin(COIN_X + i * (COIN_WIDTH * 2), GROUND_Y - 96.0f);
		objects.push_back(c);
	}
	// Question block
	for (int i = 0; i < 2; i++)
	{
		CNullBrick* nb = new CNullBrick(186.0f + i * 16.0f, BRICK_Y - 53.0f);
		objects.push_back(nb);
		CQuestionBrick* qb = new CQuestionBrick(186.0f + i * 16.0f, BRICK_Y - 53.0f, ID_ITEM_COIN);
		objects.push_back(qb);
	}
	//put null brick under question brick, delete the qb on impact and reveal the nb
	CNullBrick* nb1 = new CNullBrick(246.0f, BRICK_Y - 99.0f);
	objects.push_back(nb1);
	CNullBrick* nb2 = new CNullBrick(262.0f, BRICK_Y - 99.0f);
	objects.push_back(nb2);
	CNullBrick* nb3 = new CNullBrick((550.0f - 96.0f + 16.0f), BRICK_Y - (69.5f + 16.0f));
	objects.push_back(nb3);
	CQuestionBrick* qb1 = new CQuestionBrick(246.0f, BRICK_Y - 99.0f, ID_ITEM_COIN);
	objects.push_back(qb1);
	CQuestionBrick* qb2 = new CQuestionBrick(262.0f, BRICK_Y - 99.0f, ID_ITEM_MUSHROOM);
	objects.push_back(qb2);
	CQuestionBrick* qb3 = new CQuestionBrick((550.0f - 96.0f + 16.0f), BRICK_Y - (69.5f+16.0f), ID_ITEM_COIN);
	objects.push_back(qb3);

	CGoomba* goomba = new CGoomba(GOOMBA_X, GROUND_Y - 120.0f);
	objects.push_back(goomba);
	CKoopa* koopa = new CKoopa(150.0f, 40.0f);
	objects.push_back(koopa);
}

bool IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }

void PurgeDeletedObjects()
{
	list<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		LPGAMEOBJECT o = *it;
		if (o->IsDeleted())
		{
			delete o;
			*it = NULL;
		}
	}
	objects.erase(
		std::remove_if(objects.begin(), objects.end(), IsGameObjectDeleted),
		objects.end());
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 
	vector<LPGAMEOBJECT> coObjects;
	list<LPGAMEOBJECT>::iterator i;
	for (i = objects.begin(); i != objects.end(); ++i)
	{
		coObjects.push_back(*i);
	}

	for (i = objects.begin(); i != objects.end(); ++i)
	{
		(*i)->Update(dt,&coObjects);
	}

	PurgeDeletedObjects();

	// Update camera to follow mario
	float cx, cy;
	mario->GetPosition(cx, cy);

	cx -= SCREEN_WIDTH / 2;
	cy = 0;
	//cy -= SCREEN_HEIGHT / 2;

	if (cx < 0) cx = 0;

	CGame::GetInstance()->SetCamPos(cx, cy);
}

/*
	Render a frame 
*/
void Render()
{
	CGame* g = CGame::GetInstance();

	ID3D10Device* pD3DDevice = g->GetDirect3DDevice();
	IDXGISwapChain* pSwapChain = g->GetSwapChain();
	ID3D10RenderTargetView* pRenderTargetView = g->GetRenderTargetView();
	ID3DX10Sprite* spriteHandler = g->GetSpriteHandler();

	pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);

	spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

	FLOAT NewBlendFactor[4] = { 0,0,0,0 };
	pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

	list<LPGAMEOBJECT>::iterator i;
	for (i = objects.begin(); i != objects.end(); ++i)
	{
		(*i)->Render();
	}

	spriteHandler->End();
	pSwapChain->Present(0, 0);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = (HICON)LoadImage(hInstance, WINDOW_ICON_PATH, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	ULONGLONG frameStart = GetTickCount64();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ULONGLONG now = GetTickCount64();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = (DWORD)(now - frameStart);

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();			
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);	
	}

	return 1;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	SetDebugWindow(hWnd);

	game = CGame::GetInstance();
	game->Init(hWnd, hInstance);

	keyHandler = new CSampleKeyHandler();
	game->InitKeyboard(keyHandler);

	LoadResources();
	Reload();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}
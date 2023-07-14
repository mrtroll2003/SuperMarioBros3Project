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
#include "Firetrap.h"
#include "Coin.h"
#include "Platform.h"
#include "Environment.h"
#include "PowerUps.h"

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
#define TEXTURE_PATH_ENEMY_FLIP TEXTURES_DIR "\\enemies_transparent_flipped.png"
#define TEXTURE_PATH_ENEMY_2 TEXTURES_DIR "\\enemies2_transparent.png"
#define TEXTURE_PATH_ENEMY_2_FLIP TEXTURES_DIR "\\enemies2_transparent_flipped.png"
#define TEXTURE_PATH_ENVIRONMENT TEXTURES_DIR "\\environment_transparent.png"
#define TEXTURE_PATH_MAP TEXTURES_DIR "\\map_layout_transparent.png"
#define TEXTURE_PATH_MAIN TEXTURES_DIR "\\main_screen.png"
#define TEXTURE_PATH_BBOX TEXTURES_DIR "\\bbox.png"

CGame *game;
CMario *mario = NULL;
CMushroom* mr = NULL;
CBlockCoin* bc = NULL;
CCoin* cgb = NULL;
CTanookiLeaf* tl = NULL;

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
	LPTEXTURE texMap = textures->Get(ID_TEX_MAP);
	LPTEXTURE texMain = textures->Get(ID_TEX_MAIN);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 1, 1, 78, 1 + 131, 78 + 67, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 2, 75, 2, 75 + 47, 2 + 22, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 3, 55, 7, 55 + 15, 6 + 16, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 4, 135, 69, 224, 150, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 5, 234, 37, 234 + 217, 37 + 113, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 6, 130, 1, 130 + 31, 23, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 7, 536, 647, 536 + 63, 647 + 49, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 8, 153, 153, 153 + 185, 153 + 97, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 9, 460, 5, 547, 148, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 10, 558, 19, 655, 148, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 11, 126, 306, 213, 345, texEnvi);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 12, 2480, 0, 2575, 400, texMap);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 13, 2488, 0, 2575, 400, texMap);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 14, 2576, 319, 2656, 416, texMap);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 15, 2752, 367, 2800, 416, texMap);

	sprites->Add(ID_SPRITE_ENVIRONMENT + 16, 0, 0, 170, 159, texMain);
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

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 7);
	animations->Add(ID_ANI_LB, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 8);
	animations->Add(ID_ANI_TRIP_PLAT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 9);
	animations->Add(ID_ANI_HIGH_PLAT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 10);
	animations->Add(ID_ANI_HB, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 11);
	animations->Add(ID_ANI_SKY_PLAT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 12);
	animations->Add(ID_ANI_SPIKE_BLACK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 13);
	animations->Add(ID_ANI_PLAIN_BLACK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 14);
	animations->Add(ID_ANI_BBB, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 15);
	animations->Add(ID_ANI_SBB, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_ENVIRONMENT + 16);
	animations->Add(ID_ANI_MAIN, ani);
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

	//tanooki
	// IDLE
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_RIGHT + 1, 243, 634, 243 + 20, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_LEFT + 1, 182, 634, 182 + 20, 634 + 27, texMario);

	// WALKING LEFT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_RIGHT + 2, 272, 634, 272 + 21, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_RIGHT + 3, 303, 634, 303 + 21, 634 + 27, texMario);

	// WALKING RIGHT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_LEFT + 2, 152, 634, 152 + 21, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_LEFT + 3, 122, 634, 122 + 21, 634 + 27, texMario);

	// RUNNING RIGHT 
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 1, 331, 634, 354, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 2, 362, 634, 384, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 3, 393, 634, 413, 634 + 27, texMario);

	// RUNNING LEFT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 1, 91, 634, 114, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 2, 61, 634, 83, 634 + 27, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 3, 32, 634, 52, 634 + 27, texMario);

	// JUMP WALK RIGHT & LEFT 
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_WALK_RIGHT + 1, 392, 754, 414, 781, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_WALK_LEFT + 1, 31, 754, 53, 781, texMario);

	// JUMP RUN RIGHT & LEFT 
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_RUN_RIGHT + 1, 361, 754, 384, 781, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_RUN_LEFT + 1, 61, 754, 84, 781, texMario);

	// SIT RIGHT/LEFT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SIT_RIGHT + 1, 422, 719, 443, 736, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SIT_LEFT + 1, 2, 719, 23, 736, texMario);

	// BRACING RIGHT/LEFT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_BRACE_RIGHT + 1, 425, 633, 440, 662, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_BRACE_LEFT + 1, 5, 633, 20, 662, texMario);

	//FLY LEFT/RIGHT
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_FLY_RIGHT + 1, 331, 714, 354, 740, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_FLY_RIGHT + 2, 361, 714, 384, 741, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_FLY_LEFT + 1, 91, 714, 114, 740, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_FLY_LEFT + 2, 61, 714, 84, 741, texMario);

	//
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_RIGHT + 1, 272, 674, 294, 701, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_RIGHT + 2, 302, 674, 324, 701, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_LEFT + 1, 121, 674, 121 + 22, 701, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_LEFT + 2, 151, 674, 151 + 22, 701, texMario);

	//TAIL FLICK
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 1, 243, 634, 263, 661, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 2, 215, 674, 230, 701, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 3, 182, 634, 202, 661, texMario);
	sprites->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 4, 215, 633, 230, 661, texMario);

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
	
	//tanooki
	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_IDLE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_IDLE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_TANOOKI_WALKING_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_IDLE_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_WALKING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_TANOOKI_WALKING_LEFT, ani);

	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 2);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_RIGHT + 3);
	animations->Add(ID_ANI_MARIO_TANOOKI_RUNNING_RIGHT, ani);

	// Mario runs faster hence animation speed should be faster
	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 2);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_RUNNING_LEFT + 3);
	animations->Add(ID_ANI_MARIO_TANOOKI_RUNNING_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_WALK_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_JUMP_WALK_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_WALK_LEFT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_JUMP_WALK_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_RUN_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_JUMP_RUN_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_JUMP_RUN_LEFT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_JUMP_RUN_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SIT_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_SIT_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SIT_LEFT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_SIT_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_BRACE_RIGHT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_BRACE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_BRACE_LEFT + 1);
	animations->Add(ID_ANI_MARIO_TANOOKI_BRACE_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_FLY_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_FLY_RIGHT + 2);
	animations->Add(ID_ANI_MARIO_TANOOKI_FLY_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_FLY_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_FLY_LEFT + 2);
	animations->Add(ID_ANI_MARIO_TANOOKI_FLY_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_RIGHT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_RIGHT + 2);
	animations->Add(ID_ANI_MARIO_TANOOKI_SLOWDROP_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_LEFT + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_SLOWDROP_LEFT + 2);
	animations->Add(ID_ANI_MARIO_TANOOKI_SLOWDROP_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 1);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 2);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 3);
	ani->Add(ID_SPRITE_MARIO_TANOOKI_TAILFLICK + 4);
	animations->Add(ID_ANI_MARIO_TANOOKI_TAILFLICK, ani);
}

void LoadAssetsGoomba()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnemy = textures->Get(ID_TEX_ENEMY);

	sprites->Add(ID_SPRITE_GOOMBA_WALK + 1, 4, 13, 21, 30, texEnemy);  
	sprites->Add(ID_SPRITE_GOOMBA_WALK + 2, 24, 13, 41, 30, texEnemy); 

	sprites->Add(ID_SPRITE_PARAGOOMBA + 1, 64, 11, 83, 29, texEnemy);
	sprites->Add(ID_SPRITE_PARAGOOMBA + 2, 87, 11, 106, 29, texEnemy);
	sprites->Add(ID_SPRITE_PARAGOOMBA + 3, 110, 6, 129, 29, texEnemy);
	sprites->Add(ID_SPRITE_PARAGOOMBA + 4, 134, 6, 153, 29, texEnemy);

	sprites->Add(ID_SPRITE_GOOMBA_DIE + 1, 44, 19, 62, 30, texEnemy);

	LPANIMATION ani = new CAnimation(100);
	ani->Add(ID_SPRITE_GOOMBA_WALK + 1);
	ani->Add(ID_SPRITE_GOOMBA_WALK + 2);
	animations->Add(ID_ANI_GOOMBA_WALKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_GOOMBA_DIE + 1);
	animations->Add(ID_ANI_GOOMBA_DIE, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_PARAGOOMBA + 1);
	ani->Add(ID_SPRITE_PARAGOOMBA + 2);
	animations->Add(ID_ANI_PARAGOOMBA_WALKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_PARAGOOMBA + 1);
	ani->Add(ID_SPRITE_PARAGOOMBA + 3, 300);
	ani->Add(ID_SPRITE_PARAGOOMBA + 2);
	ani->Add(ID_SPRITE_PARAGOOMBA + 4, 300);
	animations->Add(ID_ANI_PARAGOOMBA_AIR, ani);
}
void LoadAssetsKoopa()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnemy = textures->Get(ID_TEX_ENEMY);
	LPTEXTURE texEnemyFlip = textures->Get(ID_TEX_ENEMY_2_FLIP);
	LPTEXTURE texEnemyFlip2 = textures->Get(ID_TEX_ENEMY_FLIP);

	sprites->Add(ID_SPRITE_KOOPA_WALKING + 1, 5, 129, 5 + 17, 129 + 27, texEnemy);
	sprites->Add(ID_SPRITE_KOOPA_WALKING + 2, 27, 129, 27 + 17, 129 + 27, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_SHELL + 1, 49, 138, 49 + 17, 138 + 17, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 1, 544, 49, 544 + 15, 49 + 15, texEnemyFlip);
	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 2, 527, 49, 527 + 15, 49 + 15, texEnemyFlip);
	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 3, 512, 49, 512 + 15, 49 + 15, texEnemyFlip);
	sprites->Add(ID_SPRITE_KOOPA_SPINNING + 4, 497, 49, 497 + 15, 49 + 15, texEnemyFlip);

	sprites->Add(ID_SPRITE_KOOPA_SHAKING + 1, 49, 138, 49 + 17, 138 + 17, texEnemy);
	sprites->Add(ID_SPRITE_KOOPA_SHAKING + 2, 70, 138, 70 + 17, 138 + 17, texEnemy);

	sprites->Add(ID_SPRITE_KOOPA_WALKING_RIGHT + 1, 576, 52, 576 + 15, 51 + 25, texEnemyFlip);
	sprites->Add(ID_SPRITE_KOOPA_WALKING_RIGHT + 2, 560, 52, 560 + 15, 51 + 26, texEnemyFlip);

	sprites->Add(ID_SPRITE_PARAKOOPA + 1, 93, 130, 93 + 15, 130 + 25, texEnemy);
	sprites->Add(ID_SPRITE_PARAKOOPA + 2, 114, 129, 114 + 15, 130 + 25, texEnemy);
	sprites->Add(ID_SPRITE_PARAKOOPA + 3, 135, 129, 135 + 15, 130 + 25, texEnemy);
	sprites->Add(ID_SPRITE_PARAKOOPA + 4, 157, 128, 157 + 15, 130 + 25, texEnemy);

	sprites->Add(ID_SPRITE_PARAKOOPA_RIGHT + 1, 371, 130, 371 + 15, 130 + 25, texEnemyFlip2);
	sprites->Add(ID_SPRITE_PARAKOOPA_RIGHT + 2, 350, 130, 350 + 15, 130 + 25, texEnemyFlip2);
	sprites->Add(ID_SPRITE_PARAKOOPA_RIGHT + 3, 329, 129, 329 + 15, 130 + 25, texEnemyFlip2);
	sprites->Add(ID_SPRITE_PARAKOOPA_RIGHT + 4, 307, 128, 307 + 15, 130 + 25, texEnemyFlip2);

	LPANIMATION ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_WALKING + 1);
	ani->Add(ID_SPRITE_KOOPA_WALKING + 2);
	animations->Add(ID_ANI_KOOPA_WALKING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_KOOPA_WALKING_RIGHT + 1);
	ani->Add(ID_SPRITE_KOOPA_WALKING_RIGHT + 2);
	animations->Add(ID_ANI_KOOPA_WALKING_RIGHT, ani);

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
	ani->Add(ID_SPRITE_KOOPA_SPINNING + 3);
	ani->Add(ID_SPRITE_KOOPA_SPINNING + 4);
	animations->Add(ID_ANI_KOOPA_SPINNING, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_PARAKOOPA + 1);
	ani->Add(ID_SPRITE_PARAKOOPA + 2);
	ani->Add(ID_SPRITE_PARAKOOPA + 3);
	ani->Add(ID_SPRITE_PARAKOOPA + 4);
	animations->Add(ID_ANI_KOOPA_PARA, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_PARAKOOPA_RIGHT + 1);
	ani->Add(ID_SPRITE_PARAKOOPA_RIGHT + 2);
	ani->Add(ID_SPRITE_PARAKOOPA_RIGHT + 3);
	ani->Add(ID_SPRITE_PARAKOOPA_RIGHT + 4);
	animations->Add(ID_ANI_KOOPA_PARA_RIGHT, ani);
}
void LoadAssetsBrick()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);
	LPTEXTURE texEnvi = textures->Get(ID_TEX_ENVI);
	LPTEXTURE texMap = textures->Get(ID_TEX_MAP);
	sprites->Add(ID_SPRITE_BRICK + 1, 372, 153, 372 + 15, 153 + 15, texMisc);

	sprites->Add(ID_SPRITE_GOLD_BRICK + 1, 299, 134, 299 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 2, 317, 134, 317 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 3, 335, 134, 335 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 4, 353, 134, 353 + 15, 134 + 15, texMisc);
	sprites->Add(ID_SPRITE_GOLD_BRICK + 5, 371, 134, 371 + 15, 134 + 15, texMisc);

	sprites->Add(ID_SPRITE_CLOUD_BRICK + 1, 354, 243, 369, 258, texMisc);

	sprites->Add(ID_SPRITE_INVI_BRICK + 1, 283, 244, 283 + 15, 244 + 15, texMisc);

	sprites->Add(ID_SPRITE_PIPE + 1, 4, 27, 37, 76, texEnvi);
	//for debugging
	//sprites->Add(ID_SPRITE_INVI_BRICK + 1, 300, 135, 300 + 15, 135 + 15, texMisc);

	sprites->Add(ID_SPRITE_QUESTION_BRICK + 1, 300, 117, 300 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 2, 318, 117, 318 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 3, 336, 117, 336 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_QUESTION_BRICK + 4, 354, 117, 354 + 15, 117 + 15, texMisc);

	sprites->Add(ID_SPRITE_NULL_BRICK + 1, 205, 7, 205 + 15, 7 + 15, texEnvi);

	sprites->Add(ID_SPRITE_BASE_BRICK + 1, 4, 7, 4 + 47, 7 + 15, texEnvi);

	sprites->Add(ID_SPRITE_STRIPE_BRICK + 1, 167, 7, 167 + 15, 7 + 15, texEnvi);

	sprites->Add(ID_SPRITE_SECRET_BRICK + 1, 352, 739, 352 + 15, 739 + 15, texEnvi);
	
	sprites->Add(ID_SPRITE_BLACK_PIPE + 1, 2096, 464, 2096 + 32, 464 + 32, texMap);
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

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_CLOUD_BRICK + 1);
	animations->Add(ID_ANI_CLOUD_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_STRIPE_BRICK + 1);
	animations->Add(ID_ANI_STRIPE_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_SECRET_BRICK + 1);
	animations->Add(ID_ANI_SECRET_BRICK, ani);

	ani = new CAnimation(100);
	ani->Add(ID_SPRITE_BLACK_PIPE + 1);
	animations->Add(ID_ANI_BLACK_PIPE, ani);
}
void LoadAssetsCoin()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);
	LPTEXTURE texEnvi = textures->Get(ID_TEX_ENVI);

	sprites->Add(ID_SPRITE_COIN + 1, 303, 99, 303 + 9, 99 + 15, texMisc);
	sprites->Add(ID_SPRITE_COIN + 2, 321, 99, 321 + 9, 99 + 15, texMisc);
	sprites->Add(ID_SPRITE_COIN + 3, 338, 99, 338 + 9, 99 + 15, texMisc);

	sprites->Add(ID_SPRITE_BLOCK_COIN + 1, 229, 6, 229 + 15, 6 + 17, texEnvi);
	sprites->Add(ID_SPRITE_BLOCK_COIN + 2, 247, 6, 247 + 11, 6 + 17, texEnvi);
	sprites->Add(ID_SPRITE_BLOCK_COIN + 3, 261, 6, 261 + 7, 6 + 17, texEnvi);
	sprites->Add(ID_SPRITE_BLOCK_COIN + 4, 271, 6, 271 + 11, 6 + 17, texEnvi);
	sprites->Add(ID_SPRITE_BLOCK_COIN + 5, 285, 6, 285 + 15, 6 + 17, texEnvi);

	sprites->Add(ID_SPRITE_MUSHROOM + 1, 130, 156, 130 + 17, 156 + 1, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 2, 130, 156, 130 + 17, 156 + 2, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 3, 130, 156, 130 + 17, 156 + 3, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 4, 130, 156, 130 + 17, 156 + 4, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 5, 130, 156, 130 + 17, 156 + 5, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 6, 130, 156, 130 + 17, 156 + 6, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 7, 130, 156, 130 + 17, 156 + 7, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 8, 130, 156, 130 + 17, 156 + 8, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 9, 130, 156, 130 + 17, 156 + 9, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 10, 130, 156, 130 + 17, 156 + 10, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 11, 130, 156, 130 + 17, 156 + 11, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 12, 130, 156, 130 + 17, 156 + 12, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 13, 130, 156, 130 + 17, 156 + 13, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 14, 130, 156, 130 + 17, 156 + 14, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 15, 130, 156, 130 + 17, 156 + 15, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 16, 130, 156, 130 + 17, 156 + 16, texEnvi);
	sprites->Add(ID_SPRITE_MUSHROOM + 17, 130, 156, 130 + 17, 156 + 17, texEnvi);

	sprites->Add(ID_SPRITE_TANOOKI_LEAF + 1, 300, 208, 300 + LEAF_BBOX_WIDTH, 208 + LEAF_BBOX_HEIGHT, texMisc);

	sprites->Add(ID_SPRITE_PORTAL + 1, 453, 753, 453 + 32, 753 + 3, texEnvi);
	LPANIMATION ani = new CAnimation(300);
	ani->Add(ID_SPRITE_COIN + 1);
	ani->Add(ID_SPRITE_COIN + 2);
	ani->Add(ID_SPRITE_COIN + 3);
	animations->Add(ID_ANI_COIN, ani);

	ani = new CAnimation(75);
	ani->Add(ID_SPRITE_BLOCK_COIN + 1);
	ani->Add(ID_SPRITE_BLOCK_COIN + 2);
	ani->Add(ID_SPRITE_BLOCK_COIN + 3);
	ani->Add(ID_SPRITE_BLOCK_COIN + 4);
	ani->Add(ID_SPRITE_BLOCK_COIN + 5);
	animations->Add(ID_ANI_BLOCK_COIN, ani);

	ani = new CAnimation(50);
	ani->Add(ID_SPRITE_MUSHROOM + 1);
	ani->Add(ID_SPRITE_MUSHROOM + 2);
	ani->Add(ID_SPRITE_MUSHROOM + 3);
	ani->Add(ID_SPRITE_MUSHROOM + 4);
	ani->Add(ID_SPRITE_MUSHROOM + 5);
	ani->Add(ID_SPRITE_MUSHROOM + 6);
	ani->Add(ID_SPRITE_MUSHROOM + 7);
	ani->Add(ID_SPRITE_MUSHROOM + 8);
	ani->Add(ID_SPRITE_MUSHROOM + 9);
	ani->Add(ID_SPRITE_MUSHROOM + 10);
	ani->Add(ID_SPRITE_MUSHROOM + 11);
	ani->Add(ID_SPRITE_MUSHROOM + 12);
	ani->Add(ID_SPRITE_MUSHROOM + 13);
	ani->Add(ID_SPRITE_MUSHROOM + 14);
	ani->Add(ID_SPRITE_MUSHROOM + 15);
	ani->Add(ID_SPRITE_MUSHROOM + 16);
	ani->Add(ID_SPRITE_MUSHROOM + 17);
	animations->Add(ID_ANI_SHROOM_RISING, ani);

	ani = new CAnimation(300);
	ani->Add(ID_SPRITE_MUSHROOM + 17);
	animations->Add(ID_ANI_SHROOM_MOVING, ani);

	ani = new CAnimation(300);
	ani->Add(ID_SPRITE_TANOOKI_LEAF + 1);
	animations->Add(ID_ANI_TANOOKI_LEAF, ani);

	ani = new CAnimation(300);
	ani->Add(ID_SPRITE_PORTAL + 1);
	animations->Add(ID_ANI_PORTAL, ani);
}

void LoadAssetsFiretrap()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texEnemy2 = textures->Get(ID_TEX_ENEMY_2);
	LPTEXTURE texEnemy2Flip = textures->Get(ID_TEX_ENEMY_2_FLIP);
	//down left
	sprites->Add(ID_SPRITE_FIRETRAP + 1, 128, 145, 128 + 15, 145 + 31, texEnemy2);
	sprites->Add(ID_SPRITE_FIRETRAP + 2, 144, 145, 144 + 15, 145 + 31, texEnemy2);
	//upleft
	sprites->Add(ID_SPRITE_FIRETRAP + 3, 160, 145, 160 + 15, 145 + 31, texEnemy2);
	sprites->Add(ID_SPRITE_FIRETRAP + 4, 176, 145, 176 + 15, 145 + 31, texEnemy2);
	//up right
	sprites->Add(ID_SPRITE_FIRETRAP + 5, 496, 145, 496 + 15, 145 + 31, texEnemy2Flip);
	sprites->Add(ID_SPRITE_FIRETRAP + 6, 512, 145, 512 + 15, 145 + 31, texEnemy2Flip);
	//down right
	sprites->Add(ID_SPRITE_FIRETRAP + 7, 528, 145, 528 + 15, 145 + 31, texEnemy2Flip);
	sprites->Add(ID_SPRITE_FIRETRAP + 8, 544, 145, 544 + 15, 145 + 31, texEnemy2Flip);

	//down left
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 1, 32, 145, 32 + 15, 145 + 31, texEnemy2);
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 2, 48, 145, 48 + 15, 145 + 31, texEnemy2);
	//up left
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 3, 64, 145, 64 + 15, 145 + 31, texEnemy2);
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 4, 80, 145, 80 + 15, 145 + 31, texEnemy2);
	//up right
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 5, 592, 145, 592 + 15, 145 + 31, texEnemy2Flip);
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 6, 608, 145, 608 + 15, 145 + 31, texEnemy2Flip);
	//down right
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 7, 624, 145, 624 + 15, 145 + 31, texEnemy2Flip);
	sprites->Add(ID_SPRITE_GREEN_FIRETRAP + 8, 640, 145, 640 + 15, 145 + 31, texEnemy2Flip);

	sprites->Add(ID_SPRITE_GREEN_FLYTRAP + 1, 656, 145, 656 + 15, 145 + 31, texEnemy2Flip);
	sprites->Add(ID_SPRITE_GREEN_FLYTRAP + 2, 672, 145, 672 + 15, 145 + 31, texEnemy2Flip);

	//fireball
	sprites->Add(ID_SPRITE_FIREBALL + 1, 163, 179, 163 + 9, 179 + 9, texEnemy2);
	sprites->Add(ID_SPRITE_FIREBALL + 2, 179, 179, 179 + 9, 179 + 9, texEnemy2);
	sprites->Add(ID_SPRITE_FIREBALL + 3, 163, 195, 163 + 9, 195 + 9, texEnemy2);
	sprites->Add(ID_SPRITE_FIREBALL + 4, 179, 195, 179 + 9, 195 + 9, texEnemy2);


	LPANIMATION ani = new CAnimation(150);
	ani->Add(ID_SPRITE_FIRETRAP + 1, 300);
	ani->Add(ID_SPRITE_FIRETRAP + 2);
	animations->Add(ID_ANI_FIRETRAP_AIMING_DOWN_LEFT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_FIRETRAP + 3, 300);
	ani->Add(ID_SPRITE_FIRETRAP + 4);
	animations->Add(ID_ANI_FIRETRAP_AIMING_UP_LEFT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_FIRETRAP + 5);
	ani->Add(ID_SPRITE_FIRETRAP + 6, 300);
	animations->Add(ID_ANI_FIRETRAP_AIMING_UP_RIGHT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_FIRETRAP + 7);
	ani->Add(ID_SPRITE_FIRETRAP + 8, 300);
	animations->Add(ID_ANI_FIRETRAP_AIMING_DOWN_RIGHT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 1, 300);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 2);
	animations->Add(ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_LEFT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 3, 300);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 4);
	animations->Add(ID_ANI_GREEN_FIRETRAP_AIMING_UP_LEFT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 5);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 6, 300);
	animations->Add(ID_ANI_GREEN_FIRETRAP_AIMING_UP_RIGHT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 7);
	ani->Add(ID_SPRITE_GREEN_FIRETRAP + 8, 300);
	animations->Add(ID_ANI_GREEN_FIRETRAP_AIMING_DOWN_RIGHT, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_GREEN_FLYTRAP + 1);
	ani->Add(ID_SPRITE_GREEN_FLYTRAP + 2, 300);
	animations->Add(ID_ANI_GREEN_FLYTRAP, ani);

	ani = new CAnimation(150);
	ani->Add(ID_SPRITE_FIREBALL + 1);
	ani->Add(ID_SPRITE_FIREBALL + 2);
	ani->Add(ID_SPRITE_FIREBALL + 3);
	ani->Add(ID_SPRITE_FIREBALL + 4);
	animations->Add(ID_ANI_FIREBALL, ani);
}

void LoadAssetsOther()
{
	CTextures* textures = CTextures::GetInstance();
	CSprites* sprites = CSprites::GetInstance();
	CAnimations* animations = CAnimations::GetInstance();

	LPTEXTURE texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(ID_SPRITE_CLOUD_BEGIN, 390, 117, 390 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_CLOUD_MIDDLE, 408, 117, 408 + 15, 117 + 15, texMisc);
	sprites->Add(ID_SPRITE_CLOUD_END, 426, 117, 426 + 15, 117 + 15, texMisc);

	sprites->Add(ID_SPRITE_BUTTON, 335, 282, 335 + 10, 282 + 10, texMisc);

	LPANIMATION ani = new CAnimation(150);
	ani->Add(ID_SPRITE_BUTTON);
	animations->Add(ID_ANI_BUTTON, ani);
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
	textures->Add(ID_TEX_ENEMY_FLIP, TEXTURE_PATH_ENEMY_FLIP);
	textures->Add(ID_TEX_ENEMY_2, TEXTURE_PATH_ENEMY_2);
	textures->Add(ID_TEX_ENEMY_2_FLIP, TEXTURE_PATH_ENEMY_2_FLIP);
	textures->Add(ID_TEX_MISC, TEXTURE_PATH_MISC);
	textures->Add(ID_TEX_ENVI, TEXTURE_PATH_ENVIRONMENT);
	textures->Add(ID_TEX_MAP, TEXTURE_PATH_MAP);
	textures->Add(ID_TEX_MAIN, TEXTURE_PATH_MAIN);
	textures->Add(ID_TEX_BBOX, TEXTURE_PATH_BBOX);


	LoadAssetsEnvironment();
	LoadAssetsMario();
	LoadAssetsGoomba();
	LoadAssetsKoopa();
	LoadAssetsBrick();
	LoadAssetsCoin();
	LoadAssetsFiretrap();
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

#define MARIO_START_X 6000.0f //6000
#define MARIO_START_Y 132.0f

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
	//SkyBlock
	CSkyPlat* sp = new CSkyPlat(1420.0f, BRICK_Y - 46.0f - 176.0f);
	objects.push_back(sp);
	for (int i = 0; i < 4; i++)
	{
		CInviBase* ib = new CInviBase(1420.0f - 32.0f + 16.0f * i, BRICK_Y - 12.0f - 46.0f - 176.0f);
		objects.push_back(ib);
	}
	for (int i = 1; i < 4; i++)
	{
		CCoin* c = new CCoin(1420.0f - 32.0f + 16.0f * i, BRICK_Y - 12.0f - 46.0f - 176.0f - 64.0f);
		objects.push_back(c);
	}
	for (int i = 0; i < 4; i++)
	{
		CCloudBrick* cb = new CCloudBrick(1486.0f + 16.0f * i, BRICK_Y - 12.0f - 176.0f);
		objects.push_back(cb);
	}
	CQuestionBrick* qb = new CQuestionBrick(1486.0f + 16.0f * 2, BRICK_Y - 12.0f - 176.0f - 64.0f, ID_ITEM_COIN);
	objects.push_back(qb);
	for (int i = 0; i < 2; i++)
	{
		CCoin* ib = new CCoin(1486.0f + 16.0f * 4 + 16.0f * i, BRICK_Y - 28.0f - 176.0f - 64.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 13; i++)
	{
		CCloudBrick* cb = new CCloudBrick(1566.0f + 16.0f * i, BRICK_Y - 12.0f - 176.0f - 16.0f);
		objects.push_back(cb);
	}
	for (int i = 0; i < 4; i++)
	{
		CCoin* ib = new CCoin(1566.0f + 16.0f * 3 + 16.0f * i, BRICK_Y - 12.0f - 176.0f - 64.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 4; i++)
	{
		CCoin* ib = new CCoin(1566.0f + 16.0f * 8 + 16.0f * i, BRICK_Y - 28.0f - 176.0f - 64.0f);
		objects.push_back(ib);
	}
	CCoin* c6 = new CCoin(1566.0f + 16.0f * 13, BRICK_Y - 176.0f - 60.0f);
	objects.push_back(c6);
	CCoin* c2 = new CCoin(1566.0f + 16.0f * 15, BRICK_Y - 28.0f - 176.0f - 64.0f);
	objects.push_back(c2);
	CCoin* c3 = new CCoin(1566.0f + 16.0f * 17, BRICK_Y - 12.0f - 176.0f - 64.0f);
	objects.push_back(c3);	
	CCoin* c4 = new CCoin(1566.0f + 16.0f * 20, BRICK_Y - 44.0f - 176.0f - 64.0f);
	objects.push_back(c4);
	CCoin* c5 = new CCoin(1566.0f + 16.0f * 22, BRICK_Y - 12.0f - 176.0f - 64.0f);
	objects.push_back(c5);
	//Environment background
	CWideSquaredBush* b = new CWideSquaredBush(68.5f, BRICK_Y - 20.0f);
	objects.push_back(b);
	CDoubleCloud* c1 = new CDoubleCloud(152.5f, 50.0f);
	objects.push_back(c1);
	
	for (int i = 1; i < 4; i++)
	{
		CSingleBush* sb = new CSingleBush(150.0f + i * 16.0f, BRICK_Y -6.0f);
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
	//Render Firetrap before pipe->pipe will cover the firetrap
	CFiretrap* ft1 = new CFiretrap(400.0f, BRICK_Y - 21.0f);
	objects.push_back(ft1);
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
	CSingleCloud* sc2 = new CSingleCloud(715.0f, 50.0f);
	objects.push_back(sc2);
	CDoubleCloud* dc1 = new CDoubleCloud(812.0f, 69.0f);
	objects.push_back(dc1);
	for (int i = 0; i < 6; i++)
	{
		CSingleBush* sb = new CSingleBush(732.0f + i * 16.0f, BRICK_Y - 6.0f - 17.0f);
		objects.push_back(sb);
	}
	CWideSquaredBush* wsb2 = new CWideSquaredBush(935.0f, BRICK_Y - 6.0f - 37.0f);
	objects.push_back(wsb2);
	CLowBush* lb1 = new CLowBush(1037.0f, BRICK_Y - 6.0f - 30.0f);
	objects.push_back(lb1);
	for (int i = 0; i < 2; i++)
	{
		CSingleBush* sb = new CSingleBush(1105.0f + i * 16.0f, BRICK_Y - 6.0f - 17.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 3; i++)
	{
		CSingleBush* sb = new CSingleBush(1255.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(sb);
	}
	CSingleCloud* sc3 = new CSingleCloud(1255.0f, BRICK_Y - 115.0f);
	objects.push_back(sc3);
	CTripPlat* tp = new CTripPlat(1420.0f, BRICK_Y - 46.0f);
	objects.push_back(tp);
	for (int i = 0; i < 7; i++)
	{
		CInviBase* ib = new CInviBase(1338.0f + i * (BRICK_WIDTH - 1), BRICK_Y - 22.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 7; i++)
	{
		CInviBase* ib = new CInviBase(1338.0f + 32.0f + i * (BRICK_WIDTH - 1), BRICK_Y - 22.0f - 32.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 7; i++)
	{
		CInviBase* ib = new CInviBase(1338.0f + 64.0f + i * (BRICK_WIDTH - 1), BRICK_Y - 22.0f - 32.0f - 32.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 2; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1562.0f + i * 16.0f, BRICK_Y - 38.0f - 32.0f);
		objects.push_back(sb);
	}
	CDoubleCloud* dc2 = new CDoubleCloud(1750.0f, BRICK_Y - 115.0f);
	objects.push_back(dc2);

	for (int i = 0; i < 3; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1639.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 2; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1655.0f + i * 16.0f, BRICK_Y - 22.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 1; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1671.0f + i * 16.0f, BRICK_Y - 38.0f);
		objects.push_back(sb);
	}

	for (int i = 0; i < 3; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1734.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 2; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1734.0f + i * 16.0f, BRICK_Y - 22.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 1; i++)
	{
		CStripeBrick* sb = new CStripeBrick(1734.0f + i * 16.0f, BRICK_Y - 38.0f);
		objects.push_back(sb);
	}
	for (int i = 0; i < 3; i++)
	{
		CSingleBush* sb = new CSingleBush(1800.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(sb);
	}
	//
	CGreenFlytrap* ft3 = new CGreenFlytrap(1871.5f, BRICK_Y - 6.0f + 7.0f);
	objects.push_back(ft3);
	CPipe* pl = new CPipe(1871.5f, BRICK_Y - 6.0f);
	objects.push_back(pl);

	//
	CGreenFiretrap* ft2 = new CGreenFiretrap(1945.0f, BRICK_Y - 22.0f);
	objects.push_back(ft2);
	CPipe* ph = new CPipe(1945.5f, BRICK_Y - 22.0f);
	objects.push_back(ph);
	CLowBush* lb2 = new CLowBush(1992.0f, BRICK_Y - 21.0f);
	objects.push_back(lb2);
	for (int i = 0; i < 7; i++)
	{
		CGoldBrick* gb = new CGoldBrick(2031.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(gb);
	}
	for (int i = 0; i < 5; i++)
	{
		CGoldBrick* gb = new CGoldBrick(2047.0f + i * 16.0f, BRICK_Y - 22.0f);
		objects.push_back(gb);
	}
	CButton* bt = new CButton(2063.0f + 3 * 16.0f, BRICK_Y - 38.0f);
	objects.push_back(bt);
	for (int i = 0; i < 3; i++)//fix to 4 in the final
	{
		CGoldBrick* gb = new CGoldBrick(2063.0f + i * 16.0f, BRICK_Y - 38.0f);
		objects.push_back(gb);
	}

	for (int i = 0; i < 2; i++)
	{
		CGoldBrick* gb = new CGoldBrick(2031.0f + 9 * 16.0f + i * 16.0f, BRICK_Y - 6.0f);
		objects.push_back(gb);
	}
	CGoldBrick* gb = new CGoldBrick(2031.0f + 9 * 16.0f, BRICK_Y - 22.0f);
	objects.push_back(gb);
	CHighPlat* hp = new CHighPlat(2251.0f, BRICK_Y - 70.0f);
	objects.push_back(hp);
	for (int i = 0; i < 3; i++)
	{
		CInviBase* ib = new CInviBase(2215.0f + i * (BRICK_WIDTH - 1), BRICK_Y - 38.0f);
		objects.push_back(ib);
	}
	for (int i = 0; i < 3; i++)
	{
		CInviBase* ib = new CInviBase(2247.0f + i * (BRICK_WIDTH - 1), BRICK_Y - 38.0f - 96.0f);
		objects.push_back(ib);
	}
	CPipe* flappy = new CPipe(2337.0f, BRICK_Y - 6.0f);
	objects.push_back(flappy);
	for (int i = 0; i < 3; i++)
	{
		CPipe* flappy = new CPipe(2337.0f, BRICK_Y - 6.0f - 16.0f - 97.0f - 48.0f * i);
		objects.push_back(flappy);
	}
	CPortal* pt1 = new CPortal(2337.0f, BRICK_Y - 112.0f - 96.0f - 40.0f, 3134.0f, BRICK_Y - 96.0f - 48.0f, PORTAL_INPUT_SIT);
	objects.push_back(pt1);
	for (int i = 0; i < 2; i++)
	{
		CStripeBrick* flappy = new CStripeBrick(2330.0f + i * BRICK_WIDTH, BRICK_Y - 38.0f);
		objects.push_back(flappy);
	}
	for (int i = 0; i < 2; i++)
	{
		CStripeBrick* flappy = new CStripeBrick(2330.0f + i * BRICK_WIDTH, BRICK_Y - 38.0f - 64.0f);
		objects.push_back(flappy);
	}
	CPipe* lp = new CPipe(2337.0f + 56.0f, BRICK_Y - 6.0f);
	objects.push_back(lp);
	CSingleCloud* sc4 = new CSingleCloud(2337.0f + 58.0f, BRICK_Y - 115.0f);
	objects.push_back(sc4);
	CHighBush* hb = new CHighBush(2475.0f, BRICK_Y - 60.0f);
	objects.push_back(hb);

	CSpikeBlackBG* fn = new CSpikeBlackBG(2580.0f, BRICK_Y - 60.0f);
	objects.push_front(fn);
	for (int i = 0; i < 145; i++)
	{
		CPlainBlackBG* bg = new CPlainBlackBG(2655.0f + 87.0f * i, BRICK_Y - 60.0f);
		objects.push_back(bg);
	}
	CBigBlackBush* bbb = new CBigBlackBush(2685.0f, BRICK_Y - 44.0f);
	objects.push_back(bbb);
	CSmallBlackBush* sbb = new CSmallBlackBush(2835.0f, BRICK_Y - 23.0f);
	objects.push_back(sbb);

	// Main ground
	for (int i = 0; i < NUM_BRICKS; i++)
	{
		CBaseBrick* b = new CBaseBrick(i * BASE_BRICK_BBOX_WIDTH, BRICK_Y+10.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 10; i++)
	{
		CBaseBrick* b = new CBaseBrick(695.0f + i * BRICK_WIDTH * 3.0f, BRICK_Y-7.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 7; i++)
	{
		CBaseBrick* b = new CBaseBrick(1237.0f + i * BRICK_WIDTH * 3.0f, BRICK_Y + 10.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 2; i++)
	{
		CBaseBrick* b = new CBaseBrick(1608.0f + i * BASE_BRICK_BBOX_WIDTH, BRICK_Y + 10.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 12; i++)
	{
		CBaseBrick* b = new CBaseBrick(1749.0f + i * BRICK_WIDTH * 3.0f, BRICK_Y + 10.0f);
		objects.push_back(b);
	}
	for (int i = 0; i < 12; i++)
	{
		CBaseBrick* b = new CBaseBrick(2345.0f + i * BRICK_WIDTH * 3.0f, BRICK_Y + 10.0f);
		objects.push_back(b);
	}

	// World boundaries for Mario
	for (int i = 0; i < 10; i++)
	{
		CInviBrick* b = new CInviBrick(0, BRICK_Y - i * BRICK_WIDTH);
		objects.push_back(b);
	}
	for (int i = 0; i < 10; i++)
	{
		CInviBrick* b = new CInviBrick(2825.0f, BRICK_Y - i * BRICK_WIDTH);
		objects.push_back(b);
	}
	mario = new CMario(MARIO_START_X, MARIO_START_Y);
	objects.push_back(mario);

	//Secret room
	for (int i = 0; i < 32; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + i * 16.0f, BRICK_Y);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 32; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + i * 16.0f, BRICK_Y - 16.0f);
		objects.push_back(sbr);
	}
	for(int i = 2; i<=11;i++)
		for (int j = 0; j < 8; j++)
		{
			CSecretBrick* sbr = new CSecretBrick(3000.0f + j * 16.0f, BRICK_Y - 16.0f * i);
			objects.push_back(sbr);
		}
	for (int i = 0; i < 16; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 16 + i * 16.0f, BRICK_Y-32.0f);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 15; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 17 + i * 16.0f, BRICK_Y - 48.0f);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 14; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 18 + i * 16.0f, BRICK_Y -16.0f * 4);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 13; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 19 + i * 16.0f, BRICK_Y - 80.0f);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 12; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 20 + i * 16.0f, BRICK_Y - 96.0f);
		objects.push_back(sbr);
	}
	for (int i = 0; i < 11; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 21 + i * 16.0f, BRICK_Y - 112.0f);
		objects.push_back(sbr);
	}
	for (int i = 8; i <= 11; i++)
		for (int j = 0; j < 8; j++)
		{
			CSecretBrick* sbr = new CSecretBrick(3000.0f + 16.0f * 24 + j * 16.0f, BRICK_Y - 16.0f * i);
			objects.push_back(sbr);
		}
	for (int i = 10; i <= 21; i++)
	{
		CSecretBrick* sbr = new CSecretBrick(3000.0f + i * 16.0f, BRICK_Y - 16.0f * 11);
		objects.push_back(sbr);
	}
	for (int i = 0; i<3 ; i++)
		for (int j = 0; j < 2; j++)
		{
			CCoin* c = new CCoin(3000.0f +14 * 16.0f + j * 16.0f, BRICK_Y - 16.0f * 3 - 48.0f * i);
			objects.push_back(c);
		}
	for (int i = 0; i < 2; i++)
	{
		CCoin* c = new CCoin(3000.0f + 13 * 16.0f, BRICK_Y - 16.0f * 4 - 64.0f * i);
		objects.push_back(c);
	}
	for (int i = 0; i < 2; i++)
	{
		CCoin* c = new CCoin(3000.0f + 16 * 16.0f, BRICK_Y - 16.0f * 4 - 16.0f * i);
		objects.push_back(c);
	}
	for (int i = 0; i < 2; i++)
	{
		CCoin* c = new CCoin(3000.0f + 16 * 16.0f, BRICK_Y - 16.0f * 7 - 16.0f * i);
		objects.push_back(c);
	}
	CBlackPipe* bp1 = new CBlackPipe(3000.0f + 7 * 16.0f + 24.0f, BRICK_Y - 16.0f * 11 + 8.0f);
	objects.push_back(bp1);
	CBlackPipe* bp2 = new CBlackPipe(3000.0f + 21 * 16.0f + 24.0f, BRICK_Y - 16.0f * 11 + 9.0f);
	objects.push_back(bp2);
	CPortal* pt2 = new CPortal(3000.0f + 21 * 16.0f + 24.0f, BRICK_Y - 16.0f * 11 + 27.0f, 2337.0f + 56.0f, BRICK_Y - 54.0f, PORTAL_INPUT_JUMP);
	objects.push_back(pt2);

	//Main Screen
	CInviBrick* cam = new CInviBrick(6000.0f, BRICK_Y);
	objects.push_back(cam);
	for (int i = 0; i < 10; i++)
	{
		CInviBrick* camera = new CInviBrick(6000.0f - 16.0f, BRICK_Y - 16.0f*i);
		objects.push_back(camera);
	}
	for (int i = 0; i < 10; i++)
	{
		CInviBrick* camera = new CInviBrick(6000.0f + 16.0f, BRICK_Y - 16.0f * i);
		objects.push_back(camera);
	}
	CNullBrick* nbcam = new CNullBrick(6000.0f, BRICK_Y - 48.0f);
	objects.push_back(nbcam);
	CMainScreen* main = new CMainScreen(6000.0f, BRICK_Y - 75.0f);
	objects.push_back(main);
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

	CNullBrick* nb4 = new CNullBrick(715.0f, BRICK_Y - 22.0f);
	objects.push_back(nb4);
	CNullBrick* nb5 = new CNullBrick(760.0f, BRICK_Y - 52.0f);
	objects.push_back(nb5);

	CNullBrick* nb6 = new CNullBrick(1530.0f, BRICK_Y - 38.0f);
	objects.push_back(nb6);
	//
	CQuestionBrick* qb1 = new CQuestionBrick(246.0f, BRICK_Y - 99.0f, ID_ITEM_COIN);
	objects.push_back(qb1);
	CQuestionBrick* qb2 = new CQuestionBrick(262.0f, BRICK_Y - 99.0f, ID_ITEM_MUSHROOM);
	objects.push_back(qb2);
	CQuestionBrick* qb3 = new CQuestionBrick((550.0f - 96.0f + 16.0f), BRICK_Y - (69.5f+16.0f), ID_ITEM_COIN);
	objects.push_back(qb3);
	CQuestionBrick* qb4 = new CQuestionBrick(715.0f, BRICK_Y - 22.0f);
	objects.push_back(qb4);
	CQuestionBrick* qb5 = new CQuestionBrick(760.0f, BRICK_Y - 52.0f, ID_ITEM_COIN);
	objects.push_back(qb5);
	CQuestionBrick* qb6 = new CQuestionBrick(1530.0f, BRICK_Y - 38.0f);
	objects.push_back(qb6);
	CGoomba* gb1 = new CGoomba(152.5f, 50.0f);
	objects.push_back(gb1);
	CGoomba* gb2 = new CGoomba(535.0f - 78.0f, 50.0f);
	objects.push_back(gb2);
	CKoopa* kb1 = new CKoopa(584.0f - 78.0f, 25.0f);
	objects.push_back(kb1);
	CGoomba* gb3 = new CGoomba(843.0f - 78.0f, 25.0f);
	objects.push_back(gb3);
	CGoomba* gb4 = new CGoomba(887.0f - 78.0f, 25.0f);
	objects.push_back(gb4);
	CParaGoomba* pgb1 = new CParaGoomba(956.0f - 78.0f, 25.0f);
	objects.push_back(pgb1);
	for (int i = 0; i < 3; i++)
	{
		CParaKoopa* pkb = new CParaKoopa(1340.0f - 78.0f + 32.0f * i, BRICK_Y - 64.0f);
		objects.push_back(pkb);
	}
	CKoopa* kb2 = new CKoopa(1483.0f - 78.0f, 50.0f);
	objects.push_back(kb2);
	CKoopa* kb3 = new CKoopa(2104.0f - 78.0f, 25.0f);
	objects.push_back(kb3);
	/*
	CGoomba* goomba = new CGoomba(GOOMBA_X, GROUND_Y - 120.0f);
	objects.push_back(goomba);
	CKoopa* koopa = new CKoopa(150.0f, 40.0f);
	objects.push_back(koopa);
	*/
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
	//cy = 0;
	cy -= SCREEN_HEIGHT / 2;

	if (cx < 0) cx = 0;
	if (cy > 0) cy = 0;

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
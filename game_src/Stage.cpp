#include "StdAfx.h"
#include "Stage.h"
extern HGE* g_hge;
extern Func framefunc;
extern Func renderfunc;

Stage *stage;
HEFFECT Stage::bomb =0;
HEFFECT Stage::hit = 0;
HEFFECT Stage::shoot = 0;
HEFFECT Stage::pig = 0;
HEFFECT Stage::udboss =0;
HEFFECT Stage::fire = 0;
HEFFECT Stage::jump = 0;

Stage::Stage()
{
	stageOneQuad.tex = g_hge->Texture_Load("back1.jpg");
	bgSnd = g_hge->Effect_Load("bgsound.mp3");
	if(bgSnd)
		g_hge->Effect_PlayEx(bgSnd,80,0,1.0f,true);
	Stage::bomb = g_hge->Effect_Load("bomb.mp3");
	Stage::hit = g_hge->Effect_Load("hit.wav");
	Stage::shoot = g_hge->Effect_Load("shoot.mp3");
	Stage::pig = g_hge->Effect_Load("pig.mp3");
	Stage::udboss = g_hge->Effect_Load("udboss.wav");
	Stage::fire = g_hge->Effect_Load("fire.mp3");
	Stage::jump = g_hge->Effect_Load("jump.mp3");

	if(stageOneQuad.tex)
	{
		fnt = new hgeFont("font2.fnt");
		fnt->SetColor(0xFFFFE060);
		player = new Player();

		stageOneQuad.blend = BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_NOZWRITE;
		for(int i=0; i != 4; ++i)
		{
			stageOneQuad.v[i].z = 0.5f;
			stageOneQuad.v[i].col = 0xffffffff;
		}
		stageOneQuad.v[0].x=0; stageOneQuad.v[0].y = 0; stageOneQuad.v[0].tx = 0.0f;stageOneQuad.v[0].ty = 0.0f;
		stageOneQuad.v[1].x = 3200; stageOneQuad.v[1].y = 0; stageOneQuad.v[1].tx = 1.0f; stageOneQuad.v[1].ty = 0.0f;
		stageOneQuad.v[2].x = 3200; stageOneQuad.v[2].y = 600; stageOneQuad.v[2].tx = 1.0f; stageOneQuad.v[2].ty = 1.0f;
		stageOneQuad.v[3].x = 0; stageOneQuad.v[3].y = 600; stageOneQuad.v[3].tx = 0.0f; stageOneQuad.v[3].ty = 1.0f;
		bgforward = false;
		pathLen = 0.0f;
		ready = true;
	}
	else ready = false;

	mp = new MwMap("stage1");
	memy = new MwEnemy("stage1");
}

Stage::~Stage()
{
	if(stageOneQuad.tex)
		g_hge->Texture_Free(stageOneQuad.tex);
	g_hge->Effect_Free(bgSnd);
	g_hge->Effect_Free(bomb);
	g_hge->Effect_Free(hit);
	g_hge->Effect_Free(shoot);
	g_hge->Effect_Free(pig);
	g_hge->Effect_Free(udboss);
	g_hge->Effect_Free(fire);
	g_hge->Effect_Free(jump);
	delete memy;
	delete mp;
	delete fnt;
	delete player;
/*	delete stageOneSpr;*/
}

void Stage::frame()
{
	float dt = g_hge->Timer_GetDelta();

	memy->frame(dt);
	(player->*(player->frame))(dt);
}

void Stage::render()
{
	g_hge->Gfx_RenderQuad(&stageOneQuad);
	mp->render();
	memy->render();
	(player->*(player->render))();
}

bool Stage::frameFunc()
{
	stage->frame();
	return false;
}

bool Stage::renderFunc()
{
	g_hge->Gfx_BeginScene();
//	g_hge->Gfx_Clear(0);
	stage->render();
	stage->fnt->printf(5, 5, HGETEXT_LEFT, "DT:%f\nFPS:%d",g_hge->Timer_GetDelta(),g_hge->Timer_GetFPS());

	g_hge->Gfx_EndScene();
	if(g_hge->Input_GetKeyState(HGEK_ESCAPE))
	{
		delete stage;
		stage = NULL;
		initmenu();
		g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = menuFrameFunc);
		g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = menuRenderFunc);
	}
	return false;
}

void Stage::playsound(HEFFECT heffect)
{
	g_hge->Effect_Play(heffect);
}

void Stage::youWin()
{
	g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = winFrame);
	g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = winRender);
}

bool winFrame()
{
	if(stage)
	{
		delete stage;
		stage = NULL;
	}
	return false;
}

bool winRender()
{
	static HEFFECT hali = g_hge->Effect_Load("hali.mp3");
	static HTEXTURE htex = g_hge->Texture_Load("win.png");
	static hgeSprite* spr = new hgeSprite(htex,0,0,800,600);
	static bool isPlaying = false;
	g_hge->Gfx_BeginScene();

	spr->Render(0,0);

	g_hge->Gfx_EndScene();
	if(!isPlaying)
	{
		g_hge->Effect_Play(hali);
		isPlaying = true;
	}
	if(g_hge->Input_GetKeyState(HGEK_ESCAPE))
	{
		delete spr;
		initmenu();
		g_hge->Texture_Free(htex);
		g_hge->Effect_Free(hali);
		g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = menuFrameFunc);
		g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = menuRenderFunc);
	}
	return false;
}
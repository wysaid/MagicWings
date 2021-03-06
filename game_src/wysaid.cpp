#include "StdAfx.h"
#include "wysaid.h"
#include <cmath>
extern HGE* g_hge;

extern HTEXTURE g_cursortex,g_bgtex,g_partex;
extern hgeQuad g_quad;
extern HEFFECT g_menuSnd;

extern hgeFont *g_fnt;
extern hgeGUI *g_gui;
extern hgeSprite *g_cursor,*g_par_spr,*g_bgspr;

extern Func framefunc;
extern Func renderfunc;

extern hgeParticleSystem *g_par;

extern Stage *stage;

Author *author;
hgeSprite *wysaid,*ehanQ,*cocaRush;
HTEXTURE wytex,ehtex,coctex;

char* statement = NULL;
readThreadCtrl* pThreadCtrl = NULL;


bool menuRenderFunc()
{
	g_hge->Gfx_BeginScene();	
	g_hge->Gfx_RenderQuad(&g_quad);
	g_gui->Render();
	g_par->Render();
	g_fnt->printf(5, 5, HGETEXT_LEFT, "What a loli game to play?!\nIsn't it??");
	g_hge->Gfx_EndScene();
	return false;
}

bool initmenu()
{
	g_quad.tex = g_hge->Texture_Load("bg.png");
	g_cursortex = g_hge->Texture_Load("cursor.png");
	g_menuSnd = g_hge->Effect_Load("menu.wav");
	g_partex = g_hge->Texture_Load("particles.png");
	if(!(g_quad.tex && g_cursortex && g_menuSnd && g_partex))
	{
		MessageBox(g_hge->System_GetState(HGE_HWND),"加载资源失败!","请检查当前文件夹下是否存在所需资源",0);
		return false;
	}
	g_quad.blend = BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_NOZWRITE;
	
	for(int i=0; i<4; ++i)
	{
		g_quad.v[i].z = 0.5f;
		g_quad.v[i].col = 0xFFFFFFFF;
	}
	g_quad.v[0].x=0; g_quad.v[0].y=0; 
	g_quad.v[1].x=800; g_quad.v[1].y=0; 
	g_quad.v[2].x=800; g_quad.v[2].y=600; 
	g_quad.v[3].x=0; g_quad.v[3].y=600; 
	
	g_fnt=new hgeFont("font1.fnt");
	g_fnt->SetColor(0xFFFFFFFF);

	g_cursor = new hgeSprite(g_cursortex,0,0,32,32);

	g_par_spr = new hgeSprite(g_partex,0,0,32,32);
	g_par_spr->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
	g_par_spr->SetHotSpot(16,16);
	g_par = new hgeParticleSystem("particle5.psi", g_par_spr);
	g_par->Fire();
	
	g_gui = new hgeGUI();
	
	g_gui->AddCtrl(new hgeGUIMenuItem(1,g_fnt,g_menuSnd,400,200,0.0f,"New Game"));
	g_gui->AddCtrl(new hgeGUIMenuItem(2,g_fnt,g_menuSnd,400,240,0.1f,"Options"));
	g_gui->AddCtrl(new hgeGUIMenuItem(3,g_fnt,g_menuSnd,400,280,0.2f,"Instructions"));
	g_gui->AddCtrl(new hgeGUIMenuItem(4,g_fnt,g_menuSnd,400,320,0.3f,"About us"));
	g_gui->AddCtrl(new hgeGUIMenuItem(5,g_fnt,g_menuSnd,400,360,0.4f,"Exit"));
	
	g_gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
	g_gui->SetCursor(g_cursor);
	g_gui->Enter();
	return true;
}


bool menuFrameFunc()
{
	float dt=g_hge->Timer_GetDelta();
	static float t=0.0f;
	float tx,ty;
	float x,y;
	int id;
	static int lastid=0;
	
	g_hge->Input_GetMousePos(&x,&y);
	g_par->MoveTo(x,y);
	g_par->Update(dt);

	id=g_gui->Update(dt);
	if(id == -1)
	{
		switch(lastid)
		{
		case 1:
			stage = new Stage();
			if(!stage->ready)
			{
				delete stage;
				stage = NULL;
				g_gui->Enter();
				break;
			}

			g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = Stage::frameFunc);
			g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = Stage::renderFunc);
			cleanMenu();

			lastid = 0;
			return false;
		case 2:
			if(!initOptions())
			{
				g_gui->Enter();
				break;
			}
			statement = new char[1024]();
			pThreadCtrl = new readThreadCtrl(true,"options.txt");
			_beginthread(readStatement,0,pThreadCtrl);
			g_fnt->SetColor(0xffffffff);

			g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = instructionAndOptionFrame);
			g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = instructionAndOptionRender);

			lastid = 0;
			return false;
		case 3:
			if(!initInstructions())
			{
				g_gui->Enter();
				break;
			}
			statement = new char[1024]();
			pThreadCtrl = new readThreadCtrl(true,"instructions.txt");
			_beginthread(readStatement,0,pThreadCtrl);
			g_fnt->SetColor(0xffffffff);

			g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = instructionAndOptionFrame);
			g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = instructionAndOptionRender);

			lastid = 0;
			return false;
		case 4:
			if(!initAuthor())
			{
				g_gui->Enter();
				break;
			}
			statement = new char[1024]();
			pThreadCtrl = new readThreadCtrl(true,"About us.txt");
			_beginthread(readStatement,0,pThreadCtrl);
			g_fnt->SetColor(0xFFFFFFFF);

			g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = aboutUsFrame);
			g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = aboutUsRender);

			lastid = 0;
			return false;
			
		case 5: 

			return true;
		}
	}
	else if(id)
	{ 
		lastid=id; 
		g_gui->Leave(); 
	}
	
	// Here we update our background animation
	t+=dt;
	tx=50*cosf(t/60);
	ty=50*sinf(t/60);

	g_quad.v[0].tx=tx;        g_quad.v[0].ty=ty;
	g_quad.v[1].tx=tx+800/64; g_quad.v[1].ty=ty;
	g_quad.v[2].tx=tx+800/64; g_quad.v[2].ty=ty+600/64;
	g_quad.v[3].tx=tx;        g_quad.v[3].ty=ty+600/64;
	return false;
}

bool aboutUsFrame()
{
	float dt=g_hge->Timer_GetDelta();
	float x,y;
	int i;

	for(i=0;i<AUTHORNUM;++i)
	{
		author[i].x+=author[i].dx*dt;
		if(author[i].x>SCREEN_WIDTH || author[i].x<0) author[i].dx=-author[i].dx;
		author[i].y+=author[i].dy*dt;
		if(author[i].y>SCREEN_HEIGHT || author[i].y<0) author[i].dy=-author[i].dy;
		author[i].scale+=author[i].dscale*dt;
		if(author[i].scale>2 || author[i].scale<0.5) author[i].dscale=-author[i].dscale;
		author[i].rot+=author[i].drot*dt;
	}

	g_hge->Input_GetMousePos(&x,&y);
	g_par->MoveTo(x,y);
	g_par->Update(dt);

	return false;
}

bool aboutUsRender()
{
	int i;
	
	g_hge->Gfx_BeginScene();
	g_bgspr->Render(0,0);
	
	for(i=0;i != AUTHORNUM; ++i)
	{
		wysaid->SetColor(author[i].color); 
		wysaid->RenderEx(author[i].x, author[i].y, author[i].rot, author[i].scale);
	}
	g_par->Render();
	g_fnt->printf(5, 5, HGETEXT_LEFT, statement);
	g_hge->Gfx_EndScene();

	if(g_hge->Input_GetKeyState(HGEK_LBUTTON) || g_hge->Input_GetKeyState(HGEK_ESCAPE) || g_hge->Input_GetKeyState(HGEK_SPACE)) 
	{
		cleanAuthor();

		if(pThreadCtrl) pThreadCtrl->loop = false;
		g_gui->Enter();
		g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = menuFrameFunc);
		g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = menuRenderFunc);
	}
	return false;
}

bool instructionAndOptionRender()
{
	g_hge->Gfx_BeginScene();

	g_bgspr->Render(0,0);
	g_fnt->printf(5,5,HGETEXT_LEFT,statement);
	g_par->Render();	
	g_hge->Gfx_EndScene();
	if(g_hge->Input_GetKeyState(HGEK_LBUTTON) || g_hge->Input_GetKeyState(HGEK_ESCAPE) || g_hge->Input_GetKeyState(HGEK_SPACE))
	{
		cleanInstructionAndOption();

		if(pThreadCtrl) pThreadCtrl->loop = false;
		g_gui->Enter();
		g_hge->System_SetState(HGE_FRAMEFUNC,framefunc = menuFrameFunc);
		g_hge->System_SetState(HGE_RENDERFUNC,renderfunc = menuRenderFunc);
	}	
	return false;
}

bool instructionAndOptionFrame()
{
	float dt=g_hge->Timer_GetDelta();
	float x,y;
	
	g_hge->Input_GetMousePos(&x,&y);
	g_par->MoveTo(x,y);
	g_par->Update(dt);
	
	return false;
}

bool initAuthor()
{
	wytex = g_hge->Texture_Load("wysaid.png");
	g_bgtex = g_hge->Texture_Load("bgauthor.png");
	//待添加...
	if(!wytex || !g_bgtex)
	{
		MessageBox(g_hge->System_GetState(HGE_HWND),"加载资源失败，点击确定以返回主菜单!","不要迷恋哥...",MB_ICONERROR);
		return false;
	}
	wysaid = new hgeSprite(wytex,0,0,160,160);
	wysaid->SetHotSpot(80,80);

	g_bgspr = new hgeSprite(g_bgtex,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	g_bgspr->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	
	g_bgspr->SetColor(0xFF000000,0);
	g_bgspr->SetColor(0xFF000000,1);
	g_bgspr->SetColor(0xFF000040,2);
	g_bgspr->SetColor(0xFF000040,3);

	author = new Author[AUTHORNUM];
	for(int i=0; i != AUTHORNUM; ++i)
	{
		author[i].x=g_hge->Random_Float(0,SCREEN_WIDTH);
		author[i].y=g_hge->Random_Float(0,SCREEN_HEIGHT);
		author[i].dx=g_hge->Random_Float(-200,200);
		author[i].dy=g_hge->Random_Float(-200,200);
		author[i].scale=g_hge->Random_Float(0.5f,2.0f);
		author[i].dscale=g_hge->Random_Float(-1.0f,1.0f);
		author[i].rot=g_hge->Random_Float(0,M_PI*2);
		author[i].drot=g_hge->Random_Float(-1.0f,1.0f);
		author[i].color = 0xffff0000 + g_hge->Random_Int(0x0101,0xffff);
	}
	wysaid->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	return true;
}

bool initOptions()
{
	g_bgtex = g_hge->Texture_Load("options.png");
	if(!g_bgtex)
	{
		MessageBox(g_hge->System_GetState(HGE_HWND),"加载资源失败，点击确定以返回主菜单!","不要迷恋哥...",MB_ICONERROR);
		return false;
	}
	g_bgspr = new hgeSprite(g_bgtex,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	g_bgspr->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	g_bgspr->SetColor(0xFF000000,0);
	g_bgspr->SetColor(0xFF000000,1);
	g_bgspr->SetColor(0xFF000040,2);
	g_bgspr->SetColor(0xFF000040,3);
	return true;
}

bool initInstructions()
{
	g_bgtex = g_hge->Texture_Load("instruction.png");
	if(!g_bgtex)
	{
		MessageBox(g_hge->System_GetState(HGE_HWND),"加载资源失败，点击确定以返回主菜单!","不要迷恋哥...",MB_ICONERROR);
		return false;
	}
	g_bgspr = new hgeSprite(g_bgtex,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	g_bgspr->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	g_bgspr->SetColor(0xFF000000,0);
	g_bgspr->SetColor(0xFF000000,1);
	g_bgspr->SetColor(0xFF000040,2);
	g_bgspr->SetColor(0xFF000040,3);
	return true;
}


bool cleanInstructionAndOption()
{
	if(statement)
	{
		delete[] statement;
		statement = NULL;
	}
	if(g_bgtex)
	{
		g_hge->Texture_Free(g_bgtex);
		g_bgtex = 0;
	}
	if(g_bgspr)
	{
		delete g_bgspr;
		g_bgspr = 0;
	}

	return true;
}

bool cleanMenu()
{	
	delete g_gui;g_gui=NULL;
	delete g_fnt;g_fnt=NULL;
	delete g_cursor;g_cursor=NULL;
	delete g_par;g_par=NULL;
	delete g_par_spr;g_par_spr=NULL;
	if(g_partex) 
	{
		g_hge->Texture_Free(g_partex);
		g_partex = 0;
	}
	if(g_menuSnd)
	{
		g_hge->Effect_Free(g_menuSnd);
		g_menuSnd = 0;
	}
	if(g_cursortex)
	{
		g_hge->Texture_Free(g_cursortex);
		g_cursortex = 0;
	}
	if(g_quad.tex)
	{
		g_hge->Texture_Free(g_quad.tex);
		g_quad.tex = 0;
	}
	if(statement)
	{
		delete[] statement;
		statement = NULL;
	}
	return true;
}

bool cleanAuthor()
{
	if(author)
	{
		delete[] author;
		author = NULL;;
	}
	if(wysaid)
	{
		delete wysaid;
		wysaid = NULL;
	}
	if(g_bgspr)
	{
		delete g_bgspr;
		g_bgspr =	NULL;
	}
	if(wytex)
	{
		g_hge->Texture_Free(wytex);
		wytex = 0;
	}
	if(g_bgtex)
	{
		g_hge->Texture_Free(g_bgtex);
		g_bgtex = 0;
	}
	if(statement)
	{
		delete[] statement;
		statement = NULL;
	}
	return true;
}

void readStatement(void* ctrl)
{
	readThreadCtrl* pCtrl = reinterpret_cast<readThreadCtrl*>(ctrl);
	FILE* fp = fopen(pCtrl->filename,"r");
	int n;
	if(!fp)
	{
		strcpy(statement,"Can't find the file !");
		return ;
	}
	char *p = statement;
	for(n = fgetc(fp); pCtrl->loop && n != EOF; n=fgetc(fp))
	{
		*p++ = n;
		if(n == '.')
			Sleep(1000);
		else Sleep(75);
	}
	if(pThreadCtrl)
	{
		delete pThreadCtrl;
		pThreadCtrl = NULL;
	}
}

Player::Player()		//玩家模块构造函数，设置各种参数等
{
	standTex = g_hge->Texture_Load("player\\stand.png");
	jumpTex = g_hge->Texture_Load("player\\jump.png");
	moveTex = g_hge->Texture_Load("player\\move.png");
	attackTex = g_hge->Texture_Load("player\\attack.png");

	stRightAnim = new hgeAnimation(standTex,3,3,0,0,60,70);
	stLeftAnim = new hgeAnimation(standTex,3,3,0,70,60,70);
	jpRightAnim = new hgeSprite(jumpTex,0,0,60,70);
	jpLeftAnim = new hgeSprite(jumpTex,60,0,60,70);
	jpRightAnim->SetHotSpot(30,70);
	jpLeftAnim->SetHotSpot(30,70);
	mvRightAnim = new hgeAnimation(moveTex,4,4,0,0,60,70);
	mvLeftAnim = new hgeAnimation(moveTex,4,4,0,70,60,70);
	atkRightAnim = new hgeAnimation(attackTex,3,6,0,0,66,69);
	atkLeftAnim = new hgeAnimation(attackTex,3,6,0,70,66,69);
	stRightAnim->SetHotSpot(30,70); stRightAnim->Play();
	stLeftAnim->SetHotSpot(30,70); stLeftAnim->Play();
	mvRightAnim->SetHotSpot(30,70); mvRightAnim->Play();
	mvLeftAnim->SetHotSpot(30,70); mvLeftAnim->Play();
	atkRightAnim->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	atkLeftAnim->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	atkRightAnim->SetHotSpot(33,69); //atkRightAnim->Play();
	atkLeftAnim->SetHotSpot(33,69); //atkLeftAnim->Play();
	bullet = new Bullet("player\\arrow.png");

	x = 100.0f; y = 400.0f;
	dx = PLAYERSPEED; dy = JUMPSPEED;
	float jpctrl = 0.0f;
	scale = 1.0f; rot = 0.0f;
	dscale = 0.0f; drot = 0.0f;

	ready = false;
	status /*= lastStatus*/ = PLAYER_STOP;
	for(int i=0; i != 4; ++i)
	{
		bulp[i].enabled = false;
	}


	skill = new Sbility[4];
	sk = skill;
	skill[0].init("skills\\skill0.png",8,8,0,0,440,300);
	skill[0].sbAnim->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	skill[1].init("skills\\skill1.png",2,4,0,0,147,82);
	skill[1].dx = 4.0f;
	skill[2].init("skills\\skill2.png",4,4,0,0,68,178);
	skill[2].sbAnim->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	skill[3].init("skills\\skill3.png",6,6,0,0,120,160);
	render = &Player::player_render;
	frame = &Player::player_frame;
	isAttacking = false;
	isforwarding = false;
}

Player::~Player()
{
	g_hge->Texture_Free(standTex);
	g_hge->Texture_Free(jumpTex);
	g_hge->Texture_Free(moveTex);
	g_hge->Texture_Free(attackTex);
	delete stRightAnim;
	delete stLeftAnim;
	delete jpRightAnim;
	delete jpLeftAnim;
	delete mvRightAnim;
	delete mvLeftAnim;
	delete atkRightAnim;
	delete atkLeftAnim;
	delete[] skill;
}

void Player::player_frame(float dt)
{
	static float t = 0.0f;
	static int shoot = -1;
	if(!ready)
	{

		y -= (dy -= dt * GRAVITY);
		if(dy > 0) stage->mp->check_up(x,y);
		if(dy < 0)	stage->mp->frame(x,y);
		if( y > 590.0f)
		{
			die();
		}
	}

	for(int i=0; i != 4; ++i)
		if(bulp[i].enabled)
		{
			bulp[i].x += bulp[i].dx * dt;
			bulp[i].y += bulp[i].dy * dt;
			if(bulp[i].x < 0 || bulp[i].x > SCREEN_WIDTH || bulp[i].y <0 || bulp[i].y > SCREEN_HEIGHT)
				bulp[i].enabled = false;
			for(vector<Boss*>::iterator iter = stage->memy->vecboss.begin(); iter != stage->memy->vecboss.end(); ++iter)
			{
				if(!(*iter)->iskilled && (*iter)->check_shoot(bulp[i].x,bulp[i].y))
				{
					bulp[i].enabled = false;
					Stage::playsound(Stage::hit);
					break;
				}
			}
		}

	if(isAttacking)
	{
		if(atkRightAnim->IsPlaying())
		{
			atkRightAnim->Update(dt);
			return;
		}
		else if(atkLeftAnim->IsPlaying())
		{
			atkLeftAnim->Update(dt);
			return ;
		}
		else
		{
			isAttacking = false;
			if(shoot != -1)
			{
				bulp[shoot].enabled = true;
				shoot = -1;
			}
		}
	}

	if(g_hge->Input_GetKeyState(KEY_LEFT))
	{
		if(stage->mp->check_left(x,y) && x > 0) x -= dx * PLAYERSPEED;
		status = PLAYER_HEAD_LEFT;

		stage->mp->check_left(x,y);
		stage->mp->check_down(x,y);
		mvLeftAnim->Update(dt);
		isforwarding = true;
	}
	if(g_hge->Input_GetKeyState(KEY_RIGHT))
	{
		if(stage->mp->check_right(x,y))
		if(x < BGMOVEPOSITION) x += dx * PLAYERSPEED;
		else /*if(stage->mp->pos < stage->mp->mapLen)*/
		{
			stage->pathLen += dt * BGSPEED;

			stage->mp->pos += 4/*stage->mp->fpos*/;
			if(Sbility::isUsing)
				Sbility::x -= 4.0f;

			for(vector<Boss*>::iterator iter = stage->memy->vecboss.begin(); iter != stage->memy->vecboss.end(); ++iter)
			{
				(*iter)->x -= 4.0f;
			}
		}
		status = PLAYER_HEAD_RIGHT;

		stage->mp->check_down(x,y);
		mvRightAnim->Update(dt);
		isforwarding = true;
	}


	{
		stage->stageOneQuad.v[0].tx = stage->pathLen;       
		stage->stageOneQuad.v[1].tx = stage->pathLen +1.0f;
		stage->stageOneQuad.v[2].tx = stage->pathLen +1.0f;
		stage->stageOneQuad.v[3].tx = stage->pathLen;
	}
	if(g_hge->Input_GetKeyState(KEY_JUMP) && ready)
	{
		ready = false;
		dy = JUMPSPEED;
		Stage::playsound(Stage::jump);
	}


	if(Sbility::isUsing) 
	{
		sk->frame(dt);
		if(sk == skill + 3)
		{
			Sbility::x = x - 60.0f;
			Sbility::y = y - 130.0f;
		}
	}
	else if(g_hge->Input_GetKeyState(HGEK_P))
	{
		sk = skill;
		sk->enable(x,y-200.0f);
		Stage::playsound(Stage::bomb);
	}
	else if(g_hge->Input_GetKeyState(HGEK_O))
	{
		sk = skill + 1;
		sk->enable(x,y - 62.0f);
		sk->dx = 8.0f;
	}
	else if(g_hge->Input_GetKeyState(HGEK_L))
	{
		sk = skill + 2;
		if(status == PLAYER_HEAD_RIGHT)
			sk->enable(x+100.0f,y - 150.0f);
		else sk->enable(x-130.0f,y - 150.0f);
	}
	else if(g_hge->Input_GetKeyState(HGEK_I))
	{
		sk = skill + 3;
		sk->enable(x-60.0f,y - 130.0f);
		Stage::playsound(Stage::fire);
	}
	if(g_hge->Input_GetKeyState(HGEK_U))
	{
		Sbility::isUsing = false;
		sk->sbAnim->Stop();
	}

	if(g_hge->Input_GetKeyState(KEY_ATTACK))
	{
		for(int i=0; i != 4; ++i)
			if(!bulp[i].enabled)
			{
				bulp[i].x = x;
				bulp[i].y = y-30.0f;
				switch(status)
				{
				case PLAYER_HEAD_LEFT:	
					bulp[i].dx = -1000.0f; bulp[i].dy = 0.0f; bulp[i].isRight = false;
					isAttacking = true;
					atkLeftAnim->Play();
					break;
				case PLAYER_HEAD_RIGHT:	
					bulp[i].dx = 1000.0f; bulp[i].dy = 0.0f; bulp[i].isRight = true; 
					isAttacking = true;
					atkRightAnim->Play();
					break;

				}

				shoot = i;
				Stage::playsound(Stage::shoot);

				if(!ready) dy = 0.0f;
				break;
			}
	}



}

void Player::player_render()
{
	if(isAttacking)
	{
		if(status == PLAYER_HEAD_RIGHT)
		{
			atkRightAnim->Render(x,y);
		}
		else
			atkLeftAnim->Render(x,y);

	}
	else if(ready)
	{
		if(isforwarding)
		{
			if(status == PLAYER_HEAD_RIGHT)
				mvRightAnim->Render(x,y);
			else
				mvLeftAnim->Render(x,y);
			isforwarding = false;
		}
		else
		{
			if(status == PLAYER_HEAD_RIGHT)
				stRightAnim->Render(x,y);
			else
				stLeftAnim->Render(x,y);
		}

	}
	else
	{
		if(status == PLAYER_HEAD_RIGHT)
			jpRightAnim->Render(x,y);
		else
			jpLeftAnim->Render(x,y);
	}


	for(int i=0; i != 4; ++i)
		if(bulp[i].enabled)
		{
			if(bulp[i].isRight)
				bullet->bulSprRight->Render(bulp[i].x,bulp[i].y);
			else
				bullet->bulSprLeft->Render(bulp[i].x,bulp[i].y);
		}
	if(Sbility::isUsing)
	{
		sk->render();
	}
}

void Player::die()
{
	_beginthread(gameOver,0,NULL);
	frame = &Player::die_frame;   //player will die..
	render = &Player::die_render;
	ready = true;

	if(y<60.0f || y > 590.0f || x < 10.0f)
	{
		x = 200.0f;
		y = 200.0f;
		dy = -2.0f;
		dx = 3.5f;
	}
}

void Player::die_frame(float dt)
{
	x += dx;
	y += dy;
	if(x < 0.0f || x > 700.0f)
		dx = -dx;
	if(y < 0.0f || y > 550.0f)
		dy = -dy;
}

void Player::die_render()
{
	stage->fnt->SetScale(3.0f);
	stage->fnt->printfb(x,y,300,60,HGETEXT_LEFT,"Game Over !!\n Press ESC to quit!!!");
	stage->fnt->SetScale(1.0f);
}

Bullet::Bullet(char *filename)
{
	bulTex = g_hge->Texture_Load(filename);
	bulSprRight = new hgeSprite(bulTex,0,0,100,100);
	bulSprRight->SetHotSpot(30,50);
	bulSprLeft = new hgeSprite(bulTex,100,0,100,100);
	bulSprLeft->SetHotSpot(70,50);
}

Bullet::~Bullet()
{
	g_hge->Texture_Free(bulTex);
	delete bulSprRight;
	delete bulSprLeft;

}


Block::Block(char* filename)
{
	blkTex = g_hge->Texture_Load(filename);
	blkSpr = new hgeSprite(blkTex,0,0,60,60);
	blkSpr->SetHotSpot(/*30,30*/0,0);
}

Block::Block()
{
	blkTex = 0;
	blkSpr = NULL;
}

void Block::init(char* filename,float w,float h)
{
	blkTex = g_hge->Texture_Load(filename);
	blkSpr = new hgeSprite(blkTex,0,0,w,h);
	blkSpr->SetHotSpot(/*30,30*/0,0);
}

Block::~Block()
{
	g_hge->Texture_Free(blkTex);
	delete blkSpr;
}

bool Sbility::isUsing = false;
float Sbility::x = 0,Sbility::y = 0;

void Sbility::init(char* filename,int nframes,float fps,float x,float y,float w,float h)
{
	aniTex = g_hge->Texture_Load(filename);
	sbAnim = new hgeAnimation(aniTex,nframes,fps,x,y,w,h);
/*	sbAnim->SetHotSpot(x,y);*/
}

Sbility::~Sbility()
{
	g_hge->Texture_Free(aniTex);
	delete sbAnim;
}

void Sbility::frame(float dt)
{
	sbAnim->Update(dt);
	x += dx;
	if(x < 0.0f || x > 800.0f)
		sbAnim->Stop();
}

void Sbility::render()
{
	if(sbAnim->IsPlaying())
		sbAnim->Render(x,y);
	else isUsing = false;
}

void Sbility::enable(float fx,float fy)
{
	x = fx;y = fy;
	sbAnim->Play();
	isUsing = true;
}

void gameOver(void*)
{
	static bool busy = false;
	if(busy) return;
	busy = true;
	HEFFECT gv = g_hge->Effect_Load("gameover.mp3");
	if(!gv)
		return ;
	g_hge->Effect_Play(gv);
	Sleep(5000);
	g_hge->Effect_Free(gv);
	busy = false;
}

Boss::Boss(hgeAnimation* anim,float fx,float fy,float fdx,float fdy)
{
	bosAnim = anim;
	x = fx; y = fy;
	dx = fdx; dy = fdy;
	iskilled = false;
	rot = 0.0f;
	scale = 1.0f;
}

bool Boss::check_hit(float fx,float fy)
{
	if(!iskilled && fabs(x-fx) < 60.0f && fabs(y-fy) < 50.0f)
		return true;
	return false;
}

bool Boss::check_shoot(float fx,float fy)
{
	float /*ddx = fx - x,*/ddy = y - fy;
	if(fabs(fx-x) < 33.0f && ddy > 0.0f && ddy < 67.0f)
	{
		iskilled = true;
		return true;
	}

	return false;
}

void Boss::frame(float dt)
{
	if(iskilled)
	{
		rot += 10.0f;
		scale *= 0.95f;
		return ;
	}
	bosAnim->Update(dt);
	x += dx;
	y += dy;
}

void Boss::render()
{
	if(iskilled)
	{
		bosAnim->RenderEx(x,y,rot,scale,scale);
		return ;
	}
	bosAnim->Render(x,y);
}

SpcBoss::SpcBoss(hgeAnimation* anim,hgeAnimation* bacanim,float fx,float fy,float fdx,float fdy)
	: Boss(anim,fx,fy,fdx,fdy)
{
	specAnim = bacanim;
	isFalling = false;
}

bool SpcBoss::check_hit(float fx,float fy)
{
	if(!iskilled && fabs(x-fx) < 50.0f && fabs(y-fy) < 60.0f)
		return true;
	return false;
}

bool SpcBoss::check_shoot(float fx,float fy)
{
	float /*ddx = fx - x,*/ddy = y - fy;
	if(/*ddx < 45.0f && ddx >0.0f*/fabs(x-fx) < 30.0f && ddy > 0.0f && ddy < 60.0f)
	{
		iskilled = true;
		return true;
	}
	return false;
}

void SpcBoss::frame(float dt)
{
	if(iskilled)
	{
		rot += 10.0f;
		scale *= 0.95f;
		return ;
	}
	if(isFalling)
	{
		dy += 0.2f;
		y += dy;
		check_fall();
	}
	else check_down();

	if(dx<=0.0f)
	{
		bosAnim->Update(dt);
		x += dx;
		check_left();
//		y += dy;
	}
	else
	{
		specAnim->Update(dt);
		x += dx;
		check_right();
//		y += dy;
	}
}

void SpcBoss::render()
{
	if(iskilled)
	{
		bosAnim->RenderEx(x,y,rot,scale,scale);
		return ;
	}

	if(dx<=0.0f)
	{
		bosAnim->Render(x,y);
	}
	else
	{
		specAnim->Render(x,y);
	}
}

void SpcBoss::check_down()
{
	if(stage->mp->bossCheckDown(x,y))
		isFalling = true;
}

void SpcBoss::check_fall()
{
	if(stage->mp->bossCheckFall(x,y))
	{
		isFalling = false;
		dy = 0.0f;
	}
}

void SpcBoss::check_left()
{
	if(!stage->mp->check_left(x,y))
		dx = -dx;
}

void SpcBoss::check_right()
{
	if(!stage->mp->check_right(x,y))
		dx = -dx;
}
UDBoss::UDBoss(hgeAnimation* anim,float fx,float fy,float fdx,float fdy)
	: Boss(anim,fx,fy,fdx,fdy)
{
	t = 0.0f;
	udy = fy;
	uddy = fdy;
}

bool UDBoss::check_hit(float fx,float fy)
{
	if(!iskilled && fabs(x-fx) < 60.0f && fabs(y-fy) < 60.0f)
		return true;
	return false;
}

bool UDBoss::check_shoot(float fx,float fy)
{
	float /*ddx = fx -30.0f - x,*/ddy = y - fy;
	if(/*ddx < 40.0f && ddx >0.0f*/fabs(x-fx) < 30.0f && ddy > 0.0f && ddy < 60.0f)
	{
		iskilled = true;
		return true;
	}

	return false;
}

void UDBoss::frame(float dt)
{
	if(iskilled)
	{
		rot += 10.0f;
		scale *= 0.95f;
		return ;
	}

	bosAnim->Update(dt);
	x += dx;
	dy -= 0.5f;
	y -= dy;

	if(y > udy)
	{
		y = udy;
		dy = uddy;
		Stage::playsound(Stage::udboss);
	}
}

void UDBoss::render()
{
	if(iskilled)
	{
		bosAnim->RenderEx(x,y,rot,scale,scale);
		return ;
	}

	bosAnim->Render(x,y);
}
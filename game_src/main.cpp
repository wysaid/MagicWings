
#include "stdafx.h"

HGE* g_hge = NULL;

HTEXTURE g_cursortex,g_bgtex,g_partex;
hgeQuad g_quad;
HEFFECT g_menuSnd;
hgeFont *g_fnt;
hgeGUI *g_gui;
hgeSprite *g_cursor,*g_par_spr,*g_bgspr;
hgeParticleSystem *g_par = NULL;

Func framefunc = menuFrameFunc;
Func renderfunc = menuRenderFunc;

// 游戏主函数，完成一系列游戏引擎初始化的设置，然后开始游戏。

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	g_hge = hgeCreate(HGE_VERSION);

	g_hge->System_SetState(HGE_LOGFILE, "MagicWings.log");	//设置log文件，如果游戏启动失败，可查看log文件
	g_hge->System_SetState(HGE_FRAMEFUNC, framefunc);		//设置游戏最初的FrameFunction 接口函数
	g_hge->System_SetState(HGE_RENDERFUNC, renderfunc);		//设置游戏最初的RenderFunction接口函数
	g_hge->System_SetState(HGE_TITLE, "MagicWings");		//设置游戏左上角标题栏文字
	g_hge->System_SetState(HGE_USESOUND, true);				//设置游戏是否静音
	g_hge->System_SetState(HGE_WINDOWED, true);				//让游戏引擎创建窗口
	g_hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);	//设置游戏引擎创建的窗口宽度
	g_hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);//设置游戏引擎创建的窗口高度
	g_hge->System_SetState(HGE_SCREENBPP, 32);				//设置窗口所使用的颜色位数
	g_hge->System_SetState(HGE_SHOWSPLASH,false);			//隐藏游戏引擎logo
	g_hge->System_SetState(HGE_FPS, 60);					//设置游戏的帧率为约 60 fps
	if(g_hge->System_Initiate())
	{
		if(initmenu())
			g_hge->System_Start();
	}
	else
		MessageBox(NULL,g_hge->System_GetErrorMessage(),"我也不晓得咋地了……跑不动啊……",MB_ICONERROR);
	cleanMenu();
	g_hge->System_Shutdown();
	g_hge->Release();
	return 0;
}

#pragma once

#include "stdafx.h"
#ifndef _STAGE_H_
#define _STAGE_H_

#include "wysaid.h"

class Stage				// 关卡模块
{
public:
	Stage();
	~Stage();
	void render();		//关卡模块只保留一个RenderFunction和一个FrameFunction 接口函数，
	void frame();		//直接完成一次关卡中所有元素的渲染。
	static HEFFECT bomb,hit,shoot,pig,udboss,fire,jump;
	static bool frameFunc();
	static bool renderFunc();
	static void playsound(HEFFECT);
	static void youWin();

	Player *player;
	MwMap *mp;
	MwEnemy *memy;

	hgeQuad stageOneQuad;
	HEFFECT bgSnd;
	hgeFont *fnt;

	bool ready;
	bool bgforward;
	float pathLen;

};

bool winFrame();
bool winRender();

#endif
#pragma once
#include "StdAfx.h"

#ifndef _MENUITEM_H_
#define _MENUITEM_H_

class hgeGUIMenuItem : public hgeGUIObject		//GUI菜单模块
{
public:
	hgeGUIMenuItem(int id, hgeFont *g_fnt, HEFFECT snd, float x, float y, float delay, char *title);

	virtual void	Render();
	virtual void	Update(float dt);

	virtual void	Enter();
	virtual void	Leave();
	virtual bool	IsDone();
	virtual void	Focus(bool bFocused);
	virtual void	MouseOver(bool bOver);

	virtual bool	MouseLButton(bool bDown);
	virtual bool	KeyClick(int key, int chr);

private:
	hgeFont		*fnt;
	HEFFECT		snd;
	float		delay;
	char		*title;

	hgeColor	scolor, dcolor, scolor2, dcolor2, sshadow, dshadow;
	hgeColor	color, shadow;
	float		soffset, doffset, offset;
	float		timer, timer2;
};

struct Author
{
	float x,y;
	float dx,dy;
	float scale,rot;
	float dscale,drot;
	DWORD color;
};

#endif
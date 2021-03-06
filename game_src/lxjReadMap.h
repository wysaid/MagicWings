#pragma once

#include "StdAfx.h"

#ifndef _MW_READ_MAP_H_
#define _MW_READ_MAP_H_

#define MAXMAPLEN 2000

class MwMap		//地图模块
{
	friend class Player;
public:
	MwMap(char*);
	~MwMap();
	
	//接口函数
	void render();
	void frame(float& x,float& y);
	
	//提供多个碰撞检测函数，完成游戏中碰撞检测
	void check_down(float x,float y);
	void check_up(float x,float y);
	bool check_left(float &x,float &y);
	bool check_right(float &x,float &y);

	bool bossCheckDown(float,float);
	bool bossCheckFall(float,float&);
	
private:
	unsigned __int64 smp[MAXMAPLEN];	//游戏地图的存储，游戏地图由 游戏地图编辑器完成
	int blkNum;
	int pos;
	float fpos;
	int mapLen;
	Block* blk;
	Block* backgr;
};

class MwEnemy		//敌人模块， 包含了多个Boss类的实体，Boss模块的完成在wysaid.cpp中。
{
	friend class MwMap;
	friend class Player;
public:
	MwEnemy(char*);
	~MwEnemy();
	void render();
	void frame(float);

private:
	vector<Boss*> vecboss;
	HTEXTURE bosTex[8];
	hgeAnimation *anim[8];
	hgeAnimation *anback[3];
/*	Enemy *emy[30];*/
/*	int emynum;*/
};

#endif
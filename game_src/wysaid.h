#pragma once

#include "StdAfx.h"
#ifndef _WYSAID_H_
#define _WYSAID_H_

//定义玩家图像大小
#define PLAYERWIDTH			64
#define PLAYERHEIGHT		64

//定义玩家的中心
#define PLAYERHALFWIDTH		32
#define PLAYERHALFHEIGHT	32

//定义游戏中的参数
#define AUTHORNUM		50			// About us界面下，作者回旋的图像数
#define JUMPSPEED		11.5f		// 玩家跳起时的初始速度
#define GRAVITY			20.0f		// 游戏中的重力加速度
#define BGMOVEPOSITION	400.0f		// 背景中心偏移坐标
#define PLAYERSPEED		2.0f		// 玩家前进的速度
#define BGSPEED			0.02f		// 背景移动的速度

						//玩家自行设置的快捷键,你懂的
#define KEY_LEFT			HGEK_A   
#define KEY_RIGHT			HGEK_D
#define KEY_UP				HGEK_W
#define KEY_DOWN			HGEK_S
#define KEY_ATTACK			HGEK_J
#define KEY_JUMP			HGEK_K
#define KEY_EXCEPTIONAL		HGEK_L

#define PLAYER_STOP				0xf00			//停止走动
#define PLAYER_HEAD_RIGHT		0x1				//朝右
#define PLAYER_HEAD_LEFT		0x10			//朝左
#define PLAYER_HEAD_UP			0x8				//朝上
#define PLAYER_HEAD_DOWN		0x80			//朝下
#define PLAYER_CROUCH			0x800			//蹲下
#define PLAYER_STOP_RIGHT		0xf01			//停向右
#define PLAYER_STOP_LEFT		0xf10			//停向左
#define PLAYER_JUMP				0x2				//跳起
#define PLAYER_DIE				0x100			//挂了
#define PLAYER_JUMP_RIGHT		0x3				//向右跳
#define PLAYER_JUMP_LEFT		0x12			//向左跳
#define PLAYER_HURT				0x200			//受伤
#define PLAYER_INVINCIBLE		0x400			//无敌状态

typedef bool (*Func)();		//定义游戏回调函数指针类型

struct BulPosition		// 定义玩家射出的箭的数据结构，保证箭的飞行
{
	float x,y;
	float dx,dy;
	bool enabled;
	bool isRight;
};


struct Bullet		// 保存玩家的箭（子弹）的图像 
{
	Bullet(char* filename);
	~Bullet();

	HTEXTURE bulTex;

	hgeSprite *bulSprRight;
	hgeSprite *bulSprLeft;
};

class Sbility		// 玩家技能模块
{
public:
	Sbility(){dx =0.0f;}
	void init(char*,int,float,float,float,float,float);
	~Sbility();
	void frame(float);
	void render();
	void enable(float,float);
	HTEXTURE aniTex;
	hgeAnimation* sbAnim;
	static bool isUsing;
	static float x,y;
	float dx;
	bool forward();
};

class Player		//玩家模块
{
public:
	Player();
	~Player();
	void player_render();		//玩家进行游戏的RenderFunction 接口函数
	void player_frame(float);	//玩家进行游戏的FrameFunction 接口函数
	bool head_right();			//玩家向右
	bool head_left();			//玩家向左
	void look_up();
	void crouch();
	void die();					//玩家死亡
	void die_render();			//玩家死亡以后的RenderFunction 接口函数
	void die_frame(float);		//玩家死亡以后的FrameFunction 接口函数
	void (Player::*render)();	//玩家的RenderFunction接口函数，可能的值为player_render或者die_render
	void (Player::*frame)(float); // 玩家的FrameFunction 接口函数，可能的值为player_frame或者die_frame


						// 保存玩家的
	HTEXTURE standTex;			// 静止
	HTEXTURE jumpTex;			// 跳起
	HTEXTURE moveTex;			// 移动
	HTEXTURE attackTex;			// 攻击 的图像

							// 保存玩家的各种动作的动画
	hgeAnimation *stRightAnim,*stLeftAnim;
	hgeSprite *jpRightAnim,*jpLeftAnim;
	hgeAnimation *mvRightAnim,*mvLeftAnim;
	hgeAnimation *atkRightAnim,*atkLeftAnim;

				//玩家死亡时的缩放、旋转等。
	float scale,rot;
	float dscale,drot;
				
				//玩家的子弹，玩家一次最多发射四枚子弹，可在代码中修改
	BulPosition bulp[4];	
	Bullet* bullet;
				
				//玩家的技能 
	Sbility *skill;	//玩家的所有技能
	Sbility *sk;	//玩家当前使用的技能，当sk == NULL 时，玩家未使用技能

	int hp;				//玩家的血量
	int mana;			//玩家的魔法值
	DWORD status;		//玩家的状态
	DWORD lastStatus;	//玩家的上一个状态
	bool ready;			//玩家的脚是否着地
	bool isforwarding;	//玩家是否正在走动
	bool isAttacking;	//玩家是否正在攻击

	float x,y;			//玩家所在坐标
	float dx,dy;		//玩家死亡后灵魂坐标每一帧的偏移量
 };

class Block				//障碍物模块
{
public:
	Block(char*);
	Block();
	void init(char*,float w = 60.0f,float h = 60.0f);
	~Block();
	HTEXTURE blkTex;
	hgeSprite *blkSpr;
};

class Boss			//Boss 模块，由虚类多态完成，Boss类表示横冲直撞的小猪
{
public:
	Boss(hgeAnimation* anim,float fx,float fy,float fdx = 0.0f,float fdy = 0.0f);

	virtual bool check_hit(float fx,float fy);

	virtual bool check_shoot(float fx,float fy);

	virtual void frame(float dt);

	virtual void render();

	virtual ~Boss(){}
	hgeAnimation *bosAnim;
	float x,y;
	float dx,dy;
	bool iskilled;
	float rot,scale;
};

class SpcBoss : public Boss		//属于Boss模块，表示游走的幽灵
{
public:
	SpcBoss();
	SpcBoss(hgeAnimation* anim,hgeAnimation* bacanim,float fx,float fy,float fdx = 0.0f,float fdy = 0.0f);

	~SpcBoss(){}

	bool check_hit(float fx,float fy);

	bool check_shoot(float fx,float fy);

	void frame(float dt);

	void render();

	void check_down();

	void check_fall();

	void check_left();

	void check_right();

	hgeAnimation* specAnim;
	bool isFalling;
};

class UDBoss : public Boss			//属于Boss模块，表示上串下跳的小瓶子。
{
public:
	UDBoss();
	UDBoss(hgeAnimation* anim,float fx,float fy,float fdx = 0.0f,float fdy = 0.0f);

	~UDBoss(){}

	bool check_hit(float fx,float fy);

	bool check_shoot(float fx,float fy);

	void frame(float dt);

	void render();

	float t;
	float udy,uddy;
};

class readThreadCtrl			//为菜单中各个选项阅读而创建的线程控制类。
{
public:
	readThreadCtrl()
	{
		loop = true;
		*filename = 0;
	}
	readThreadCtrl(bool ctrl,char* fname)
	{
		loop = ctrl;
		strcpy(filename,fname);
	}
	bool loop;
	char filename[32];
};


bool initmenu();
bool menuFrameFunc();
bool menuRenderFunc();
bool aboutUsFrame();
bool aboutUsRender();
bool initAuthor();
bool initOptions();
bool initInstructions();
bool cleanAuthor();
bool cleanMenu();

bool cleanInstructionAndOption();
bool instructionAndOptionRender();
bool instructionAndOptionFrame();
void readStatement(void*);
void gameOver(void*);
#endif
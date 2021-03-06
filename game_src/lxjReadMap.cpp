#include "StdAfx.h"
#include "lxjReadMap.h"

extern Stage* stage;
extern HGE* g_hge;
MwMap::MwMap(char* docname)
{
	char dir[128] = ".\\",filename[128];
	strcat(strcat(dir,docname),"\\");	
	strcat(strcat(strcpy(filename,dir),docname),".lxj");

	blkNum = 32;

	FILE* fp = fopen(filename,"rb");
	mapLen = fread(smp,sizeof(unsigned __int64),1,fp);
	for(; fread(smp + mapLen,sizeof(unsigned __int64),1,fp) == 1 && mapLen < MAXMAPLEN; ++mapLen);
	for(int i = mapLen+1; i < MAXMAPLEN; ++i)
		smp[i] = -1;
	pos = 0;
	fpos = 0.0f;
	blk = new Block[blkNum];
	backgr = new Block[15];
	for(int i =0; i != blkNum; ++i)
	{
		sprintf(filename,"%sblock_img%d.png",dir,i);
		blk[i].init(filename);

	}

	sprintf(filename,"%senemy_img%d.png",dir,16);
	backgr[0].init(filename,89,60);
	sprintf(filename,"%senemy_img%d.png",dir,17);
	backgr[1].init(filename,60,60);
	sprintf(filename,"%senemy_img%d.png",dir,18);
	backgr[2].init(filename,60,60);
	sprintf(filename,"%senemy_img%d.png",dir,19);
	backgr[3].init(filename,360,337);
	sprintf(filename,"%senemy_img%d.png",dir,20);
	backgr[4].init(filename,134,67);
	sprintf(filename,"%senemy_img%d.png",dir,21);
	backgr[5].init(filename,180,89);
	sprintf(filename,"%senemy_img%d.png",dir,22);
	backgr[6].init(filename,180,78);
	sprintf(filename,"%senemy_img%d.png",dir,23);
	backgr[7].init(filename,83,59);
	sprintf(filename,"%senemy_img%d.png",dir,24);
	backgr[8].init(filename,50,77);
	sprintf(filename,"%senemy_img%d.png",dir,25);
	backgr[9].init(filename,60,66);
	sprintf(filename,"%senemy_img%d.png",dir,26);
	backgr[10].init(filename,160,61);
	sprintf(filename,"%senemy_img%d.png",dir,27);
	backgr[11].init(filename,223,128);
	sprintf(filename,"%senemy_img%d.png",dir,28);
	backgr[12].init(filename,306,316);
	sprintf(filename,"%senemy_img%d.png",dir,29);
	backgr[13].init(filename,184,79);
	sprintf(filename,"%senemy_img%d.png",dir,30);
	backgr[14].init(filename,241,333);
}

MwMap::~MwMap()
{
	delete[] blk;
//	delete[] backgr;
/*	delete[] emy;*/
}

void MwMap::frame(float& x,float& y)
{
	if(y < 60.0f) return;
	int ix = (x+pos)/60,iy = y/60;
	int ix1 = (x/*-20.0f*/+pos)/60/*,ix2 = (x+20.0f+pos)/60*/;
	unsigned tmp1 = smp[ix1] >> iy * 6,tmp2 = smp[ix] >> ((int)(y-15.0f)/60*6);
	if((tmp1 & 32) /*|| (tmp3 & 63) == 63*/ || (tmp2 & 63) != 63) //待完善
		return ;
	y = iy*60;
	stage->player->ready = true;
	stage->player->dy = 0.0f;
}

void MwMap::check_down(float x,float y)
{
	int ix1 = (x-20.0f+pos)/60,ix2 = (x+20.0f+pos)/60,iy = y/60 + 0.9f;
	unsigned tmp1 = smp[ix1] >> iy * 6,tmp2 = smp[ix2] >> iy*6;
	if((tmp1 & 63) == 63 && (tmp2 & 63) == 63)
	{
		stage->player->ready = false;
/*		stage->player->jpctrl = stage->player->dy;*/
	}
}

void MwMap::check_up(float x,float y)
{
	if(y < 60.0f) return;
	int ix1 = (x-20.0f+pos)/60,ix2 = (x+20.0f+pos)/60,iy = (y - 60.0f)/60;
	unsigned tmp1 = smp[ix1] >> iy * 6,tmp2 = smp[ix2] >> iy*6;

	if(((tmp1 & 63) == 63 && (tmp2 & 63) == 63) || ((tmp1&63) > 5 && (tmp2 & 63) > 5))
		return ;

	stage->player->dy = 0.0f;
}

bool MwMap::check_left(float &x,float &y)
{
	if(y < 0.0f) return true;
	int ix = (x-PLAYERHALFWIDTH+pos)/60,iy = (y-0.01f)/60 ;
	unsigned tmp1 = smp[ix] >> iy * 6/*,tmp2 = smp[ix] >> ((int)(y-15.0f)/60*6)*/;
	if((tmp1 & 63) == 63 || (tmp1 & 63) > 5)
		return true;
	return false;
}

bool MwMap::check_right(float &x,float &y)
{
	if(y < 0.0f) return true;
	int ix = (x+PLAYERHALFWIDTH+pos)/60,iy = (y-0.01f)/60 ;
	unsigned tmp1 = smp[ix] >> iy * 6/*,tmp2 = smp[ix] >> ((int)(y-15.0f)/60*6)*/;
	if((tmp1 & 63) == 63 || (tmp1 & 63) > 5)
		return true;
	return false;
}

void MwMap::render()
{
	int tmp1;
	int x,y;
	unsigned __int64 tmp2;
	for(x=-(pos%60),tmp1 = pos/60; x <800; x += 60, ++tmp1)
		for(y=0,tmp2 = smp[tmp1]; y < 600; y += 60)
		{
			if((tmp2 & 32) && (tmp2&31) == 19)
			{
				backgr[(tmp2&31) - 16].blkSpr->Render(x,y);
				if(x < 250)
				{
					Stage::youWin();
				}
			}
			if(!(tmp2 & 32))
			{
				blk[(tmp2&31)/* % blkNum*/].blkSpr->Render(x,y);
			}
			else
			{
				if((tmp2&31) < 8)
				{
					switch(tmp2&31)
					{
					case 0:
						stage->memy->vecboss.push_back(new Boss(stage->memy->anim[0],x+30.0f,y +60.0f,-6.0f));
						Stage::playsound(Stage::pig);
						break;
					case 1:
						stage->memy->vecboss.push_back(new SpcBoss(stage->memy->anim[1],stage->memy->anback[(tmp2&31) -1],x+30.0f,y + 60.0f,-2.0f));
						break;
					case 2:
						stage->memy->vecboss.push_back(new SpcBoss(stage->memy->anim[2],stage->memy->anback[(tmp2&31) -1],x+30.0f,y + 60.0f,-3.0f));
						break;
					case 3:
						stage->memy->vecboss.push_back(new SpcBoss(stage->memy->anim[3],stage->memy->anback[(tmp2&31) -1],x+30.0f,y + 60.0f,-4.0f));
						break;
					case 4:
						stage->memy->vecboss.push_back(new UDBoss(stage->memy->anim[4],x+30.0f,y+60.0f,0.0f,15.0f));
						break;
					case 5:
						stage->memy->vecboss.push_back(new UDBoss(stage->memy->anim[5],x+30.0f,y+60.0f,0.0f,10.0f));
						break;
					case 6:

					default:;
					}
					smp[tmp1] |= 63ull << (y/60)*6;
				}

			}
			tmp2 >>= 6;
		}

}

bool MwMap::bossCheckDown(float x,float y)
{
	int ix1 = (x-20.0f+pos)/60,ix2 = (x+20.0f+pos)/60,iy = y/60 + 0.9f;
	unsigned tmp1 = smp[ix1] >> iy * 6,tmp2 = smp[ix2] >> iy*6;
	if((tmp1 & 63) == 63 && (tmp2 & 63) == 63)
	{
		return true;
	}
	return false;
}

bool MwMap::bossCheckFall(float x,float &y)
{
	int ix = (x+pos)/60,iy = y/60;
	int ix1 = (x+pos)/60;
	unsigned tmp1 = smp[ix1] >> iy * 6,tmp2 = smp[ix] >> ((int)(y-15.0f)/60*6);

	if((tmp1 & 32) || (tmp2 & 63) != 63) 
		return false;
	y = iy*60;
	return true;
}

MwEnemy::MwEnemy(char* docname)
{
	char dir[128] = ".\\",filename[128];
	strcat(strcat(dir,docname),"\\");

	sprintf(filename,"%senemy_img%d.png",dir,0);
	bosTex[0] = g_hge->Texture_Load(filename);
	anim[0] = new hgeAnimation(bosTex[0],2,6,0,0,67,67);
	anim[0]->SetHotSpot(63,67);
	for(int i=1; i != 4; ++i)
	{
		sprintf(filename,"%senemy_img%d.png",dir,i);
		bosTex[i] = g_hge->Texture_Load(filename);
		anim[i] = new hgeAnimation(bosTex[i],4,4,0,0,45,60);
		anback[i-1] = new hgeAnimation(bosTex[i],4,4,0,60,45,60);
		anim[i]->SetHotSpot(22,60);
		anback[i-1]->SetHotSpot(22,60);
	}

	for(int i=4; i != 7; ++i)
	{
		sprintf(filename,"%senemy_img%d.png",dir,i);
		bosTex[i] = g_hge->Texture_Load(filename);
		anim[i] = new hgeAnimation(bosTex[i],4,2,0,0,100,60);
		anim[i]->SetHotSpot(50,60);
	}

	sprintf(filename,"%senemy_img%d.png",dir,7);
	bosTex[7] = g_hge->Texture_Load(filename);
	anim[7] = new hgeAnimation(bosTex[7],4,4,0,0,100,60);
	anim[7]->SetHotSpot(50,60);
	for(int i=0; i!=8; ++i)
	{
		anim[i]->Play();
	}
	for(int i=0; i != 3; ++i)
		anback[i]->Play();
}

MwEnemy::~MwEnemy()
{
 	for(int i =0; i != 7;++i)
	{
		g_hge->Texture_Free(bosTex[i]);
		delete anim[i];
	}
	for(int i=0; i != 3; ++i)
	{
		delete anback[i];
	}
	vecboss.clear();
}

void MwEnemy::frame(float dt)
{

	for(int i=0; i != vecboss.size(); ++i)
	{
		vecboss[i]->frame(dt);
		if(vecboss[i]->x < 60.0f || vecboss[i]->x > 900.0f || vecboss[i]->scale < 0.001f)
		{
			delete vecboss[i];
			vecboss.erase(vecboss.begin() + i);
			--i;
		}
	}
}

void MwEnemy::render()
{

	for(int i = 0; i != vecboss.size(); ++i)
	{
		if(vecboss[i]->check_hit(stage->player->x,stage->player->y))
			stage->player->die();
		vecboss[i]->render();
	}
}
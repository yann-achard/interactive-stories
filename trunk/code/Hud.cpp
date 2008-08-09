#include "Hud.h"
#include "Group.h"
#include "Clan.h"
#include "Game.h"
#include "Map.h"
#include "Federation.h"
#include "d3ddefs.h"
#include "main.h"
//---------------------------------------------------------
static char* apstrs[8] = {
	"\n\n\n",
	"\n\n\n\n\n",
	"\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
};
static double attackbonus[] = {
	0.0, 1.0, 1.1, 1.3, 1.6, 2.0, 2.5, 4.1, 4.7
};
//---------------------------------------------------------
Hud::Hud(){
	show = true;
	getmouse = false;
	caption = false;
	font = NULL;
	tex = NULL;
	frame_counter = 0;
	last_time = 0;

	D3DXFONT_DESC fontDesc = {16,0,600,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,
                          CLIP_DEFAULT_PRECIS,FF_MODERN,"CourierNew"};
	D3DXCreateFontIndirect(g_device,&fontDesc,&font);

	InitLeftPanel();
	InitAttackPanel();
	InitGoldPanel();
	InitOffer();

	UpdateSelect();
}
//---------------------------------------------------------
void Hud::InitOffer(){
	offer = false;
	goldoffer = false;

	of_vb = NULL;
	of = new s_vert2dc[15];
	// Gold offer 
	of[0].set(g_winx/2+80, g_winy/2-233, 0xFFFFFFFF);
	of[1].set(g_winx/2+220, g_winy/2-233, 0xFFFFFFFF);
	of[2].set(g_winx/2+220, g_winy/2-203, 0xFFFFFFFF);
	of[3].set(g_winx/2+80, g_winy/2-203, 0xFFFFFFFF);
	of[4].set(g_winx/2+80, g_winy/2-233, 0xFFFFFFFF);

	// Peace offer
	of[5].set(g_winx/2+80, g_winy/2-112, 0xFFFFFFFF);
	of[6].set(g_winx/2+220, g_winy/2-112, 0xFFFFFFFF);
	of[7].set(g_winx/2+220, g_winy/2-82, 0xFFFFFFFF);
	of[8].set(g_winx/2+80, g_winy/2-82, 0xFFFFFFFF);
	of[9].set(g_winx/2+80, g_winy/2-112, 0xFFFFFFFF);

	// Federate
	of[10].set(g_winx/2+80, g_winy/2+15, 0xFFFFFFFF);
	of[11].set(g_winx/2+220, g_winy/2+15, 0xFFFFFFFF);
	of[12].set(g_winx/2+220, g_winy/2+45, 0xFFFFFFFF);
	of[13].set(g_winx/2+80, g_winy/2+45, 0xFFFFFFFF);
	of[14].set(g_winx/2+80, g_winy/2+15, 0xFFFFFFFF);

	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*15,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&of_vb,NULL);
	of_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,of,sizeof(s_vert2dc)*15);
	of_vb->Unlock();
}
//---------------------------------------------------------
void Hud::InitGoldPanel(){
	goldpanel = false;

	fontgp.left = (LONG)(g_winx/2-400+150);
	fontgp.right = (LONG)(g_winx/2+400+150);
	fontgp.top = (LONG)(g_winy/2-370);
	fontgp.bottom = (LONG)(g_winy/2+270);

	fontfgp.left = (LONG)(g_winx/2-300);
	fontfgp.right = (LONG)(g_winx/2-30);
	fontfgp.top = (LONG)(g_winy/2-270);
	fontfgp.bottom = (LONG)(g_winy/2+200);

	fontnfgp.left = (LONG)(g_winx/2+330);
	fontnfgp.right = (LONG)(g_winx/2+600);
	fontnfgp.top = (LONG)(g_winy/2-270);
	fontnfgp.bottom = (LONG)(g_winy/2+200);

	fedlistlen = 0;
	nfedlistlen = 0;
	fedclans = new Clan*[g_nbClans];
	nfedclans = new Clan*[g_nbClans];
	fedliststr = new char*[g_nbClans];
	nfedliststr = new char*[g_nbClans];
	fedliststances = new char*[g_nbClans];
	nfedliststances = new char*[g_nbClans];
	for (int i=g_nbClans-1; i>=0; --i) {
		fedliststances[i] = new char[8];
		nfedliststances[i] = new char[8];
	}

	gp_vb = NULL;
	gp = new s_vert2dc[38];
	// Background
	gp[0].set(g_winx/2+650, g_winy/2-400, 0xFF000000);
	gp[1].set(g_winx/2+650, g_winy/2+300, 0xFF000000);
	gp[2].set(g_winx/2-350, g_winy/2-400, 0xFF000000);
	gp[3].set(g_winx/2-350, g_winy/2+300, 0xFF000000);

	// Outline
	gp[4].set(g_winx/2+650, g_winy/2-400, 0xFFFFFFFF);
	gp[5].set(g_winx/2+650, g_winy/2+300, 0xFFFFFFFF);
	gp[6].set(g_winx/2-350, g_winy/2+300, 0xFFFFFFFF);
	gp[7].set(g_winx/2-350, g_winy/2-400, 0xFFFFFFFF);
	gp[8].set(g_winx/2+650, g_winy/2-400, 0xFFFFFFFF);

	// Close
	gp[9].set(g_winx/2+120, g_winy/2+280, 0xFFFFFFFF);
	gp[10].set(g_winx/2+180, g_winy/2+280, 0xFFFFFFFF);
	gp[11].set(g_winx/2+180, g_winy/2+245, 0xFFFFFFFF);
	gp[12].set(g_winx/2+120, g_winy/2+245, 0xFFFFFFFF);
	gp[13].set(g_winx/2+120, g_winy/2+280, 0xFFFFFFFF);

	// Vertical dividers
	gp[14].set(g_winx/2+20, g_winy/2-300, 0xFFFFFFFF);
	gp[15].set(g_winx/2+20, g_winy/2+200, 0xFFFFFFFF);
	gp[16].set(g_winx/2+280, g_winy/2-300, 0xFFFFFFFF);
	gp[17].set(g_winx/2+280, g_winy/2+200, 0xFFFFFFFF);

	// Stamina
	gp[18].set(g_winx/2+200, g_winy/2-250, 0xFFFFFFFF);
	gp[19].set(g_winx/2+200, g_winy/2-220, 0xFFFFFFFF);
	gp[20].set(g_winx/2+100, g_winy/2-220, 0xFFFFFFFF);
	gp[21].set(g_winx/2+100, g_winy/2-250, 0xFFFFFFFF);
	gp[22].set(g_winx/2+200, g_winy/2-250, 0xFFFFFFFF);

	// Attack
	gp[23].set(g_winx/2+200, g_winy/2-153, 0xFFFFFFFF);
	gp[24].set(g_winx/2+200, g_winy/2-123, 0xFFFFFFFF);
	gp[25].set(g_winx/2+100, g_winy/2-123, 0xFFFFFFFF);
	gp[26].set(g_winx/2+100, g_winy/2-153, 0xFFFFFFFF);
	gp[27].set(g_winx/2+200, g_winy/2-153, 0xFFFFFFFF);

	// Defense
	gp[28].set(g_winx/2+200, g_winy/2-65, 0xFFFFFFFF);
	gp[29].set(g_winx/2+200, g_winy/2-35, 0xFFFFFFFF);
	gp[30].set(g_winx/2+100, g_winy/2-35, 0xFFFFFFFF);
	gp[31].set(g_winx/2+100, g_winy/2-65, 0xFFFFFFFF);
	gp[32].set(g_winx/2+200, g_winy/2-65, 0xFFFFFFFF);

	// Culture
	gp[33].set(g_winx/2+200, g_winy/2+30, 0xFFFFFFFF);
	gp[34].set(g_winx/2+200, g_winy/2+60, 0xFFFFFFFF);
	gp[35].set(g_winx/2+100, g_winy/2+60, 0xFFFFFFFF);
	gp[36].set(g_winx/2+100, g_winy/2+30, 0xFFFFFFFF);
	gp[37].set(g_winx/2+200, g_winy/2+30, 0xFFFFFFFF);

	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*38,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&gp_vb,NULL);
	gp_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,gp,sizeof(s_vert2dc)*38);
	gp_vb->Unlock();
}
//---------------------------------------------------------
void Hud::InitAttackPanel(){
	attackpanel = false;
	resultpanel = false;
	fontap.left = (LONG)(g_winx/2-400+150);
	fontap.right = (LONG)(g_winx/2+400+150);
	fontap.top = (LONG)(g_winy/2-250);
	fontap.bottom = (LONG)(g_winy/2+170);
	ap_vb = NULL;
	ap = new s_vert2dc[24];
	ap[0].set(g_winx/2+500+150, g_winy/2-300, 0xFF000000);
	ap[1].set(g_winx/2+500+150, g_winy/2+200, 0xFF000000);
	ap[2].set(g_winx/2-500+150, g_winy/2-300, 0xFF000000);
	ap[3].set(g_winx/2-500+150, g_winy/2+200, 0xFF000000);

	ap[4].set(g_winx/2+500+150, g_winy/2-300, 0xFFFFFFFF);
	ap[5].set(g_winx/2+500+150, g_winy/2+200, 0xFFFFFFFF);
	ap[6].set(g_winx/2-500+150, g_winy/2+200, 0xFFFFFFFF);
	ap[7].set(g_winx/2-500+150, g_winy/2-300, 0xFFFFFFFF);
	ap[8].set(g_winx/2+500+150, g_winy/2-300, 0xFFFFFFFF);

	ap[9].set(g_winx/2+20, g_winy/2+180, 0xFFFFFFFF);
	ap[10].set(g_winx/2+105, g_winy/2+180, 0xFFFFFFFF);
	ap[11].set(g_winx/2+105, g_winy/2+145, 0xFFFFFFFF);
	ap[12].set(g_winx/2+20, g_winy/2+145, 0xFFFFFFFF);
	ap[13].set(g_winx/2+20, g_winy/2+180, 0xFFFFFFFF);

	ap[14].set(g_winx/2+20+173, g_winy/2+180, 0xFFFFFFFF);
	ap[15].set(g_winx/2+105+173, g_winy/2+180, 0xFFFFFFFF);
	ap[16].set(g_winx/2+105+173, g_winy/2+145, 0xFFFFFFFF);
	ap[17].set(g_winx/2+20+173, g_winy/2+145, 0xFFFFFFFF);
	ap[18].set(g_winx/2+20+173, g_winy/2+180, 0xFFFFFFFF);

	ap[19].set(g_winx/2+120, g_winy/2+180, 0xFFFFFFFF);
	ap[20].set(g_winx/2+180, g_winy/2+180, 0xFFFFFFFF);
	ap[21].set(g_winx/2+180, g_winy/2+145, 0xFFFFFFFF);
	ap[22].set(g_winx/2+120, g_winy/2+145, 0xFFFFFFFF);
	ap[23].set(g_winx/2+120, g_winy/2+180, 0xFFFFFFFF);

	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*24,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&ap_vb,NULL);
	ap_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,ap,sizeof(s_vert2dc)*24);
	ap_vb->Unlock();
}
//---------------------------------------------------------
void Hud::InitLeftPanel(){

	D3DXCreateTextureFromFile(g_device, "../art/hud.png", &tex);
	texpts = new s_vert2dt[12];
	texpts[0].set(294,655,0.56f,0);
	texpts[1].set(294,970,0.56f,0.6f);
	texpts[2].set(6,655,0,0);
	texpts[3].set(6,655,0,0);
	texpts[4].set(294,970,0.56f,0.6f);
	texpts[5].set(6,970,0,0.6f);

	texpts[6].set(293,397,1,0.8125f);
	texpts[7].set(293,493,1,0.999f);
	texpts[8].set(7,397,0.4425f,0.8125f);
	texpts[9].set(7,397,0.4425f,0.8125f);
	texpts[10].set(293,493,1,0.999f);
	texpts[11].set(7,493,0.4425f,0.999f);

	g_device->CreateVertexBuffer(sizeof(s_vert2dt)*12,D3DUSAGE_WRITEONLY,VERT2DT,D3DPOOL_DEFAULT,&tex_vb,NULL);
	tex_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,texpts,sizeof(s_vert2dt)*12);
	tex_vb->Unlock();

	fontglob.left = 0;
	fontglob.right = 300;
	fontglob.top = 7;
	fontglob.bottom = 175;
	fontfed.left = 0;
	fontfed.right = 300;
	fontfed.top = 101;
	fontfed.bottom = 400;
	fontclan.left = 0;
	fontclan.right = 300;
	fontclan.top = 220;
	fontclan.bottom = 500;
	fontgroup.left = 0;
	fontgroup.right = 300;
	fontgroup.top = 485;
	fontgroup.bottom = (LONG)(g_winy*0.99f);
	fontsp.left = 300;
	fontsp.right = 495;
	fontsp.top = 590;
	fontsp.bottom = 620;
	fontcaption.left = 310;
	fontcaption.right = (LONG)(g_winx);
	fontcaption.top = 5;
	fontcaption.bottom = (LONG)(g_winy*0.99f);

	subpopup = false;

	sp_vb = NULL;
	sp = new s_vert2dc[9];
	sp[0].set(500, 590, 0xff000000);
	sp[1].set(500, 615, 0xff000000);
	sp[2].set(300, 590, 0xff000000);
	sp[3].set(300, 615, 0xff000000);
	sp[4].set(500, 590, 0xffffffff);
	sp[5].set(500, 615, 0xffffffff);
	sp[6].set(300, 615, 0xffffffff);
	sp[7].set(300, 590, 0xffffffff);
	sp[8].set(500, 590, 0xffffffff);
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*9,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&sp_vb,NULL);
	sp_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,sp,sizeof(s_vert2dc)*9);
	sp_vb->Unlock();

	back_vb = NULL;
	back = new s_vert2dc[4];
	back[0].set(300,0,0xFF000000);
	back[1].set(300,g_winy,0xFF000000);
	back[2].set(0,0,0xFF000000);
	back[3].set(0,g_winy,0xFF000000);
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*4,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&back_vb,NULL);
	back_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,back,sizeof(s_vert2dc)*4);
	back_vb->Unlock();

	lines_vb = NULL;
	lnb = 0;
	lines = new s_vert2dc[300]; /*----!----*/
	// Panel frame
	lines[lnb++].set(300,0,0xFFFFFFFF);
	lines[lnb++].set(300,g_winy,0xFFFFFFFF);

	// Global panel
	lines[lnb++].set(5,15,0xFFFFFFFF);
	lines[lnb++].set(40,15,0xFFFFFFFF);

	lines[lnb++].set(40,5,0xFFFFFFFF);
	lines[lnb++].set(40,25,0xFFFFFFFF);
	lines[lnb++].set(120,15,0xFFFFFFFF);
	lines[lnb++].set(295,15,0xFFFFFFFF);
	lines[lnb++].set(120,5,0xFFFFFFFF);
	lines[lnb++].set(120,25,0xFFFFFFFF);

	lines[lnb++].set(40,5,0xFF00FF00);
	lines[lnb++].set(120,5,0xFFFFBB00);
	lines[lnb++].set(40,25,0xFF00FF00);
	lines[lnb++].set(120,25,0xFFFFBB00);

	lines[lnb++].set(295,15,0xFFFFFFFF);
	lines[lnb++].set(295,100,0xFFFFFFFF);
	lines[lnb++].set(5,15,0xFFFFFFFF);
	lines[lnb++].set(5,100,0xFFFFFFFF);
	lines[lnb++].set(5,100,0xFFFFFFFF);
	lines[lnb++].set(295,100,0xFFFFFFFF);

	// Federation panel
	lines[lnb++].set(5,125,0xFFFFFFFF);
	lines[lnb++].set(40,125,0xFFFFFFFF);

	lines[lnb++].set(40,115,0xFFFFFFFF);
	lines[lnb++].set(40,135,0xFFFFFFFF);
	lines[lnb++].set(160,125,0xFFFFFFFF);
	lines[lnb++].set(295,125,0xFFFFFFFF);
	lines[lnb++].set(160,115,0xFFFFFFFF);
	lines[lnb++].set(160,135,0xFFFFFFFF);

	lines[lnb++].set(40,115,0xFF00FF00);
	lines[lnb++].set(160,115,0xFFFFBB00);
	lines[lnb++].set(40,135,0xFF00FF00);
	lines[lnb++].set(160,135,0xFFFFBB00);

	lines[lnb++].set(295,125,0xFFFFFFFF);
	lines[lnb++].set(295,220,0xFFFFFFFF);
	lines[lnb++].set(5,125,0xFFFFFFFF);
	lines[lnb++].set(5,220,0xFFFFFFFF);
	lines[lnb++].set(5,220,0xFFFFFFFF);
	lines[lnb++].set(295,220,0xFFFFFFFF);

	// Clan panel
	lines[lnb++].set(5,245,0xFFFFFFFF);
	lines[lnb++].set(40,245,0xFFFFFFFF);
	lines[lnb++].set(100,245,0xFFFFFFFF);
	lines[lnb++].set(295,245,0xFFFFFFFF);
	
	lines[lnb++].set(40,235,0xFF00FF00);
	lines[lnb++].set(100,235,0xFFFFBB00);
	lines[lnb++].set(40,255,0xFF00FF00);
	lines[lnb++].set(100,255,0xFFFFBB00);
	
	lines[lnb++].set(40,235,0xFFFFFFFF);
	lines[lnb++].set(40,255,0xFFFFFFFF);
	lines[lnb++].set(100,235,0xFFFFFFFF);
	lines[lnb++].set(100,255,0xFFFFFFFF);

	lines[lnb++].set(5,245,0xFFFFFFFF);
	lines[lnb++].set(5,390,0xFFFFFFFF);
	lines[lnb++].set(295,245,0xFFFFFFFF);
	lines[lnb++].set(295,390,0xFFFFFFFF);

	lines[lnb++].set(5,390,0xFFFFFFFF);
	lines[lnb++].set(295,390,0xFFFFFFFF);

	// Group panel
	lines[lnb++].set(5,510,0xFFFFFFFF);
	lines[lnb++].set(40,510,0xFFFFFFFF);
	lines[lnb++].set(295,510,0xFFFFFFFF);
	lines[lnb++].set(120,510,0xFFFFFFFF);

	lines[lnb++].set(40,520,0xFF00FF00);
	lines[lnb++].set(120,520,0xFFFFBB00);
	lines[lnb++].set(40,500,0xFF00FF00);
	lines[lnb++].set(120,500,0xFFFFBB00);

	lines[lnb++].set(40,500,0xFFFFFFFF);
	lines[lnb++].set(40,520,0xFFFFFFFF);
	lines[lnb++].set(120,520,0xFFFFFFFF);
	lines[lnb++].set(120,500,0xFFFFFFFF);

	lines[lnb++].set(5,510,0xFFFFFFFF);
	lines[lnb++].set(5,g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,510,0xFFFFFFFF);
	lines[lnb++].set(295,g_winy-50,0xFFFFFFFF);

	// subgroup
	lines[lnb++].set(60,575,0xFF5555FF);
	lines[lnb++].set(60,605,0xFF5555FF);
	lines[lnb++].set(240,575,0xFF5555FF);
	lines[lnb++].set(240,605,0xFF5555FF);
	lines[lnb++].set(60,575,0xFF5555FF);
	lines[lnb++].set(240,575,0xFF5555FF);
	lines[lnb++].set(60,605,0xFF5555FF);
	lines[lnb++].set(240,605,0xFF5555FF);

	
	// Uparrow
	lines[lnb++].set(130,645,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(170,645,0xFF5555FF);

	// Downarrow
	lines[lnb++].set(130,g_winy-65,0xFF5555FF);
	lines[lnb++].set(150,g_winy-55,0xFF5555FF);
	lines[lnb++].set(150,g_winy-55,0xFF5555FF);
	lines[lnb++].set(170,g_winy-65,0xFF5555FF);

	// List up
	lines[lnb++].set(5,630,0xFFFFFFFF);
	lines[lnb++].set(295,630,0xFFFFFFFF);
	lines[lnb++].set(5,650,0xFF555555);
	lines[lnb++].set(295,650,0xFF555555);

	// List Down
	lines[lnb++].set(5,g_winy-70,0xFF555555);
	lines[lnb++].set(295,g_winy-70,0xFF555555);
	lines[lnb++].set(5,g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,g_winy-50,0xFFFFFFFF);

	// List plus & minus
	lines[lnb++].set(259,640,0xFF5555FF);
	lines[lnb++].set(271,640,0xFF5555FF);
	lines[lnb++].set(279,640,0xFF5555FF);
	lines[lnb++].set(291,640,0xFF5555FF);
	lines[lnb++].set(285,634,0xFF5555FF);
	lines[lnb++].set(285,646,0xFF5555FF);

	lines[lnb++].set(275,630,0xFFAAAAAA);
	lines[lnb++].set(275,650,0xFFAAAAAA);
	lines[lnb++].set(255,630,0xFFAAAAAA);
	lines[lnb++].set(255,650,0xFFAAAAAA);

	// End Turn
	lines[lnb++].set(170,g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,g_winy-30,0xFF5555FF);
	lines[lnb++].set(170,g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,g_winy-30,0xFF5555FF);


	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*lnb,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&lines_vb,NULL);
	lines_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,lines,sizeof(s_vert2dc)*lnb);
	lines_vb->Unlock();

	listspan = (int)g_winy-720;
	listscale = 5;
	listgap = (float)(listspan/listscale);
	listoffset = 0;
	stam = NULL;
	qstam = NULL;
	snb = 0;
	stam_vb = NULL;
	qsnb = 0;
	qstam_vb = NULL;
}
//---------------------------------------------------------
Hud::~Hud(){
	if (back_vb) back_vb->Release();
	if (lines_vb) lines_vb->Release();
	if (stam_vb) stam_vb->Release();
	if (qstam_vb) qstam_vb->Release();
	if (tex_vb) tex_vb->Release();
	if (tex) tex->Release();
	if (font) font->Release();
	if (ap_vb) ap_vb->Release();
	if (gp_vb) gp_vb->Release();
	delete fedliststr;
	delete nfedliststr;
	delete fedclans;
	delete nfedclans;
	for (int i=g_nbClans-1; i>=0; --i) {
		delete fedliststances[i];
		delete nfedliststances[i];
	}
	delete fedliststances;
	delete nfedliststances;
	delete texpts;
	delete back;
	delete lines;
	if (stam) delete stam;
	if (qstam) delete qstam;
}
//---------------------------------------------------------
void Hud::Click(int x, int y){
	Group* g;
	if (attackpanel && x>300){
		if (y>=g_winy/2+145 && y<=g_winy/2+180){
			// Inside Cancel/Attack buttons area
			if (x>=g_winx/2+20 && x<=g_winx/2+105){
				// Cancel
				attackpanel = false;
				getmouse = false;
				SetCaption("Attack cancelled.");
			} else if (x>=g_winx/2+193 && x<=g_winx/2+278) {
				// Attack
				resultpanel = true;
				double bodycount = floor(apow-dpow);
				if (bodycount < 0) bodycount = 0;
				if (bodycount > enemy->size) bodycount = enemy->size;
				double gc = floor((enemy->clan->gold/enemy->clan->size)*bodycount);
				sprintf(attackrez, "Attack power %.1f VS Defense power %.1f\n\n Enemy units killed: %0.f of %.0f\nGold collected: %0f", apow, dpow, bodycount, enemy->size, gc);
				enemy->clan->gold -= gc;
				g_clan->gold += gc;
				enemy->Kill(bodycount);
			}
		}
	}
	if (resultpanel && y>=g_winy/2+145 && y<=g_winy/2+180 && x>=g_winx/2+120 && x<=g_winx/2+180) {
		// Close button
		resultpanel = false;
		attackpanel = false;
		getmouse = false;
	}
	if (goldoffer) {
		if (y>=g_winy/2+245 && y<=g_winy/2+280 && x>=g_winx/2+120 && x<=g_winx/2+180) {
			// Close button
			goldoffer = false;
			goldpanel = true;
		}
	} else if (offer) {
		if (y>=g_winy/2+245 && y<=g_winy/2+280 && x>=g_winx/2+120 && x<=g_winx/2+180) {
			// Close button
			offer = false;
		} else if (x>=g_winx/2+80 && x<=g_winx/2+200) {
			// Buttons area
			if (y>=g_winy/2-233 && y<=g_winy/2-203) {
				// Gold offer
				offer = false;
				goldoffer = true;
				spstr[0] = '|';
				spstr[1] = 0;
				spnum = 0;
			} else if (y>=g_winy/2-112 && y<=g_winy/2-82){
				// Peace offer
				if (destclan->RecievePeaceOffer(g_clan)){
					SetCaption("Peace offer accepted.");
				} else {
					SetCaption("Peace offer rejected.");
				}
				offer = false;
				goldpanel = true;
			} else if (y>=g_winy/2+15 && y<=g_winy/2+45){
				// Federate
				if (g_clan->FederateVote(destclan, fedrez)){
					if (g_clan->IsNextTo(destclan)){
						if (destclan->RecieveFederationOffer(g_clan)){
							SetCaption("Clan successfully federated.");
							g_clan->AddToFederation(destclan);
							UpdateText();
							GoldPanel();
						} else {
							SetCaption("The clan refused your offer.");
						}
					} else {
						SetCaption("The clan would be accepted, send a group to make the proposal.");
					}
				} else {
					SetCaption(fedrez);
				}
			}
		}
	} else if (goldpanel){
		if (x>=g_winx/2+100 && x<=g_winx/2+200){
			// Clan Attributes buttons
			if (y>=g_winy/2-250 && y<=g_winy/2-220){
				// Stamina
				if (g_clan->gold >= 100){
					g_clan->gold -= 100;
					g_clan->UpgradeStamina();
					UpdateSelect();
					sprintf(clan,CLANFROMAT,g_clan->size, g_clan->nbGroups, g_clan->gold, g_clan->stamina, g_clan->attack, g_clan->defense, g_clan->culture);
					SetCaption("Stamina attribute increased.");
				} else {
					SetCaption("Not enough gold.");
				}
			} else if (y>=g_winy/2-153 && y<=g_winy/2-123) {
				// Attack
				if (g_clan->gold >= 100){
					g_clan->gold -= 100;
					g_clan->attack += 1;
					sprintf(clan,CLANFROMAT,g_clan->size, g_clan->nbGroups, g_clan->gold, g_clan->stamina, g_clan->attack, g_clan->defense, g_clan->culture);
					SetCaption("Attack attribute increased.");
				} else {
					SetCaption("Not enough gold.");
				}
			} else if (y>=g_winy/2-65 && y<=g_winy/2-35) {
				// Defense
				if (g_clan->gold >= 100){
					g_clan->gold -= 100;
					g_clan->defense += 1;
					sprintf(clan,CLANFROMAT,g_clan->size, g_clan->nbGroups, g_clan->gold, g_clan->stamina, g_clan->attack, g_clan->defense, g_clan->culture);
					SetCaption("Defense attribute increased.");
				} else {
					SetCaption("Not enough gold.");
				}
			} else if (y>=g_winy/2+30 && y<=g_winy/2+60) {
				// Culture
				if (g_clan->gold >= 100){
					g_clan->gold -= 100;
					g_clan->culture += 1;
					sprintf(clan,CLANFROMAT,g_clan->size, g_clan->nbGroups, g_clan->gold, g_clan->stamina, g_clan->attack, g_clan->defense, g_clan->culture);
					SetCaption("Culture attribute increased.");
				} else {
					SetCaption("Not enough gold.");
				}
			}
		} else if (x>=fontfgp.left && x<=fontfgp.right) {
			int k = (y+fedlistscale/2-fontfgp.top)/fedlistscale;
			if (k>=0 && k<fedlistlen){
				destclan = fedclans[k];
				Offer();
			}
		} else if (x>=fontnfgp.left && x<=fontnfgp.right) {
			int k = (y+nfedlistscale/2-fontnfgp.top)/nfedlistscale;
			if (k>=0 && k<nfedlistlen){
				destclan = nfedclans[k];
				Offer();
			}
		}
		if (y>=g_winy/2+245 && y<=g_winy/2+280 && x>=g_winx/2+120 && x<=g_winx/2+180) {
			// Close button
			goldpanel = false;
			getmouse = false;
		}
	}
	if (x<300 && y >= 397 && y <= 493){
		if (x<104){
			// Dig
			if (g_nbSelected==1){
				g = g_selected[0];
				if (g_map->gold[g->x*g_side+g->z]){
					g->mining = !g->mining;
					SetCaption(g->mining ? "Now mining." : "Stopped mining.");
				} else {
					SetCaption("No gold here, can't mine.");
				}
			} else {
				SetCaption(g_nbSelected ? "Too many groups selected." : "No group selected.");
			}
		} else if (x>196) {
			// Attack
			if (goldpanel) {
				goldpanel = false;
				getmouse = false;
			}
			if (g_nbSelected){
				g_game.attacking = !g_game.attacking;
				if (attackpanel){
					attackpanel = false;
					SetCaption("Attack cancelled.");
				}
				SetCaption(g_game.attacking ? "Select group to attack." : "Attack cancelled.");
			} else {
				if (g_game.attacking){
					g_game.attacking = false;
					SetCaption("Attack cancelled.");
				} else {
					SetCaption("No group selected.");
				}
			}
		} else {
			// Gold
			if (attackpanel) {
				attackpanel = false;
				SetCaption("Attack cancelled.");
			} else if (resultpanel) {
				resultpanel = false;
				attackpanel = false;
			}
			goldpanel = ! goldpanel;
			if (goldpanel){
				GoldPanel();
			}
		}
	} else if (y >= 560){
		if (y<=590 && x>=60 && x<=240 && g_nbSelected==1){
			SetCaption("Enter subgroup size");
			subpopup = !subpopup;
			spstr[0] = '|';
			spstr[1] = 0;
			spnum = 0;
		} else if (y>=g_winy-30 && y<=g_winy-8 && x>=170 && x<=275){
			g_game.Turn();
		} else if (y>=630 && y<=650){
			if (x>255){
				if (x < 275){
					listscale -= listscale>1 ? 1 : 0;
					listgap = (float)(listspan/listscale);
				} else {
					listscale += listscale<20 ? 1 : 0;
					listgap = (float)(listspan/listscale);
				}
			} else {
				listoffset -= listoffset>0 ? 1 : 0;
			}
			UpdateSelect();
		} else if (y>=g_winy-70 && y<=g_winy-50){
			listoffset += listoffset==g_nbSelected ? 0 : 1;
			UpdateSelect();
		}
	}
}
//---------------------------------------------------------
void Hud::Offer(){
	offer = true;
	sprintf(offertitle, "Making an offer to %s", destclan->name);
}
//---------------------------------------------------------
void Hud::GoldPanel(){
	getmouse = true;
	int a=0;
	int b=0;
	for (int i=0; i<g_clan->id; ++i){
		if (g_clans[i]->fed == g_clan->fed){
			sprintf(nfedliststances[a], "%3.2f%%", g_stances[g_clans[i]->id][g_clan->id]);
			fedclans[a] = g_clans[i];
			fedliststr[a++] = g_clans[i]->name;

		} else {
			sprintf(fedliststances[b], "%3.2f%%", g_stances[g_clans[i]->id][g_clan->id]);
			nfedclans[b] = g_clans[i];
			nfedliststr[b++] = g_clans[i]->name;
		}
	}
	for (int i=g_clan->id+1; i<g_nbClans; ++i){
		if (g_clans[i]->fed == g_clan->fed){
			sprintf(fedliststances[a], "%3.2f%%", g_stances[g_clans[i]->id][g_clan->id]);
			fedclans[a] = g_clans[i];
			fedliststr[a++] = g_clans[i]->name;

		} else {
			sprintf(nfedliststances[b], "%3.2f%%", g_stances[g_clans[i]->id][g_clan->id]);
			nfedclans[b] = g_clans[i];
			nfedliststr[b++] = g_clans[i]->name;
		}
	}
	fedlistlen = a;
	nfedlistlen = b;
	fedlistscale = 580/(fedlistlen+1);
	nfedlistscale = 580/(nfedlistlen+1);
}
//---------------------------------------------------------
void Hud::AttackPanel(int x, int z){
	attackpanel = true;
	getmouse = true;
	Group* g = NULL;
	enemy = g_board[x*g_side+z];
	dpow = enemy->size * enemy->clan->defense;
	sprintf(attacktitle, "Attacking %s group of %.0f units", enemy->clan->name, enemy->size);
	apow = 0;
	for (int i=g_nbSelected-1; i>=0; --i){
		g = g_selected[i];
		apow += g->size;
		sprintf(attackstr[i], "%sGroup %d: %.0f units", apstrs[i] , i+1, g->size);
	}
	sprintf(attackrez, "%s\n\n\n        Total attack power: %.0f x %.0f x %.1f = %.1f", apstrs[7], apow, g->clan->attack, attackbonus[g_nbSelected], apow * attackbonus[g_nbSelected] * g->clan->attack);
	apow *= attackbonus[g_nbSelected] * g->clan->attack;
}
//---------------------------------------------------------
void Hud::SetCaption(char* str){
	captionstr = str;
	caption = true;
}
//---------------------------------------------------------
void Hud::CaptionOff(){
	caption = false;
}
//---------------------------------------------------------
void Hud::Update(float time, float dt){
	if (time - last_time > 1.0f) {
		sprintf(fps,FPSFROMAT,frame_counter);
		frame_counter = 0;
		last_time = time;
	}
}
//---------------------------------------------------------
void Hud::UpdateSelect(){
	sprintf(group,GROUPFROMAT,g_nbSelected, g_selectedpop);

	if (stam) delete stam;
	if (qstam) delete qstam;
	snb = 0;
	qsnb = 0;
	stam = new s_vert2dc[listscale*8];
	qstam = new s_vert2dc[listscale*12];
	if (listoffset+listscale > g_nbSelected) listoffset = g_nbSelected-listscale;
	if (g_nbSelected <= listscale) listoffset = 0;
	float ilg, rr, rl;
	for (double i=listoffset; i<listscale+listoffset; ++i){
		if (i+listoffset>=g_nbSelected) break;
		ilg = (float)((i-listoffset)*listgap);
		stam[snb++].set(10,655+ilg,0xFFAAAAAA);
		stam[snb++].set(290,655+ilg,0xFFAAAAAA);
		stam[snb++].set(10,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(290,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(10,655+ilg,0xFFAAAAAA);
		stam[snb++].set(10,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(290,655+ilg,0xFFAAAAAA);
		stam[snb++].set(290,645+ilg+listgap,0xFFAAAAAA);


		Group* g = g_selected[(int)(i)];
		if (g->stamina > 0){
			rr = 280.0f*(float)((g->clan->stamina-g->stamina)/g->clan->stamina);
			qstam[qsnb++].set(290-rr,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFF0080FF);
			qstam[qsnb++].set(10,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(10,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFF0080FF);
			qstam[qsnb++].set(10,645+ilg+listgap,0xFF0080FF);
			if (g->planed+g_game.nextcost[(int)(i)]){
				rl = (280.0f-rr)*(float)((g->stamina-(g->planed+g_game.nextcost[(int)(i)]))/g->stamina);
				qstam[qsnb++].set(290-rr,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFFFF2000);
				qstam[qsnb++].set(10+rl,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(10+rl,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFFFF2000);
				qstam[qsnb++].set(10+rl,645+ilg+listgap,0xFFFF2000);
			}
		}
	}

	if (stam_vb) stam_vb->Release();
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*(snb+2),D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&stam_vb,NULL);
	stam_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,stam,sizeof(s_vert2dc)*snb);
	stam_vb->Unlock();	
	if (qstam_vb) qstam_vb->Release();
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*(qsnb+2),D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&qstam_vb,NULL);
	qstam_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,qstam,sizeof(s_vert2dc)*qsnb);
	qstam_vb->Unlock();	
}
//---------------------------------------------------------
void Hud::UpdateText(){
	Clan& c = *g_clan;
	sprintf(glob,GLOBFROMAT,g_nbClans, g_pop, g_nbFeds);
	sprintf(fed,FEDFROMAT, c.fed->nb, c.fed->pop, g_pop ? ((c.fed->pop*100)/g_pop) : 0);
	sprintf(clan,CLANFROMAT,c.size, c.nbGroups, c.gold, c.stamina, c.attack, c.defense, c.culture);
	sprintf(turn,TURNFROMAT,g_turn);
}
//---------------------------------------------------------
void Hud::PopupEnter(){
	if (spnum){
		if (goldoffer){
			goldoffer = false;
			offer = false;
			if (destclan->RecieveGoldOffering(spnum, g_clan)){
				g_clan->gold -= spnum;
				SetCaption("Offer accepted");
			} else {
				SetCaption("Offer refused");
			}
		} else {
			g_selected[0]->Subgroup(spnum);
			g_selectedpop = spnum;
		}
		UpdateText();
		UpdateSelect();
	}
	subpopup = false;
}
//---------------------------------------------------------
void Hud::PopupDelete(){
	int i;
	for (i=0; spstr[i]; ++i);
	if (i>1){
		spstr[i-2] = '|';
		spstr[i-1] = 0;
		//spnum -= spnum % 10;
		spnum /= 10;
	}
}
//---------------------------------------------------------
void Hud::PopupText(int key){
	unsigned int nextnum = spnum*10 + key;
	if ((goldoffer && nextnum<=g_clan->gold) || nextnum<g_selectedpop){
		int i;
		for (i=0; spstr[i]; ++i);
		spstr[i-1] = '0'+key;
		spstr[i] = '|';
		spstr[i+1] = 0;
		spnum = nextnum;
	}
}
//---------------------------------------------------------
void Hud::RenderTextures(){
	g_device->SetTexture(0,tex);
	g_device->SetStreamSource(0,tex_vb,0,sizeof(s_vert2dt));
	g_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,4);
	g_device->SetTexture(0,NULL);
}
//---------------------------------------------------------
void Hud::RenderText(){
	font->DrawText(NULL,fps,-1,&fontcaption, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"     Global",-1,&fontglob, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,glob,-1,&fontglob, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Federation",-1,&fontfed, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,fed,-1,&fontfed, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Clan",-1,&fontclan, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,clan,-1,&fontclan, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Groups",-1,&fontgroup, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,group,-1,&fontgroup, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,turn,-1,&fontgroup, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
	if (caption) font->DrawText(NULL,captionstr,-1,&fontcaption, DT_BOTTOM | DT_LEFT | DT_NOCLIP,0xffffffff);
}
//---------------------------------------------------------
void Hud::Render(){
	if (show) {
		g_device->SetFVF(VERT2DC);
		g_device->SetStreamSource(0,back_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		g_device->SetFVF(VERT2DT);
		RenderTextures();
		g_device->SetFVF(VERT2DC);

		g_device->SetStreamSource(0,qstam_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,qsnb/3);

		g_device->SetStreamSource(0,stam_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_LINELIST,0,snb/2);

		g_device->SetStreamSource(0,lines_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_LINELIST,0,lnb/2);
		
		if (subpopup){
			g_device->SetStreamSource(0,sp_vb,0,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			g_device->SetStreamSource(0,sp_vb,sizeof(s_vert2dc)*4,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_LINESTRIP,0,4);
			font->DrawText(NULL,spstr,-1,&fontsp, DT_TOP | DT_RIGHT | DT_NOCLIP,0xffffffff);
		}

		if (attackpanel){
			g_device->SetStreamSource(0,ap_vb,0,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			g_device->DrawPrimitive(D3DPT_LINESTRIP,4,4);
			font->DrawText(NULL,attacktitle,-1,&fontap, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
			if (resultpanel){
				g_device->DrawPrimitive(D3DPT_LINESTRIP,19,4);
				font->DrawText(NULL,attackrez,-1,&fontap, DT_VCENTER | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"Close",-1,&fontap, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
			} else {
				g_device->DrawPrimitive(D3DPT_LINESTRIP,9,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,14,4);
				for (int i=g_nbSelected-1; i>=0; --i){
					font->DrawText(NULL,attackstr[i],-1,&fontap, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
				}
				font->DrawText(NULL,attackrez,-1,&fontap, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"Cancel           Attack",-1,&fontap, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
			}
		} else if (goldpanel) {
			g_device->SetStreamSource(0,gp_vb,0,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			g_device->DrawPrimitive(D3DPT_LINESTRIP,4,4);
			g_device->DrawPrimitive(D3DPT_LINESTRIP,9,4);
			font->DrawText(NULL,"Close",-1,&fontgp, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);

			if (offer) {
				font->DrawText(NULL,offertitle,-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"\n\n\n\n\n\n\n\n\nGold Offer",-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"\n\nPeace Offer\n\n\n\n\n\n\n\nFederate",-1,&fontgp, DT_VCENTER | DT_CENTER | DT_NOCLIP,0xffffffff);
				g_device->SetStreamSource(0,of_vb,0,sizeof(s_vert2dc));
				g_device->DrawPrimitive(D3DPT_LINESTRIP,0,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,5,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,10,4);
			} else if (goldoffer){
				font->DrawText(NULL,offertitle,-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"\n\n\n\n\n\n\n\n\nGold Offer",-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,spstr,-1,&fontgp, DT_VCENTER | DT_CENTER | DT_NOCLIP,0xffffffff);
			} else {
				g_device->DrawPrimitive(D3DPT_LINELIST,14,2);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,18,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,23,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,28,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,33,4);
				font->DrawText(NULL,"Gold & Diplomacy\n\n\n\nCost: 100 Gold\n\n\n\nStamina\n\n\n\n\n\nAttack",-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"\n\n\nIntra-federation                Clan attributes                Extra-federation",-1,&fontgp, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"\n\n\n\n\n\nDefense\n\n\n\n\n\nCulture",-1,&fontgp, DT_VCENTER | DT_CENTER | DT_NOCLIP,0xffffffff);

				vfontgp.top = fontfgp.top;
				vfontgp.bottom = fontfgp.bottom;
				vfontgp.left = fontfgp.left;
				vfontgp.right = fontfgp.right;
				for (int i=0; i<fedlistlen; ++i){
					font->DrawText(NULL,fedliststr[i],-1,&vfontgp, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
					font->DrawText(NULL,fedliststances[i],-1,&vfontgp, DT_TOP | DT_RIGHT | DT_NOCLIP,0xffffffff);
					vfontgp.top += fedlistscale;
				}

				vfontgp.top = fontnfgp.top;
				vfontgp.bottom = fontnfgp.bottom;
				vfontgp.left = fontnfgp.left;
				vfontgp.right = fontnfgp.right;
				for (int i=0; i<nfedlistlen; ++i){
					font->DrawText(NULL,nfedliststr[i],-1,&vfontgp, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
					font->DrawText(NULL,nfedliststances[i],-1,&vfontgp, DT_TOP | DT_RIGHT | DT_NOCLIP,0xffffffff);
					vfontgp.top += nfedlistscale;
				}
			}
		}

		RenderText();
	}
	frame_counter++;
}
//---------------------------------------------------------
void Hud::Toggle(){
	show = !show;
}

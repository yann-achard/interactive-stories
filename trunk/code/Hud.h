#ifndef __HUD_H__
#define __HUD_H__
//---------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include "d3ddefs.h"
//---------------------------------------------------------
class Group;
class Clan;
//---------------------------------------------------------
#define FPSFROMAT "%3d fps"
#define TURNFROMAT "Turn%9d  <End Turn>"
#define GLOBFROMAT "\n\nClans           %10d\nPopulation %15d"
#define FEDFROMAT "\n\n\nAllies          %10d\nPopulation %15d\nPop/GlobPop%14.3f%%"
#define CLANFROMAT "\n\n\nPopulation %15d\nGroups     %15d\nGold       %15d\nStamina    %15g\nAttack     %15d\nDefense    %15d\nCulture    %15d"
#define GROUPFROMAT "\n\n\nSelected  %15d\nPopulation%15d\n\n<Select Subgroup>"
//---------------------------------------------------------
class Hud {
public:

	bool								show;
	bool								getmouse;
	s_vert2dc*					back;
	VBUF								back_vb;
	s_vert2dc*					lines;
	VBUF								lines_vb;
	int									lnb;
	s_vert2dc*					stam;
	VBUF								stam_vb;
	int									snb;
	s_vert2dc*					qstam;
	VBUF								qstam_vb;
	int									qsnb;
	s_vert2dt*					texpts;
	VBUF								tex_vb;
	LPDIRECT3DTEXTURE9	tex;
	int									listspan;
	float								listgap;
	int									listscale;
	double							listoffset;
	RECT								portion;
	LPD3DXFONT					font;
	float								last_time;
	int									frame_counter;
	RECT								fontglob;
	RECT								fontfed;
	RECT								fontclan;
	RECT								fontgroup;
	bool								caption;
	char								captionstr[128];
	RECT								fontcaption;
	char								fps[16];
	char								turn[64];
	char								glob[256];
	char								fed[256];
	char								clan[512];
	char								group[512];
	bool								subpopup;
	VBUF								sp_vb;
	s_vert2dc*					sp;
	RECT								fontsp;
	char								spstr[64];
	int									spnum;
	bool								attackpanel;
	bool								goldpanel;
	bool								resultpanel;
	char								attacktitle[128];
	char								attackstr[8][128];
	int									apow;
	int									dpow;
	Group*							enemy;
	char								attackrez[128];		
	VBUF								ap_vb;
	s_vert2dc*					ap;
	RECT								fontap;
	VBUF								gp_vb;
	s_vert2dc*					gp;
	RECT								fontgp;
	RECT								vfontgp;
	RECT								fontfgp;
	RECT								fontnfgp;
	int									fedlistlen;
	int									nfedlistlen;
	char**							fedliststr;
	char**							nfedliststr;
	char**							fedliststances;
	char**							nfedliststances;
	int									fedlistscale;
	int									nfedlistscale;
	Clan**							fedclans;
	Clan**							nfedclans;
	Clan*								destclan;
	bool								offer;
	char								offertitle[128];
	VBUF								of_vb;
	s_vert2dc*					of;
	bool								goldoffer;
	char								fedrez[128];


	Hud();
	~Hud();

	void InitLeftPanel();
	void InitAttackPanel();
	void InitGoldPanel();
	void InitOffer();
	void Update(float time, float dt);
	void AttackPanel(int x, int z);
	void GoldPanel();
	void Offer();
	void SetCaption(char* str);
	void CaptionOff();
	void Click(int x, int y);
	void UpdateText();
	void PopupText(int key);
	void PopupEnter();
	void PopupDelete();
	void UpdateSelect();
	void Render();
	void RenderText();
	void RenderTextures();
	void Toggle();
};
//---------------------------------------------------------
#endif//__HUD_H__
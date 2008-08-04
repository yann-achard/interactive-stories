#ifndef __HUD_H__
#define __HUD_H__
//---------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include "d3ddefs.h"
//---------------------------------------------------------
#define FPSFROMAT "%3d fps"
#define TURNFROMAT "Turn%9d  <End Turn>"
#define GLOBFROMAT "\n\nClans           %10d\nPopulation %15.0f\nFederations%15d"
#define FEDFROMAT "\n\n\nClans           %10d\nPopulation %15.0f\nPop/GlobPop%14.3f%%"
#define CLANFROMAT "\n\n\nPopulation %15.0f\nGroups     %15d\nGold       %15g\nStamina    %15g\nAttack     %15g\nDefense    %15g\nCulture    %15g"
#define GROUPFROMAT "\n\n\nSelected  %15d\nPopulation%15.0f\n\n<Select Subgroup>"
//---------------------------------------------------------
class Hud {
public:

	bool								show;
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
	RECT								fontcaption;
	char*								captionstr;
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
	double							spnum;
	bool								attackpanel;
	bool								resultpanel;
	char								attacktitle[256];
	char								attackstr[4][2][128];
	double							attackstam[4];
	double							apow;
	char								attackrez[256];		
	VBUF								ap_vb;
	s_vert2dc*					ap;
	RECT								fontap;


	Hud(LPDIRECT3DDEVICE9& device);
	~Hud();

	void Update(float time, float dt);
	void AttackPanel(int x, int z);
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
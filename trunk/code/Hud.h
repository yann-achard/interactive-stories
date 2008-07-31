#ifndef __HUD_H__
#define __HUD_H__
//---------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include "d3ddefs.h"
//---------------------------------------------------------
#define FPSFROMAT "%3d fps"
#define TURNFROMAT "Turn%9d  <End Turn>"
#define GLOBFROMAT "\n\n\n\nClans           %10d\nPopulation %15.0f\n\n"
#define CLANFROMAT "\n\n\nPopulation %15.0f\nGroups     %15d\nGold       %15g\nStamina    %15.1f\nAttack     %15g\nDefense    %15g\nCulture    %15g\n"
#define GROUPFROMAT "\n\n\nSelected  %15d\nPopulation%15.0f\n\n<Select Subgroup>\n"
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
	int									listspan;
	float								listgap;
	int									listscale;
	double							listoffset;
	RECT								portion;
	LPD3DXFONT					font;
	LPDIRECT3DTEXTURE9	tex;
	float								last_time;
	int									frame_counter;
	RECT								fontglob;
	RECT								fontclan;
	RECT								fontgroup;
	char								fps[16];
	char								turn[64];
	char								glob[256];
	char								clan[512];
	char								group[512];
	bool								subpopup;
	VBUF								sp_vb;
	s_vert2dc*					sp;
	RECT								fontsp;
	char								spstr[64];
	double								spnum;


	Hud(LPDIRECT3DDEVICE9& device);
	~Hud();

	void Update(float time, float dt);
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
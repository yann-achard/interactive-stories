#ifndef __MAIN_H__
#define __MAIN_H__
//---------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>
//---------------------------------------------------------
using namespace std;
//---------------------------------------------------------
#include "Game.h"
#include "d3ddefs.h"
#include "Vec3Df.h"
#include "Group.h"
#include "Clan.h"
#include "Hud.h"
#include "Map.h"
//---------------------------------------------------------
#define TITLE				"Clan Wars"
#define WINDOW_X		1680
#define WINDOW_Y		1050
//---------------------------------------------------------
#define SELECT_MAX	4096
#define MAX_CLANS		1024
#define MAX_ENEMIES	1024
#define MAX_ALLIES	1024
#define MAX_NEUTRAL	1024
//---------------------------------------------------------
LRESULT CALLBACK	WindowProcedure(HWND,UINT,WPARAM,LPARAM);
//---------------------------------------------------------
extern Game	g_game;
// DISPLAY //
extern float				g_winx;
extern float				g_winy;
extern bool					g_wireframe;
extern bool					g_grid;
extern Vec3Df				g_rot;
extern Vec3Df				g_pos;
extern Hud*					g_hud;
extern LPDIRECT3DDEVICE9	g_device;
extern VOID*				pData;
// INPUT //
extern bool					g_mousebut[3];
extern int					g_mousex;
extern int					g_mousey;
extern bool					g_dragging;
extern int					g_dragged;
extern bool					g_ctrl;
extern bool					g_up;
extern bool					g_down;
extern bool					g_left;
extern bool					g_right;
// GAME //
extern int					g_side;
extern int					g_rez;
extern float				g_size;
extern float				g_hgap;
extern Map*					g_map;
extern int					g_nbClans;
extern int					g_nbFeds;
extern Clan*				g_clan;
extern Clan**				g_clans;
extern int					g_nbEnemy;
extern Clan**				g_enemy;
extern int					g_nbAlly;
extern Clan**				g_ally;
extern int					g_nbNeutral;
extern Clan**				g_neutral;
extern int					g_nbSelected;
extern Group**			g_selected;
extern Group**			g_board;
extern double				g_pop;
extern double				g_selectedpop;
extern int					g_turn;
extern char*				g_viz;
//------------------------------------------------
#endif//__MAIN_H__
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
class Game;
class Relations;
class Vec3Df;
class Clan;
class Hud;
class Group;
class Map;
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
extern int					g_nbMines;
extern int					g_nbFreeMines;
extern int					(*g_mines)[2];
extern float				g_hgap;
extern Map*					g_map;
extern int					g_nbClans;
extern int					g_nbAliveClans;
extern Clan*				g_clan;
extern Clan**				g_clans;
extern int					g_nbSelected;
extern Group**			g_selected;
extern Group**			g_board;
extern int					g_pop;
extern int					g_selectedpop;
extern int					g_turn;
extern int					g_maxallies;
extern float				g_miner;
extern char*				g_viz;
extern Relations*		g_rel;
 // STATS //
extern float				g_cpppt;
//------------------------------------------------
#endif//__MAIN_H__
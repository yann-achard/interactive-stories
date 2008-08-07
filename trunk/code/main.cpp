#include "main.h"
//---------------------------------------------------------
// DISPLAY //
 float				g_winx;
 float				g_winy;
 float				g_screnratio;
 bool					g_wireframe;
 bool					g_grid;
 bool					g_normals;
 int					g_altitude;
 float				g_origin;
 Vec3Df				g_rot;
 Vec3Df				g_pos;
 LPDIRECT3DDEVICE9	g_device;
 Hud*					g_hud;
 VOID*				pData;
// INPUT //
 bool					g_mousebut[3];
 int					g_mousex;
 int					g_mousey;
 bool					g_dragging;
 int					g_dragged;
 bool					g_up;
 bool					g_ctrl;
 bool					g_down;
 bool					g_left;
 bool					g_right;
// GAME //
 Game					g_game;
 int					g_side;
 int					g_rez;
 float				g_size;
 float				g_hgap;
 Map*					g_map;
 int					g_nbClans;
 int					g_nbFeds;
 Clan*				g_clan;
 Clan**				g_clans;
 int					g_nbEnemy;
 Clan**				g_enemy;
 int					g_nbAlly;
 Clan**				g_ally;
 int					g_nbNeutral;
 Clan**				g_neutral;
 int					g_nbSelected;
 Group**			g_selected;
 Group**			g_board;
 int					g_turn;
 double				g_selectedpop;
 double				g_pop;
 char*				g_viz;
 float**			g_stances;
//------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPreviousInstance,LPSTR lpcmdline,int nCmdShow){
	MSG Message;

	if (!g_game.run)	return 0;

	while(g_game.run){
		if (PeekMessage(&Message,NULL,0,0,PM_REMOVE)){
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		} else {
			g_game.Update();
			g_game.Render();
		}
	}
	return 0;
}
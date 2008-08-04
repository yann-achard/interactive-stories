#ifndef __GAME_H__
#define __GAME_H__
//---------------------------------------------------------
#include "main.h"
#include "d3ddefs.h"
#include "Vec3Df.h"
//---------------------------------------------------------
#define NB_MAT	10
enum e_matcol {
	MC_BLACK,
	MC_EBLACK,
	MC_EGOLD,
	MC_EGOLDBLUE,
	MC_EORANGE,
	MC_ERED,
	MC_EBLUE,
	MC_ELIGHTBLUE,
	MC_EGREEN,
	MC_EGRAY
};
//---------------------------------------------------------
class Group;
//---------------------------------------------------------
class Game {
public:
								Game(void);
								~Game(void);

	void						InitScene(void);
	void						InitGame(void);
	void						CheckDeviceCaps(void);
	void						SaveSettings(void);
	void						LoadSettings(void);

	void						CheckDevice(void);

	void						KeyDown(int key);
	void						KeyUp(int key);

	void						LeftButtonDown(int w, int l);
	void						LeftButtonUp(int w, int l);
	void						RightButtonDown(int w, int l);
	void						RightButtonUp(int w, int l);
	void						MidButtonDown(int key);
	void						MidButtonUp(int key);
	void						MouseWheel(int key);
	void						MouseMove(int mod, int x, int y);

	void						UpdateWay();
	void						UpdateActions();
	void						ExecuteActions();
	void						ActionsExecuted();
	void						Attack();

	void						Hover(int x, int z);

	void						Turn(void);
	void						Update(void);
	void						Render(void);

	void						UpdateRay(void);
	void						SetTempoTranslate(float x, float y, float z);
	void						SetEColor(e_matcol col);
	void						SetTempoEColor(e_matcol col);
	void						PopTempoEColor();
	void						Timeu();

	//---------------------------------------------------------
	bool										run;
	bool										windowed;

	HWND										hWindow;

	LPDIRECT3D9							object;
	D3DPRESENT_PARAMETERS		params;
	float										aspRatio,
													fov,
													nPlane,
													fPlane;
	DWORD										aniso;

	D3DXMATRIX							matProj,
													matView,
													matRotX,
													matRotY,
													matTrans,
													matTemp;

	
	D3DLIGHT9								light;
	D3DLIGHT9								foglight;
	float										time;
	Vec3Df									ray_ori;
	Vec3Df									ray_dir;
	s_tvert*								quad;
	VBUF										quad_vb;
	LPDIRECT3DTEXTURE9			quad_tex;
	LPDIRECT3DTEXTURE9			mine_tex;
	LPDIRECT3DTEXTURE9			attack_tex;
	Vec3Df*									square;
	VBUF										square_vb;
	Vec3Df*									path;
	int*										xpathindex;
	int*										zpathindex;
	float*									stampathindex;
	Group**									pathindexowner;
	int											nbpath;
	int											maxpath;
	VBUF										path_vb;
	Vec3Df*									way;
	VBUF										way_vb;
	int											nbway;
	int*										nextx;
	int*										nextz;
	float*									nextcost;
	int											maxnext;
	int											lastway;
	bool										actionplan;
	bool										showway;
	bool										attacking;
	char										cursorpos[128];

	D3DMATERIAL9*						mats;
	e_matcol								curcol;

};
//---------------------------------------------------------
#endif//__GAME_H__
#include "main.h"
#include "random.h"
#include "sidefunctions.h"
#include "Group.h"
#include "Hud.h"
#include "Game.h"
#include "Clan.h"
#include "Map.h"
//---------------------------------------------------------
Game::Game(void){
	run = true;
	g_device = NULL;
	object = NULL;
#ifdef DEBUG
	windowed = true;
#else
	windowed = false;
#endif
	RECT DesktopSize;
	GetClientRect(GetDesktopWindow(),&DesktopSize);
	g_winx = (float)DesktopSize.right;
	g_winy = (float)DesktopSize.bottom;
	aniso = 0;
	fov = 0.9f;
	nPlane = 500.0f;
	fPlane = 50000.0f;
	aspRatio = g_winx-300 / g_winy;

	// Init Window
	WNDCLASSEX WindowClass;
	//InitCommonControls();
	//fill window class and register class
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = GetModuleHandle(NULL);
	WindowClass.hIcon = NULL;
	WindowClass.hCursor = NULL;
	WindowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = "ClassName";
	WindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	RegisterClassEx(&WindowClass);
	//create window
	hWindow = CreateWindowEx(WS_EX_CONTROLPARENT,"ClassName",	TITLE,
		WS_OVERLAPPED | WS_CAPTION | WS_VISIBLE,
		0, 0,	(int)g_winx, (int)g_winy,
		NULL,NULL,GetModuleHandle(NULL),NULL);
	
	quad_vb = NULL;
	square_vb = NULL;
	quad_tex = NULL;
	mine_tex = NULL;
	attack_tex = NULL;

	if ((object = Direct3DCreate9(D3D_SDK_VERSION)) == NULL){
		MessageBox(hWindow,"Direct3DCreate9() failed!","InitD3D()",MB_OK);
		run = false;
	}
	ZeroMemory(&params,sizeof(params));
	params.Windowed = windowed;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.EnableAutoDepthStencil = true;
	params.AutoDepthStencilFormat = D3DFMT_D24X8;
	params.hDeviceWindow = hWindow;
	params.BackBufferCount = 1;
	params.BackBufferWidth = (UINT)g_winx;
	params.BackBufferHeight = (UINT)g_winy;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(object->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWindow,D3DCREATE_HARDWARE_VERTEXPROCESSING,&params,&g_device))){
		MessageBox(hWindow,"CreateDevice() failed!","InitD3D()",MB_OK);
		run = false;
	}
	InitGame();
	InitScene();
	CheckDeviceCaps();

	D3DXMatrixRotationX(&matRotX, g_rot.x);
	D3DXMatrixTranslation(&matTrans,g_pos.x,g_pos.y,g_pos.z);
	matView = g_map->trans * matTrans * matRotX;

	mats = new D3DMATERIAL9[NB_MAT];
	for (int i=NB_MAT-1; i>=0; --i){
		ZeroMemory(mats+i,sizeof(mats[i]));
		mats[i].Diffuse.r = 1;
		mats[i].Diffuse.g = 1;
		mats[i].Diffuse.b = 1;
	}

	mats[MC_BLACK].Diffuse.r = 0;
	mats[MC_BLACK].Diffuse.g = 0;
	mats[MC_BLACK].Diffuse.b = 0;

	mats[MC_EBLUE].Emissive.b = 0.3f;
	mats[MC_ELIGHTBLUE].Emissive.b = 1.0f;
	mats[MC_ERED].Emissive.r = 1;
	mats[MC_EGREEN].Emissive.g = 1.0f;

	mats[MC_EGRAY].Emissive.r = 0.2f;
	mats[MC_EGRAY].Emissive.g = 0.2f;
	mats[MC_EGRAY].Emissive.b = 0.2f;

	mats[MC_EORANGE].Emissive.r = 1;
	mats[MC_EORANGE].Emissive.g = 0.3f;

	mats[MC_EGOLD].Emissive.r = 0.3f;
	mats[MC_EGOLD].Emissive.g = 0.2f;
	mats[MC_EGOLD].Emissive.b = 0.01f;

	mats[MC_EGOLDBLUE].Emissive.r = 0.3f;
	mats[MC_EGOLDBLUE].Emissive.g = 0.2f;
	mats[MC_EGOLDBLUE].Emissive.b = 0.3f;


	float cell = g_size/(float)(g_side);
	quad = new s_tvert[4];
	quad[0].pos.set(0,0,0);
	quad[0].u = 0;
	quad[0].v = 1;
	quad[1].pos.set(0,0,cell);
	quad[1].u = 0;
	quad[1].v = 0;
	quad[2].pos.set(cell,0,0);
	quad[2].u = 1;
	quad[2].v = 1;
	quad[3].pos.set(cell,0,cell);
	quad[3].u = 1;
	quad[3].v = 0;

	square = new Vec3Df[5];
	square[0].set(cell*0.1f,0,cell*0.1f);
	square[1].set(cell*0.1f,0,cell*0.9f);
	square[2].set(cell*0.9f,0,cell*0.9f);
	square[3].set(cell*0.9f,0,cell*0.1f);
	square[4].set(cell*0.1f,0,cell*0.1f);


	path = NULL;
	way = NULL;
	way_vb = NULL;
	lastway = -1;
	nbway = 0;
	path_vb = NULL;
	maxpath = g_side;
	maxnext = 32;
	nextx = new int[maxnext];
	nextz = new int[maxnext];
	nextcost = new float[maxnext];
	xpathindex = new int[maxpath];
	zpathindex = new int[maxpath];
	stampathindex = new float[maxpath];
	pathindexowner = new Group*[maxpath];
	nbpath = 0;
	actionplan = false;
	attacking = false;

	g_device->CreateVertexBuffer(sizeof(s_tvert)*4,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_MANAGED,&quad_vb,NULL);
	quad_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,quad,sizeof(s_tvert)*4);
	quad_vb->Unlock();

	g_device->CreateVertexBuffer(sizeof(Vec3Df)*5,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_MANAGED,&square_vb,NULL);
	square_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,square,sizeof(Vec3Df)*5);
	square_vb->Unlock();

	MACRO_BTEXFF("../art/IS.png", &quad_tex);
	MACRO_BTEXFF("../art/groupdig.png", &mine_tex);
	MACRO_BTEXFF("../art/groupattack.png", &attack_tex);
}
//---------------------------------------------------------
Game::~Game(void){
	if (quad_tex) quad_tex->Release();
	if (mine_tex) mine_tex->Release();
	if (g_device) g_device->Release();
	if (object) object->Release();
	if (quad_vb) quad_vb->Release();
	if (square_vb) square_vb->Release();
	if (path_vb) path_vb->Release();

	UnregisterClass("ClassName",GetModuleHandle(NULL));	

	for (int i=g_nbClans-1; i>=0; --i){
		delete g_clans[i];
		delete g_stances[i];
		delete g_belligerence[i];
		delete g_friendliness[i];
	}
	delete g_clans;
	delete g_stances;
	delete g_belligerence;
	delete g_friendliness;
	delete g_board;
	delete g_allies;
	delete g_viz;
	delete g_map;
	delete g_selected;
	delete g_hud;
	delete quad;
	delete square;
	delete mats;
	delete xpathindex;
	delete zpathindex;
	delete pathindexowner;
	if (path) delete path;
}
//---------------------------------------------------------
void Game::InitScene(void){
	aspRatio = (g_winx-400) / g_winy;
	D3DXMatrixPerspectiveFovLH(&matProj,fov,aspRatio,nPlane,fPlane);
	g_device->SetTransform(D3DTS_PROJECTION,&matProj);
	D3DXMatrixRotationX(&matRotX, g_rot.x);
	D3DXMatrixTranslation(&matTrans,g_pos.x,g_pos.y,g_pos.z);
	matView = g_map->trans * matTrans * matRotX;
	g_device->SetTransform(D3DTS_VIEW,&matView);
	g_map->UpdateView();


	//g_device->SetRenderState(D3DRS_AMBIENT,RGB(0,0,0));
	g_device->SetRenderState(D3DRS_AMBIENT,RGB(255,255,255));
	g_device->SetRenderState(D3DRS_LIGHTING,true);
	g_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	g_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	

	ZeroMemory(&light,sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction.x = 0;
	light.Direction.y = -5;
	light.Direction.z = 5;
	light.Falloff = 1.0f;
	light.Range = 10000.0f;
	g_device->SetLight(0,&light);
	g_device->LightEnable(0,true);

	ZeroMemory(&foglight,sizeof(foglight));
	foglight.Type = D3DLIGHT_DIRECTIONAL;
	foglight.Diffuse.r = 0.25f;
	foglight.Diffuse.g = 0.25f;
	foglight.Diffuse.b = 0.25f;
	foglight.Direction.x = 5;
	foglight.Direction.y = -15;
	foglight.Direction.z = 0;
	foglight.Falloff = 1.0f;
	foglight.Range = 10000.0f;


	for(unsigned i = 0;i < 8;++i)	{
		g_device->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		g_device->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		g_device->SetSamplerState(i,D3DSAMP_MIPFILTER,D3DTEXF_ANISOTROPIC);
		g_device->SetSamplerState(i,D3DSAMP_MAXANISOTROPY,aniso);
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW)); 

	g_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
}
//---------------------------------------------------------
void Game::InitGame(void){
	g_clan = NULL;
	g_ctrl = false;
	g_up = false;
	g_down = false;
	g_right = false;
	g_left = false;
	g_grid = true;
	g_wireframe = false;

	g_map = new Map();
	g_rez = g_map->hd;
	g_size = g_map->hspan;
	g_side = g_map->gd;
	g_hgap = g_map->gdhgap;

	g_nbSelected = 0;
	g_selected = new Group*[SELECT_MAX];
	g_board = new Group*[g_side*g_side];
	ZeroMemory(g_board,g_side*g_side*4);
	g_viz = new char[g_side*g_side];
	memset(g_viz,1,g_side*g_side);

	g_nbClans = 5;
	g_nbAliveClans = g_nbClans;
	g_clans = new Clan*[g_nbClans];
	g_pop = 0;
	for (int i=g_nbClans-1; i>=0; --i){
		int x = random(g_map->gd);
		int z = random(g_map->gd);
		if (g_board[x*g_side+z]){
			++i;
			continue;
		}
		g_clans[i] = new Clan(i,x,z,800);
		g_pop += g_clans[i]->size;
	}
	g_friendliness = new float*[g_nbClans];
	g_belligerence = new float*[g_nbClans];
	g_stances = new float*[g_nbClans];
	g_allies = new char*[g_nbClans];
	for (int i=g_nbClans-1; i>=0; --i){
		g_friendliness[i] = new float[g_nbClans];
		g_belligerence[i] = new float[g_nbClans];
		g_stances[i] = new float[g_nbClans];
		g_allies[i] = new char[g_nbClans];
		for (int j=g_nbClans-1; j>=0; --j){
			g_friendliness[i][j] = random(-10.0f,10.0f);
			g_stances[i][j] = g_friendliness[i][j];
			g_belligerence[i][j] = 1.0f;
			g_allies[i][j] = 0;
		}
	}

	//g_nbMines = int((((float)g_pop/1.5f)/50)/5);
	g_nbMines = 5;
	g_nbFreeMines = g_nbMines;
	g_mines = new int[g_nbMines][2];
	/*
	g_mines[0][0] = 0;
	g_mines[0][1] = 0;
	g_mines[1][0] = 1;
	g_mines[1][1] = 1;
	g_map->gold[0] = 1;
	g_map->gold[g_side+1] = 1;
*/
	for (int i=g_nbMines-1; i>=0; --i){
		int x = rand()%g_side;
		int z = rand()%g_side;
		if (g_map->gold[x*g_side+z]){
			++i;
			continue;
		}
		g_map->gold[x*g_side+z] = 1;
		g_mines[i][0] = x;
		g_mines[i][1] = z;
	}

	g_selectedpop = 0;
	g_clan = g_clans[0];
	g_clan->SetVizibility();
	g_pos.set(
		-g_clan->groups[0]->x*g_hgap+g_map->hspan/2.0f,
		-20000,
		-g_clan->groups[0]->z*g_hgap+g_map->hspan/1.5f);
	g_rot.set(-1.0f,0,0);	
	g_hud = new Hud();
	g_hud->UpdateText();
}
//---------------------------------------------------------
void Game::Turn(void){
	Stats();
	sprintf(logstr, "############# Turn %3d #############\n",g_turn); Log();
	int newpop = 0;
	for (int i=g_nbClans-1; i>=0; --i){
		if (g_clans[i]->alive) {
			g_clans[i]->Turn();
			newpop += g_clans[i]->size;
		}
	}
	g_pop = newpop;
	
	// Update model
	float dt = 0.1f;
	// First eval
	for (int i=g_nbClans-1; i>=0; --i){
		Clan& c = *g_clans[i];
		c.d1temper = c.pacifism*c.temper*g_nbClans;
		for (int j=g_nbClans-1; j>=0; --j){
			c.d1temper -= g_belligerence[i][j]*g_clans[j]->temper+g_friendliness[i][j];
		}
		c.d1temper *= dt;
	}
	// Second eval
	for (int i=g_nbClans-1; i>=0; --i){
		Clan& c = *g_clans[i];
		c.d2temper = c.pacifism*(c.temper+0.5f*c.d1temper)*g_nbClans;
		for (int j=g_nbClans-1; j>=0; --j){
			c.d2temper -= g_belligerence[i][j]*(g_clans[j]->temper+0.5f*g_clans[j]->d1temper)+g_friendliness[i][j];
		}
		c.d2temper *= dt;
		c.temper += c.d2temper;
		if (c.temper > 100.0f) c.temper = 100.0f;
		else if (c.temper < 0.0f) c.temper = 0.0f;
	}
	// Update stances
	for (int i=g_nbClans-1; i>=0; --i){
		Clan& c = *g_clans[i];
		for (int j=g_nbClans-1; j>=0; --j){
			g_stances[i][j] = c.temper*(c.pacifism-g_belligerence[i][j])+g_friendliness[i][j];
		}
	}


	Log("####################################\n");
	++g_turn;
	g_hud->UpdateText();
	g_hud->UpdateSelect();
}
//---------------------------------------------------------
void Game::Timeu(){

	static BOOL     m_bTimerInitialized = FALSE;
	static LONGLONG m_llQPFTicksPerSec  = 0;
	double fTime;
	LARGE_INTEGER qwTime;

	if(FALSE == m_bTimerInitialized){
		m_bTimerInitialized = TRUE;
		LARGE_INTEGER qwTicksPerSec;
		QueryPerformanceFrequency( &qwTicksPerSec );
		m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
	}

	QueryPerformanceCounter(&qwTime);
	fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
	time = (float)fTime;
}
//---------------------------------------------------------
void Game::Update(){
	Timeu();
	static float st = time;
	float dt = time - st;
	st = time;

	g_hud->Update(time, dt);

	static bool ldx = true;
	static bool ldz = false;
	float star = dt*0.4f;
	if (ldx) {
		if (light.Direction.x > 5.0f) ldx = false;
		light.Direction.x += star;
		foglight.Direction.x -= star;
	} else {
		if (light.Direction.x < -5.0f) ldx = true;
		light.Direction.x -= star;
		foglight.Direction.x += star;
	}
	if (ldz) {
		if (light.Direction.z > 5.0f) ldz = false;
		light.Direction.z += star;
		foglight.Direction.z -= star;
	} else {
		if (light.Direction.z < -5.0f) ldz = true;
		light.Direction.z -= star;
		foglight.Direction.z += star;
	}

	float pan = -dt*g_pos.y;
	if (g_up || g_down || g_left || g_right){
		if (g_up){
			g_pos.z -= pan;
		}else if (g_down){
			g_pos.z += pan;
		}
		if (g_left){
			g_pos.x += pan;
		}else if (g_right){
			g_pos.x -= pan;
		}
		D3DXMatrixTranslation(&matTrans,g_pos.x,g_pos.y,g_pos.z);
		matView = g_map->trans * matTrans * matRotX;
		UpdateRay();
		g_map->UpdateSelector(ray_ori,ray_dir);
		g_map->UpdateView();
	}
}
//---------------------------------------------------------
void Game::UpdateActions(){
	int i,j,k,x,z;
	Group* g;

	if (g_ctrl){
		x = g_map->curx;
		z = g_map->curz;
		for (i=nbpath-1; i>=0; --i){
			if (xpathindex[i] == x && zpathindex[i] == z){
				g = pathindexowner[i];
				for (j=i-(i%2); j<nbpath; ++j){
					if (pathindexowner[j] == g){
						g->planed -= stampathindex[j];
						for (k=j+1; k<nbpath; ++k){
							stampathindex[k-1] = stampathindex[k];
							xpathindex[k-1] = xpathindex[k];
							zpathindex[k-1] = zpathindex[k];
							pathindexowner[k-1] = pathindexowner[k];
						}
						--nbpath;
						--j;
					}
				}
			}
		}
	} else {
		for (i=g_nbSelected-1; i>=0; --i){
			x = g_selected[i]->x;
			z = g_selected[i]->z;
			for (j=nbpath-1; j>=0; --j){
				if (pathindexowner[j] == g_selected[i]){
					x = xpathindex[j];
					z = zpathindex[j];
					break;
				}
			}
			if (g_map->curx==x && g_map->curz==z){
				continue;
			}
			if (nbpath+2 >= maxpath){
				maxpath *= 2;
				int* xp = new int[maxpath];
				int* zp = new int[maxpath];
				float *sp = new float[maxpath];
				Group** op = new Group*[maxpath];
				for (k=0; k<nbpath; ++k){
					xp[k] = xpathindex[k];
					zp[k] = zpathindex[k];
					op[k] = pathindexowner[k];
				}
				delete xpathindex;
				delete zpathindex;
				delete stampathindex;
				delete pathindexowner;
				xpathindex = xp;
				zpathindex = zp;
				stampathindex = sp;
				pathindexowner = op;
			}
			g_selected[i]->planed += nextcost[i];
			stampathindex[nbpath] = nextcost[i];
			nextcost[i] = 0;
			xpathindex[nbpath] = x;
			zpathindex[nbpath] = z;
			pathindexowner[nbpath++] = g_selected[i];
			stampathindex[nbpath] = 0;
			xpathindex[nbpath] = nextx[i];
			zpathindex[nbpath] = nextz[i];
			pathindexowner[nbpath++] = g_selected[i];
		}
	}

	if (path) delete path;
	path = new Vec3Df[nbpath];
	for (j=nbpath-1; j>=0; --j){
		path[j].set((xpathindex[j]+0.5f)*g_hgap, g_map->hmap[xpathindex[j]+zpathindex[j]*g_side], (zpathindex[j]+0.5f)*g_hgap);
	}

	if (path_vb) path_vb->Release();
	g_device->CreateVertexBuffer(sizeof(Vec3Df)*(nbpath+2),D3DUSAGE_WRITEONLY,D3DFVF_XYZ,D3DPOOL_DEFAULT,&path_vb,NULL);
	path_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,path,sizeof(Vec3Df)*nbpath);
	path_vb->Unlock();
	g_hud->UpdateSelect();
}
//---------------------------------------------------------
void Game::ExecuteActions(){
	Group* g;
	for (int i=0; i<g_nbSelected; ++i){
		g = g_selected[i];
		g->stamina -= g->planed;
		g->planed = 0;
		for (int j=nbpath-1; j>=0; --j){
			if (pathindexowner[j] == g){
				g_board[g->x*g_side+g->z] = 0;
				g->MoveTo(xpathindex[j],zpathindex[j]);
				g_board[g->x*g_side+g->z] = g;
				break;
			}
		}
	}
	ActionsExecuted();
}
//---------------------------------------------------------
void Game::ActionsExecuted(){
	if (g_nbSelected) g_selected[0]->clan->MarkGroupsPositions();
	showway = false;
	nbpath = 0;
	g_hud->UpdateSelect();
}
//---------------------------------------------------------
void Game::Render(void){

	g_device->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,10,50),1.0f,0);
	g_device->BeginScene();

	g_device->SetLight(0,&light);
	g_device->SetTransform(D3DTS_VIEW,&matView);
	SetEColor(MC_EBLACK);
	g_map->Render();

	// Actions
	SetEColor(MC_EGREEN);
	g_device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	g_device->SetFVF(D3DFVF_XYZ);
	g_device->SetStreamSource(0,path_vb,0,sizeof(Vec3Df));
	g_device->DrawPrimitive(D3DPT_LINELIST,0,nbpath/2);
	if (showway){
		SetEColor(MC_EBLUE);
		g_device->SetStreamSource(0,way_vb,0,sizeof(Vec3Df));
		g_device->DrawPrimitive(D3DPT_LINELIST,0,nbway);
	}


	// Groups
	g_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	g_device->SetTexture(0,quad_tex);
	g_device->SetFVF(TVERT);
//	SetEColor(MC_ELIGHTBLUE);
	g_clan->Render();
	//SetEColor(MC_ERED);
	for (int i=g_nbClans-1; i>0; --i){
		g_clans[i]->Render();
	}
	/*
	// Group rendering through board
	int idx;
	for (int x=g_side-1; x>=0; --x){
		idx = x*g_side;
		for (int z=0; z<g_side; ++z, ++idx){
			if (g_viz[idx] && g_board[idx] && g_board[idx]->clan!=g_clan){
				g_board[idx]->Render();
			}
		}
	}
	*/
	// Attacking
	if (attacking){
		g_device->SetRenderState(D3DRS_LIGHTING,false);
		SetEColor(MC_EBLACK);
		g_device->SetTexture(0,attack_tex);
		SetTempoTranslate(g_map->gdhgap*g_map->curx,
			g_map->hmap[g_map->curx+g_map->curz*g_map->gd],
			g_map->gdhgap*g_map->curz);

		g_device->SetStreamSource(0,quad_vb,0,sizeof(s_tvert));
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		g_device->SetRenderState(D3DRS_LIGHTING,true);
	}
	g_device->SetTexture(0,NULL);
	g_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);

	// Selected groups
	g_device->SetFVF(D3DFVF_XYZ);
	SetEColor(MC_EBLUE);
	for (int i=g_nbSelected-1; i>=0; --i){
		g_selected[i]->RenderSelection();
	}	
	g_device->SetTransform(D3DTS_VIEW,&matView);

	g_hud->Render();
	g_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);


	g_device->EndScene();
	if (g_device->Present(NULL,NULL,NULL,NULL) == D3DERR_DEVICELOST)
		CheckDevice();
}
//---------------------------------------------------------
void Game::Attack(){
	int x = g_map->curx;
	int z = g_map->curz;
	Group* g = g_board[x*g_side+z];
	if (g){
		if (g->clan==g_clan) {
			g_hud->SetCaption("We shall not attack our owns.");
		} else {
			for (int i=g_nbSelected-1; i>=0; --i){
				if (g_selected[i]->x>x+1 || g_selected[i]->x<x-1 ||
				g_selected[i]->z>z+1 || g_selected[i]->z<z-1){
					g_hud->SetCaption("All the groups are not close enough to attack.");
					return;
				}
			}
			// Attack panel
			g_hud->AttackPanel(x,z);
		}
	} else {
		g_hud->SetCaption("Nobody there to attack.");
	}
}
//---------------------------------------------------------
void Game::SetEColor(e_matcol col){
	curcol = col;
	g_device->SetMaterial(&mats[curcol]);
}
//---------------------------------------------------------
void Game::SetTempoEColor(e_matcol col){
	g_device->SetMaterial(&mats[col]);
}
//---------------------------------------------------------
void Game::PopTempoEColor(){
	g_device->SetMaterial(&mats[curcol]);
}
//---------------------------------------------------------
void Game::SetTempoTranslate(float x, float y, float z){
	D3DXMatrixTranslation(&matTemp, x, y, z);
	matTemp *= matView;
	g_device->SetTransform(D3DTS_VIEW,&matTemp);
}
//---------------------------------------------------------
void Game::UpdateRay(void){
	POINT p;
	GetCursorPos(&p);
	POINT p0 = {(LONG)g_winx,(LONG)g_winy};
	if (windowed){
		ScreenToClient(hWindow,&p);
		ScreenToClient(hWindow,&p0);
	}

	// Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  ( ( ( 2.0f * p.x ) / p0.x  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * p.y ) / p0.y ) - 1 ) / matProj._22;
  v.z =  1.0f;

  // Get the inverse view matrix
  D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &matView);

  // Transform the screen space pick ray into 3D space
  ray_dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
  ray_dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
  ray_dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	D3DXVec3Normalize((D3DXVECTOR3*)(&ray_dir),(D3DXVECTOR3*)(&ray_dir));
  ray_ori.x = m._41;
  ray_ori.y = m._42;
  ray_ori.z = m._43;

	// calc origin as intersection with near frustum
	ray_ori+=ray_dir*nPlane;
}
//---------------------------------------------------------
void Game::UpdateWay(){
	int ix, iz, dx, dz, sx, sz, x, z, cx, cz, n=0;
	float ax, az, kx, kz;
	Group* g;
	cx = g_map->curx;
	cz = g_map->curz;
	if(lastway == cx+g_side*cz) return;
	ZeroMemory(g_map->hl, g_map->gd2);
	if (lastway == -1){
		lastway = cx+g_side*cz;
		nbway = g_nbSelected;
		if (nbway > maxnext){
			maxnext *= 2;
			delete nextx;
			delete nextz;
			delete nextcost;
			nextx = new int[maxnext];
			nextz = new int[maxnext];
			nextcost = new float[maxnext];
		}
		if (way) delete way;
		way = new Vec3Df[nbway*2];
	}
	lastway = cx+g_side*cz;
	for (int i=0; i<g_nbSelected; ++i){
		g = g_selected[i];
		sx = g->x;
		sz = g->z;
		for (int j=nbpath-1; j>=0; --j){
			if (pathindexowner[j] == g){
				sx = xpathindex[j];
				sz = zpathindex[j];
				break;
			}
		}
		x = sx;  z = sz;
		way[n++].set((x+0.5f)*g_hgap, g_map->hmap[x+g_side*z], (z+0.5f)*g_hgap);
		dx = cx-x;	dz = cz-z;
		ix = (dx==0 ? 0 : (dx>0 ? 1 : -1));
		iz = (dz==0 ? 0 : (dz>0 ? 1 : -1));
		if (ix==0){
			while (z!=cz){
				z+=iz;
				if ((g->stamina < g->planed+distFromManhattan(x-sx,z-sz)) || (g_board[x*g_side+z] && g_board[x*g_side+z]->clan != g->clan)){
					z-=iz;
					break;
				}
				g_map->hl[x*g_map->gd+z] = 1;
			}
		} else if (iz==0){
			while (x!=cx){
				x+=ix;
				if ((g->stamina < g->planed+distFromManhattan(x-sx,z-sz)) || (g_board[x*g_side+z] && g_board[x*g_side+z]->clan != g->clan)){
					x-=ix;
					break;
				}
				g_map->hl[x*g_map->gd+z] = 1;
			}
		} else {
			dx *= ix; dz *= iz;
			ax = 0.5f; az = 0.5f; 
			while (x!=cx || z!=cz){
				kx = (1.0f-ax)/dx;
				kz = (1.0f-az)/dz;
				if (kx < kz){
					x += ix;
					if ((g->stamina < g->planed+distFromManhattan(x-sx,z-sz)) || (g_board[x*g_side+z] && g_board[x*g_side+z]->clan != g->clan)){
						x-=ix;
						break;
					}
					ax = 0.0f;
					az += kx*dz;
				} else if (kz < kx){
					z += iz;
					if ((g->stamina < g->planed+distFromManhattan(x-sx,z-sz)) || (g_board[x*g_side+z] && g_board[x*g_side+z]->clan != g->clan)){
						z-=iz;
						break;
					}
					az = 0.0f;
					ax += kz*dx;
				} else {
					x += ix;
					z += iz;
					if ((g->stamina < g->planed+distFromManhattan(x-sx,z-sz)) || (g_board[x*g_side+z] && g_board[x*g_side+z]->clan != g->clan)){
						x-=ix;
						z-=iz;
						break;
					}
					ax = 0.0f;
					az = 0.0f;
				}
				g_map->hl[x*g_map->gd+z] = 1;
			}
		}
		nextx[i] = x;
		nextz[i] = z;
		nextcost[i] = distFromManhattan(x-sx,z-sz);
		way[n++].set((x+0.5f)*g_hgap, g_map->hmap[x+g_side*z], (z+0.5f)*g_hgap);
	}
	
	if (way_vb) way_vb->Release();
	g_device->CreateVertexBuffer(sizeof(Vec3Df)*(n+2),D3DUSAGE_WRITEONLY,D3DFVF_XYZ,D3DPOOL_DEFAULT,&way_vb,NULL);
	way_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,way,sizeof(Vec3Df)*n);
	way_vb->Unlock();
	g_hud->UpdateSelect();
}
//---------------------------------------------------------
void Game::Hover(int x, int z){
	if (g_board[x*g_side+z]){
		sprintf(g_hud->captionstr, "%s group: %d units.",g_board[x*g_side+z]->clan->name, g_board[x*g_side+z]->size);
		g_hud->caption = true;
	} else {
		g_hud->CaptionOff();
	}
}
//---------------------------------------------------------
void Game::KeyDown(int key){
 	if ((key>=96 && key<=105) || (key>=48 && key<=57) || key==13 || key==8 || key==46){
		if (!g_hud->subpopup && !g_hud->goldoffer){
			if (key==13) return;
			g_hud->Click(150,575);
		}
		if (key==13){
			g_hud->PopupEnter();
		} else if (key==8 || key==46){
			g_hud->PopupDelete();
		} else {
			g_hud->PopupText(key%16);
		}
	} else {
		switch (key){
			case VK_UP: g_up=true; break;
			case VK_DOWN: g_down=true; break;
			case VK_LEFT: g_left=true; break;
			case VK_RIGHT: g_right=true; break;
			case 84: g_map->Terraforming(); break;
			case 87: g_wireframe ^= 1; break;
			case 72: g_hud->Toggle(); break;
			case 77: g_hud->Click(50,400); break;
			case 71: g_hud->Click(150,400); break;
			case 65: g_hud->Click(250,400); break;
			default: break;
		}
	}
}
//---------------------------------------------------------
void Game::KeyUp(int key){
	switch (key){
		case VK_UP: g_up=false; break;
		case VK_DOWN: g_down=false; break;
		case VK_LEFT: g_left=false; break;
		case VK_RIGHT: g_right=false; break;
		case VK_END: Turn(); break;
		default: break;
	}
}
//---------------------------------------------------------
void Game::LeftButtonDown(int w, int l){
	POINT p;
	GetCursorPos(&p);
	if (windowed){
		ScreenToClient(hWindow,&p);
	}
	
	if (p.x < 300 || g_hud->getmouse){
		g_hud->Click(p.x,p.y);
		return;
	}

	if (attacking){
		attacking = false;
		Attack();
		return;
	}
	g_mousebut[0] = true;
	if (g_hud->subpopup) g_hud->subpopup = false;
	if ((w&MK_CONTROL) != MK_CONTROL){
		nbpath = 0;
	}
}
//---------------------------------------------------------
void Game::LeftButtonUp(int w, int l){
	if (g_mousebut[0] == false){
		return;
	}
	g_mousebut[0] = false;

	POINT p;
	GetCursorPos(&p);
	if (windowed){
		ScreenToClient(hWindow,&p);
	}
	
	//int px = LOWORD(l);
	//int py = HIWORD(l);
	if (p.x < 300 || g_hud->getmouse){
		return;
	}
	int xmin, xmax, zmin, zmax;
	if (g_map->startx<g_map->curx){
		xmin = g_map->startx;
		xmax = g_map->curx;
	} else {
		xmin = g_map->curx;
		xmax = g_map->startx;
	}
	if (g_map->startz<g_map->curz){
		zmin = g_map->startz;
		zmax = g_map->curz;
	} else {
		zmin = g_map->curz;
		zmax = g_map->startz;
	}
	Group* g;
	if ((w&MK_CONTROL) == MK_CONTROL){
		for (int x=xmin; x<=xmax; ++x){
			for (int z=zmin; z<=zmax; ++z){
				if (g_board[x*g_side+z]){
					g = g_board[x*g_side+z];
					for (int i=g_nbSelected-1; i>=0; --i){
						if (g_selected[i] == g){
							g_selected[i]->planed = 0;
							nextcost[i] = 0;
							g_selectedpop -= g->size;
							g_selected[i] = g_selected[--g_nbSelected];
							if (g_board[g->x*g_side+g->z] && g_board[g->x*g_side+g->z] != g){
								g->clan->MergeGroups(g,g_board[g->x*g_side+g->z]);
							}
							g = NULL;
							break;
						}
					}
					if (g && g->clan==g_clan) {
						g_selected[g_nbSelected++] = g;
						g_selectedpop += g->size;
					}
				}
			}
		}
	} else {
		for (int i=0; i<g_nbSelected; ++i) {
			g = g_selected[i];
			g->planed = 0;
			nextcost[i] = 0;
			if (g_board[g->x*g_side+g->z] && g_board[g->x*g_side+g->z] != g){
				g->clan->MergeGroups(g,g_board[g->x*g_side+g->z]);
			}
		}
		g_nbSelected = 0;
		g_selectedpop = 0;
		for (int x=xmin; x<=xmax; ++x){
			for (int z=zmin; z<=zmax; ++z){
				if (g_board[x*g_side+z]){
					g = g_board[x*g_side+z];
					if (g->clan != g_clan) continue;
					g_selected[g_nbSelected++] = g;
					g_selectedpop += g->size;
				}
			}
		}
	}
	g_hud->UpdateSelect();
	g_map->RebuildSelector();
}
//---------------------------------------------------------
void Game::RightButtonDown(int w, int l){
	if (attacking){
		attacking = false;
		g_hud->SetCaption("Attack cancelled.");
		return;
	}
	if (g_hud->attackpanel){
		g_hud->attackpanel = false;
		g_hud->getmouse = false;
		return;
	} else if (g_hud->resultpanel) {
		g_hud->resultpanel = false;
		g_hud->attackpanel = false;
		g_hud->getmouse = false;
		return;
	} else if (g_hud->goldpanel) {
		g_hud->goldpanel = false;
		g_hud->getmouse = false;
	}
	g_mousebut[2] = true;
	if (g_nbSelected && (w&MK_CONTROL)!=MK_CONTROL){
		showway = true;
		UpdateWay();
	}
}
//---------------------------------------------------------
void Game::RightButtonUp(int w, int l){
	if (g_mousebut[2] == false){
		return;
	}
	if (showway){
		showway = false;
		lastway = -1;
		ZeroMemory(g_map->hl, g_map->gd2);
	}
	if (g_nbSelected){
		if ((w&MK_CONTROL) == MK_CONTROL){
			g_ctrl = true;
			UpdateActions();
			g_ctrl = false;
		} else {
			UpdateActions();
		}
	}
	g_mousebut[2] = false;
}
//---------------------------------------------------------
void Game::MidButtonDown(int key){
	g_mousebut[1] = true;
}
//---------------------------------------------------------
void Game::MidButtonUp(int key){
	if (g_nbSelected){
		ExecuteActions();
	}
	g_mousebut[1] = false;
}
//---------------------------------------------------------
void Game::MouseWheel(int key){
	int roll = HIWORD(key);
	if (roll < 600) {//WUP
		if (g_pos.y < -2000){
			g_pos.y += 2000.0f; //zoom
			D3DXMatrixTranslation(&matTrans,g_pos.x,g_pos.y,g_pos.z);
			matView = g_map->trans * matTrans * matRotX;
		}
	} else {//WDOWN
		if (g_pos.y > -22000){
			g_pos.y -= 2000.0f; 
			D3DXMatrixTranslation(&matTrans,g_pos.x,g_pos.y,g_pos.z);
			matView = g_map->trans * matTrans * matRotX;
		}
	}
	g_map->UpdateView();
}
//---------------------------------------------------------
void Game::MouseMove(int mod, int x, int y){
	if (g_hud->getmouse){
		return;
	}
	static int sx;
	static int sy;
	if (!g_dragging){
		g_dragging = false;
	}
	sx = x;	sy = y;

	UpdateRay();
	g_map->UpdateSelector(ray_ori, ray_dir);
}
//---------------------------------------------------------
void Game::CheckDeviceCaps(void){
	D3DCAPS9 caps;
	HRESULT hr5 = g_device->GetDeviceCaps(&caps);

	HRESULT ok = D3D_OK;
	HRESULT na = D3DERR_NOTAVAILABLE;
	HRESULT nic = D3DERR_INVALIDCALL;
	return;
}
//---------------------------------------------------------
void Game::CheckDevice(void){
	HRESULT hr = g_device->TestCooperativeLevel();
	while (hr==D3DERR_DEVICELOST || hr==D3DERR_DEVICENOTRESET){
		if (hr == D3DERR_DEVICELOST){
			//g_hud->font->OnLostDevice();
			//g_hud->sprite->OnLostDevice();
			hr = g_device->TestCooperativeLevel();
		} else if (hr == D3DERR_DEVICENOTRESET){
			if (FAILED(g_device->Reset(&params))){
				continue;
			}
			//g_hud->font->OnResetDevice();
			//g_hud->sprite->OnResetDevice();
			InitScene();
			return;
		}
	}
}

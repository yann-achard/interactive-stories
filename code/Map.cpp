//---------------------------------------------------------
#include <time.h>
#include <math.h>
#include "random.h"
#include "main.h"
#include "Game.h"
#include "Map.h"
#include "helpers.h"
//---------------------------------------------------------
#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest.x=v1.y*v2.z-v1.z*v2.y; \
          dest.y=v1.z*v2.x-v1.x*v2.z; \
          dest.z=v1.x*v2.y-v1.y*v2.x;
#define DOT(v1,v2) (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z)
#define SUB(dest,v1,v2) \
          dest.x=v1.x-v2.x; \
          dest.y=v1.y-v2.y; \
          dest.z=v1.z-v2.z; 
//---------------------------------------------------------
 Vec3Df edge1, edge2, tvec, pvec, qvec, t0, t1, t2, t3;
 float det,inv_det;
 float t,u,v;
//---------------------------------------------------------
Map::Map(){
	hspan = 40000.0f;
	vspan =  6000.0f;
	hd = 500;
	gd = 10;
	hdhgap = hspan/(float)hd;
	gdhgap = hspan/(float)gd;
	hdgd = hd/gd;
	hdgd2 = hdgd*hdgd;
	gd2tri = hdgd2*2;
	gd2pts = hdgd2*6;
	hd2 = hd*hd;
	gd2 = gd*gd;
	nbthd = hd2*2;
	hdz = hd*6;
	nb_hdv = nbthd*3;
	hdv = new s_vert[nb_hdv];
	hdvb = NULL;

	nb_grid = (gd+1)*(hd+1);
	grid = new Vec3Df*[2];
	grid[0] = new Vec3Df[nb_grid];
	grid[1] = new Vec3Df[nb_grid];
	grid_vb[0] = NULL;
	grid_vb[1] = NULL;

	nb_select = hd*4;
	select = new Vec3Df[nb_select];
	select_vb = NULL;
	know_pick = false;
	hmap = new float[gd2];
	hl = new char[gd2];
	ZeroMemory(hl,gd2);
	gold = new char[gd2];
	startx = 0;
	startz = 0;
	curx = 0;
	curz = 0;
	
	vxmin = 0;
	vxmax = gd;
	vzmin = 0;
	vzmax = gd;
	D3DXMatrixTranslation(&trans, -hspan/2.0f, -vspan, -hspan/2.0f);
	InitArrays();
	Terraforming();
}
//---------------------------------------------------------
Map::~Map(){
	delete hdv;
	delete grid[0];
	delete grid[1];
	delete grid;
	delete select;
	delete hmap;
	if (hdvb) hdvb->Release();
	if (grid_vb[0]) grid_vb[0]->Release();
	if (grid_vb[1]) grid_vb[1]->Release();
	if (select_vb) select_vb->Release();
}
//---------------------------------------------------------
void Map::InitArrays(){
	int tri;
	for (int gx=0; gx<gd ; ++gx){
		for (int gz=0; gz<gd; ++gz){
			int sq = (gx*gd+gz)*gd2pts;
			for (int hx=0; hx<hdgd; ++hx){
				for (int hz=0; hz<hdgd; ++hz){
					tri = sq+(hx*hdgd+hz)*6;
					int x = gx*hdgd+hx;
					int z = gz*hdgd+hz;
					for (int i=0; i<6; ++i){
						hdv[tri+i].normal.y = 1;
					}
					hdv[tri].pos.x = x*hdhgap;
					hdv[tri].pos.z = z*hdhgap;
					hdv[tri+1].pos.x = x*hdhgap;
					hdv[tri+1].pos.z = (z+1)*hdhgap;
					hdv[tri+2].pos.x = (x+1)*hdhgap;
					hdv[tri+2].pos.z = z*hdhgap;
					hdv[tri+3].pos.x = (x+1)*hdhgap;
					hdv[tri+3].pos.z = (z+1)*hdhgap;
					hdv[tri+4].pos.x = (x+1)*hdhgap;
					hdv[tri+4].pos.z = z*hdhgap;
					hdv[tri+5].pos.x = x*hdhgap;
					hdv[tri+5].pos.z = (z+1)*hdhgap;
				}
			}
		}
	}
}
//---------------------------------------------------------
void Map::Harmonize(){
	VOID* pData;
	
	if (hdvb) hdvb->Release();
	g_device->CreateVertexBuffer(sizeof(s_vert)*nb_hdv,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_DEFAULT,&hdvb,NULL);
	hdvb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,hdv,sizeof(s_vert)*nb_hdv);
	hdvb->Unlock();

	if (grid_vb[0]) grid_vb[0]->Release();
	g_device->CreateVertexBuffer(sizeof(Vec3Df)*nb_grid,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_DEFAULT,&grid_vb[0],NULL);
	grid_vb[0]->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,grid[0],sizeof(Vec3Df)*nb_grid);
	grid_vb[0]->Unlock();

	if (grid_vb[1]) grid_vb[1]->Release();
	g_device->CreateVertexBuffer(sizeof(Vec3Df)*nb_grid,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_DEFAULT,&grid_vb[1],NULL);
	grid_vb[1]->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,grid[1],sizeof(Vec3Df)*nb_grid);
	grid_vb[1]->Unlock();
	RebuildSelector();
}
//---------------------------------------------------------
void Map::Point(Vec3Df& p1, Vec3Df& p2, int x, int y, float h1, float h2){
	POINT w = {(LONG)g_winx,(LONG)g_winy};
	if (g_game.windowed){
		ScreenToClient(g_game.hWindow,&w);
	}

  D3DXVECTOR3 v;
  v.x =  ( ( ( 2.0f * x ) / w.x  ) - 1 ) / g_game.matProj._11;
	v.y = -( ( ( 2.0f * y ) / w.y ) - 1 ) / g_game.matProj._22;
  v.z =  1.0f;
  D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &g_game.matView);
  ray_dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
  ray_dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
  ray_dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	D3DXVec3Normalize((D3DXVECTOR3*)(&ray_dir),(D3DXVECTOR3*)(&ray_dir));
  ray_ori.x = m._41;
  ray_ori.y = m._42;
  ray_ori.z = m._43;
	ray_ori+=ray_dir*g_game.nPlane;
	p1 = ray_ori;
	p2 = ray_ori;
	float k = (h1-(ray_ori.y))/ray_dir.y;
	p1 += ray_dir*k;
	k = (h2-(ray_ori.y))/ray_dir.y;
	p2 += ray_dir*k;
}
//---------------------------------------------------------
void Map::UpdateView(){
	int x, z;
	float gap = (float)gdhgap;
	float ztmp1, ztmp2;
	Vec3Df p1;
	Vec3Df p2;

	Point(p1,p2,300,0,0,vspan);
	x = int ((p1.x<p2.x ?  p1.x : p2.x)/gap);
	if (x < 0.0f) x = 0;
	vxmin = x;

	ztmp1 = (p1.z>p2.z ?  p1.z : p2.z);

	Point(p1,p2,(int)g_winx,0,0,vspan);
	x = int ((p1.x>p2.x ?  p1.x : p2.x)/gap);
	if (x > gd) x = gd;
	vxmax = x;

	ztmp2 = (p1.z>p2.z ?  p1.z : p2.z);

	z = int((ztmp1>ztmp2 ?  ztmp1 : ztmp2)/gap+1.0f);
	if (z > gd) z = gd;
	vzmax = z;

	Point(p1,p2,(int)g_winx/2,(int)g_winy,0,vspan);
	z = int((p1.z<p2.z ?  p1.z : p2.z)/gap);
	if (z < 0) z = 0;
	vzmin = z;
}
//---------------------------------------------------------
void Map::Render(){
	int emit;
	int idx;
	if (g_wireframe) g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_device->SetFVF(VERT);
	g_device->SetStreamSource(0,hdvb,0,sizeof(s_vert));
	for (int x=vxmin; x<vxmax; ++x){
		for (int z=vzmin; z<vzmax; ++z){
			idx = x*gd+z;
			if (!g_viz[idx]){
				g_device->SetLight(0,&(g_game.foglight));
				if (hl[idx]){
					g_game.SetEColor(MC_EBLUE);
					g_device->DrawPrimitive(D3DPT_TRIANGLELIST,(idx)*gd2pts,gd2tri);
					g_game.SetEColor(MC_EBLACK);
				} else {
					g_device->DrawPrimitive(D3DPT_TRIANGLELIST,(idx)*gd2pts,gd2tri);
				}
				g_device->SetLight(0,&(g_game.light));
				continue;
			}
			emit = hl[idx] + gold[idx];
			if (emit) {
				g_game.SetEColor(emit==2 ? MC_EGOLDBLUE : (gold[idx] ? MC_EGOLD : MC_EBLUE));
				g_device->DrawPrimitive(D3DPT_TRIANGLELIST,(idx)*gd2pts,gd2tri);
				g_game.SetEColor(MC_EBLACK);
			} else {
				g_device->DrawPrimitive(D3DPT_TRIANGLELIST,(idx)*gd2pts,gd2tri);
			}
		}
	}
	if (g_wireframe) g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	g_device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	g_device->SetFVF(D3DFVF_XYZ);
	if (g_grid)
		RenderGrid();
	RenderSelector();

	g_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
}
//---------------------------------------------------------
void Map::RenderSelector(){
	g_game.SetEColor(MC_EORANGE);
	g_device->SetStreamSource(0,select_vb,0,sizeof(Vec3Df));
	g_device->DrawPrimitive(D3DPT_LINESTRIP,0,nb_select_to_render-1);
}
//---------------------------------------------------------
void Map::RebuildSelector(){
	int xmin, zmin, xmax, zmax;
	int n, goal, off;
	Vec3Df* g;

	if (g_game.showway) g_game.UpdateWay();
	if (!g_mousebut[0]){
		startx = curx;
		startz = curz;
		xmin = curx;
		zmin = curz;
		xmax = curx+1;
		zmax = curz+1;
		if (g_clan) g_game.Hover(curx,curz);
	} else {
		if (startx<curx){
			xmin = startx;
			xmax = curx+1;
		} else {
			xmin = curx;
			xmax = startx+1;
		}
		if (startz<curz){
			zmin = startz;
			zmax = curz+1;
		} else {
			zmin = curz;
			zmax = startz+1;
		}
	}
	n=0;

	g = grid[1];
	off = zmin*(hd+1);
	goal = xmax*hdgd;
	for (int x=xmin*hdgd; x<=goal; ++x){
		select[n++] = g[off+x];
	}

	g = grid[0];
	off = xmax*(hd+1);
	goal = zmax*hdgd;
	for (int z=zmin*hdgd+1; z<=goal; ++z){
		select[n++] = g[off+z];
	}

	g = grid[1];
	off = zmax*(hd+1);
	goal = xmin*hdgd;
	for (int x=xmax*hdgd-1; x>=goal; --x){
		select[n++] = g[off+x];
	}

	g = grid[0];
	off = xmin*(hd+1);
	goal = zmin*hdgd;
	for (int z=zmax*hdgd-1; z>=goal; --z){
		select[n++] = g[off+z];
	}
	nb_select_to_render = n;

	VOID* pData;
	if (select_vb) select_vb->Release();
	g_device->CreateVertexBuffer(sizeof(Vec3Df)*nb_select,D3DUSAGE_WRITEONLY,VERT,D3DPOOL_DEFAULT,&select_vb,NULL);
	select_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,select,sizeof(Vec3Df)*nb_select_to_render);
	select_vb->Unlock();
}
//---------------------------------------------------------
bool Map::Intersect_triangle(Vec3Df& p0, Vec3Df& p1, Vec3Df& p2){
	   /* find vectors for two edges sharing p0 */
   SUB(edge1, p1, p0);
   SUB(edge2, p2, p0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

   if (det > -EPSILON && det < EPSILON)
     return false;
   inv_det = 1.0f / det;

   /* calculate distance from p0 to ray ori */
   SUB(tvec, ori, p0);

   /* calculate U parameter and test bounds */
   u = DOT(tvec, pvec) * inv_det;
   if (u < 0.0f || u > 1.0f)
     return false;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   v = DOT(dir, qvec) * inv_det;
   if (v < 0.0f || u + v > 1.0f)
     return false;

   /* calculate t, ray intersects triangle */
   //t = DOT(edge2, qvec) * inv_det;
   return true;
}
//---------------------------------------------------------
bool Map::Intersect(int x, int z){
	int idx = x+z*gd;
	t0.set(x*gdhgap, hmap[idx], z*gdhgap);
	t1.set(x*gdhgap, hmap[idx], (z+1)*gdhgap);
	t2.set((x+1)*gdhgap, hmap[idx], z*gdhgap);
	t3.set((x+1)*gdhgap, hmap[idx], (z+1)*gdhgap);
	return (Intersect_triangle(t0, t1, t2) ||	Intersect_triangle(t2, t3, t1));
}
//---------------------------------------------------------
void Map::UpdateSelector(Vec3Df& _ori, Vec3Df& _dir){
	ori = _ori;
	dir = _dir;
	
	if (know_pick){
		if (Intersect(curx,curz)){
			return;
		} else if (curx<gd-1 && Intersect(curx+1,curz)) {
			++curx; RebuildSelector(); return;
		} else if (curx>0 && Intersect(curx-1,curz)) {
			--curx; RebuildSelector(); return;
		} else if (curz<gd-1 && Intersect(curx,curz+1)) {
			++curz; RebuildSelector(); return;
		} else if (curz>0 && Intersect(curx,curz-1)) {
			--curz; RebuildSelector(); return;
		} else if (curx<gd-1 && curz<gd-1 && Intersect(curx+1,curz+1)) {
			++curx; ++curz; RebuildSelector(); return;
		} else if (curx>0 && curz>0 && Intersect(curx-1,curz-1)) {
			--curx; --curz; RebuildSelector(); return;
		} else if (curx<gd-1 && curz>0 && Intersect(curx+1,curz-1)) {
			++curx; --curz; RebuildSelector(); return;
		} else if (curx>0 && curz<gd-1 && Intersect(curx-1,curz+1)) {
			--curx; ++curz; RebuildSelector(); return;
		} else {
			know_pick = false;
		}
	}
	if (!know_pick){
		for (int x=0; x<gd; ++x){
			for (int z=0; z<gd; ++z){
				if (Intersect(x,z)){
					know_pick = true;
					curx = x;
					curz = z;
					RebuildSelector();
					return;
				}
			}
		}
	}
}
//---------------------------------------------------------
void Map::RenderGrid(){
	g_game.SetEColor(MC_EGRAY);
	g_device->SetStreamSource(0,grid_vb[0],0,sizeof(Vec3Df));
	for (int x=vxmax; x>=vxmin; --x){
		g_device->DrawPrimitive(D3DPT_LINESTRIP,x*(hd+1),hd);
	}
	g_device->SetStreamSource(0,grid_vb[1],0,sizeof(Vec3Df));
	for (int x=vzmax; x>=vzmin; --x){
		g_device->DrawPrimitive(D3DPT_LINESTRIP,x*(hd+1),hd);
	}
}
//---------------------------------------------------------
void Map::Update(float dt){
}
//---------------------------------------------------------
inline float mix(float a, float b, float c, float d, float r){
	return (((a+b+c+d)/4.0f)+random(-r,r));
}
//---------------------------------------------------------
inline int inbase(int x, int z, int s){
	if (x<0) x=-x;
	if (z<0) z=-z;
	if (x>=s) x=x-s;
	if (z>=s) z=z-s;
	return (x+z*s);
}
//---------------------------------------------------------
float* smooth(float* base, int ss, int s){
	float* smoo = new float[ss];
	for (int u=1; u<s-1; ++u){
		int v = s;
		for (int z=1; z<s-1; ++z, v+=s){
			smoo[u+v] = (((base[u-1+v]+base[u+1+v]+base[u+v-s]+base[u+v+s])/2.0f)
										+(base[u-1+v-s]+base[u+1+v+s]+base[u+1+v-s]+base[u-1+v+s])/4.0f)/3.0f;
		}
	}
	delete base;
	return (smoo);
}
//---------------------------------------------------------
void Map::Terraforming(){
	float range = 100000.0f;
	float smoothing = 0.60f;

	srand((unsigned int)time(NULL));
	int s = 4;
	while (s<hd) s*=2;
	++s;
	int ss = s*s;
	float* base = new float[ss];
	for (int i=0; i<ss; ++i) base[i] = 0;

	// Creating terrain
	int nvs;
	for (int vs=s-1; vs>=2; vs=nvs){
		nvs = vs/2;
		
		// Diamond
		for (int x=0; x+vs<s; x+=vs){
			for (int z=0; z+vs<s; z+=vs){
				base[x+nvs+(z+nvs)*s] = mix(base[x+z*s],
																		base[x+vs+z*s],
																		base[x+(z+vs)*s],
																		base[x+vs+(z+vs)*s],
																		range);
			}
		}

		// Square
		for (int x=0; x+vs<s; x+=vs){
			for (int z=0; z+vs<s; z+=vs){
				base[x+(z+nvs)*s] = mix(base[inbase(x,z,s)],
																base[inbase(x,z+vs,s)],
																base[inbase(x+nvs,z+nvs,s)],
																base[inbase(x-nvs,z+nvs,s)],
																range);
				base[x+nvs+z*s] = mix(base[inbase(x,z,s)],
																base[inbase(x+nvs,z+nvs,s)],
																base[inbase(x+vs,z,s)],
																base[inbase(x+nvs,z-nvs,s)],
																range);
				base[x+vs+(z+nvs)*s] = mix(base[inbase(x+vs,z,s)],
																base[inbase(x+vs,z+vs,s)],
																base[inbase(x+nvs,z+nvs,s)],
																base[inbase(x+nvs+vs,z+nvs,s)],
																range);
				base[x+nvs+(z+vs)*s] = mix(base[inbase(x+vs,z+vs,s)],
																base[inbase(x,z+vs,s)],
																base[inbase(x+nvs,z+nvs,s)],
																base[inbase(x+nvs,z+nvs+vs,s)],
																range);
			}
		}
		range *= smoothing;
	}
	
	// Smoothing
	for (int i=0; i<1; ++i)
		base = smooth(base, ss, s);

	// Computing min and max
	float min = base[(s/2)*(s+1)];
	float max = min;
	int off = (s-hd-1)/2;
	for (int x=off; x<s-off-1; ++x){
		for (int z=off; z<s-off-1; ++z){
			if (base[x+z*s] < min){
				min = base[x+z*s];
			} else if (base[x+z*s] > max){
				max = base[x+z*s];
			}
		}
	}
	float delta = max-min;

	// Computing colors
	float* colx = new float[ss];
	float* coly = new float[ss];
	float* colz = new float[ss];

	float zero = min+delta*0.1f;
	for (int n=0; n<ss; ++n){
		float h = (base[n]-min)/delta;
		float r;

		if (h < 0.1f){// Deep water to water
			base[n] = zero;
		}
		if (h < 0.15){// Water to sand
			r = h/0.15f;
			colx[n] = 0.93f-r*0.63f;
			coly[n] = 0.77f-r*0.55f;
			colz[n] = 0.56f-r*0.49f;
		} else if (h < 0.3f){// Sand to ground
			r = (h-0.15f)/0.15f;
			colx[n] = 0.30f+r*0.2f;
			coly[n] = 0.22f+r*0.3f;
			colz[n] = 0.07f+r*0.08f;
		} else if (h < 0.4f){// Dirt to clear grass
			r = (h-0.3f)/0.1f;
			colx[n] = 0.50f-r*0.30f;
			coly[n] = 0.52f-r*0.02f;
			colz[n] = 0.15f-r*0.05f;
		} else if (h < 0.65f){// Clear grass to lush grass
			r = (h-0.4f)/0.25f;
			colx[n] = 0.2f-r*0.2f;
			coly[n] = 0.5f-r*0.3f;
			colz[n] = 0.1f-r*0.1f;
		} else if (h < 0.9f){// Lush grass to snow
			r = (h-0.65f)/0.25f;
			colx[n] = r;
			coly[n] = 0.2f+r*0.8f;
			colz[n] = r;
		} else {// Snow
			r = (h-0.9f)/0.1f;
			colx[n] = 1.0f;
			coly[n] = 1.0f;
			colz[n] = 1.0f;
		}
	}
	
	// Smoothing
	for (int i=0; i<3; ++i)
		base = smooth(base, ss, s);

	// Computing min and max
	min = base[(s/2)*(s+1)];
	max = min;
	off = (s-hd-1)/2;
	for (int x=off; x<s-off-1; ++x){
		for (int z=off; z<s-off-1; ++z){
			if (base[x+z*s] < min){
				min = base[x+z*s];
			} else if (base[x+z*s] > max){
				max = base[x+z*s];
			}
		}
	}
	delta = max-min;

	// Re-scaling
	float ratio = vspan/delta;
	for (int x=off; x<s-off; ++x){
		for (int z=off; z<s-off; ++z){
			base[x+z*s] = (base[x+z*s]-min)*ratio;
		}
	}

	// Building hmap
	for (int x=0; x<gd; ++x){
		for (int z=0; z<gd; ++z){
			int idx = x+z*gd;
			hmap[idx] = 0.0f;
			for (int u=0; u<=hdgd; ++u){
				for (int v=0; v<=hdgd; ++v){
					float tmp = base[x*hdgd+u+off+(z*hdgd+v+off)*s];
					if (tmp > hmap[idx]){
							hmap[idx] = tmp;
					}
				}
			}
		}
	}

	//Building Grid
	for (int x=0; x<=gd; ++x){
		for (int z=0; z<=hd; ++z){
			grid[0][x*(hd+1)+z].set(x*gdhgap, base[(x*hdgd)+off+(z+off)*s], z*hdhgap);
			grid[1][x*(hd+1)+z].set(z*hdhgap, base[(z)+off+(x*hdgd+off)*s], x*gdhgap);
		}
	}

	// Calculating normals
	float* normx = new float[ss];
	float* normz = new float[ss];
	float size2 = hdhgap*2;
	for (int u=1; u<s-1; ++u){
		int v = s;
		for (int z=1; z<s-1; ++z, v+=s){
			normx[u+z*s] = -(base[u+1+v]-base[u-1+v])/size2;
			normz[u+z*s] = -(base[u+v+s]-base[u+v-s])/size2;
		}
	}

	// Setting gold
	ZeroMemory(gold,gd2);
	/*
	for (int x=1; x<gd-1; ++x){
		for (int z=1; z<gd-1; ++z){
			int idx = x+z*gd;
			if (rand()%1000 < 20 || (gold[idx-1-gd]+gold[idx-gd]+gold[idx-gd+1]+gold[idx-1]>=1) && rand()%1000<200){
				gold[idx] = 1;
			}
		}
	}
	*/

	// Saving values
	int tri;
	int a = off;
	int b = off;
	for (int x=0; x<hd; ++x, ++a){
		b = off*s;
		for (int z=0; z<hd; ++z, b+=s){

			//tri = x*hdz+z*6;
			tri = ((x/hdgd)*gd+(z/hdgd))*gd2pts + ((x%hdgd)*hdgd+(z%hdgd))*6;
			// Heights
			hdv[tri].pos.y = base[a+b];
			hdv[tri+1].pos.y = base[a+b+s];
			hdv[tri+2].pos.y = base[a+1+b];
			hdv[tri+3].pos.y = base[a+1+b+s];
			hdv[tri+4].pos.y = base[a+1+b];
			hdv[tri+5].pos.y = base[a+b+s];

			// Colors
			int id = a+b;
			hdv[tri].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));
			id+=1;
			hdv[tri+2].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));
			hdv[tri+4].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));
			id+=s-1;
			hdv[tri+1].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));
			hdv[tri+5].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));
			id+=1;
			hdv[tri+3].color = (0xff<<24) | ((int)(colx[id]*255.0f))<<16 | ((int)(coly[id]*255.0f))<<8 | ((int)(colz[id]*255.0f));

			// Normals
			hdv[tri].normal.x = normx[a+b];
			hdv[tri].normal.z = normz[a+b];
			hdv[tri+1].normal.x = normx[a+b+s];
			hdv[tri+1].normal.z = normz[a+b+s];
			hdv[tri+2].normal.x = normx[a+b+1];
			hdv[tri+2].normal.z = normz[a+b+1];
			hdv[tri+3].normal.x = normx[a+b+1+s];
			hdv[tri+3].normal.z = normz[a+b+1+s];
			hdv[tri+4].normal.x = hdv[tri+2].normal.x;
			hdv[tri+4].normal.z = hdv[tri+2].normal.z;
			hdv[tri+5].normal.x = hdv[tri+1].normal.x;
			hdv[tri+5].normal.z = hdv[tri+1].normal.z;
		}
	}

	for (int x=0; x<hd ; ++x){
		int z = 0;
		int tri = ((x/hdgd)*gd+(z/hdgd))*gd2pts + ((x%hdgd)*hdgd+(z%hdgd))*6;
		hdv[tri].normal.set(0,1,0);
		hdv[tri+2].normal.set(0,1,0);
		hdv[tri+4].normal.set(0,1,0);
	
		z = hd-1;
		tri = ((x/hdgd)*gd+(z/hdgd))*gd2pts + ((x%hdgd)*hdgd+(z%hdgd))*6;
		hdv[tri+1].normal.set(0,1,0);
		hdv[tri+3].normal.set(0,1,0);
		hdv[tri+5].normal.set(0,1,0);
	}
	for (int z=0; z<hd; ++z){
		int x = 0;
		int tri = ((x/hdgd)*gd+(z/hdgd))*gd2pts + ((x%hdgd)*hdgd+(z%hdgd))*6;
		hdv[tri].normal.set(0,1,0);
		hdv[tri+1].normal.set(0,1,0);
		hdv[tri+5].normal.set(0,1,0);
	
		x = hd-1;
		tri = ((x/hdgd)*gd+(z/hdgd))*gd2pts + ((x%hdgd)*hdgd+(z%hdgd))*6;
		hdv[tri+2].normal.set(0,1,0);
		hdv[tri+3].normal.set(0,1,0);
		hdv[tri+4].normal.set(0,1,0);
	}

	// Deleting data
	delete base;
	delete normx;
	delete normz;
	delete colx;
	delete coly;
	delete colz;

	Harmonize();
}
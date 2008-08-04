#ifndef __D3DDEFS_H__
#define __D3DDEFS_H__
//---------------------------------------------------------
#include "Vec3Df.h"
//---------------------------------------------------------
#define VERT	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define VERT2DC	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define VERT2DT	(D3DFVF_XYZRHW | D3DFVF_TEX1)
#define TVERT	(D3DFVF_XYZ | D3DFVF_TEX1)
#define VBUF	LPDIRECT3DVERTEXBUFFER9
#define MACRO_BTEXFF(a,b) D3DXCreateTextureFromFileExA(g_device,a,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0xff000000,NULL,NULL,b)
#define MACRO_RTEXFF(a,b) D3DXCreateTextureFromFileExA(g_device,a,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0xffff0000,NULL,NULL,b)
//---------------------------------------------------------
typedef struct s_vert2dc {
	float		x, y, z, w;
	int			color;

	void set(float _x, float _y, int c){
		x = _x; y = _y;
		color = c;
		z = 0;
		w = 1;
	};
} s_vert2dc;
//---------------------------------------------------------
typedef struct s_vert2dt {
	float		x, y, z, w;
	float		u, v;

	void set(float _x, float _y, float _u, float _v){
		x = _x; y = _y;
		u = _u; v = _v;
		z = 0;
		w = 1;
	};
} s_vert2dt;
//---------------------------------------------------------
typedef struct s_vert {
	Vec3Df	pos;
	Vec3Df	normal;
	int			color;
} s_vert;
//---------------------------------------------------------
typedef struct s_tvert {
	Vec3Df pos;
	float u, v;
} s_tvert;
//---------------------------------------------------------
#endif//__D3DDEFS_H__
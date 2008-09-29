#ifndef __MAP_H__
#define __MAP_H__
//---------------------------------------------------------
#include "d3ddefs.h"
#include "Vec3Df.h"
//---------------------------------------------------------
class Map {
public:
	int			hd;
	int			gd;
	int			hdgd;
	int			hdgd2;
	int			gd2pts;
	int			gd2tri;
	int			hd2;
	int			gd2;
	float		hdhgap;
	float		gdhgap;
	float		hspan;
	float		vspan;
	int			nbthd;
	int			hdz;
	float*	hmap;
	char*		hl;
	char*		gold;

	int			nb_hdv;
	s_vert*	hdv;
	VBUF		hdvb;

	int				nb_grid;
	Vec3Df**	grid;
	VBUF			grid_vb[2];

	bool		know_pick;
	int			nb_select;
	int			nb_select_to_render;
	Vec3Df* select;
	VBUF		select_vb;
	Vec3Df	ori;
	Vec3Df	dir;
	int			startx;
	int			startz;
	int			curx;
	int			curz;

	int			vxmin;
	int			vxmax;
	int			vzmin;
	int			vzmax;
	Vec3Df	ray_ori;
	Vec3Df	ray_dir;

	D3DXMATRIX		trans;

	Map();
	~Map();

	void InitArrays();
	void Terraforming();
	void Harmonize();
	void Point(Vec3Df& c1, Vec3Df& c2, int x, int y, float h1, float h2);
	void UpdateView();
	void Render();
	void RenderSelector();
	void RebuildSelector();
	bool Intersect(int x, int z);
	bool Intersect_triangle(Vec3Df& p0, Vec3Df& p1, Vec3Df& p2);
	void UpdateSelector(Vec3Df& ori, Vec3Df& dir);
	void RenderGrid();
	void Update(float dt);
};
//---------------------------------------------------------
#endif//__MAP_H__
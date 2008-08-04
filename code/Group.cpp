#include <math.h>
#include "d3ddefs.h"
#include "main.h"
#include "Game.h"
#include "Clan.h"
#include "Group.h"
//---------------------------------------------------------
Group::Group(int _x, int _z, double _size, Clan* _clan){
	x = _x;
	z = _z;
	size = _size;
	clan = _clan;
	stamina = clan->stamina;
	planed = 0;
	g_board[x*g_side+z] = this;
	mining = false;
}
//---------------------------------------------------------
Group::~Group(){
}
//---------------------------------------------------------
void Group::SetVizibility(){
	int pos = x*g_side+z;
	for (int px=x-3; px<=x+3; ++px){
		if (px>=0 && px<g_side){
			for (int pz=z-3; pz<=z+3; ++pz){
				if (pz>=0 && pz<g_side){
					++g_viz[px*g_side+pz];
				}
			}
		}
	}
	if (x>2 && z>2) --g_viz[pos-3*(g_side+1)];
	if (x>2 && z+3<g_side) --g_viz[pos-3*g_side+3];
	if (x+3<g_side && z+3<g_side) --g_viz[pos+3*(g_side+1)];
	if (x+3<g_side && z>2) --g_viz[pos+3*g_side-3];
}
//---------------------------------------------------------
void Group::ResetVizibility(){
int pos = x*g_side+z;
	for (int px=x-3; px<=x+3; ++px){
		if (px>=0 && px<g_side){
			for (int pz=z-3; pz<=z+3; ++pz){
				if (pz>=0 && pz<g_side){
					--g_viz[px*g_side+pz];
				}
			}
		}
	}
	if (x>2 && z>2) ++g_viz[pos-3*(g_side+1)];
	if (x>2 && z+3<g_side) ++g_viz[pos-3*g_side+3];
	if (x+3<g_side && z+3<g_side) ++g_viz[pos+3*(g_side+1)];
	if (x+3<g_side && z>2) ++g_viz[pos+3*g_side-3];
}
//---------------------------------------------------------
void Group::MoveTo(int _x, int _z){
	mining = false;
	ResetVizibility();
	x = _x;
	z = _z;
	SetVizibility();
}
//---------------------------------------------------------
void Group::MarkPosition(){
	g_board[x*g_side+z] = this;
}
//---------------------------------------------------------
void Group::Turn(){
	stamina = clan->stamina;
}
//---------------------------------------------------------
void Group::Subgroup(double subsize){
	clan->AddGroup(x, z, size-subsize);
	clan->groups[clan->nbGroups-1]->stamina = stamina;
	size = subsize;
}
//---------------------------------------------------------
void Group::RenderSelection(){
	g_game.SetTempoTranslate(g_map->gdhgap*x,
		g_map->hmap[x+z*g_map->gd],
		g_map->gdhgap*z);

	g_device->SetStreamSource(0,g_game.square_vb,0,sizeof(Vec3Df));
	g_device->DrawPrimitive(D3DPT_LINESTRIP,0,4);
}
//---------------------------------------------------------
void Group::Render(){
	g_game.SetTempoTranslate(g_map->gdhgap*x,
		g_map->hmap[x+z*g_map->gd],
		g_map->gdhgap*z);

	g_device->SetStreamSource(0,g_game.quad_vb,0,sizeof(s_tvert));
	if (mining){
		g_device->SetTexture(0,g_game.mine_tex);
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		g_device->SetTexture(0,g_game.quad_tex);
	} else {
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	}
}
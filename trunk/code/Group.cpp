#include <math.h>
#include "d3ddefs.h"
#include "sidefunctions.h"
#include "main.h"
#include "Game.h"
#include "Map.h"
#include <cassert>
#include "Hud.h"
#include "Clan.h"
#include "Group.h"
//---------------------------------------------------------
Group::Group(int _id, int _x, int _z, int _size, Clan* _clan){
	id = _id;
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
	g_board[x*g_side+z] = 0;
	if (clan == g_clan){
		for (int i=g_nbSelected-1; i>=0; --i){
			if (g_selected[i] == this){
				g_selected[i] = g_selected[--g_nbSelected];
				break;
			}
		}
		ResetVizibility();
	}
	clan = 0;
}
//---------------------------------------------------------
void Group::StartMining(){
	if (mining) return;
	mining = true;
	clan->goldintake += size>=50 ? 250 : size*5;
	sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) started mining\n", clan->name,id,size,x,z);	Log();
}
//---------------------------------------------------------
void Group::StopMining(){
	if (!mining) return;
	mining = false;
	clan->goldintake -= size>=50 ? 250 : size*5;
	sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) stopped mining\n", clan->name,id,size,x,z);	Log();
}
//---------------------------------------------------------
void Group::Combat(Group& target){
	int apow;	int dpow; int bodycount;
	sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) vs %s[%d]{%d}(%d,%d)\n", clan->name,id,size,x,z, target.clan->name,target.id,target.size,target.x,target.z);	Log();
	assert(target.clan != clan);
	assert(target.x == target.clan->groups[target.id]->x);
	assert(target.z == target.clan->groups[target.id]->z);
	apow = size * clan->attack;
	dpow = target.size * target.clan->defense;
	bodycount = int((apow/(float)dpow)*target.size*0.25f);
	if (bodycount < 0) bodycount = 0;
	if (bodycount+1 >= target.size) bodycount = target.size;
	int gc = (target.clan->gold*bodycount)/target.clan->size;
	target.clan->gold -= gc;
	clan->gold += gc;
	target.Kill(bodycount);
	if (target.size <= 0) target.clan->KillGroup(target.id);
}
//---------------------------------------------------------
void Group::Kill(int bodycount){
	sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) lost %d units\n", clan->name,id,size,x,z, bodycount);	Log();
	if (mining){
		clan->goldintake -= size>=50 ? 250 : size*5;
	}
	g_pop -= bodycount;
	clan->size -= bodycount;
	for (int i=clan->nbAllies-1; i>=0; --i){
		clan->allies[i]->alliancePop -= bodycount;
	}
	clan->alliancePop -= bodycount;
	if (bodycount >= size){
		size = 0;
	} else {
		if (mining){
			clan->goldintake += size>=50 ? 250 : size*5;
		}
		size -= bodycount;
	}
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
	StopMining();
	ResetVizibility();
	if (g_board[x*g_side+z] == this){
		g_board[x*g_side+z] = 0;
	}
	x = _x;
	z = _z;
	g_board[x*g_side+z] = this;
	SetVizibility();
}
//---------------------------------------------------------
void Group::AiMoveTo(int _x, int _z){
	//sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) -> (%d,%d)\n", clan->name,id,size,x,z, _x,_z);	Log();
	StopMining();
	if (g_board[x*g_side+z] == this) {
		g_board[x*g_side+z] = 0;
	}
	x = _x;
	z = _z;
	g_board[x*g_side+z] = this;
}
//---------------------------------------------------------
bool Group::IsNextTo(Clan *c){
	if (x>0 && g_board[(x-1)*g_side+z] && g_board[(x-1)*g_side+z]->clan==c) return (true);
	if (z>0 && g_board[x*g_side+z-1] && g_board[x*g_side+z-1]->clan==c) return (true);
	if (x+1<g_side && g_board[(x+1)*g_side+z] && g_board[(x+1)*g_side+z]->clan==c) return (true);
	if (z+1<g_side && g_board[x*g_side+z+1] && g_board[x*g_side+z+1]->clan==c) return (true);
	if (x>0 && z>0 && g_board[(x-1)*g_side+z-1] && g_board[(x-1)*g_side+z-1]->clan==c) return (true);
	if (x>0 && z+1<g_side && g_board[(x-1)*g_side+z+1] && g_board[(x-1)*g_side+z+1]->clan==c) return (true);
	if (x+1<g_side && z>0 && g_board[(x+1)*g_side+z-1] && g_board[(x+1)*g_side+z-1]->clan==c) return (true);
	if (x+1<g_side && z+1<g_side && g_board[(x+1)*g_side+z+1] && g_board[(x+1)*g_side+z+1]->clan==c) return (true);
	return (false);
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
void Group::HeadFor(int hx, int hz){
	int cx = x;	int cz = z;
	int safex = x;	int safez = z;
	int dx = hx-cx;	int dz = hz-cz;
	int ix = (dx==0 ? 0 : (dx>0 ? 1 : -1));
	int iz = (dz==0 ? 0 : (dz>0 ? 1 : -1));
	if (ix==0){
		while (cz!=hz){
			cz+=iz;
			if (stamina < planed+distFromManhattan(cx-x,cz-z)){
				cz-=iz;
				break;
			}
			if (!g_board[cx*g_side+cz] || g_board[cx*g_side+cz]->clan==clan){
				safex = cx; safez = cz;
			}
		}
	} else if (iz==0){
		while (cx!=hx){
			cx+=ix;
			if (stamina < planed+distFromManhattan(cx-x,cz-z)){
				cx-=ix;
				break;
			}
			if (!g_board[cx*g_side+cz] || g_board[cx*g_side+cz]->clan==clan){
				safex = cx; safez = cz;
			}
		}
	} else {
		dx *= ix; dz *= iz;
		float ax = 0.5f; float az = 0.5f; 
		while (cx!=hx || cz!=hz){
			float kx = (1.0f-ax)/dx;
			float kz = (1.0f-az)/dz;
			if (kx < kz){
				cx += ix;
				if (stamina < planed+distFromManhattan(cx-x,cz-z)){
					cx-=ix;
					break;
				}
				if (!g_board[cx*g_side+cz] || g_board[cx*g_side+cz]->clan==clan){
					safex = cx; safez = cz;
				}
				ax = 0.0f;
				az += kx*dz;
			} else if (kz < kx){
				cz += iz;
				if (stamina < planed+distFromManhattan(cx-x,cz-z)){
					cz-=iz;
					break;
				}
				if (!g_board[cx*g_side+cz] || g_board[cx*g_side+cz]->clan==clan){
					safex = cx; safez = cz;
				}
				az = 0.0f;
				ax += kz*dx;
			} else {
				cx += ix;
				cz += iz;
				if (stamina < planed+distFromManhattan(cx-x,cz-z)){
					cx-=ix;
					cz-=iz;
					break;
				}
				if (!g_board[cx*g_side+cz] || g_board[cx*g_side+cz]->clan==clan){
					safex = cx; safez = cz;
				}
				ax = 0.0f;
				az = 0.0f;
			}
		}
	}
	stamina -= distFromManhattan(safex-x,safez-z);
	sprintf(logstr, "\t\t\t- Group %s[%d]{%d}(%d,%d) -> (%d,%d) >> (%d,%d)\n", clan->name,id,size,x,z,safex,safez,hx,hz);	Log();
	AiMoveTo(safex, safez);
}
//---------------------------------------------------------
int Group::GetNextCell(int hx, int hz){
	int dx = hx-x;	int dz = hz-z;
	int ix = (dx==0 ? 0 : (dx>0 ? 1 : -1));
	int iz = (dz==0 ? 0 : (dz>0 ? 1 : -1));
	dx *= ix; dz *= iz;
	if (dx > dz){
		return ((x+ix)*g_side+z);
	} else if (dz > dx){
		return (x*g_side+z+iz);
	} else {
		return ((x+ix)*g_side+z+iz);
	}
}
//---------------------------------------------------------
void Group::Subgroup(int subsize){
	clan->AddGroup(id+1, x, z, size-subsize, stamina);
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
	if (g_viz[x*g_side+z]==0) return;
	g_game.SetTempoTranslate(g_map->gdhgap*x,
		g_map->hmap[x+z*g_map->gd],
		g_map->gdhgap*z);

	g_device->SetMaterial(&clan->mat);
	g_device->SetStreamSource(0,g_game.quad_vb,0,sizeof(s_tvert));
	if (mining){
		g_device->SetTexture(0,g_game.mine_tex);
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		g_device->SetTexture(0,g_game.quad_tex);
	} else {
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	}
}
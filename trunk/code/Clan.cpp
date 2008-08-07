#include "main.h"
#include "Clan.h"
#include "Group.h"
#include "Names.h"
//---------------------------------------------------------
Clan::Clan(int _id, int x, int z, double _size){
	id = _id;
	name = g_names[id];
	size = _size;
	nbFedClans = 1;
	fid = 0;
	fed = new Clan*[1];
	fed[0] = this;
	nbGroups = 1;
	gold = 100;
	stamina = STAMINA+100;
	attack = ATTACK;
	defense = DEFENSE;
	culture = CULTURE;
	groups = new Group*[nbGroups];
	groups[0] = new Group(0, x, z, size, this);
}
//---------------------------------------------------------
Clan::~Clan(){
	for (int i=0; i<nbFedClans; ++i){
		--fed[i]->nbFedClans;
	}
	fed[nbFedClans]->fid = fid;
	fed[fid] = fed[nbFedClans];
	if (nbFedClans==0) {
		delete fed;
	}
	for (int i=nbGroups-1; i>=0; --i){
		delete groups[i];
	}
	delete groups;
}
//---------------------------------------------------------
void Clan::KillGroup(int gid){
	delete groups[gid];
	groups[gid] = groups[--nbGroups];
	if (nbGroups == 0){
		g_clans[id] = g_clans[--g_nbClans];
		g_hud->UpdateText();
		delete this;
	}
}
//---------------------------------------------------------
void Clan::SetVizibility(){
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->SetVizibility();
	}
}
//---------------------------------------------------------
void Clan::AddGroup(int x, int z, double _size){
	nbGroups += 1;
	Group** ngs = new Group*[nbGroups];
	for (int i=nbGroups-2; i>=0; --i) ngs[i] = groups[i];
	groups = ngs;
	groups[nbGroups-1] = new Group(nbGroups-1, x, z, _size, this);
	groups[nbGroups-1]->SetVizibility();
}
//---------------------------------------------------------
void Clan::MergeGroups(Group* g1, Group* g2){
	int i1, i2;
	for (i1=0; groups[i1]!=g1; ++i1);
	for (i2=0; groups[i2]!=g2; ++i2);
	g1->stamina = ((g1->stamina*g1->size) + (g2->stamina*g2->size)) / (g1->size+g2->size);
	g1->size += g2->size;
	delete g2;
	groups[i2] = groups[--nbGroups];
	g_board[g1->x*g_side+g1->z] = g1;
	for (i1=0; i1<g_nbSelected && g_selected[i1]!=g1; ++i1);
	for (i2=0; i2<g_nbSelected && g_selected[i2]!=g2; ++i2);
	if (i1<g_nbSelected){
		g_selected[i1] = g1;
		if (i2<g_nbSelected){
			g_selected[i2] = g_selected[--g_nbSelected];
		}
	} else if (i2<g_nbSelected){
		g_selected[i2] = g1;
	}
}
//---------------------------------------------------------
void Clan::MarkGroupsPositions(){
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->MarkPosition();
	}
}
//---------------------------------------------------------
void Clan::Turn(){
	double newSize = 0;
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->Turn();
		newSize += groups[i]->size;
	}
	size = newSize;
}
//---------------------------------------------------------
void Clan::Render(){
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->Render();
	}
}
#include <cassert>
#include <math.h>
#include "main.h"
#include "sidefunctions.h"
#include "Clan.h"
#include "Hud.h"
#include "Group.h"
#include "names.h"
#include "colors.h"
#include "Event.h"
//---------------------------------------------------------
Clan::Clan(int _id, int x, int z, int _size){
	alive = true;
	id = _id;
	name = g_names[id];
	ZeroMemory(&mat,sizeof(mat));
	mat.Emissive.r = g_colors[id][0];
	mat.Emissive.g = g_colors[id][1];
	mat.Emissive.b = g_colors[id][2];
	size = _size;
	allies = NULL;
	alliancePop = size;
	nbAllies = 0;
	nbGroups = 1;
	gold = 20000;
	myMines = 0;
	goldintake = 0;
	stamina = 2.9;
	attack = 1;
	defense = 1;
	culture = 1;
	groups = new Group*[nbGroups];
	groups[0] = new Group(0, x, z, size, this);
	temper = 50.0f;
}
//---------------------------------------------------------
Clan::~Clan(){
	if (alive) KillClan();
}
//---------------------------------------------------------
void Clan::UpgradeStamina(){
	++stamina;
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->stamina += 1;
	}
}
//---------------------------------------------------------
int Clan::AllianceCost(const Clan& c){
	int price;
	if (g_stances[id][c.id] >= 50.0f){
		price = 0;
	} else {
		price = int((float)c.gold*((50.0f-g_stances[id][c.id])/150.0f));
	}
	sprintf(logstr, "\t\t\t- Price of alliance from %s to %s: %d\n", c.name, name, price); Log();
	return (price);
}
//---------------------------------------------------------
void Clan::DeadAlly(Clan* c){
	g_allies[id][c->id] = 0;
	if (nbAllies == 1){
		delete allies;
		nbAllies = 0;
		return;
	}
	for (int i=nbAllies-1; i>=0; --i){
		if (allies[i] == c){
			allies[i] = allies[--nbAllies];
			break;
		}
	}
}
//---------------------------------------------------------
void Clan::AddAlly(Clan* c){
	g_allies[id][c->id] = 1;
	alliancePop += c->size;
	Clan** newallies = new Clan*[nbAllies+1];
	for (int i=nbAllies-1; i>=0; --i){
		newallies[i] = allies[i];
	}
	if (nbAllies){
		delete allies;
	}
	newallies[nbAllies++] = c;
	allies = newallies;
}
//---------------------------------------------------------
void Clan::RemoveAlly(Clan* c){
	sprintf(logstr, "- %s no longer sees %s as an ally (%3.1f)\n", name, c->name, g_stances[id][c->id]); Log();
	assert(g_allies[id][c->id] == 1);
	g_allies[id][c->id] = 0;
	alliancePop -= c->size;
	for (int i=0; i<nbAllies; ++i){
		if (allies[i] == c){
			allies[i] = allies[--nbAllies];
			break;
		}
	}
}
//---------------------------------------------------------
void Clan::KillClan(){
	--g_nbAliveClans;
	alive = false;
	g_pop -= size;
	if (nbAllies){
		for (int i=nbAllies-1; i>=0; --i){
			allies[i]->alliancePop -= size;
			allies[i]->DeadAlly(this);
		}
		delete allies;
		nbAllies = 0;
	}
	alliancePop = 0;
	size = 0;
	for (int i=nbGroups-1; i>=0; --i){
		delete groups[i];
	}
	if (nbGroups)
		delete groups;
	nbGroups = 0;
}
//---------------------------------------------------------
bool Clan::IsNextTo(Clan* c){
	if (nbGroups > c->nbGroups) return (c->IsNextTo(this));
	for (int i=0; i<nbGroups; ++i){
		if (groups[i]->IsNextTo(c)) return (true);
	}
	return (false);
}
//---------------------------------------------------------
bool Clan::ReceiveAllianceOffer(const Clan& c, int bribe){
	/*EVENT*/Event(c, *this, ET_Offer);
	if (bribe >= AllianceCost(c)){
		return true;
	}
	return (g_stances[id][c.id] >= 50.0f);
}
//---------------------------------------------------------
bool Clan::ReceivePeaceOffer(const Clan& c){
	/*EVENT*/Event(c, *this, ET_Offer);
	return (g_peacewill[id][c.id] > g_belligerence[id][c.id]);
}
//---------------------------------------------------------
bool Clan::ReceiveGoldOffering(int amount, Clan& donor){
	/*EVENT*/Event(donor, *this, ET_Offer);
	gold += amount;
	donor.gold -= amount;
	return (true);
}
//---------------------------------------------------------
void Clan::KillGroup(int gid){
	Group* g = groups[gid];
	for (int i=gid+1; i<nbGroups; ++i){
		groups[i-1] = groups[i];
		groups[i-1]->id = i-1;
	}
	--nbGroups;
	delete g;
	if (nbGroups == 0){
		KillClan();
	}
}
//---------------------------------------------------------
void Clan::SetVizibility(){
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->SetVizibility();
	}
}
//---------------------------------------------------------
void Clan::AddGroup(int x, int z, int _size){
	nbGroups += 1;
	Group** ngs = new Group*[nbGroups];
	for (int i=nbGroups-2; i>=0; --i) ngs[i] = groups[i];
	groups = ngs;
	groups[nbGroups-1] = new Group(nbGroups-1, x, z, _size, this);
	if (this == g_clan)
		groups[nbGroups-1]->SetVizibility();
}
//---------------------------------------------------------
void Clan::AddGroup(int _id, int x, int z, int _size, double _stam){
	DeplantGroups();
	Group** ngs = new Group*[nbGroups+1];
	for (int i=0; i<_id; ++i) ngs[i] = groups[i];
	for (int i=_id; i<nbGroups; ++i) {
		ngs[i+1] = groups[i];
		ngs[i+1]->id = i+1;
	}
	++nbGroups;
	groups = ngs;
	groups[_id] = new Group(_id, x, z, _size, this);
	groups[_id]->stamina = _stam;
	if (this == g_clan)
		groups[_id]->SetVizibility();
	ImplantGroups();
}
//---------------------------------------------------------
void Clan::MergeGroups(Group* g1, Group* g2){
	int i1 = g1->id;
	int i2 = g2->id;
	DeplantGroups();

	assert(g1!=g2);
	assert(g1->clan==g2->clan);
	assert(i1<nbGroups);
	assert(i2<nbGroups);

	g1->stamina = ((g1->stamina*g1->size) + (g2->stamina*g2->size)) / (g1->size+g2->size);
	if (g1->mining || g2->mining){
		g1->StopMining();
		g2->StopMining();
		g1->size += g2->size;
		g1->StartMining();
	} else {
		g1->size += g2->size;
	}
	for (int i=i2+1; i<nbGroups; ++i){
		groups[i-1] = groups[i];
		groups[i-1]->id = i-1;
	}
	--nbGroups;
	delete g2;
	ImplantGroups();

	assert(groups[g1->id] == g1);
	for (int i=0; i<nbGroups; ++i){
		assert(groups[i]->id == i);
	}

	if (g1->clan != g_clan) return;
	for (i1=0; i1<g_nbSelected && g_selected[i1]!=g1; ++i1);
	for (i2=0; i2<g_nbSelected && g_selected[i2]!=g2; ++i2);
	g_selectedpop = g1->size;
	if (i1<g_nbSelected){
		g_selected[i1] = g1;
		if (i2<g_nbSelected){
			g_selected[i2] = g_selected[--g_nbSelected];
		}
	} else if (i2<g_nbSelected){
		g_selected[i2] = g1;
	}
	g_hud->UpdateSelect();
}
//---------------------------------------------------------
void Clan::MarkGroupsPositions(){
	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->MarkPosition();
	}
}
//---------------------------------------------------------
void Clan::Turn(){
	//if (this == g_clan)	return FinishTurn();

#ifdef DEBUG
	CheckBoardConsistency();
#endif///DEBUG

	//sprintf(logstr, "%s begins it's turn with %d gold and %d units\n", name, gold, size); Log();
	// Get more allies
	float score=-1000, bestscore=0;
	Clan* bestclan = NULL;
	while (true){
		int savings = (goldintake-size)>0 ? gold : gold+(goldintake-size)*10;
		bestclan = NULL;
		for (int i=g_nbClans-1; i>=0; --i){
			if (g_allies[id][i] || i==id || g_clans[i]->alive==false) continue;
			score = g_stances[id][i];
			if (score>bestscore) {
				bestscore = score;
				bestclan = g_clans[i];
			}
		}
		if (bestclan==NULL) break;
		int price = bestclan->AllianceCost(*this);
		if (price==0 || price<=savings){
			//sprintf(logstr, "\t- Offering alliance to %s clan for %d gold\n", bestclan->name, price); Log();
			if (bestclan->ReceiveAllianceOffer(*this, price)){
				/*EVENT*/Event(*bestclan, *this, ET_AcceptAllianceOffer);
			//	Log("\t\t- Alliance accepted\n");
				gold -= price;
				bestclan->gold += price;
				AddAlly(bestclan);
				bestclan->AddAlly(this);
			} else {
			//	Log("\t\t- Alliance rejected\n");
				/*EVENT*/Event(*bestclan, *this, ET_TurnDownOffer);
			}
		}
	}

	// Update the number of mines owned
	myMines = 0;
	g_nbFreeMines = 0;
	for (int i=0; i<g_nbMines; ++i) {
		Group* g = g_board[g_mines[i][0]*g_side+g_mines[i][1]];
		if (g){
			if (g->clan==this && g->mining) ++myMines;
		} else {
			++g_nbFreeMines;
		}
	}
	//sprintf(logstr, "\t- Mines owned: %d/%d gold:%d intake:%d size:%d\n", myMines, g_nbMines, gold, goldintake, size); Log();

	// Clan Growth
	if (g_nbFreeMines){
		//sprintf(logstr, "\t- Going for free mines (%d)\n", g_nbFreeMines); Log();
		RebalanceMineExploitation();
	} else {
		if (goldintake > size) {
			int future = (myMines==g_nbMines) ? 20 : 10;
			if (g_nbAliveClans>1 && gold-size*future>0) {
				//Log("\t- Going to war by choice\n");
				TotalWar();
			} else {
				//Log("\t- Sotcking on gold\n");
				if (CheckForUnbalancedMineExploitation()){
				//	Log("\t\t- Rebalancing exploitation\n");
					RebalanceMineExploitation();
				}
			}
		} else {
			if (CheckForMineUnderExploitation()){
			//	Log("\t- Going for under-exploited mines\n");
				RebalanceMineExploitation();
			} else if (g_nbAliveClans>1){
			//	Log("\t- Going to war for survival\n");
				SendSurplusToWar();
			} else {
			//	Log("\t- Slowly dying with nothing to do\n");
			}
		}
	}
	MergeGroups();
	ImplantGroups();
	FinishTurn();
#ifdef DEBUG
	CheckBoardConsistency();
#endif///DEBUG
	//sprintf(logstr, "%s ends it's turn with %d gold and %d units\n\n", name, gold, size); Log();
}
//---------------------------------------------------------
void Clan::AttackTarget(Group& target){
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group& g = *groups[igroup];
		if (g.mining) continue;
		int cx = g.x;
		int cz = g.z;
		g.HeadFor(target.x,target.z);
		bool fight = false;
		if (target.x<=g.x+1 && target.x>=g.x-1 && target.z<=g.z+1 && target.z>=g.z-1){
			// Next to the target
			g.Combat(target);
		} else if (g.x==cx && g.z==cz){
			// Something is blocking us
			int idx = g.GetNextCell(target.x,target.z);
			assert(g_board[idx]);
			g.Combat(*g_board[idx]);
		}
	}
}
//---------------------------------------------------------
void Clan::TotalWar(){
	int nbFreeGroups = 0;
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group& g = *groups[igroup];
		if (g.mining) {
			if (g.size>1){
				g.Subgroup(1);
				g_board[g.x*g_side+g.z] = groups[igroup];	
			}
		} else {
			++nbFreeGroups;
		}
	}

	if (nbFreeGroups==0) return;

	float bestc;
	Clan* ctarget = NULL;
	for (int iClan=g_nbClans-1; iClan>=0; --iClan){
		Clan* c = g_clans[iClan];
		if (c==this || c->alive==false) continue;
		if (ctarget==NULL || g_stances[id][c->id]<bestc){
			bestc = g_stances[id][c->id];
			ctarget = c;
		}
	}
	assert(ctarget);
	int bestg;
	Group* target = NULL;
	for (int iGroup=ctarget->nbGroups-1; iGroup>=0; --iGroup){
		Group* g = ctarget->groups[iGroup];
		if (target==NULL || bestg>g->size){
			bestg = g->size;
			target = g;
		}
	}

	assert(target);
	if (nbFreeGroups>1) {
		RegroupNonMiningGroups(target);
	} else {
		AttackTarget(*target);
	}
}
//---------------------------------------------------------
void Clan::RegroupNonMiningGroups(Group* target){
	assert(target);
	Group* big = NULL;
	float dis = g_side*g_side*4.0f;
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group* g = groups[igroup];
		float tmpdist = sqdist1D(g->x,target->x)+sqdist1D(g->z,target->z);
		if (tmpdist < dis){
			big = g;
			dis = tmpdist;
		}
	}
	assert(big);

	for (int igroup=0; igroup<nbGroups; ++igroup){
		if (groups[igroup]->mining == false){
			Group& g = *groups[igroup];
			int cx = g.x;
			int cz = g.z;
			groups[igroup]->HeadFor(big->x,big->z);
			if (g.x==cx && g.z==cz && g.x!=big->x && g.z!=big->z){
				// Something is blocking us
				int idx = g.GetNextCell(big->x,big->z);
				assert(g_board[idx]);
				g.Combat(*g_board[idx]);
			}
		}
	}
}
//---------------------------------------------------------
void Clan::SendSurplusToWar(){
	int nbFreeGroups = 0;
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group& g = *groups[igroup];
		if (g.mining) {
			if (g.size>50){
				g.Subgroup(50);
				g_board[g.x*g_side+g.z] = groups[igroup];	
			}
		} else {
			++nbFreeGroups;
		}
	}

	if (nbFreeGroups==0) return TotalWar();

	Group* target = NULL;
	for (int i=g_nbMines-1; i>=0; --i){
		int xm = g_mines[i][0];
		int zm = g_mines[i][1];
		Group* mg = g_board[xm*g_side+zm];
		assert(mg);
		if (mg->clan!=this && (target==NULL || g_stances[id][mg->id]<g_stances[id][target->id] || (target->clan==mg->clan || target->size>mg->size))){
			target = mg;
		}
	}
	if (!target) return;
	if (nbFreeGroups>1) {
		RegroupNonMiningGroups(target);
	} else {
		AttackTarget(*target);
	}
}
//---------------------------------------------------------
bool Clan::CheckForMineUnderExploitation(){
	bool idle = false;
	bool under = false;
	for (int i=nbGroups-1; i>=0; --i){
		if (groups[i]->mining==false){
			idle = true;
		} else if (groups[i]->size<50){
			under = true;
		}
	}
	return (idle && under);
}
//---------------------------------------------------------
bool Clan::CheckForUnbalancedMineExploitation(){
	if (g_nbFreeMines+myMines==0) return false;
	int slice = size/(g_nbFreeMines+myMines);
	bool idle = false;
	for (int i=nbGroups-1; i>=0; --i){
		if (groups[i]->mining==false || groups[i]->size>slice+1){
			idle = true;
			break;
		}
	}
	return (idle);
}
//---------------------------------------------------------
void Clan::ImplantGroups(){
	for (int i=0; i<nbGroups; ++i){
		Group* g = groups[i];
		g_board[g->x*g_side+g->z] = g;
	}
}
//---------------------------------------------------------
void Clan::DeplantGroups(){
	for (int i=0; i<nbGroups; ++i){
		Group* g = groups[i];
		g_board[g->x*g_side+g->z] = 0;
	}
}
//---------------------------------------------------------
void Clan::MergeGroups(){
	bool redo = true;
	while (redo){
		redo = false;
		for (int i=0; redo==false && i<nbGroups; ++i){
			Group* g = groups[i];
			int idx = g->x*g_side+g->z;
			if (g_board[idx] && g_board[idx]!=g){
				MergeGroups(g, g_board[idx]);
				redo = true;
			}
		}
	}
	ImplantGroups();
}
//---------------------------------------------------------
void Clan::RebalanceMineExploitation(){
	assert(g_nbFreeMines+myMines!=0);
	int slice = size/(g_nbFreeMines+myMines);
	if (size/(float)(g_nbFreeMines+myMines)-slice > 0.0f){
		++slice;
	}
	assert(slice>0 && slice<=size);
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group& g = *groups[igroup];
		if (g.mining && g.size>slice){
			g.Subgroup(slice);
		}
	}
	for (int igroup=0; igroup<nbGroups; ++igroup){
		Group& g = *groups[igroup];
		if (g.mining) {
			assert(g.size <= slice);
			g_board[g.x*g_side+g.z] = groups[igroup];
		}
	}

	int igroup = 0;
	for (int i=g_nbMines-1; i>=0; --i){
		int xm = g_mines[i][0];
		int zm = g_mines[i][1];
		Group* mg = g_board[xm*g_side+zm];
		int toSend = slice;
		if (mg){
			if (mg->clan!=this) continue;
			if (mg->mining){
				assert(toSend >= mg->size);
				toSend -= mg->size;
			}
		}
		for (; toSend && igroup<nbGroups; ++igroup){
			Group& g = *groups[igroup];
			if (g.mining) continue;
			if (g.size > toSend){
				assert(toSend>0 && toSend<g.size);
				g.Subgroup(toSend);
			}
			toSend -= g.size;
			g.HeadFor(xm, zm);
			if (g.x==xm && g.z==zm){
				g.StartMining();
			}
		}
	}
}
//---------------------------------------------------------
void Clan::FinishTurn(){
	int newSize = 0;
	int goldin = goldintake;
	int goldout = size;
	int starve = 0;

	for (int i=nbGroups-1; i>=0; --i){
		groups[i]->Turn();
	}

	// Gold Intake
	gold += goldin;	

	// Gold expenditures
	gold -= goldout;
	if (gold < 0){
		// Starvation
		int duegold = -gold;
		sprintf(logstr, "\t- Starvation: lacking %d gold\n", duegold); Log();
		gold = 0;
		if (duegold >= size) {
			KillClan();
			return;
		} else {
			int oldsize = size;
			for (int i=0; i<nbGroups; ++i){
				groups[i]->Kill(0,(duegold*groups[i]->size)/oldsize);
			}
			for (int i=0; i<nbGroups; ++i){
				if (groups[i]->size == 0){
					KillGroup(i);
					--i;
				}
			}
		}
	}
	for (int i=nbGroups-1; i>=0; --i){
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
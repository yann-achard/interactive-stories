#include "Federation.h"
#include "Clan.h"
//---------------------------------------------------------
extern int g_nbFeds;
//---------------------------------------------------------
Federation::Federation(Clan *clan){
	++g_nbFeds;
	id = clan->id;
	pop = clan->size;
	nb = 1;
	clans = new Clan*[1];
	clans[0] = clan;
}
//---------------------------------------------------------
Federation::~Federation(){
	delete clans;
}
//---------------------------------------------------------
void Federation::AddClan(Clan* clan){
	pop += clan->size;
	++nb;
	Clan** newclans = new Clan*[nb];
	for (int i=nb-2; i>=0; --i){
		newclans[i] = clans[i];
	}
	newclans[nb-1] = clan;
	clans = newclans;
	clan->fed = this;
	clan->fid = nb-1;
}
//---------------------------------------------------------
void Federation::RemoveClan(int cid){
	pop -= clans[cid]->size;
	clans[cid] = clans[--nb];
	clans[cid]->fid = cid;
}
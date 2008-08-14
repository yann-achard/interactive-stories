#ifndef __CLAN_H__
#define __CLAN_H__
//---------------------------------------------------------
#include "Vec3Df.h"
//---------------------------------------------------------
class Group;
class Federation;
//---------------------------------------------------------
class Clan {
public:
	int					id;
	char*				name;
	double			size;
	double			gold;
	int					nbGroups;
	Group**			groups;

	double			stamina;
	double			attack;
	double			defense;
	double			culture;

	Federation*	fed;
	int					fid;

	float				temper;
	float				d1temper;
	float				d2temper;
	float				pacifism;

	Clan(int _id, int x, int z, double _size);
	~Clan();

	void KillClan();
	void UpgradeStamina();
	bool IsNextTo(Clan* c);
	void AddToFederation(Clan* c);
	bool FederateVote(Clan* c, char* res);
	bool RecieveFederationOffer(Clan* c);
	bool RecievePeaceOffer(Clan* c);
	bool RecieveGoldOffering(double amount, Clan* donor);
	void KillGroup(int gid);
	void AddGroup(int x, int z, double _size);
	void MergeGroups(Group* g1, Group* g2);
	void MarkGroupsPositions();
	void SetVizibility();
	void Render();
	void Turn();
};
//---------------------------------------------------------
#endif//__CLAN_H__

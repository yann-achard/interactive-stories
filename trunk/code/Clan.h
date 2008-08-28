#ifndef __CLAN_H__
#define __CLAN_H__
//---------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include "Vec3Df.h"
//---------------------------------------------------------
class Group;
//---------------------------------------------------------
class Clan {
public:
	bool					alive;
	int						id;
	char*					name;
	D3DMATERIAL9	mat;
	int						size;
	int						gold;
	int						myMines;
	int						goldintake;
	int						nbGroups;
	Group**				groups;

	double				stamina;
	int						attack;
	int						defense;
	int						culture;

	Clan**				allies;
	int						nbAllies;
	int						alliancePop;

	float					temper;
	float					d1temper;
	float					d2temper;

	Clan(){};
	Clan(int _id, int x, int z, int _size);
	~Clan();

	void	KillClan();
	void	AddAlly(Clan* c);
	void	RemoveAlly(Clan* c);
	void	DeadAlly(Clan* c);
	void	UpgradeStamina();
	bool	IsNextTo(Clan* c);
	int		AllianceCost(const Clan& c);
	bool	ReceiveAllianceOffer(const Clan& c, int bribe);
	bool	ReceivePeaceOffer(const Clan& c);
	bool	ReceiveGoldOffering(int amount, Clan& donor);
	void	KillGroup(int gid);
	void	AddGroup(int x, int z, int _size);
	void	AddGroup(int _id, int x, int z, int _size, double _stam);
	void	ImplantGroups();
	void	DeplantGroups();
	void	MergeGroups();
	void	MergeGroups(Group* g1, Group* g2);
	void	MarkGroupsPositions();
	void	SetVizibility();
	void	Render();
	void	SendSurplusToWar();
	void	TotalWar();
	void	RegroupNonMiningGroups(Group* target);
	bool	CheckForUnbalancedMineExploitation();
	bool	CheckForMineUnderExploitation();
	void	RebalanceMineExploitation();
	void	AttackTarget(Group& target);
	void	Turn();
	void	FinishTurn();
};
//---------------------------------------------------------
#endif//__CLAN_H__

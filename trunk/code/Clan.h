#ifndef __CLAN_H__
#define __CLAN_H__
//---------------------------------------------------------
class Group;
//---------------------------------------------------------
#define STAMINA	1.0f
#define ATTACK	1.0f
#define DEFENSE	1.0f
#define CULTURE	1.0f
//---------------------------------------------------------
class Clan {
public:
	int			id;
	char*		name;
	double	size;
	double	gold;
	int			nbGroups;
	Group**	groups;

	double	stamina;
	double	attack;
	double	defense;
	double	culture;

	int			nbFedClans;
	Clan**	fed;
	int			fid;


	Clan(int _id, int x, int z, double _size);
	~Clan();
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

#ifndef __CLAN_H__
#define __CLAN_H__
//---------------------------------------------------------
class Group;
//---------------------------------------------------------
#define STAMINA	10.0f
#define ATTACK	10.0f
#define DEFENSE	10.0f
#define CULTURE	10.0f
//---------------------------------------------------------
class Clan {
public:
	double	size;
	double	gold;
	int			nbGroups;
	Group**	groups;

	double	stamina;
	double	attack;
	double	defense;
	double	culture;


	Clan(int x, int z, double _size);
	~Clan();
	void AddGroup(int x, int z, double _size);
	void MergeGroups(Group* g1, Group* g2);
	void MarkGroupsPositions();
	void SetVizibility();
	void Render();
	void Turn();
};
//---------------------------------------------------------
#endif//__CLAN_H__

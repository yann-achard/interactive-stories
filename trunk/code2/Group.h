#ifndef __GROUP_H__
#define __GROUP_H__
//---------------------------------------------------------
class Clan;
//---------------------------------------------------------
class Group {
public:
	int			id;
	Clan*		clan;
	bool		mining;
	int			size;
	double	stamina;
	double	planed;
	int			x;
	int			z;

	Group(int _id, int _x, int _z, int _size, Clan* _clan);
	~Group();
	bool	IsNextTo(Clan* c);
	void	Render();
	void	StartMining();
	void	StopMining();
	void	Combat(Group& target);
	void	Kill(Clan* attacker, int bodycount);
	void	MarkPosition();
	void	RenderSelection();
	void	SetVizibility();
	void	ResetVizibility();
	void	MoveTo(int _x, int _z);
	void	AiMoveTo(int _x, int _z);
	void	Turn();
	int		GetNextCell(int _x, int _z);
	void	HeadFor(int _x, int _z);
	void	Subgroup(int subsize);
};
//---------------------------------------------------------
#endif//__GROUP_H__

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
	double	size;
	double	stamina;
	double	planed;
	int			x;
	int			z;

	Group(int _id, int _x, int _z, double _size, Clan* _clan);
	~Group();
	bool IsNextTo(Clan* c);
	void Render();
	void Kill(double bodycount);
	void MarkPosition();
	void RenderSelection();
	void SetVizibility();
	void ResetVizibility();
	void MoveTo(int _x, int _z);
	void Turn();
	void Subgroup(double subsize);
};
//---------------------------------------------------------
#endif//__GROUP_H__

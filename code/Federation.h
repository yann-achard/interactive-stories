#ifndef __FEDERATION_H__
#define __FEDERATION_H__
//---------------------------------------------------------
class Clan;
//---------------------------------------------------------
class Federation {
public:
	int			id;
	int			nb;
	double	pop;
	Clan**	clans;

	Federation(){};
	Federation(Clan* clan);
	~Federation();

	void AddClan(Clan* clan);
	void RemoveClan(int cid);
};
//---------------------------------------------------------
#endif//__FEDERATION_H__
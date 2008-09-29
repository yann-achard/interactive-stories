#ifndef __RELATIONS_H__
#define __RELATIONS_H__
//---------------------------------------------------------
#include "random.h"
//---------------------------------------------------------
class Relations {
private:
	int			n;
	int			n2;
	float*	relation;
	float*	stance;
	bool*		ally;

public:
	//Relations(): n(0), n2(0), nr(0), bel(0), pw(0), ally(0), fri(0), stance(0) {};
	Relations(int nbClans): n(nbClans), n2(n*n), relation(new float[n2]), ally(new bool[n2]), stance(new float[n2])
		{
			for (int i=0; i<n2; ++i){
				stance[i] = random(-0.5f,0.5f);
				relation[i] = stance[i]*30.0f;
				ally[i] = false;
			}
		};
	~Relations(){
		delete stance;
		delete relation;
		delete ally;
		n=0; n2=0;
	};
	
	inline bool&	Ally(int a, int b) {return(ally[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Stance(int a, int b) {return(stance[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Relation(int a, int b) {return(relation[(a>b ? a*n+b : b*n+a)]);};
	
};
//---------------------------------------------------------
#endif//__RELATIONS_H__
#ifndef __RELATIONS_H__
#define __RELATIONS_H__
//---------------------------------------------------------
#include "random.h"
//---------------------------------------------------------
class Relations {
private:
	int			n;
	int			n2;
	float*	bel;
	float*	pw;
	float*	fri;
	float*	stance;
	bool*		ally;

public:
	//Relations(): n(0), n2(0), nr(0), bel(0), pw(0), ally(0), fri(0), stance(0) {};
	Relations(int nbClans): n(nbClans), n2(n*n), bel(new float[n2]), ally(new bool[n2]),
													pw(new float[n2]), fri(new float[n2]), stance(new float[n2])
		{
			for (int i=0; i<n2; ++i){
				fri[i] = random(-10.0f,10.0f);
				stance[i] = fri[i];
				ally[i] = false;
				bel[i] = 1.0f;
				pw[i] = 1.0f;
			}
		};
	~Relations(){
		delete bel;
		delete pw;
		delete fri;
		delete stance;
		delete ally;
		n=0; n2=0;
	};
	
	inline bool&	Ally(int a, int b) {return(ally[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Fri(int a, int b) {return(fri[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Stance(int a, int b) {return(stance[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Bel(int a, int b) {return(bel[(a>b ? a*n+b : b*n+a)]);};
	inline float&	Pw(int a, int b) {return(pw[(a>b ? a*n+b : b*n+a)]);};
	
};
//---------------------------------------------------------
#endif//__RELATIONS_H__
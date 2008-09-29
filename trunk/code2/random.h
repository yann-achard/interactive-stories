#include <stdlib.h>
//------------------------------------------------
#ifndef __RANDOM_H__
#define __RANDOM_H__
//------------------------------------------------
// Produce a float between 0 and 1
inline float random01(){
	return (((float)(rand()%10000))/10000.0f);
}
//------------------------------------------------
// Produce a float between min and max
inline float random(float min, float max){
	return ((((float)(rand()%10000))/10000.0f)*(max-min)+min);
}
//------------------------------------------------
// Produce an int between [0 and max[
inline int random(int max){
	return (rand()%(max));
}
//------------------------------------------------
// Produce an int between [min and max[
inline int random(int min, int max){
	return (rand()%(max-min)+min);
}
//------------------------------------------------
#endif//__RANDOM_H__

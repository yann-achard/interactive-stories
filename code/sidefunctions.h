#ifndef __SIDEFUNCTIONS_H__
#define __SIDEFUNCTIONS_H__
//---------------------------------------------------------
void	CheckBoardToGroups();
void	CheckGroupsToBoard();
void	CheckBoardConsistency();
float distFromManhattan(int x, int z);
float sqdist1D(int a, int b);
void	Log();
void	Stats();
void	Log(char* str);
//---------------------------------------------------------
extern char	logstr[1024];
//---------------------------------------------------------
#endif//__SIDEFUNCTIONS_H__
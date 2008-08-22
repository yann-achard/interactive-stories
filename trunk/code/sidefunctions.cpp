#include "sidefunctions.h"
#include <cassert>
#include "main.h"
#include "Group.h"
#include "Clan.h"
//---------------------------------------------------------
char	logstr[1024];
//---------------------------------------------------------
void CheckBoardConsistency(){
	CheckBoardToGroups();
	CheckGroupsToBoard();
}
//---------------------------------------------------------
void CheckBoardToGroups(){
	for (int idx=g_side*g_side-1; idx>=0; --idx){
		if (g_board[idx]){
			Group& g = *g_board[idx];
			assert(idx == g.x*g_side+g.z);
		}
	}
}
//---------------------------------------------------------
void CheckGroupsToBoard(){
	for (int ic=g_nbClans-1; ic>=0; --ic){
		Clan& c = *g_clans[ic];
		for (int ig=c.nbGroups-1; ig>=0; --ig){
			Group& g = *c.groups[ig];
			assert(g_board[g.x*g_side+g.z] == c.groups[ig]);
		}
	}
}
//---------------------------------------------------------
float distFromManhattan(int x, int z){
	return (sqrtf((float)((x*x)+(z*z))));
}
//---------------------------------------------------------
float sqdist1D(int a, int b){
	return ((float)((a-b)*(a-b)));
}
//---------------------------------------------------------
void Log(){
	Log(logstr);
}
//---------------------------------------------------------
void Log(char* str){
	static bool first = true;
	FILE* fh;
	if (first){
		first = false;
		fh = fopen("log.txt","w");
	} else {
		fh = fopen("log.txt","a");
	}
	fputs(str, fh);
	fflush(fh);
	fclose(fh);
}//---------------------------------------------------------
void Stats(){
	static bool first = true;
	FILE* fh;
	if (first){
		first = false;
		fh = fopen("stats.txt","w");
	} else {
		fh = fopen("stats.txt","a");
	}
	fprintf(fh, "%d\t%d",g_turn, g_pop);
	for (int i=0; i<g_nbClans; ++i){
		Clan& c = *g_clans[i];
		fprintf(fh, "\t%d",c.size);
	}
	for (int i=0; i<g_nbClans; ++i){
		Clan& c = *g_clans[i];
		fprintf(fh, "\t%d",c.gold);
	}
	for (int i=0; i<g_nbClans; ++i){
		Clan& c = *g_clans[i];
		fprintf(fh, "\t%3.2f",c.temper);
	}
	fprintf(fh,"\n");
	fflush(fh);
	fclose(fh);
}
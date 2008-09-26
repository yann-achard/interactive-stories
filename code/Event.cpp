#include <stdio.h>
#include <assert.h>
#include "Event.h"
#include "Clan.h"
#include "sidefunctions.h"
#include "Relations.h"
//---------------------------------------------------------
extern Clan**				g_clans;
extern int					g_nbClans;
extern int					g_turn;
extern float				g_cpppt;
extern Relations*		g_rel;
extern int					g_nbAliveClans;
//---------------------------------------------------------
char*	eventstr[] = {
	"Offer",
	"TurnDown",
	"AcceptPeace",
	"AcceptGold",
	"AcceptAlliance",
	"Attack",
	"AllyAttack",
	"EnemyAttack",
	"Non"
};
//---------------------------------------------------------
EventDef**	g_events;
//---------------------------------------------------------
Event::Event(const Clan& ac, const Clan& re, EventType t, float relev):
actor(ac),
receiver(re),
type(t),
relevance(relev)
{

	sprintf(logstr, "EVENT >> %s -> %s %s %.3f\n", ac.name, re.name, eventstr[t], relev); Log();
	g_events[t]->ProcessEvent(*this);
}
//---------------------------------------------------------
EventDef::EventDef(EventType etype, float bel, float peace, void (*sp)(const Event&)):
type(etype),
belligerence(bel),
peacewill(peace),
specialProcess(sp)
{
}
//---------------------------------------------------------
void EventDef::ProcessEvent(const Event& e) const {

	float factor;
	int i = e.actor.id;
	int j = e.receiver.id;
	float tempa = e.actor.temper;
	float tempr = e.receiver.temper;

	if (peacewill==0.0f && belligerence==0.0f) {
		sprintf(logstr, "\tBefore:  %s(%3.2f) <- %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), e.receiver.name, tempr); Log();
		if (EventDef::specialProcess != 0){
			(EventDef::specialProcess)(e);
		}
		sprintf(logstr, "\tAfter:  %s(%3.2f) <- %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), e.receiver.name, tempr); Log();
		return;
	}

	sprintf(logstr, "\tBefore:  %s(%3.2f) <- %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), e.receiver.name, tempr); Log();

	assert(g_rel->Stance(i,j)<=100.001f && g_rel->Stance(i,j)>=-100.001f);
	assert(fabs(g_rel->Stance(i,j) - (((tempa+tempr) * (g_rel->Pw(i,j)-g_rel->Bel(i,j)))/ 2.0f + g_rel->Fri(i,j))) < 0.001f);

	float future = g_rel->Stance(i,j)+(peacewill-belligerence)*e.relevance;

	if (future > 100.0f) {
		factor = (100.0f-g_rel->Stance(i,j)) / (peacewill-belligerence);
	} else if (future < -100.0f){
		factor = (-100.0f-g_rel->Stance(i,j)) / (peacewill-belligerence);
	} else {
		factor = e.relevance;
	}

	assert(g_rel->Pw(i,j)>0.0f);
	assert(g_rel->Bel(i,j)>0.0f);

	g_rel->Bel(i,j) = (g_rel->Bel(i,j)*(tempa+tempr) + belligerence*factor) / (tempa+tempr);
	g_rel->Pw(i,j) = (g_rel->Pw(i,j)*(tempa+tempr) + peacewill*factor) / (tempa+tempr);

	assert(g_rel->Pw(i,j)>0.0f);
	assert(g_rel->Bel(i,j)>0.0f);

	// stats
	float prev = g_rel->Stance(i,j);

	g_rel->Stance(i,j) = ((tempa+tempr) * (g_rel->Pw(i,j)-g_rel->Bel(i,j)))/ 2.0f + g_rel->Fri(i,j);
	sprintf(logstr, "\tAfter:  %s(%3.2f) <- %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), e.receiver.name, tempr); Log();

	// stats
	g_cpppt += (prev > g_rel->Stance(i,j) ? prev-g_rel->Stance(i,j) : g_rel->Stance(i,j)-prev) / (float)g_nbAliveClans;

	assert(g_rel->Stance(i,j)<=100.001f && g_rel->Stance(i,j)>=-100.001f);

	if (EventDef::specialProcess != 0){
		(EventDef::specialProcess)(e);
	}
}
//---------------------------------------------------------
void PropagateToAlliesAndEnemies(const Event& e){
	int aid = e.actor.id;
	int rid = e.receiver.id;
	for (int i=g_nbClans-1; i>=0; --i){
		if (i==aid || i==rid || g_clans[i]->alive==false) continue;
		if (g_rel->Ally(rid,i)){
			/*EVENT*/Event(e.actor, *g_clans[i], ET_AllyAttack, e.relevance);
		} else {
			/*EVENT*/Event(e.actor, *g_clans[i], ET_EnemyAttack, e.relevance);
		}
	}
}
//---------------------------------------------------------
void EstablishPeace(const Event& e){
	/*
	int a = e.actor.id;
	int r = e.receiver.id;
	
	// Old
	if (g_rel->Stance(i,j) < 0.0f){
		g_rel->Pw(i,j) = (tempr*g_rel->Bel(i,j))/tempa;
		g_rel->Stance(i,j) = 0.0f;
	}
	if (g_rel->Stance(i,j) < 0.0f){
		g_rel->Pw(i,j) = (tempa*g_rel->Bel(i,j))/tempr;
		g_rel->Stance(i,j) = 0.0f;
	}
	
	// New
	if (g_rel->Stance(i,j) < 0.0f){
		float rez = (-g_rel->Fri(i,j))/(tempa - tempr);
		g_rel->Pw(i,j) = rez;
		g_rel->Bel(i,j) = rez;
		g_rel->Stance(i,j) = 0.0f;
	}
	if (g_rel->Stance(i,j) < 0.0f){
		float rez = (-g_rel->Fri(i,j))/(tempr - tempa);
		g_rel->Pw(i,j) = rez;
		g_rel->Bel(i,j) = rez;
		g_rel->Stance(i,j) = 0.0f;
	}
	*/
}
//---------------------------------------------------------
void EstablishAlliance(const Event& e){
	int i = e.actor.id;
	int j = e.receiver.id;
	float tempa = e.actor.temper;
	float tempr = e.receiver.temper;
	
	// Modify PeaceWill

	if (g_rel->Stance(i,j) < 60.0f){
		g_rel->Pw(i,j) = ((60.0f - g_rel->Fri(i,j))*2.0f)/(tempa+tempr) + g_rel->Bel(i,j);
		g_rel->Stance(i,j) = ((tempa+tempr) * (g_rel->Pw(i,j)-g_rel->Bel(i,j)))/ 2.0f + g_rel->Fri(i,j);
	}
	assert(g_rel->Stance(i,j)<=100.001f && g_rel->Stance(i,j)>=59.99f);


	// Modify Bel
	/*
	if (g_rel->Stance(i,j) < 60.0f){
		g_rel->Bel(i,j) = (tempa*g_rel->Pw(i,j)-60.0f)/tempr;
		g_rel->Stance(i,j) = 60.0f;
	}
	if (g_rel->Stance(i,j) < 60.0f){
		g_rel->Bel(i,j) = (tempr*g_rel->Pw(i,j)-60.0f)/tempa;
		g_rel->Stance(i,j) = 60.0f;
	}
	*/
	
	// Modify both
	/*
	if (g_rel->Stance(i,j) < 60.0f){
		float rez = (60.0f-g_rel->Fri(i,j))/(tempa - tempr);
		g_rel->Pw(i,j) = rez;
		g_rel->Bel(i,j) = rez;
		g_rel->Stance(i,j) = g_rel->Pw(i,j)*tempa - g_rel->Bel(i,j)*tempr + g_rel->Fri(i,j);
	}
	if (g_rel->Stance(i,j) < 60.0f){
		float rez = (60.0f-g_rel->Fri(i,j))/(tempr - tempa);
		g_rel->Pw(i,j) = rez;
		g_rel->Bel(i,j) = rez;
		g_rel->Stance(i,j) = g_rel->Pw(i,j)*tempr - g_rel->Bel(i,j)*tempa + g_rel->Fri(i,j);
	}
	*/
}
//---------------------------------------------------------
void InitEventsDefinitions(){
	g_events = new EventDef*[8];
	//																							 Event Type					   Bel    Peace
	g_events[ET_Attack] =								new EventDef(ET_Attack,						 300,   -300, &PropagateToAlliesAndEnemies);
	g_events[ET_AllyAttack] =						new EventDef(ET_AllyAttack,				  50,    -40);
	g_events[ET_EnemyAttack] =					new EventDef(ET_EnemyAttack,			 -50,     40);

	g_events[ET_Offer] =								new EventDef(ET_Offer,							-1,      5);
	g_events[ET_TurnDownOffer] =				new EventDef(ET_TurnDownOffer, 			10,    -10);
	g_events[ET_AcceptGoldOffer] =			new EventDef(ET_AcceptGoldOffer,		 0,      5);
	g_events[ET_AcceptPeaceOffer] =			new EventDef(ET_AcceptPeaceOffer,		 0,      0, &EstablishPeace);
	g_events[ET_AcceptAllianceOffer] =	new EventDef(ET_AcceptAllianceOffer, 0,      0, &EstablishAlliance);
}
//---------------------------------------------------------
void DeleteEventsDefinitions(){
	for (int i=7; i>=0; --i){
		delete g_events[i];
	}
	delete g_events;
}
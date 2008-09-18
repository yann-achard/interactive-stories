#include <stdio.h>
#include <assert.h>
#include "Event.h"
#include "Clan.h"
#include "sidefunctions.h"
//---------------------------------------------------------
extern Clan**				g_clans;
extern int					g_nbClans;
extern int					g_turn;
extern char**				g_allies;
extern float**			g_stances;
extern float**			g_peacewill;
extern float**			g_friendliness;
extern float**			g_belligerence;
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
	sprintf(logstr, "\tBefore: %s(%.1f) %.1f -> %.1f %s(%.1f)\n", ac.name, ac.temper, g_stances[ac.id][re.id], g_stances[re.id][ac.id], re.name, re.temper); Log();
	g_events[t]->ProcessEvent(*this);
	sprintf(logstr, "\tAfter:  %s(%.1f) %.1f -> %.1f %s(%.1f)\n", ac.name, ac.temper, g_stances[ac.id][re.id], g_stances[re.id][ac.id], re.name, re.temper); Log();
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
	int a = e.actor.id;
	int r = e.receiver.id;

	if (g_turn == 7)
		int z = 0;

	float future = g_stances[r][a]+(peacewill-belligerence)*e.relevance;

	if (future > 100.0f) {
		factor = (99.9f-g_stances[r][a]) / (peacewill-belligerence);
	} else if (future < -100.0f){
		factor = (-99.9f-g_stances[r][a]) / (peacewill-belligerence);
	} else {
		factor = e.relevance;
	}

	g_belligerence[r][a] = (g_belligerence[r][a]*e.actor.temper + belligerence*factor) / e.actor.temper;
	g_peacewill[r][a] = (g_peacewill[r][a]*e.receiver.temper + peacewill*factor) / e.receiver.temper;

	g_stances[r][a] = g_peacewill[r][a]*e.receiver.temper - g_belligerence[r][a]*e.actor.temper + g_friendliness[r][a];

	assert(g_stances[r][a]<=100.0f && g_stances[r][a]>=-100.0f);

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
		if (g_allies[rid][i]){
			/*EVENT*/Event(e.actor, *g_clans[i], ET_AllyAttack, e.relevance);
		} else {
			/*EVENT*/Event(e.actor, *g_clans[i], ET_EnemyAttack, e.relevance);
		}
	}
}
//---------------------------------------------------------
void EstablishPeace(const Event& e){
	int a = e.actor.id;
	int r = e.receiver.id;
	
	// Old
	if (g_stances[a][r] < 0.0f){
		g_peacewill[a][r] = (e.receiver.temper*g_belligerence[r][a])/e.actor.temper;
		g_stances[a][r] = 0.0f;
	}
	if (g_stances[r][a] < 0.0f){
		g_peacewill[r][a] = (e.actor.temper*g_belligerence[a][r])/e.receiver.temper;
		g_stances[r][a] = 0.0f;
	}
	
	// New
	/*
	if (g_stances[a][r] < 0.0f){
		float rez = (-g_friendliness[a][r])/(e.actor.temper - e.receiver.temper);
		g_peacewill[a][r] = rez;
		g_belligerence[a][r] = rez;
		g_stances[a][r] = 0.0f;
	}
	if (g_stances[r][a] < 0.0f){
		float rez = (-g_friendliness[r][a])/(e.receiver.temper - e.actor.temper);
		g_peacewill[r][a] = rez;
		g_belligerence[r][a] = rez;
		g_stances[r][a] = 0.0f;
	}
	*/
}
//---------------------------------------------------------
void EstablishAlliance(const Event& e){
	int a = e.actor.id;
	int r = e.receiver.id;
	
	// Modify PeaceWill
	if (g_stances[a][r] < 60.0f){
		g_peacewill[a][r] = (e.receiver.temper*g_belligerence[a][r]+60.0f)/e.actor.temper;
		g_stances[a][r] = 60.0f;
	}
	if (g_stances[r][a] < 60.0f){
		g_peacewill[r][a] = (e.actor.temper*g_belligerence[r][a]+60.0f)/e.receiver.temper;
		g_stances[r][a] = 60.0f;
	}

	// Modify Bel
	/*
	if (g_stances[a][r] < 60.0f){
		g_belligerence[a][r] = (e.actor.temper*g_peacewill[a][r]-60.0f)/e.receiver.temper;
		g_stances[a][r] = 60.0f;
	}
	if (g_stances[r][a] < 60.0f){
		g_belligerence[r][a] = (e.receiver.temper*g_peacewill[r][a]-60.0f)/e.actor.temper;
		g_stances[r][a] = 60.0f;
	}
	
	// Modify both
	if (g_stances[a][r] < 60.0f){
		float rez = (60.0f-g_friendliness[a][r])/(e.actor.temper - e.receiver.temper);
		g_peacewill[a][r] = rez;
		g_belligerence[a][r] = rez;
		g_stances[a][r] = 60.0f;
	}
	if (g_stances[r][a] < 60.0f){
		float rez = (60.0f-g_friendliness[r][a])/(e.receiver.temper - e.actor.temper);
		g_peacewill[r][a] = rez;
		g_belligerence[r][a] = rez;
		g_stances[r][a] = 60.0f;
	}
	*/

}
//---------------------------------------------------------
void InitEventsDefinitions(){
	g_events = new EventDef*[8];
	//																							 Event Type					   Bel    Peace
	g_events[ET_Attack] =								new EventDef(ET_Attack,						 400,   -200, &PropagateToAlliesAndEnemies);
	g_events[ET_AllyAttack] =						new EventDef(ET_AllyAttack,				  30,    -20);
	g_events[ET_EnemyAttack] =					new EventDef(ET_EnemyAttack,			 -30,     20);

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
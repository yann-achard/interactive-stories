#include <stdio.h>
#include "Event.h"
#include "Clan.h"
#include "sidefunctions.h"
//---------------------------------------------------------
extern Clan**				g_clans;
extern int					g_nbClans;
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
EventDef::EventDef(EventType etype, float ab, float ap, float rb, float rp, void (*sp)(const Event&)):
type(etype),
affectActorBelligerence(ab),
affectActorPeacewill(ap),
affectReceiverBelligerence(rb),
affectReceiverPeacewill(rp),
specialProcess(sp)
{
}
//---------------------------------------------------------
void EventDef::ProcessEvent(const Event& e) const {
	int a = e.actor.id;
	int r = e.receiver.id;

	g_belligerence[a][r] = (g_belligerence[a][r]*e.receiver.temper + affectActorBelligerence*e.relevance) / e.receiver.temper;
	g_peacewill[a][r] = (g_peacewill[a][r]*e.actor.temper + affectActorPeacewill*e.relevance) / e.actor.temper;

	g_belligerence[r][a] = (g_belligerence[r][a]*e.actor.temper + affectReceiverBelligerence*e.relevance) / e.actor.temper;
	g_peacewill[r][a] = (g_peacewill[r][a]*e.receiver.temper + affectReceiverPeacewill*e.relevance) / e.receiver.temper;

	g_stances[a][r] = g_peacewill[a][r]*e.actor.temper - g_belligerence[a][r]*e.receiver.temper + g_friendliness[a][r];
	if (g_stances[a][r] > 100.0f) g_stances[a][r] = 100.0f;
	else if (g_stances[a][r] < -100.0f) g_stances[a][r] = -100.0f;

	g_stances[r][a] = g_peacewill[r][a]*e.receiver.temper - g_belligerence[r][a]*e.actor.temper + g_friendliness[r][a];
	if (g_stances[r][a] > 100.0f) g_stances[r][a] = 100.0f;
	else if (g_stances[r][a] < -100.0f) g_stances[r][a] = -100.0f;

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
	g_events = new EventDef*[9];
	//																							 Event Type						   AcBel	AcPea   ReBel   RePea
	g_events[ET_Attack] =								new EventDef(ET_Attack,							  0,	   0,      400,   -200, &PropagateToAlliesAndEnemies);
	g_events[ET_AllyAttack] =						new EventDef(ET_AllyAttack,						0,     0,       30,    -20);
	g_events[ET_EnemyAttack] =					new EventDef(ET_EnemyAttack,					0,     0,      -30,     20);

	g_events[ET_Offer] =								new EventDef(ET_Offer,								0,     0,       -1,      5);
	g_events[ET_TurnDownOffer] =				new EventDef(ET_TurnDownOffer, 				0,     0,       10,    -10);
	g_events[ET_AcceptGoldOffer] =			new EventDef(ET_AcceptGoldOffer,			0,     0,        0,      5);
	g_events[ET_AcceptPeaceOffer] =			new EventDef(ET_AcceptPeaceOffer,			0,     0,        0,      0, &EstablishPeace);
	g_events[ET_AcceptAllianceOffer] =	new EventDef(ET_AcceptAllianceOffer,  0,     0,        0,      0, &EstablishAlliance);

	g_events[ET_NonPayment] =						new EventDef(ET_NonPayment,						0,     0,        0,      0);
}
//---------------------------------------------------------
void DeleteEventsDefinitions(){
	for (int i=8; i>=0; --i){
		delete g_events[i];
	}
	delete g_events;
}
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
	sprintf(logstr, "/*EVENT*/ %s -> %s %s %.3f\n", ac.name, re.name, eventstr[t], relev); Log();
	sprintf(logstr, "\t\tBefore: %s(%.1f) %.1f -> %.1f %s(%.1f)\n", ac.name, ac.temper, g_stances[ac.id][re.id], g_stances[re.id][ac.id], re.name, re.temper); Log();
	g_events[t]->ProcessEvent(*this);
	sprintf(logstr, "\t\tAfter:  %s(%.1f) %.1f -> %.1f %s(%.1f)\n", ac.name, ac.temper, g_stances[ac.id][re.id], g_stances[re.id][ac.id], re.name, re.temper); Log();
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
	int aid = e.actor.id;
	int rid = e.receiver.id;

	g_belligerence[aid][rid] += affectActorBelligerence * e.relevance;
	g_peacewill[aid][rid] += affectActorPeacewill * e.relevance;
	g_belligerence[rid][aid] += affectReceiverBelligerence * e.relevance;
	g_peacewill[rid][aid] += affectReceiverPeacewill * e.relevance;

	g_stances[aid][rid] = g_peacewill[aid][rid]*e.actor.temper - g_belligerence[aid][rid]*e.receiver.temper + g_friendliness[aid][rid];
	if (g_stances[aid][rid] > 100.0f) g_stances[aid][rid] = 100.0f;
	else if (g_stances[aid][rid] < -100.0f) g_stances[aid][rid] = -100.0f;

	g_stances[rid][aid] = g_peacewill[rid][aid]*e.receiver.temper - g_belligerence[rid][aid]*e.actor.temper + g_friendliness[rid][aid];
	if (g_stances[rid][aid] > 100.0f) g_stances[rid][aid] = 100.0f;
	else if (g_stances[rid][aid] < -100.0f) g_stances[rid][aid] = -100.0f;

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
	int aid = e.actor.id;
	int rid = e.receiver.id;
	
	if (g_stances[aid][rid] < 0.0f){
		g_peacewill[aid][rid] = (e.receiver.temper*g_belligerence[rid][aid])/e.actor.temper;
		g_stances[aid][rid] = 0.0f;
	}
	if (g_stances[rid][aid] < 0.0f){
		g_peacewill[rid][aid] = (e.actor.temper*g_belligerence[aid][rid])/e.receiver.temper;
		g_stances[rid][aid] = 0.0f;
	}
}
//---------------------------------------------------------
void EstablishAlliance(const Event& e){
	int aid = e.actor.id;
	int rid = e.receiver.id;
	
	if (g_stances[aid][rid] < 60.0f){
		g_peacewill[aid][rid] = (e.receiver.temper*g_belligerence[rid][aid]+60.0f)/e.actor.temper;
		g_stances[aid][rid] = 60.0f;
	}
	if (g_stances[rid][aid] < 60.0f){
		g_peacewill[rid][aid] = (e.actor.temper*g_belligerence[aid][rid]+60.0f)/e.receiver.temper;
		g_stances[rid][aid] = 60.0f;
	}
}
//---------------------------------------------------------
void InitEventsDefinitions(){
	g_events = new EventDef*[9];
	//																							 Event Type						   AcBel	AcPea   ReBel   RePea
	g_events[ET_Attack] =								new EventDef(ET_Attack,							 -0.5f,	 0.5f,   2.2f,  -0.4f, &PropagateToAlliesAndEnemies);
	g_events[ET_AllyAttack] =						new EventDef(ET_AllyAttack,						0,     0,      0.6f,  -0.2f);
	g_events[ET_EnemyAttack] =					new EventDef(ET_EnemyAttack,					0,     0,     -0.6f,   0.2f);

	g_events[ET_Offer] =								new EventDef(ET_Offer,								0,     0,     -0.1f,   0.2f);
	g_events[ET_TurnDownOffer] =				new EventDef(ET_TurnDownOffer, 				0,     0,      0.1f,  -0.3f);
	g_events[ET_AcceptGoldOffer] =			new EventDef(ET_AcceptGoldOffer,			0,     0,     -0.3f,   0.3f);
	g_events[ET_AcceptPeaceOffer] =			new EventDef(ET_AcceptPeaceOffer,			0,     0,     -0.4f,   0.3f, &EstablishPeace);
	g_events[ET_AcceptAllianceOffer] =	new EventDef(ET_AcceptAllianceOffer,  0,     0,     -0.5f,   0.4f, &EstablishAlliance);

	g_events[ET_NonPayment] =						new EventDef(ET_NonPayment,						0,     0.3f,   0.5f,  -0.2f);
}
//---------------------------------------------------------
void DeleteEventsDefinitions(){
	for (int i=8; i>=0; --i){
		delete g_events[i];
	}
	delete g_events;
}
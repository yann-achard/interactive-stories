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
	"ThirdPartyAttack",
	"None"
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

	sprintf(logstr, "EVENT >> %s -> %s %s %1.5f\n", ac.name, re.name, eventstr[t], relev); Log();
	g_events[t]->ProcessEvent(*this);
}
//---------------------------------------------------------
EventDef::EventDef(EventType etype, float impact, void (*sp)(const Event&)):
type(etype),
impact(impact),
specialProcess(sp)
{
}
//---------------------------------------------------------
void EventDef::ProcessEvent(const Event& e) const {

	float change = impact*e.relevance;
	int i = e.actor.id;
	int j = e.receiver.id;
	float tempa = e.actor.temper;
	float tempr = e.receiver.temper;

	assert(g_rel->Stance(i,j)>-10.001f);
	assert(g_rel->Stance(i,j)< 10.001f);
	assert(g_rel->Relation(i,j)>-100.001f);
	assert(g_rel->Relation(i,j)< 100.001f);

	if (change==0.0f) {
		sprintf(logstr, "\tBefore: %s(%3.2f) <- %1.2f | %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), g_rel->Relation(i,j), e.receiver.name, tempr); Log();
		if (EventDef::specialProcess != 0){
			(EventDef::specialProcess)(e);
		}
		sprintf(logstr, "\tAfter:  %s(%3.2f) <- %1.2f | %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), g_rel->Relation(i,j), e.receiver.name, tempr); Log();
		assert(g_rel->Stance(i,j)>-10.001f);
		assert(g_rel->Stance(i,j)< 10.001f);
		assert(g_rel->Relation(i,j)>-100.001f);
		assert(g_rel->Relation(i,j)< 100.001f);
		return;
	}

	sprintf(logstr, "\tBefore: %s(%3.2f) <- %1.2f | %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), g_rel->Relation(i,j), e.receiver.name, tempr); Log();

	float future = g_rel->Relation(i,j)+change;

	if (future > 100.0f) {
		change = 100.0f-g_rel->Relation(i,j);
	} else if (future < -100.0f){
		change = -100.0f-g_rel->Relation(i,j);
	}

	g_rel->Stance(i,j) = ((g_rel->Relation(i,j)+change) * 2.0f) / (tempa+tempr);

	// stats
	float prev = g_rel->Relation(i,j);

	assert(fabs(g_rel->Relation(i,j) + change - ((tempa+tempr) * g_rel->Stance(i,j)) / 2.0f) < 0.001f);
	g_rel->Relation(i,j) = ((tempa+tempr) * g_rel->Stance(i,j)) / 2.0f;
	sprintf(logstr, "\tAfter:  %s(%3.2f) <- %1.2f | %3.2f -> %s(%3.2f)\n", e.actor.name, tempa, g_rel->Stance(i,j), g_rel->Relation(i,j), e.receiver.name, tempr); Log();

	// stats
	g_cpppt += (prev > g_rel->Relation(i,j) ? prev-g_rel->Relation(i,j) : g_rel->Relation(i,j)-prev) / (float)g_nbAliveClans;

	if (EventDef::specialProcess != 0){
		(EventDef::specialProcess)(e);
	}

	assert(g_rel->Stance(i,j)>-10.001f);
	assert(g_rel->Stance(i,j)< 10.001f);
	assert(g_rel->Relation(i,j)>-100.001f);
	assert(g_rel->Relation(i,j)< 100.001f);
}
//---------------------------------------------------------
void PropagateToAlliesAndEnemies(const Event& e){
	int aid = e.actor.id;
	int rid = e.receiver.id;
	for (int i=g_nbClans-1; i>=0; --i){
		if (i==aid || i==rid || g_clans[i]->alive==false) continue;
		/*EVENT*/Event(e.actor, *g_clans[i], ET_ThirdPartyAttack, g_rel->Relation(rid,i)/100.0f);
	}
}
//---------------------------------------------------------
void EstablishPeace(const Event& e){
}
//---------------------------------------------------------
void EstablishAlliance(const Event& e){
	int i = e.actor.id;
	int j = e.receiver.id;
	float tempa = e.actor.temper;
	float tempr = e.receiver.temper;
	
	/*
	if (g_rel->Relation(i,j) < 60.0f){
		g_rel->Stance(i,j) = 120.0f/(tempa+tempr);
	}
	*/
	if (g_rel->Stance(i,j) < 0.6f){
		g_rel->Stance(i,j) = 0.6f;
	}

	// stats
	float prev = g_rel->Relation(i,j);

	g_rel->Relation(i,j) = ((tempa+tempr) * g_rel->Stance(i,j)) / 2.0f;
	//assert(g_rel->Relation(i,j)>=59.99f);

	// stats
	g_cpppt += (prev > g_rel->Relation(i,j) ? prev-g_rel->Relation(i,j) : g_rel->Relation(i,j)-prev) / (float)g_nbAliveClans;
}
//---------------------------------------------------------
void InitEventsDefinitions(){
	g_events = new EventDef*[7];
	//																							 Event Type					   Impact
	g_events[ET_Attack] =								new EventDef(ET_Attack,						-400, &PropagateToAlliesAndEnemies);
	g_events[ET_ThirdPartyAttack] =			new EventDef(ET_ThirdPartyAttack,	 -60);

	g_events[ET_Offer] =								new EventDef(ET_Offer,						  01);
	g_events[ET_TurnDownOffer] =				new EventDef(ET_TurnDownOffer, 		 -10);
	g_events[ET_AcceptGoldOffer] =			new EventDef(ET_AcceptGoldOffer,		 0);
	g_events[ET_AcceptPeaceOffer] =			new EventDef(ET_AcceptPeaceOffer,		 0, &EstablishPeace);
	g_events[ET_AcceptAllianceOffer] =	new EventDef(ET_AcceptAllianceOffer, 0, &EstablishAlliance);
}
//---------------------------------------------------------
void DeleteEventsDefinitions(){
	for (int i=6; i>=0; --i){
		delete g_events[i];
	}
	delete g_events;
}
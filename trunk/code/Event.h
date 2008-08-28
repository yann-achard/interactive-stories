#ifndef __EVENT_H__
#define __EVENT_H__
//---------------------------------------------------------
class Clan;
//---------------------------------------------------------
class EventDef;
extern EventDef**	g_events;
//---------------------------------------------------------
void InitEventsDefinitions();
void DeleteEventsDefinitions();
//---------------------------------------------------------
enum	EventType {
	ET_Offer,
	ET_TurnDownOffer,
	ET_AcceptPeaceOffer,
	ET_AcceptGoldOffer,
	ET_AcceptAllianceOffer,
	ET_Attack,
	ET_AllyAttack,
	ET_EnemyAttack,
	ET_NonPayment
};
//---------------------------------------------------------
class Event {
public:
	const Clan&			actor;
	const Clan&			receiver;
	const EventType	type;
	const float			relevance;

	Event(const Clan& ac, const Clan& re, EventType t, float relev=1.0f);
};
//---------------------------------------------------------
class EventDef {
public:
	EventType	type;
	float			affectActorBelligerence;
	float			affectActorPeacewill;
	float			affectReceiverBelligerence;
	float			affectReceiverPeacewill;
	void			(*specialProcess)(const Event& e);

	EventDef();
	EventDef(EventType etype, float ab, float ap, float rb, float rp, void (*sp)(const Event&)=0);
	
	void ProcessEvent(const Event& e) const;
};
//---------------------------------------------------------
#endif//__EVENT_H__
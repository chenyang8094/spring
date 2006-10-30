#ifndef WAIT_COMMANDS_AI_H
#define WAIT_COMMANDS_AI_H
// WaitCommandsAI.h: interface for the CWaitCommandsAI class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include <map>
#include <set>
using namespace std;

#include "System/Object.h"

class float3;
class CUnit;
class CObject;
struct Command;


class CWaitCommandsAI {
	public:
		CWaitCommandsAI();
		~CWaitCommandsAI();

		void Update();
		void DrawCommands();

		// called from SelectedUnits
		void AddTimeWait(const Command& cmd);
		void AddSquadWait(const Command& cmd);
		void AddDeathWatch(const Command& cmd);
		void AddRallyPoint(const Command& cmd);

		void NewUnit(CUnit* unit, const CUnit* builder);

	private:
		class Wait;
		void DeleteWaitObject(Wait*);

	private:
		typedef set<CUnit*> UnitSet;

		// Wait Base Class
		class Wait : public CObject {
			public:
				virtual ~Wait();
				virtual void DependentDied(CObject* o) = 0; // from CObject
				virtual void AddUnit(CUnit* unit) = 0;
				virtual void Update() = 0;
				virtual void Draw() const = 0;
				float GetKey() const { return key; }
				float GetCode() const { return code; }
			protected:
				Wait(float code);
				enum WaitState {
					Active, Queued, Missing
				};
				WaitState GetWaitState(const CUnit* unit) const;
				bool IsWaitingOn(const CUnit* unit) const;
				void SendCommand(const Command& cmd, const UnitSet& unitSet);
				void SendWaitCommand(const UnitSet& unitSet);
				UnitSet::iterator RemoveUnit(UnitSet::iterator, UnitSet&);
			protected:
				const float code;
				float key;
				int deadFrame;
		};
		typedef set<Wait*> WaitSet;
		WaitSet waitSet;

		// TimeWait				
		class TimeWait : public Wait {
			public:
				static TimeWait* New(const Command& cmd, CUnit* unit);
				static TimeWait* New(int duration, CUnit* unit);
				~TimeWait();
				void DependentDied(CObject* o);
				void AddUnit(CUnit* unit);
				void Update();
				void Draw() const;
				int GetDuration() const { return duration; }
			private:
				TimeWait(const Command& cmd, CUnit* unit);
				TimeWait(int duration, CUnit* unit);
			private:
				CUnit* unit;
				bool enabled;
				int duration;
				int endFrame;
				static float keySource;
		};

		// SquadWait				
		class SquadWait : public Wait {
			public:
				static SquadWait* New(const Command& cmd);
				~SquadWait();
				void DependentDied(CObject* o);
				void AddUnit(CUnit* unit);
				void Update();
				void Draw() const;
			private:
				SquadWait(const Command& cmd);
			private:
				int squadCount;
				UnitSet buildUnits;
				UnitSet waitUnits;
				static float keySource;
		};

		// DeathWatch
		class DeathWatch : public Wait {
			public:
				static DeathWatch* New(const Command& cmd);
				~DeathWatch();
				void DependentDied(CObject* o);
				void AddUnit(CUnit* unit);
				void Update();
				void Draw() const;
			private:
				DeathWatch(const Command& cmd);
				void SelectAreaUnits(const float3& pos0, const float3& pos1,
				                     UnitSet& units, bool enemies);
			private:
				UnitSet waitUnits;
				UnitSet deathUnits;
				static float keySource;
		};

		// RallyPoint
		class RallyPoint : public Wait {
			public:
				static RallyPoint* New(const Command& cmd);
				~RallyPoint();
				void DependentDied(CObject * o);
				void AddUnit(CUnit* unit);
				void Update();
				void Draw() const;
			private:
				RallyPoint(const Command& cmd);
			private:
				UnitSet waitUnits;
				static float keySource;
		};

	private:
		WaitSet::iterator RemoveWait(WaitSet::iterator, WaitSet&);
};


extern CWaitCommandsAI waitCommandsAI;


#endif /* WAIT_COMMANDS_AI_H */

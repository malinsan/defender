#pragma once
class AIStateMachine : public Component
{
	// The state interface
	class State
	{
	public:
		AvancezLib * system;
		Sprite * sprite_left;
		Sprite * sprite_right;
	public:
		virtual ~State() {}
		virtual void Enter(AIStateMachine& state_machine) {}
		virtual void Update(AIStateMachine& state_machine) {}
	};

	class IdleState : public State 
	{
	public:
		IdleState(AvancezLib* system) 
		{
			this->system = system;
		}

		virtual void Enter(AIStateMachine &state_machine) 
		{
			state_machine.current_state = this;

			//state_machine.lander->horizontalVelocity = 1.0f;
		}
		virtual void Update(AIStateMachine& state_machine) {
			state_machine.state_idle->Enter(state_machine);
		}
	};

	class AggressiveState : public State 
	{

	};

	class AbductorState : public State
	{

	};

	class AttackState : public State
	{

	};

	class ApproachState : public State
	{

	};

public:
	float last_attack_time = 0.0f;

	const float	ATTACK_TIME = 0.2f;
	const float	ATTACK_COOLDOWN_TIME = 0.05f;

	Lander		* lander;
	AvancezLib  * system;

	State *				current_state;
	IdleState *			state_idle;

	AggressiveState *	state_aggressive;
	AbductorState *		state_abductor;

	AttackState *		state_attack;
	
	ApproachState *		state_approach;
	
	virtual ~AIStateMachine() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);

		this->system = system;
		lander = (Lander*)go;
	}

	virtual void Init()
	{
		state_idle = new IdleState(system);
		//state_attack = new AttackState(system);
		//state_aggressive = new AggressiveState(system);
		//state_approach = new ApproachState(system);
		
		current_state = state_idle;
	}

	// Main loop
	virtual void Update(float dt)
	{
		// No need for input
		current_state->Update(*this);
	}
};
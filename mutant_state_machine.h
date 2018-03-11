#pragma once
class MutantStateMachine : public Component
{
	// The state interface
	class State
	{
	public:
		AvancezLib * system;
	public:
		virtual ~State() {}
		virtual void Enter(MutantStateMachine& state_machine) {}
		virtual void Update(MutantStateMachine& state_machine, float dt) {}

		//parameter chance is the 1/5000 percent chance of getting a true
		bool Chance(int chance)
		{
			//0-4999
			return rand() % 5000 <= chance;
		}
		//proximity to player or humans, parameter isPlayerTarget should be true if checking proximity to player and false otherwise
		bool InProximityTo(MutantStateMachine& state_machine, GameObject* target, float range) {
			//where are we?
			float posX = state_machine.mutant->horizontalPosition;
			float posY = state_machine.mutant->verticalPosition;

			//where is actor?
			if (target == NULL) {
				return false;
			}
			float targetPosX = target->horizontalPosition;
			float targetPosY = target->verticalPosition;

			return Distance(posX, posY, targetPosX, targetPosY) <= range;

		}

		//returns distance between point a and b using pythagorean theorem
		float Distance(float posX, float posY, float targetX, float targetY) {
			float lengthA = (float)pow(abs(posX - targetX), 2);
			float lengthB = (float)pow(abs(posY - targetY), 2);
			float distance = (float)sqrt(lengthA + lengthB);
			return distance;
		}

	};

	class IdleState : public State
	{
		bool goingRight = true;

	public:
		IdleState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(MutantStateMachine &state_machine)
		{
			state_machine.current_state = this;
		}
		virtual void Update(MutantStateMachine& state_machine, float dt) {
			//when idle move side to side 
			float movement = goingRight ? LANDER_ACCELERATION * dt : -LANDER_ACCELERATION * dt;
			state_machine.mutant->velocity.x += movement;

			if (goingRight && state_machine.mutant->velocity.x >= LANDER_MAX_SPEED * 3) {
				goingRight = (goingRight == false); //flip bool
			}
			else if (!goingRight && state_machine.mutant->velocity.x <= -LANDER_MAX_SPEED * 3) {
				goingRight = (goingRight == false); //flip bool
			}

			//have high chance of going for player 
			if (Chance(1000)) {
				state_machine.state_aggressive->Enter(state_machine);
			}
		}
	};


	class AggressiveState : public State
	{
	public:
		AggressiveState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(MutantStateMachine &state_machine)
		{
			state_machine.current_state = this;
			//have chance of going back to idle
			if (Chance(100)) {
				state_machine.state_idle->Enter(state_machine);
			}
		}
		virtual void Update(MutantStateMachine& state_machine, float dt)
		{
			//if close to player, attack
			if (InProximityTo(state_machine, state_machine.player, PLAYER_RANGE)) {
				state_machine.state_attack->Enter(state_machine);
			}
			else { //otherwise: approach player
				state_machine.state_approach->Enter(state_machine, true);
			}
		}

	};

	class ApproachState : public State
	{
		float range;
		GameObject* target;

	public:
		ApproachState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(MutantStateMachine &state_machine, bool isPlayerTarget)
		{
			state_machine.current_state = this;
			target = state_machine.player;
			
		}

		virtual void Update(MutantStateMachine& state_machine, float dt) {
			
			//move towards target
			float horizontalMovement = target->horizontalPosition > state_machine.mutant->horizontalPosition ? MUTANT_MAX_SPEED : -MUTANT_MAX_SPEED;
			float verticalMovement = target->verticalPosition > state_machine.mutant->verticalPosition ? MUTANT_MAX_SPEED : -MUTANT_MAX_SPEED;

			state_machine.mutant->velocity.x = horizontalMovement;
			state_machine.mutant->velocity.y = verticalMovement;


			if (InProximityTo(state_machine, target, PLAYER_RANGE)) {
				//if we are in range go back to the state we came from to either shoot at the player or abduct a human
				state_machine.state_aggressive->Enter(state_machine);
			}
		}
	};

	class AttackState : public State
	{
	public:
		AttackState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(MutantStateMachine &state_machine)
		{
			state_machine.current_state = this;

			//send an attack towards the player
			Bomb * bomb = state_machine.bomb_pool->FirstAvailable();
			if (bomb != NULL && (system->getElapsedTime() - state_machine.last_attack_time) > 1.0f) //shoot faster than normal landers
			{
				state_machine.last_attack_time = system->getElapsedTime();
				int x = 10;
				bool shootLeft = state_machine.player->horizontalPosition < state_machine.go->horizontalPosition;
				bomb->Init(state_machine.go->horizontalPosition + x, state_machine.go->verticalPosition, shootLeft);
				state_machine.game_objects->insert(bomb);
			}
		}
		virtual void Update(MutantStateMachine& state_machine, float dt) {
			//go back to aggressive state
			state_machine.state_aggressive->Enter(state_machine);
		}


	};




public:

	AvancezLib * system;

	Mutant		* mutant;
	float last_attack_time = 0.0f;

	Player		* player;
	ObjectPool<Bomb>* bomb_pool;

	State *				current_state;
	IdleState *			state_idle;
	AggressiveState *	state_aggressive;
	ApproachState *		state_approach;
	AttackState *		state_attack;


	virtual ~MutantStateMachine() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, 
		Player* player, ObjectPool<Bomb>* bomb_pool)
	{
		Component::Create(system, go, game_objects);

		this->system = system;
		mutant = (Mutant*)go;
		this->player = player;
		this->bomb_pool = bomb_pool;
	}

	virtual void Init()
	{
		state_idle = new IdleState(system);
		state_aggressive = new AggressiveState(system);
		state_approach = new ApproachState(system);
		state_attack = new AttackState(system);


		current_state = state_idle;
	}

	// Main loop
	virtual void Update(float dt)
	{
		if (go->verticalPosition < 100) {
			go->verticalPosition = 100;
			go->velocity.y *= -1;
		}
		else if (go->verticalPosition > 670) {
			go->verticalPosition = 670;
			go->velocity.y *= -1;
		}


		go->horizontalPosition += go->velocity.x * dt;
		go->verticalPosition += go->velocity.y * dt;


		// No need for input
		current_state->Update(*this, dt);
	}

	void Receive(Message m) {
		if (m == SMARTBOMB_DROPPED) {
			if (go->horizontalPosition < 1200 && go->horizontalPosition > 0) {
				go->Receive(HIT);
			}
		}
	}

}; 
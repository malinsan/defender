#pragma once
class LanderStateMachine : public Component
{
	// The state interface
	class State
	{
	public:
		AvancezLib * system;
		virtual ~State() {}
		virtual void Enter(LanderStateMachine& state_machine) {}
		virtual void Update(LanderStateMachine& state_machine, float dt) {}

		//parameter chance is the 1/5000 percent chance of getting a true
		bool Chance(int chance) 
		{
			//0-4999
			return rand() % 5000 <= chance;
		}

		//proximity to player or humans, parameter isPlayerTarget should be true if checking proximity to player and false otherwise
		bool InProximityTo(LanderStateMachine& state_machine, GameObject* target, float range) {
			//where are we?
			float posX = state_machine.lander->horizontalPosition;
			float posY = state_machine.lander->verticalPosition;

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

		virtual void Enter(LanderStateMachine &state_machine) 
		{
			state_machine.current_state = this;
		}
		virtual void Update(LanderStateMachine& state_machine, float dt) {
			
			//when idle move side to side 
			float movement = goingRight ? LANDER_ACCELERATION * dt : -LANDER_ACCELERATION * dt;				
			state_machine.lander->velocity.x += movement;
			
			if (goingRight && state_machine.lander->velocity.x >= LANDER_MAX_SPEED*3) {
				goingRight = (goingRight == false); //flip bool
			}
			else if (!goingRight && state_machine.lander->velocity.x <= -LANDER_MAX_SPEED*3) {
				goingRight = (goingRight == false); //flip bool
			}

			//have chance of going for player 
			if (Chance(1)) {
				state_machine.state_aggressive->Enter(state_machine);
			}

			//have chance of going for a human
			if (Chance(1)) {
				state_machine.state_humanAggressive->Enter(state_machine);
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

		virtual void Enter(LanderStateMachine &state_machine)
		{
			state_machine.current_state = this;
			//have chance of going back to idle
			if (Chance(1000)) {
				state_machine.state_idle->Enter(state_machine);
			}
		}
		virtual void Update(LanderStateMachine& state_machine, float dt) 
		{
			//if close to player, attack
			if (InProximityTo(state_machine, state_machine.player, PLAYER_RANGE)) {
				state_machine.state_attack->Enter(state_machine);
			}
			else { //approach player
				state_machine.state_approach->Enter(state_machine, true);
			}
		}

	};

	class ApproachState : public State
	{
		bool isPlayerTarget; //abducting or aggressive?
		float range;
		State* cameFromState;
		GameObject* target;
	public:
		ApproachState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(LanderStateMachine &state_machine, bool isPlayerTarget)
		{
			state_machine.current_state = this;
			this->isPlayerTarget = isPlayerTarget;

			if (isPlayerTarget) {
				range = PLAYER_RANGE; //shoot player
				cameFromState = state_machine.state_aggressive;
				target = state_machine.player;
			}
			else {
				range = HUMAN_RANGE; //pickup humans
				cameFromState = state_machine.state_humanAggressive;
				target = state_machine.closestHuman;
			}
		}

		virtual void Update(LanderStateMachine& state_machine, float dt) {
			//still go for target?
			if (!isPlayerTarget) {
				Human * h = (Human*)target;
				if (h->abducted) {
					cameFromState->Enter(state_machine);
				}
			}
			//move towards target
			float horizontalMovement = target->horizontalPosition > state_machine.lander->horizontalPosition ? LANDER_MAX_SPEED : -LANDER_MAX_SPEED;
			float verticalMovement = target->verticalPosition > state_machine.lander->verticalPosition ? LANDER_MAX_SPEED : -LANDER_MAX_SPEED;

			state_machine.lander->velocity.x = horizontalMovement;
			state_machine.lander->velocity.y = verticalMovement;


			if (InProximityTo(state_machine, target, range)) {
				//if we are in range go back to the state we came from to either shoot at the player or abduct a human
				cameFromState->Enter(state_machine);
			}

		}


	};

	class HumanAggressiveState : public State
	{
	public:
		HumanAggressiveState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(LanderStateMachine &state_machine)
		{
			state_machine.current_state = this;
			state_machine.FindClosestHuman();
		//	startTime = system->getElapsedTime();
		}
		virtual void Update(LanderStateMachine& state_machine, float dt) {

			//if close to human, abduct
			if (InProximityTo(state_machine, state_machine.closestHuman, HUMAN_RANGE)) 
			{
				state_machine.state_abductor->Enter(state_machine);
			}
			else 
			{//else approach
				state_machine.state_approach->Enter(state_machine, false);
			}
		}

	};

	class AbductorState : public State
	{
	public:
		AbductorState(AvancezLib* system)
		{
			this->system = system;
		}
		virtual void Enter(LanderStateMachine &state_machine)
		{
			state_machine.current_state = this;
			//pick up the human
			state_machine.lander->abductedHuman = state_machine.closestHuman;
			state_machine.closestHuman->abducted = true;
		}
		virtual void Update(LanderStateMachine& state_machine, float dt) {
			
			//if at top and carrying a human : turn into a mutant
			if (state_machine.go->verticalPosition <= 100.0f) {
				Mutant * mutant = state_machine.mutant_pool->FirstAvailable();
				if (mutant != NULL) {
					mutant->Init(state_machine.go->horizontalPosition, state_machine.go->verticalPosition);
					state_machine.go->enabled = false;
					state_machine.lander->abductedHuman->enabled = false;
					state_machine.game_objects->insert(mutant);
				}
	
			}

			//move upwards 
			state_machine.go->velocity.y = -LANDER_MAX_SPEED;
			state_machine.go->velocity.x = 0;
		}
	};

	class AttackState : public State
	{
	public:
		AttackState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(LanderStateMachine &state_machine)
		{
			state_machine.current_state = this;

			//send an attack towards the player
			Bomb * bomb = state_machine.bomb_pool->FirstAvailable();
			if (bomb != NULL && (system->getElapsedTime() - state_machine.last_attack_time) > 1.0f)
			{
				state_machine.last_attack_time = system->getElapsedTime();
				int x = 10;
				bool shootLeft = state_machine.player->horizontalPosition < state_machine.lander->horizontalPosition;
				bomb->Init(state_machine.lander->horizontalPosition + x, state_machine.lander->verticalPosition, shootLeft);
				state_machine.game_objects->insert(bomb);
			}
		}
		virtual void Update(LanderStateMachine& state_machine, float dt) {
			//go back to aggressive state
			state_machine.state_aggressive->Enter(state_machine);
		}


	};

	class BumpedState : public State
	{
	public:
		
		BumpedState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(LanderStateMachine &state_machine)
		{
			state_machine.current_state = this;
			if (state_machine.lander->abductedHuman != NULL) {
				state_machine.lander->abductedHuman->Receive(DROPPED);
			}
		}
		
		virtual void Update(LanderStateMachine& state_machine, float dt) {
			//go back to aggressive state
			state_machine.state_aggressive->Enter(state_machine);

		}

	};

public:
	float last_attack_time = 0.0f;

	
	AvancezLib  * system;
	Player		* player;
	Lander		* lander;
	ObjectPool<Bomb>* bomb_pool;
	ObjectPool<Human>* human_pool;
	ObjectPool<Mutant>* mutant_pool;
	Human		* closestHuman;

	State *				current_state;
	IdleState *			state_idle;

	AggressiveState *	state_aggressive;
	HumanAggressiveState * state_humanAggressive;
	AbductorState *		state_abductor;

	AttackState *		state_attack;
	
	ApproachState *		state_approach;
	BumpedState *		state_bumped;
	
	virtual ~LanderStateMachine() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, 
		Player* player, ObjectPool<Bomb> * bomb_pool, ObjectPool<Human> * human_pool, ObjectPool<Mutant>* mutant_pool)
	{
		Component::Create(system, go, game_objects);
		this->system = system;

		lander = (Lander*)go;
		this->player = player;
		this->bomb_pool = bomb_pool;
		this->human_pool = human_pool;
		this->mutant_pool = mutant_pool;
	}

	virtual void Init()
	{
		FindClosestHuman();
		state_idle = new IdleState(system);
		state_attack = new AttackState(system);
		state_aggressive = new AggressiveState(system);
		state_approach = new ApproachState(system);
		state_humanAggressive = new HumanAggressiveState(system);
		state_abductor = new AbductorState(system);
		state_bumped = new BumpedState(system);
		
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


		lander->horizontalPosition += lander->velocity.x * dt;
		lander->verticalPosition += lander->velocity.y * dt;


		if (lander->bumped) {
			lander->bumped = false;
			state_bumped->Enter(*this);
		}
		// No need for input
		current_state->Update(*this, dt);
	}

	void FindClosestHuman() 
	{
		closestHuman = human_pool->FirstAvailable();
		float distance = 100000.0f;
		for (auto human = human_pool->pool.begin(); human != human_pool->pool.end(); human++)
		{
			Human* castHuman = *human;
			if (castHuman->enabled && !castHuman->abducted && !castHuman->carried) {
				float tmpDistance = current_state->Distance((float)lander->horizontalPosition, (float)lander->verticalPosition, (float)castHuman->horizontalPosition, (float)castHuman->verticalPosition);
				if (tmpDistance < distance && !castHuman->abducted) {
					distance = tmpDistance;
					closestHuman = castHuman;
				}
			}
		}
	}

	void Receive(Message m) {
		if (m == SMARTBOMB_DROPPED) {
			if (go->horizontalPosition < 1200 && go->horizontalPosition > 0) {
				go->Receive(HIT);
			}
		}
	}
};
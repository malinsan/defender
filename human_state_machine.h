#pragma once
class HumanStateMachine : public Component
{
	// The state interface
	class State
	{
	public:
		AvancezLib * system;
	public:
		virtual ~State() {}
		virtual void Enter(HumanStateMachine& state_machine) {}
		virtual void Update(HumanStateMachine& state_machine, float dt) {}

		//parameter chance is the 1/5000 percent chance of getting a true
		bool Chance(int chance)
		{
			//0-4999
			return rand() % 5000 <= chance;
		}
		//proximity to player or humans, parameter isPlayerTarget should be true if checking proximity to player and false otherwise
		bool InProximityTo(HumanStateMachine& state_machine, GameObject* target, float range) {
			//where are we?
			float posX = state_machine.human->horizontalPosition;
			float posY = state_machine.human->verticalPosition;

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
		float startTime;
		float changeDirectionTime = rand() % 5 + 1;
		bool goingRight = true;
	public:
		IdleState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(HumanStateMachine &state_machine)
		{
			state_machine.current_state = this;
			startTime = system->getElapsedTime();
		}
		virtual void Update(HumanStateMachine& state_machine, float dt) {

			//if abducted go to abducted state
			if (state_machine.human->abducted) {
				state_machine.state_abducted->Enter(state_machine);
			}

			//when idle move side to side 
			float movement = goingRight ? LANDER_SPEED * dt : -LANDER_SPEED * dt;
			state_machine.human->horizontalPosition += movement;

			if ((system->getElapsedTime() - startTime) >= changeDirectionTime) {
				goingRight = (goingRight == false); //flip bool
				changeDirectionTime = rand() % 5 + 1; //new direction time
				startTime = system->getElapsedTime(); //reset time
			}

		}
	};


	class AbductedState : public State
	{
	public:
		AbductedState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(HumanStateMachine &state_machine)
		{
			state_machine.current_state = this;
		}
		virtual void Update(HumanStateMachine& state_machine, float dt) {
			if (state_machine.human->dropped) {
				state_machine.state_dropped->Enter(state_machine);
			}
			state_machine.human->verticalPosition -= dt * LANDER_SPEED;
		}
	};

	class DroppedState : public State
	{

	public:
		float intensity = 0.0f;
		DroppedState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(HumanStateMachine &state_machine)
		{
			state_machine.current_state = this;
		}
		virtual void Update(HumanStateMachine& state_machine, float dt) {

			//get picked up by the player if they are close
			if (InProximityTo(state_machine, state_machine.player, 35)) {
				intensity = 0.0f;
				state_machine.human->dropped = false;
				state_machine.state_carried->Enter(state_machine);
			}

			//either die or go to idle when you get to bottom
			if (state_machine.human->verticalPosition >= 620) {
				intensity = 0.0f;
				state_machine.human->dropped = false;
				if (intensity >= 7.0f) {
					state_machine.human->enabled = false;
				}
				else {
					state_machine.state_idle->Enter(state_machine);
				}
			}

			state_machine.human->verticalPosition += dt * LANDER_SPEED * intensity;
			intensity += 0.01f;
		}
	};

	class CarriedState : public State
	{

	public:
		CarriedState(AvancezLib* system)
		{
			this->system = system;
		}

		virtual void Enter(HumanStateMachine &state_machine)
		{
			state_machine.human->carried = true;
			state_machine.current_state = this;
			state_machine.carryNumber = 1 + state_machine.player->carriedHumans++;
		}
		virtual void Update(HumanStateMachine& state_machine, float dt) {

			if (state_machine.human->verticalPosition >= 620) {
				state_machine.player->carriedHumans--;
				state_machine.human->carried = false;
				state_machine.player->score += POINTS_PER_HUMAN;
				state_machine.state_idle->Enter(state_machine);
			}

			state_machine.human->verticalPosition = state_machine.player->verticalPosition + (34 * state_machine.carryNumber);
			state_machine.human->horizontalPosition = state_machine.player->leftFacing ? state_machine.player->horizontalPosition + 45 : state_machine.player->horizontalPosition;
		}
	};

public:

	AvancezLib  * system;
	Human		* human;
	Player		* player;
	
	State *				current_state;
	IdleState *			state_idle;
	AbductedState *		state_abducted;
	DroppedState *		state_dropped;
	CarriedState *		state_carried;

	int carryNumber;


	virtual ~HumanStateMachine() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Player* player)
	{
		Component::Create(system, go, game_objects);

		this->system = system;
		human = (Human*)go;
		this->player = player;
	}

	virtual void Init()
	{
		state_idle = new IdleState(system);
		state_abducted = new AbductedState(system);
		state_dropped = new DroppedState(system);
		state_carried = new CarriedState(system);

		current_state = state_idle;
	}

	// Main loop
	virtual void Update(float dt)
	{
		// No need for input
		current_state->Update(*this, dt);
	}
};
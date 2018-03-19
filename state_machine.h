#pragma once
class StateMachine : public Component
{

public:
	AvancezLib *	system;
	State *			current_state;

	GameObject *	state_machine_object;
	

	virtual ~StateMachine() {}
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects) {
		Component::Create(system, go, game_objects);
		this->system = system;
		state_machine_object = go;
	}

	virtual void Init() {}

	// Main loop
	virtual void Update(float dt)
	{
		// No need for input
		current_state->Update(*this, dt);
	}


};

// The state interface
class State
{
public:
	AvancezLib * system;
public:
	virtual ~State() {}
	virtual void Enter(StateMachine& state_machine) {}
	virtual void Update(StateMachine& state_machine, float dt) {}

	//parameter chance is the 1/5000 percent chance of getting a true
	bool Chance(int chance)
	{
		//0-4999
		return rand() % 5000 <= chance;
	}

	//proximity to player or humans, parameter isPlayerTarget should be true if checking proximity to player and false otherwise
	bool InProximityTo(StateMachine state_machine, GameObject* target, float range) {
		//where are we?
		float posX = state_machine.state_machine_object->horizontalPosition;
		float posY = state_machine.state_machine_object->verticalPosition;

		//where is target?
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

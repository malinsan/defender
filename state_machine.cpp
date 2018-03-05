
#include "state_machine.h"

void StateMachine::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects) {
	Component::Create(system, go, game_objects);
	this->system = system;
	state_machine_object = go;
}


void StateMachine::Update(float dt)
{
	// No need for input
	current_state->Update(*this, dt);
}


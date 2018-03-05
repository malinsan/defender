
#include "state.h"
#include "state_machine.h"
#include "game_object.h"

//proximity to player or humans, parameter isPlayerTarget should be true if checking proximity to player and false otherwise
bool State::InProximityTo(StateMachine& state_machine, GameObject* target, float range) {
	//where are we?
	float posX = state_machine.state_machine_object->horizontalPosition;
	float posY = state_machine.state_machine_object->verticalPosition;

	//where is actor?
	if (target == NULL) {
		return false;
	}
	float targetPosX = target->horizontalPosition;
	float targetPosY = target->verticalPosition;

	return Distance(posX, posY, targetPosX, targetPosY) <= range;

}

//returns distance between point a and b using pythagorean theorem
float State::Distance(float posX, float posY, float targetX, float targetY) {
	float lengthA = (float)pow(abs(posX - targetX), 2);
	float lengthB = (float)pow(abs(posY - targetY), 2);
	float distance = (float)sqrt(lengthA + lengthB);
	return distance;
}
#pragma once

// rockets are shot by the player towards the aliens
class Bomb : public GameObject
{

public:

	bool leftFacing;

	virtual void Init(double xPos, double yPos, bool leftFacing)
	{
		SDL_Log("Bomb::Init");
		GameObject::Init();

		horizontalPosition = xPos;
		verticalPosition = yPos;
		this->leftFacing = leftFacing;

	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Rocket::Hit");
		}
	}
};


class BombBehaviourComponent : public Component
{

	bool goingLeft = true;
	bool goingBack = false;

	bool playerGoingLeft = false;
	bool playerGoingRight = false;

public:

	void Update(float dt)
	{
		
		Bomb* bomb = (Bomb*)go;
		
		if (bomb->leftFacing) { //rocket going left
			go->horizontalPosition -= BOMB_SPEED * dt;
		}
		else { // rocket going right
			go->horizontalPosition += BOMB_SPEED * dt;
		}

		if (go->horizontalPosition < 0 || go->horizontalPosition > WORLD_WIDTH) // When the rocket reaches the ends of the screen, it disappears.
			go->enabled = false;
	}

};
#pragma once
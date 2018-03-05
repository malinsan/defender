

// rockets are shot by the player towards the aliens
class Rocket : public GameObject
{

public:

	bool leftFacing;

	virtual void Init(double xPos, double yPos, bool leftFacing)
	{
		SDL_Log("Rocket::Init");
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


class RocketBehaviourComponent : public Component
{

	bool goingLeft = true;
	bool goingBack = false;

public:

	void Update(float dt)
	{

		Rocket* rocket = (Rocket*)go;
		

		if (rocket->leftFacing) { //rocket going left
			go->horizontalPosition -= ROCKET_SPEED * dt; 
		}
		else { // rocket going right
			go->horizontalPosition += ROCKET_SPEED * dt;
		}
		


		if (go->horizontalPosition <= 0 || go->horizontalPosition >= WORLD_WIDTH) // When the rocket reaches the ends of the screen, it disappears.
			go->enabled = false;
	}

};

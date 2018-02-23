

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
		float mult = goingBack ? 2.0f : 1.0f;
		goingBack = false;


		if (rocket->leftFacing) { //rocket going left
			go->horizontalPosition -= ROCKET_SPEED * dt * mult; 
		}
		else { // rocket going right
			go->horizontalPosition += ROCKET_SPEED * dt * mult;
		}
		
		if (go->horizontalPosition < 0) // When the rocket reaches the ends of the screen, it disappears.
			go->enabled = false;
	}

	void Receive(Message m) {
		if (m == GOING_BACK) {
			goingBack = true;
		}

	}

};



// rockets are shot by the player towards the aliens
class Rocket : public GameObject
{

public:

	bool leftFacing;
	float startPoint;
	float width;

	virtual void Init(double xPos, double yPos, bool leftFacing)
	{
		SDL_Log("Rocket::Init");
		GameObject::Init();

		horizontalPosition = xPos;
		verticalPosition = yPos + 13;
		this->leftFacing = leftFacing;
		startPoint = horizontalPosition;
		width = 0;

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

class RocketRenderComponent : public Component
{
	float startTime;
public:
	
	void Init() {
		startTime = system->getElapsedTime();
	}
	
	void Update(float dt) 
	{
		Rocket* rocket = (Rocket*)go;

		float height = 4; 

		//color
		int R = rand() % 255 + 100;
		int G = rand() % 255 + 100;
		int B = rand() % 255 + 100;

		if (system->getElapsedTime() - startTime < 0.5f) {
			rocket->width += 315 * dt;
		}
		else {
			rocket->width -= 315 * dt;
		}

		if (rocket->leftFacing) {
			rocket->startPoint = go->horizontalPosition;
			system->drawRect(rocket->startPoint, rocket->verticalPosition, rocket->width, height, R, G, B);
		}
		else {
			rocket->startPoint = go->horizontalPosition - rocket->width;
			system->drawRect(rocket->startPoint, rocket->verticalPosition, rocket->width, height, R, G, B);
		}

	}

};


class RocketBehaviourComponent : public Component
{

	bool goingLeft = true;
	bool goingBack = false;

	float startTime;
	float time = 1.0f;

public:

	void Init() {
		startTime = system->getElapsedTime();
	}

	void Update(float dt)
	{
		Rocket* rocket = (Rocket*)go;

		if (system->getElapsedTime() - startTime >= time) { //disappear after a certain amount of param time
			go->enabled = false;
		}
		

		if (rocket->leftFacing) { //rocket going left
			go->horizontalPosition -= dt * ROCKET_SPEED;
		}
		else { // rocket going right
			go->horizontalPosition += dt * ROCKET_SPEED;	
		}
	}

};

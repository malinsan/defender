

// rockets are shot by the player towards the aliens
class Rocket : public GameObject
{

public:

	bool leftFacing;
	float endPoint;

	virtual void Init(double xPos, double yPos, bool leftFacing)
	{
		SDL_Log("Rocket::Init");
		GameObject::Init();

		horizontalPosition = xPos;
		verticalPosition = yPos + 13;
		this->leftFacing = leftFacing;
		endPoint = 0;

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

class RocketRender : public Component
{
public:
	
	
	void Update(float dt) 
	{

	}

};


class RocketBehaviourComponent : public Component
{

	bool goingLeft = true;
	bool goingBack = false;

	float endPoint = 0;
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
			rocket->endPoint = 0;
			go->enabled = false;
		}
		rocket->endPoint += 0.7f;
		float h = 4;
		float w = rocket->endPoint;
		
		//color
		int R = rand() % 255 + 100;
		int G = rand() % 255 + 100;
		int B = rand() % 255 + 100;
		

		if (rocket->leftFacing) { //rocket going left
			go->horizontalPosition -= dt * ROCKET_SPEED;
			system->drawRect(go->horizontalPosition + rocket->endPoint, go->verticalPosition, -w, h, R, G, B);
		}
		else { // rocket going right
			go->horizontalPosition += dt * ROCKET_SPEED;
			system->drawRect(go->horizontalPosition - rocket->endPoint, go->verticalPosition, w, h, R, G, B);		
		}
	}

};

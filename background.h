class Background : public GameObject 
{
public:


	virtual void Init() {

		GameObject::Init();

		//width = 2400
		//middle = 1200
		//screen width 1200, /2 = 600
		horizontalPosition = -600; //offset so we draw it in the middle
		verticalPosition = 0;

	}

	virtual void Receive(Message m) {



	}


};

class BackgroundBehaviourComponent : public Component
{
public:

	bool moveLeft = false;
	bool moveRight = false;

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float xPos, float yPos)
	{
		Component::Create(system, go, game_objects);
		go->horizontalPosition = xPos;
		go->verticalPosition = yPos;

	}

	
	virtual void Receive(Message m) {
		if (m == GOING_LEFT) {
			moveLeft = true;
		}
		if (m == GOING_RIGHT) {
			moveRight = true;
		}
	}

	virtual void Update(float dt) 
	{
		if (moveLeft) {
			moveLeft = false;
			Move(dt * PLAYER_SPEED );
		}
		if (moveRight) {
			moveRight = false;
			Move(- dt * PLAYER_SPEED);
		}

	}

	void Move(float move) {

		go->horizontalPosition += move;
		
		//going right wraparound
		if (go->horizontalPosition < - WORLD_WIDTH) {
			go->horizontalPosition = 0;
		}
		//left wraparound
		if (go->horizontalPosition > 0) {
			go->horizontalPosition = - WORLD_WIDTH;
		}

	}
};
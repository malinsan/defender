class Background : public GameObject 
{
public:


	virtual void Init() {

		GameObject::Init();

		horizontalPosition = -(WORLD_WIDTH / 2); //offset so we draw it in the middle
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
	bool goingBack = false;
	bool playerTeleported = false;
	
	Player * player;

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Player * player, float xPos, float yPos)
	{
		Component::Create(system, go, game_objects);
		go->horizontalPosition = xPos;
		go->verticalPosition = yPos;
		this->player = player;
	}

	
	virtual void Receive(Message m) {
		if (m == GOING_LEFT) {
			moveLeft = true;
		}
		if (m == GOING_RIGHT) {
			moveRight = true;
		}
		if (m == GOING_BACK) {
			goingBack = true;
		}
		if (m == TELEPORTED) {
			playerTeleported = true;
		}
	}

	virtual void Update(float dt) 
	{
		float mult = goingBack ? 1.5f : 1.0f;
		goingBack = false;

		if (playerTeleported) { //move so that player is in the middle
			go->horizontalPosition -= (rand() % ((WORLD_WIDTH/2)-WIDTH)) + WIDTH;
			Move(dt); //to fix wrapping

			//SDL_Log(player->horizontalPosition);
			playerTeleported = false;
		}

		if (moveLeft) {
			moveLeft = false;
			Move(dt * PLAYER_SPEED * mult);
		}
		if (moveRight) {
			moveRight = false;
			Move(- dt * PLAYER_SPEED * mult);
		}

	}

	void Move(float move) {

		go->horizontalPosition += move;

		//going right wraparound
		if (go->horizontalPosition < -(WORLD_WIDTH / 2)) {
			go->horizontalPosition = 0 + move;
		}
		//left wraparound
		if (go->horizontalPosition > 0) {
			go->horizontalPosition = -(WORLD_WIDTH / 2) + move;
		}

	}
};
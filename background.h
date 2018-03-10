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

		if (playerTeleported) { //move so that player is in the middle
			playerTeleported = false;

			go->horizontalPosition -= (rand() % ((WORLD_WIDTH/2)-WIDTH)) + WIDTH;
			Move(dt); //to fix wrapping
		}

		//decrease velocity if buttons are released. i.e go towards 0
		if (!moveLeft && !moveRight) {
			if (go->velocity.x > 1) {
				go->velocity.x -= 1.0f;
			}
			else if (go->velocity.x < -1) {
				go->velocity.x += 1.0f;
			}
		}

		if (goingBack) {
			goingBack = false;
			float movement = PLAYER_MAX_VELOCITY * dt * 0.5f * (moveLeft ? 1 : -1);
			go->horizontalPosition += movement;
		}

		if (moveLeft) {
			moveLeft = false;
			if (go->velocity.x < PLAYER_MAX_VELOCITY) {
				go->velocity.x += PLAYER_ACCELERATION * dt;
			}
			//Move(dt * PLAYER_SPEED * mult);
		}
		if (moveRight) {
			moveRight = false;

			if (go->velocity.x > -PLAYER_MAX_VELOCITY) {
				go->velocity.x -= PLAYER_ACCELERATION * dt;
			}
			//Move(- dt * PLAYER_SPEED * mult);
		}
		Move(dt);
	}

	void Move(float move) {

		go->horizontalPosition += go->velocity.x * move;

		//going right wraparound
		if (go->horizontalPosition < -(WORLD_WIDTH / 2)) {
			go->horizontalPosition = 0 - move;
		}
		//left wraparound
		if (go->horizontalPosition > 0) {
			go->horizontalPosition = -(WORLD_WIDTH / 2) + move;
		}
	}
};
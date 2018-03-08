// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	int score;
	int smartBombs;

	int carriedHumans;

	bool leftFacing = true;
	
	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();

		lives = NUM_LIVES;
		score = 0;
		smartBombs = NUM_SMARTBOMBS;

		carriedHumans = 0;
		
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			SDL_Log("Player::Hit!");
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == ALIEN_HIT) {
			score += POINTS_PER_ALIEN;
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};


class PlayerBehaviourComponent : public Component
{

	Player * thisPlayer;
	float time_fire_pressed;	// time from the last time the fire button was pressed
	ObjectPool<Rocket> * rockets_pool;
	
	//teleport stuff
	float time_teleported;
	float teleport_cooldown = 3.0f;

	bool movingHorizontally = true;
	//bool leftFacing = true;
	
public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Rocket> * rockets_pool)
	{
		Component::Create(system, go, game_objects);
		this->rockets_pool = rockets_pool;
		thisPlayer = (Player *)go;

	}

	virtual void Init()
	{
		go->horizontalPosition = 320;
		go->verticalPosition = 480 - 32;
		
		go->horizontalVelocity = 0.0f;
		go->verticalVelocity = 0.0f;

		time_fire_pressed = -10000.f;
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);
		

		
		if (keys.down) {
			movingHorizontally = false;
			Move(dt * PLAYER_SPEED);
		}

		if (keys.up) {
			movingHorizontally = false;
			Move(-dt * PLAYER_SPEED);
		}
		if (keys.right) {
			movingHorizontally = true;
			thisPlayer->leftFacing = false;
			Send(GOING_RIGHT); //tell rendering to change sprite
			Move(dt * PLAYER_SPEED);
		}

		if (keys.left) {
			movingHorizontally = true;
			thisPlayer->leftFacing = true;
			Send(GOING_LEFT);
			Move(-dt * PLAYER_SPEED);
		}
		if (keys.fire)
		{
			if (CanFire())
			{
				// fetches a rocket from the pool and use it in game_objects
				Rocket * rocket = rockets_pool->FirstAvailable();
				if (rocket != NULL)	// rocket is NULL is the object pool can not provide an object
				{
					int x = thisPlayer->leftFacing ? -PLAYER_WIDTH : PLAYER_WIDTH; //offset from player so rockets doesn't start from middle of player
					rocket->Init(go->horizontalPosition + x, go->verticalPosition, thisPlayer->leftFacing);
					game_objects->insert(rocket);
				}

				Send(SHOOT); //for playing sound
				
			}
		}
		//teleportation
		if (keys.teleport) {
			if ((system->getElapsedTime() - time_teleported) > teleport_cooldown) {
				Teleport(dt);
				time_teleported = system->getElapsedTime();
			}
		}
	}

	//teleport the player to a random position on the screen 
	void Teleport(float dt) {
		Send(TELEPORTED); //background update, animation, play sound
		go->horizontalPosition = WIDTH / 2; //put player in the middle of the screen

	}

	// move the player left or right, up or down
	// param move depends on the time, so the player moves always at the same speed on any computer
	void Move(float move)
	{
		if (movingHorizontally) {
			
			//going to the right
			//move the ship backwards and the background forwards
			if (go->horizontalPosition > 400 && !thisPlayer->leftFacing) {
				Send(GOING_BACK); //send to rocket 
				go->horizontalPosition -= move * 0.5; // *0.5 to offset the background moving the other way 
			}

			//going to the left
			if (go->horizontalPosition < 800 && thisPlayer->leftFacing) {
				Send(GOING_BACK);
				go->horizontalPosition -= move * 0.5;
			}
		}
		else { //moving vertically
			go->verticalPosition += move;
			if (go->verticalPosition < 0) {
				go->verticalPosition = 0 + PLAYER_HEIGHT;
			}
			if (go->verticalPosition > 670) {
				go->verticalPosition = 670 - PLAYER_HEIGHT;
			}
		}
	}

	// return true if enough time has passed from the previous rocket
	bool CanFire()
	{
		// shoot just if enough time passed by
		if ((system->getElapsedTime() - time_fire_pressed) < (FIRE_TIME_INTERVAL / game_speed))
			return false;

		time_fire_pressed = system->getElapsedTime();

		SDL_Log("fire!");
		return true;
	}
};



//could become the new render component instead actually 
class PlayerRenderComponent : public Component 
{
	Sprite* leftSprite;
	Sprite* leftActiveSprite;
	Sprite* rightSprite;
	Sprite* rightActiveSprite;

	Sprite * currentInactiveSprite;
	Sprite * currentSprite;

	bool active = false;

	//teleportation
	Sprite* oldSprite;
	Sprite* firstTPSprite;
	Sprite* secondTPSprite;
	bool teleporting = false;
	int tpFrames = 0;
	const int TOTAL_TP_FRAMES = 200;

public:
	virtual ~PlayerRenderComponent() {}


	virtual void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, 
		const char * left_sprite_name, const char * right_sprite_name,
		const char * leftActive_sprite, const char * rightActive_sprite,
		const char * first_TP_sprite, const char * second_TP_sprite)
	{
		Component::Create(system, go, game_objects);

		leftSprite = system->createSprite(left_sprite_name);
		leftActiveSprite = system->createSprite(leftActive_sprite);
		rightSprite = system->createSprite(right_sprite_name);
		rightActiveSprite = system->createSprite(rightActive_sprite);
		//teleport
		firstTPSprite = system->createSprite(first_TP_sprite);
		secondTPSprite = system->createSprite(second_TP_sprite);

		currentInactiveSprite = rightSprite;

	}

	virtual void Update(float dt) {
		if (!go->enabled)
			return;

#pragma region Teleport

		//teleportation
		if (teleporting) {
			if (tpFrames < TOTAL_TP_FRAMES) {
				if (tpFrames < TOTAL_TP_FRAMES / 2) {
					currentSprite = firstTPSprite;
					tpFrames++;
				}
				else {
					currentSprite = secondTPSprite;
					tpFrames++;
				}
			}
			else { //teleportation over
				teleporting = false;
				tpFrames = 0;
				currentSprite = oldSprite;
			}
		}

#pragma endregion


		if (!teleporting && !active) {
			currentSprite = currentInactiveSprite;
		}

		if (currentSprite)
			currentSprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->angle);

		active = false;

	}
	virtual void Destroy() {
		if (currentSprite != NULL)
			currentSprite->destroy();
		currentSprite = NULL;

	}

	virtual void Receive(Message m) {
		if (m == GOING_LEFT) {
			currentSprite = leftActiveSprite;
			currentInactiveSprite = leftSprite;
			active = true;
		}
		if (m == GOING_RIGHT) {
			currentSprite = rightActiveSprite;
			currentInactiveSprite = rightSprite;
			active = true;
		}
		if (m == TELEPORTED) {
			oldSprite = currentSprite; //save which way we were going
			teleporting = true;
		}
	}


};



// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	bool shooting; //for sound component

	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		lives = NUM_LIVES;
		shooting = false;
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
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};


class PlayerBehaviourComponent : public Component
{

	float time_fire_pressed;	// time from the last time the fire button was pressed
	ObjectPool<Rocket> * rockets_pool;

	bool movingHorizontally = true;
	bool leftFacing = true;
	
public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Rocket> * rockets_pool)
	{
		Component::Create(system, go, game_objects);
		this->rockets_pool = rockets_pool;
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
			leftFacing = false;
			Send(GOING_RIGHT); //tell rendering to change sprite
			Move(dt * PLAYER_SPEED);
		}

		if (keys.left) {
			movingHorizontally = true;
			leftFacing = true;
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
					int x = leftFacing ? -PLAYER_WIDTH : PLAYER_WIDTH; //offset from player so rockets doesn't start from middle of player
					rocket->Init(go->horizontalPosition + x, go->verticalPosition, leftFacing);
					game_objects->insert(rocket);
				}

				Send(SHOOT); //for playing sound
				
			}
		}
	}


	// move the player left or right, up or down
	// param move depends on the time, so the player moves always at the same speed on any computer
	void Move(float move)
	{
		if (movingHorizontally) {
			
			//going to the right
			//move the ship backwards and the background forwards
			if (go->horizontalPosition > 400 && !leftFacing) {
				Send(GOING_BACK); //send to rocket 
				go->horizontalPosition -= move * 2; // *2 to offset the background moving the other way 
			}

			//going to the left
			if (go->horizontalPosition < 800 && leftFacing) {
				Send(GOING_BACK);
				go->horizontalPosition -= move * 2;
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
	Sprite* rightSprite;
	Sprite * currentSprite;

public:
	virtual ~PlayerRenderComponent() {}


	virtual void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * left_sprite_name, const char * right_sprite_name)
	{
		Component::Create(system, go, game_objects);

		leftSprite = system->createSprite(left_sprite_name);
		rightSprite = system->createSprite(right_sprite_name);
		currentSprite = leftSprite;
	}

	virtual void Update(float dt) {
		if (!go->enabled)
			return;

		if (currentSprite)
			currentSprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->angle);

	}
	virtual void Destroy() {
		if (currentSprite != NULL)
			currentSprite->destroy();
		currentSprite = NULL;

	}

	virtual void Receive(Message m) {
		if (m == GOING_LEFT) {
			currentSprite = leftSprite;
		}
		if (m == GOING_RIGHT) {
			currentSprite = rightSprite;
		}
	}


};



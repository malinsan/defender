#pragma once

class Lander : public GameObject
{
public:

	Human * abductedHuman;
	bool bumped = false;

	virtual ~Lander() { SDL_Log("Lander::Lander"); }

	virtual void Init(float xPos, float yPos)
	{
		SDL_Log("Lander::Init");
		GameObject::Init();
		horizontalPosition = xPos;
		verticalPosition = yPos;
	}

	virtual void Receive(Message m) {
		if (!enabled) {
			return;
		}
		if (m == HIT) {
			if (abductedHuman != NULL) {
				abductedHuman->Receive(DROPPED);
			}
			Send(ALIEN_HIT);
			enabled = false;
		}
		if (m == BUMP_HIT) {
			bumped = true;
		}
	}


};


class MoveAccordingToPlayerComponent : public Component 
{
	bool moveLeft = false;
	bool moveRight = false;
	bool goingBack = false;
	bool wrapAround = false;	
	Player * player;

	
public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Player * player, bool wrapAround)
	{
		Component::Create(system, go, game_objects);
		this->wrapAround = wrapAround;
		this->player = player;
	}

	virtual void Update(float dt) 
	{

		if (goingBack) {
			goingBack = false;
			float movement = PLAYER_MAX_VELOCITY * dt * 0.5f * (moveLeft ? 1 : -1);
			go->horizontalPosition += movement;
		}

		if (moveLeft || moveRight) {
			moveLeft = false; moveRight = false;
			Move(dt);
		}

	}

	void Move(float move) {

		go->horizontalPosition -= player->velocity.x * move;

		if (wrapAround) {
			//going right wraparound
			if (go->horizontalPosition >= WORLD_WIDTH) {
				go->horizontalPosition = -WORLD_WIDTH;
			}
			//left wraparound
			if (go->horizontalPosition < -WORLD_WIDTH) {
				go->horizontalPosition = WORLD_WIDTH;
			}
		}
		
	}

	virtual void Receive(Message m) {
		//move left if the player is moving left 
		if (m == GOING_LEFT) {
			moveLeft = true;
		}

		//move right if player going right
		if (m == GOING_RIGHT) {
			moveRight = true;
		}

		if (m == GOING_BACK) {
			goingBack = true;
		}
	}

};
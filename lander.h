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

	
public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float xPos, float yPos, bool wrapAround)
	{
		Component::Create(system, go, game_objects);
		go->horizontalPosition = xPos;
		go->verticalPosition = yPos;
		this->wrapAround = wrapAround;
		
	}

	virtual void Update(float dt) 
	{
		float mult = goingBack ? 1.5f : 1.0f;
		goingBack = false;

		if (moveLeft) {
			moveLeft = false;
			Move(dt * PLAYER_SPEED * mult);
		}
		if (moveRight) {
			moveRight = false;
			Move(-dt * PLAYER_SPEED * mult);
		}

	}

	void Move(float move) {

		go->horizontalPosition += move;

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
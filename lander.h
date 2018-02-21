#pragma once

class Lander : public GameObject
{

	virtual ~Lander() { SDL_Log("Lander::Lander"); }

	virtual void Init()
	{
		SDL_Log("Lander::Init");
		GameObject::Init();
	}


};

class LanderBehaviourComponent : public Component 
{
	float xPos, yPos;
	float moveX, moveY;
	bool moveLeft;
	bool moveRight;

public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float xPos, float yPos)
	{
		Component::Create(system, go, game_objects);
		this->xPos = xPos;
		this->yPos = yPos;
		moveX = 0.0f;
		moveY = 0.0f;
	}

	virtual void Init()
	{
		moveLeft = false;
		moveRight = false;
		go->horizontalPosition = xPos;
		go->verticalPosition = yPos;

	}

	virtual void Update(float dt) 
	{
		if (moveLeft) {
			moveLeft = false;
			Move(dt * PLAYER_SPEED);
		}
		if (moveRight) {
			moveRight = false;
			Move(-dt * PLAYER_SPEED);
		}

	}

	void Move(float move) {
		go->horizontalPosition += move;
	}

	virtual void Receive(Message m) {
		//move left if the player is moving left 
		if (m == GOING_LEFT) {
			moveLeft = true;
		}
		if (m == GOING_RIGHT) {
			moveRight = true;
		}
	}

};
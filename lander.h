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
	bool moveLeft = false;
	bool moveRight = false;
	
public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float xPos, float yPos)
	{
		Component::Create(system, go, game_objects);
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

		//going right wraparound
		if (go->horizontalPosition >= WORLD_WIDTH) {
			go->horizontalPosition = 0;
		}
		//left wraparound
		if(go->horizontalPosition < 0){
			go->horizontalPosition = WORLD_WIDTH;
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
	}

};
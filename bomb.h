#pragma once

// rockets are shot by the player towards the aliens
class Bomb : public GameObject
{

public:

	bool leftFacing;

	virtual void Init(double xPos, double yPos, bool leftFacing)
	{
		SDL_Log("Bomb::Init");
		GameObject::Init();

		horizontalPosition = xPos;
		verticalPosition = yPos;
		this->leftFacing = leftFacing;

	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Bomb::Hit");
		}
	}
};


class BombBehaviourComponent : public Component
{
	float startTime;
	float time_to_exist = 2.0f;

	//homing missiles
	Player * player;
	Vector2 movement;

	
public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Player * player)
	{
		Component::Create(system, go, game_objects);
		this->player = player;
	}

	virtual void Init() {
		Bomb * bomb = (Bomb*)go;

		movement.x =  bomb->leftFacing? -BOMB_SPEED : BOMB_SPEED;
		startTime = system->getElapsedTime();
	}

	void Update(float dt)
	{
		if ((system->getElapsedTime() - startTime) > time_to_exist) {
			go->enabled = false;
		}

		
		//homing missiles
		movement.y = player->verticalPosition > go->verticalPosition ? BOMB_SPEED : -BOMB_SPEED;

		
		go->horizontalPosition += movement.x * dt;
		go->verticalPosition += movement.y * dt;



		if (go->horizontalPosition < 0 || go->horizontalPosition > WORLD_WIDTH) // When the rocket reaches the ends of the screen, it disappears.
			go->enabled = false;
	}

};
#pragma once

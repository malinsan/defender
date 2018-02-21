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
		go->horizontalPosition = xPos;
		go->verticalPosition = yPos;

	}

	virtual void Update(float dt) 
	{
		go->horizontalPosition += go->horizontalVelocity * dt;
		go->verticalPosition += go->verticalVelocity * dt;
	}

};
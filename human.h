#pragma once
class Human : public GameObject 
{
public:
	virtual ~Human() {}

	virtual void Init(float xPos)
	{
		SDL_Log("Human Init");
		GameObject::Init();
		horizontalPosition = xPos;
		verticalPosition = 620;
	}
};

class HumanBehaviourComponent : public Component
{
	bool abducted = false;
	bool goingRight = true;
	float startTime;
	float changeDirectionTime;
public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);
		startTime = system->getElapsedTime();
		changeDirectionTime = rand() % 5 + 1;
	}

	virtual void Update(float dt) 
	{
		if (!abducted) { //move side to side 
			float movement = goingRight ? HUMAN_SPEED * dt : -HUMAN_SPEED * dt;
			go->horizontalPosition += movement;

			if ((system->getElapsedTime() - startTime) > changeDirectionTime) {
				goingRight = (goingRight == false); //flip bool
				startTime = system->getElapsedTime(); //reset time
			}
		}
	}
};
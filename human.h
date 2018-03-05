#pragma once
class Human : public GameObject 
{
public:
	bool abducted;
	bool dropped;

	virtual ~Human() {}

	virtual void Init(float xPos)
	{
		SDL_Log("Human Init");
		GameObject::Init();
		horizontalPosition = xPos;
		verticalPosition = 620;
		abducted = false;
	}

	virtual void Receive(Message m) {
		if (m == ABDUCTED) {
			abducted = true;
		}
	}
};

class HumanBehaviourComponent : public Component
{
	bool goingRight = true;
	float startTime;
	float changeDirectionTime;
	Human * thisHuman;
public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);

		thisHuman = (Human*)go;
		startTime = system->getElapsedTime();
		changeDirectionTime = rand() % 5 + 1;
	}

	virtual void Update(float dt) 
	{
		if (!thisHuman->abducted) { //move side to side 
			float movement = goingRight ? HUMAN_SPEED * dt : -HUMAN_SPEED * dt;
			go->horizontalPosition += movement;

			if ((system->getElapsedTime() - startTime) > changeDirectionTime) {
				goingRight = (goingRight == false); //flip bool
				startTime = system->getElapsedTime(); //reset time
			}
		}
		else {
			go->verticalPosition -= LANDER_SPEED * dt;
		}
	}

};
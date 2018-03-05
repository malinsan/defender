#pragma once
class Spawner : public GameObject 
{
	virtual ~Spawner() {}

	virtual void Init() 
	{
		GameObject::Init();
	}
};

class SpawnerComponent : public Component 
{
	float startTime;
	float humanStartTime;
	float spawnTime; //time between spawning of new enemies
	float humanSpawnTime = 0.05f;
	ObjectPool<Lander> * lander_pool;
	ObjectPool<Human> * human_pool;
	int numberOfHumansSpawned = 0;

public:
	virtual ~SpawnerComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Lander>* lander_pool, ObjectPool<Human>* human_pool){
		Component::Create(system, go, game_objects);
		this->lander_pool = lander_pool;
		this->human_pool = human_pool;
	}

	virtual void Init() 
	{
		startTime = system->getElapsedTime();
		humanStartTime = system->getElapsedTime();

		for (int i = 0; i < NUM_HUMANS; i++) {
			SpawnHuman();
		}
	}

	virtual void Update(float dt) 
	{
		SpawnLander();
	}

	void SpawnLander() {
		//varying spawntime
		spawnTime = rand() % 30 + 1;

		//spawn an enemy
		Lander * lander = lander_pool->FirstAvailable();
		if (lander != NULL && (system->getElapsedTime() - startTime) > spawnTime) {
			//random location
			int rX = rand() % WORLD_WIDTH;
			int rY = rand() % (HEIGHT-200);
			float xPos = (float)rX;
			float yPos = (float)rY;

			lander->Init(xPos, yPos);
			game_objects->insert(lander);
			startTime = system->getElapsedTime();
		}

	}

	void SpawnHuman() {
		//spawn all humans
		Human * human = human_pool->FirstAvailable();
		if (human != NULL) {
			int xPos = 10 + (WORLD_WIDTH / 10) * numberOfHumansSpawned;
			human->Init((float)xPos);
			game_objects->insert(human);
			numberOfHumansSpawned++;
			humanStartTime = system->getElapsedTime();
		}
	}
};
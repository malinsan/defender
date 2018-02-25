#pragma once
class EnemySpawner : public GameObject 
{
	virtual ~EnemySpawner() {}

	virtual void Init() 
	{
		GameObject::Init();
	}
};

class SpawnEnemiesComponent : public Component 
{
	float startTime;
	float spawnTime = 1.0f;
	ObjectPool<Lander> * lander_pool;

public:
	virtual ~SpawnEnemiesComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Lander>* lander_pool){
		Component::Create(system, go, game_objects);
		this->lander_pool = lander_pool;
	}

	virtual void Init() 
	{
		startTime = system->getElapsedTime();
	}

	virtual void Update(float dt) 
	{
		//spawn an enemy
		Lander * lander = lander_pool->FirstAvailable();
		float xPos = rand() % WORLD_WIDTH;
		float yPos = rand() % HEIGHT;
		if (lander != NULL && (system->getElapsedTime() - startTime) > spawnTime) {
			lander->Init(xPos, yPos);
			game_objects->insert(lander);
			startTime = system->getElapsedTime();
		}
	}
};
#pragma once
class Spawner : public GameObject 
{
public:
	bool spawn;
	bool mutantSpawn;
	int mutant_waves;
	int NUM_ALIENS_TO_SPAWN = NUM_ALIENS;
	float waveMultiplier = 0.6666f;

	virtual ~Spawner() {}

	virtual void Init() 
	{
		GameObject::Init();
		spawn = true;
		mutantSpawn = false;
		mutant_waves = 0;
	}

	void Receive(Message m) {
		if (m == NEW_WAVE || m == NEW_MUTANT_WAVE) {
			NUM_ALIENS_TO_SPAWN += NUM_ALIENS_TO_SPAWN * waveMultiplier;
			spawn = true;
			mutantSpawn = false;
		}
		if (m == NEW_MUTANT_WAVE) {
			mutant_waves++;
			mutantSpawn = true;
		}
	}
};

class SpawnerComponent : public Component 
{
	Spawner * spawner;

	float startTime;
	float humanStartTime;
	float spawnTime; //time between spawning of new enemies
	float humanSpawnTime = 0.05f;
	ObjectPool<Lander> * lander_pool;
	ObjectPool<Mutant> * mutant_pool;
	ObjectPool<Human> * human_pool;

	int numberOfEnemiesSpawned = 0;
	int numberOfHumansSpawned = 0;

public:
	virtual ~SpawnerComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Lander>* lander_pool, ObjectPool<Mutant>* mutant_pool, ObjectPool<Human>* human_pool){
		Component::Create(system, go, game_objects);
		this->lander_pool = lander_pool;
		this->mutant_pool = mutant_pool;
		this->human_pool = human_pool;
		spawner = (Spawner*)go;
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
		if (!spawner->mutantSpawn && spawner->mutant_waves == 2) {
			spawner->mutant_waves = 0;
			for (int i = 0; i < NUM_HUMANS; i++) {
				SpawnHuman();
			}
		}

		if (spawner->spawn) {
			if (numberOfEnemiesSpawned < spawner->NUM_ALIENS_TO_SPAWN) {
				if (spawner->mutantSpawn) {
					SpawnMutant();
				}
				else {
					SpawnLander();
				}
				Send(ALIEN_SPAWNED);
			}
			else {
				spawner->spawn = false;
				numberOfEnemiesSpawned = 0;
			}
		}

		//SpawnLander();
		
	}
	void SpawnMutant() {
		//varying spawntime
		spawnTime = rand() % 30 + 1;

		//spawn an enemy
		Mutant * mutant = mutant_pool->FirstAvailable();
		if (mutant != NULL && (system->getElapsedTime() - startTime) > spawnTime) {
			//random location
			int rX = rand() % WORLD_WIDTH;
			int rY = rand() % (HEIGHT - 400) + 100;
			float xPos = (float)rX;
			float yPos = (float)rY;

			mutant->Init(xPos, yPos);
			game_objects->insert(mutant);

			numberOfEnemiesSpawned++;
			startTime = system->getElapsedTime();
		}
	}

	void SpawnLander() {
		//varying spawntime
		spawnTime = rand() % 30 + 1;

		//spawn an enemy
		Lander * lander = lander_pool->FirstAvailable();
		if (lander != NULL && (system->getElapsedTime() - startTime) > spawnTime) {
			//random location
			int rX = rand() % WORLD_WIDTH;
			int rY = rand() % (HEIGHT-400) + 100;
			float xPos = (float)rX;
			float yPos = (float)rY;

			lander->Init(xPos, yPos);
			game_objects->insert(lander);

			numberOfEnemiesSpawned++;
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
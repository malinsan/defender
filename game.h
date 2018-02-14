

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/

	AvancezLib* system;
	FMOD::Studio::System* soundSystem;

	//b2World * world;


	//define the timestep
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	//background 
	Background * background;

	//player & rockets
	Player * player;
	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets


	//background
	
	bool game_over;

	unsigned int score = 0;

public:

	virtual void Create(AvancezLib* system, FMOD::Studio::System* soundSystem)
	{
		SDL_Log("Game::Create");

		this->system = system;
		this->soundSystem = soundSystem;

		StartMusic();
		
		background = new Background();
		RenderComponent* background_render = new RenderComponent();
		background_render->Create(system, background, &game_objects, "data/background.bmp");

		background->Create();
		background->AddComponent(background_render);
		//game_objects.insert(background);


		player = new Player();
		PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects, &rockets_pool);
		RenderComponent * player_render = new RenderComponent();
		player_render->Create(system, player, &game_objects, "data/player.bmp");
		
		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddReceiver(this);
		game_objects.insert(player);

		rockets_pool.Create(30);
		for (auto rocket = rockets_pool.pool.begin(); rocket != rockets_pool.pool.end(); rocket++)
		{
			RocketBehaviourComponent * behaviour = new RocketBehaviourComponent();
			behaviour->Create(system, *rocket, &game_objects);
			RenderComponent * render = new RenderComponent();
			render->Create(system, *rocket, &game_objects, "data/rocket.bmp");
			(*rocket)->Create();
			(*rocket)->AddComponent(behaviour);
			(*rocket)->AddComponent(render);
		}


		score = 0;
	}

	virtual void Init()
	{

		//init background
		background->Init();

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Init();

		enabled = true;
		game_over = false;
	}

	virtual void Update(float dt)
	{
		if (IsGameOver())
			dt = 0.f;

		soundSystem->update();

		//first component should be background
		background->Update(dt);

		for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
			//except for background 
			(*go)->Update(dt);
			
		}
	}

	virtual void Draw()
	{
		char msg[1024];
		sprintf_s(msg, "physics demo");
		system->drawText(300, 32, msg);

		if (IsGameOver())
		{
			sprintf_s(msg, "*** G A M E  O V E R ***");
			system->drawText(250, 8, msg);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;
	}


	bool IsGameOver()
	{
		return game_over;
	}

	unsigned int Score()
	{
		return score;
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		
	}

	void StartMusic() {
		FMOD::Studio::EventDescription* musicDescription = NULL;
		soundSystem->getEvent("event:/music", &musicDescription);
		FMOD::Studio::EventInstance* music = NULL;
		musicDescription->createInstance(&music);

		music->start();

	}


};
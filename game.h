

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/

	AvancezLib* system;
	

	//define the timestep
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	//soundmaker
	SoundMaker* soundMaker;

	//background 
	Background * background;

	//player & rockets
	Player * player;
	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets


	//background
	
	bool game_over;

	unsigned int score = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;
		
		
		//create soundmaker
		soundMaker = new SoundMaker();
		SoundComponent * sound_component = new SoundComponent();
		sound_component->Create(system, soundMaker, &game_objects);
		soundMaker->AddComponent(sound_component);
		game_objects.insert(soundMaker);
		
		
		background = new Background();
		RenderComponent* background_render = new RenderComponent();
		background_render->Create(system, background, &game_objects, "data/background.bmp");
		BackgroundBehaviourComponent * background_component = new BackgroundBehaviourComponent();
		background_component->Create(system, background, &game_objects);

		background->Create();
		background->AddComponent(background_render);
		background->AddComponent(background_component);
		//game_objects.insert(background);


		player = new Player();
		PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects, &rockets_pool);
		//sound component listens to players actions
		player_behaviour->AddReceiver(sound_component);
		//background listens to player movement
		player_behaviour->AddReceiver(background_component);
		RenderComponent * player_render = new RenderComponent();
		player_render->Create(system, player, &game_objects, "data/player.bmp");
		
		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		//player->AddComponent(player_shoot);
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

		//first component should be background
		background->Update(dt); //doesn't run?

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



};
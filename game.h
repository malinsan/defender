

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/

	AvancezLib* system;
	
	//soundmaker
	SoundMaker* soundMaker;

	//background 
	Background * background;

	//player & rockets
	Player * player;
	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets

	//enemy
	Spawner * spawner;
	ObjectPool<Lander> lander_pool;
	ObjectPool<Mutant> mutant_pool;
	//Lander * lander;
	ObjectPool<Bomb> bomb_pool;
	ObjectPool<Human> human_pool;

	//UI Sprites
	Sprite * lifeSprite;
	Sprite * bombSprite;

	bool startScreen = true;
	bool paused;
	float pauseStartTime = -1000.0f;
	float pauseTime = 2.5f;
	bool game_over;

	unsigned int waveNumber = 1;
	unsigned int current_aliens = 0;
	unsigned int current_humans = NUM_HUMANS;
	bool mutant_wave = false;
	int mutant_waves = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;

		player = new Player();
		
		//create soundmaker
		soundMaker = new SoundMaker();
		SoundComponent * sound_component = new SoundComponent();
		sound_component->Create(system, soundMaker, &game_objects);
		soundMaker->AddComponent(sound_component);
		game_objects.insert(soundMaker);
		
		
		background = new Background();
		RenderComponent* background_render = new RenderComponent();
		background_render->Create(system, background, &game_objects, "data/bakgrund.bmp");
		BackgroundBehaviourComponent * background_component = new BackgroundBehaviourComponent();
		background_component->Create(system, background, &game_objects, player, -(WORLD_WIDTH/2), 0);

		background->Create();
		background->AddComponent(background_render);
		background->AddComponent(background_component);
		//game_objects.insert(background);


		PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects, &rockets_pool);
		
		//sound component listens to players actions
		player_behaviour->AddReceiver(sound_component);
		
		//background listens to player movement
		player_behaviour->AddReceiver(background_component);
		
		//player renderering
		PlayerRenderComponent * player_render = new PlayerRenderComponent();
		player_render->Create(system, player, &game_objects, "data/shipL.bmp", "data/shipR.bmp",
			"data/shipLActive.bmp", "data/shipRActive.bmp",
			"data/shipRTeleport1.bmp", "data/shipRTeleport2.bmp");
		player_behaviour->AddReceiver(player_render);

		//collision with bombs
		PlayerBombCollision* bomb_collision = new PlayerBombCollision();
		bomb_collision->Create(system, player, &game_objects, (ObjectPool<GameObject>*)&bomb_pool);

		//collision with landers
		BumbCollideComponent* player_lander_collide = new BumbCollideComponent();
		player_lander_collide->Create(system, player, &game_objects, (ObjectPool<GameObject>*)&lander_pool);
		//collision with mutants
		BumbCollideComponent* player_mutant_collide = new BumbCollideComponent();
		player_mutant_collide->Create(system, player, &game_objects, (ObjectPool<GameObject>*)&mutant_pool);

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddComponent(bomb_collision);
		player->AddComponent(player_lander_collide);
		player->AddComponent(player_mutant_collide);
		player->AddReceiver(this);
		game_objects.insert(player);


		rockets_pool.Create(30);
		for (auto rocket = rockets_pool.pool.begin(); rocket != rockets_pool.pool.end(); rocket++)
		{
			MoveAccordingToPlayerComponent * main_move_behaviour = new MoveAccordingToPlayerComponent();
			main_move_behaviour->Create(system, *rocket, &game_objects, player, false);
			player_behaviour->AddReceiver(main_move_behaviour);

			RocketBehaviourComponent * behaviour = new RocketBehaviourComponent();
			behaviour->Create(system, *rocket, &game_objects);

			RocketRenderComponent * render = new RocketRenderComponent();
			render->Create(system, *rocket, &game_objects);

			(*rocket)->Create();
			(*rocket)->AddComponent(main_move_behaviour);
			(*rocket)->AddComponent(behaviour);
			(*rocket)->AddComponent(render);
		}

		//HUMANS
		human_pool.Create(NUM_HUMANS);
		for (auto human = human_pool.pool.begin(); human != human_pool.pool.end(); human ++) 
		{
			MoveAccordingToPlayerComponent * main_move_behaviour = new MoveAccordingToPlayerComponent();
			main_move_behaviour->Create(system, *human, &game_objects, player, true);
			player_behaviour->AddReceiver(main_move_behaviour);

			HumanStateMachine * behaviour = new HumanStateMachine();
			behaviour->Create(system, *human, &game_objects, player);

			//collision with rockets
			CollideComponent* collision = new CollideComponent();
			collision->Create(system, *human, &game_objects, (ObjectPool<GameObject>*)&rockets_pool, 32, 32);

			RenderComponent * render = new RenderComponent();
			render->Create(system, *human, &game_objects, "data/human.bmp");

			(*human)->Create();
			(*human)->AddComponent(main_move_behaviour);
			(*human)->AddComponent(behaviour);
			(*human)->AddComponent(collision);
			(*human)->AddComponent(render);
			(*human)->AddReceiver(this);
		}

		//#################################################//
		//						ENEMIES					   //
		//#################################################//

		//enemy spawner
		spawner = new Spawner();
		SpawnerComponent * spawn_component = new SpawnerComponent();
		spawn_component->Create(system, spawner, &game_objects, &lander_pool, &mutant_pool, &human_pool);
		spawner->Create();
		spawner->AddComponent(spawn_component);
		this->AddReceiver(spawner); //listen for new wave
		current_aliens = spawner->NUM_ALIENS_TO_SPAWN;

		game_objects.insert(spawner);

		lander_pool.Create(30);
		for (auto lander = lander_pool.pool.begin(); lander != lander_pool.pool.end(); lander++) {
			
			//movement according to player
			MoveAccordingToPlayerComponent* lander_behaviour = new MoveAccordingToPlayerComponent();
			lander_behaviour->Create(system, *lander, &game_objects, player, true);
			
			//listen to player behaviour
			player_behaviour->AddReceiver(lander_behaviour);
			
			//AI behaviour
			LanderStateMachine * landerAI = new LanderStateMachine();
			landerAI->Create(system, *lander, &game_objects, player, &bomb_pool, &human_pool, &mutant_pool);
			
			//ai listens to player behaviour
			player_behaviour->AddReceiver(landerAI);
			
			//render component
			RenderComponent * landerRender = new RenderComponent();
			landerRender->Create(system, *lander, &game_objects, "data/lander.bmp");

			//collision with rockets
			CollideComponent* collision = new CollideComponent();
			collision->Create(system, *lander, &game_objects, (ObjectPool<GameObject>*)&rockets_pool, 28, 32);

			(*lander)->Create();
			(*lander)->AddComponent(lander_behaviour);
			(*lander)->AddComponent(landerAI);
			(*lander)->AddComponent(landerRender);
			(*lander)->AddComponent(collision);
			(*lander)->AddReceiver(player);
			(*lander)->AddReceiver(this); //listen for alien dying
		}

		//mutants
		mutant_pool.Create(30);
		for (auto mutant = mutant_pool.pool.begin(); mutant != mutant_pool.pool.end(); mutant++) {
			//movement according to player
			MoveAccordingToPlayerComponent* main_move_behaviour = new MoveAccordingToPlayerComponent();
			main_move_behaviour->Create(system, *mutant, &game_objects, player, true);
			//listen to player behaviour
			player_behaviour->AddReceiver(main_move_behaviour);
			//AI behaviour
			MutantStateMachine * mutantAI = new MutantStateMachine();
			mutantAI->Create(system, *mutant, &game_objects, player, &bomb_pool);
			player_behaviour->AddReceiver(mutantAI);
			//render component
			RenderComponent * mutantRender = new RenderComponent();
			mutantRender->Create(system, *mutant, &game_objects, "data/mutant.bmp");

			//collision with rockets
			CollideComponent* collision = new CollideComponent();
			collision->Create(system, *mutant, &game_objects, (ObjectPool<GameObject>*)&rockets_pool, 28, 32);

			(*mutant)->Create();
			(*mutant)->AddComponent(main_move_behaviour);
			(*mutant)->AddComponent(mutantAI);
			(*mutant)->AddComponent(mutantRender);
			(*mutant)->AddComponent(collision);
			(*mutant)->AddReceiver(player);
			(*mutant)->AddReceiver(this); //listen for alien dying
		}


		//alien bombs
		bomb_pool.Create(30);
		for (auto bomb = bomb_pool.pool.begin(); bomb != bomb_pool.pool.end(); bomb++)
		{
			MoveAccordingToPlayerComponent * main_move_behaviour = new MoveAccordingToPlayerComponent();
			main_move_behaviour->Create(system, *bomb, &game_objects, player, false);
			player_behaviour->AddReceiver(main_move_behaviour);

			BombBehaviourComponent * behaviour = new BombBehaviourComponent();
			behaviour->Create(system, *bomb, &game_objects, player);
			RenderComponent * render = new RenderComponent();
			render->Create(system, *bomb, &game_objects, "data/bomb.bmp");
			(*bomb)->Create();
			(*bomb)->AddComponent(main_move_behaviour);
			(*bomb)->AddComponent(behaviour);
			(*bomb)->AddComponent(render);
		}

		///////////////////////////////////
		//            U I                //
		///////////////////////////////////
		lifeSprite = system->createSprite("data/shipR.bmp");
		bombSprite = system->createSprite("data/smartbomb.bmp");

	}

	virtual void Init()
	{
		//init background first
		background->Init();

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Init();

		enabled = true;
		paused = false;
		game_over = false;
	}

	virtual void Update(float dt)
	{

		if (startScreen) {
			ShowStartScreen();
		}
		else if(IsGameOver()){
			ShowEndScreen();
		}
		else {

			if ((system->getElapsedTime() - pauseStartTime) < pauseTime) {
				ShowWaveScreen();
			}
			else {
				//first component should be background
				background->Update(dt);

				for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
					(*go)->Update(dt);

				}
			}

		}
	}

	virtual void Draw()
	{
		char msg[1024];
		
		for (int i = 0; i < player->lives; i++) {
			lifeSprite->draw(10 + (PLAYER_WIDTH + 10) * i, 15);
		}

		for (int i = 0; i < player->smartBombs; i++) {
			bombSprite->draw(250, 15 + (i * 20));
		}

		//player score
		sprintf_s(msg, "%07d", player->score);
		system->drawText(30, 50, msg, 129, 54, 255);

		//wave and current aliens
		sprintf_s(msg, "WAVE  %d", waveNumber);
		system->drawText(WIDTH-150, 50, msg, 255, 255, 255);

		sprintf_s(msg, "ALIENS: %d", current_aliens);
		system->drawText(WIDTH-150, 20, msg, 255, 255, 255);

	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;

		if (m == ALIEN_HIT) {
			current_aliens--;
			if (current_aliens == 0) { //wave cleared
				NextWave();
			}
		}

		if (m == HUMAN_HIT) {
			current_humans--;
			if (current_humans == 0) {
				mutant_wave = true;
			}
		}
		
	}

	void NextWave() {
		pauseStartTime = system->getElapsedTime();
		waveNumber++;
		if (mutant_wave && mutant_waves < 2) {
			Send(NEW_MUTANT_WAVE);
			mutant_waves++;
		}
		else {
			mutant_wave = false;
			mutant_waves = 0;
			Send(NEW_WAVE);
		}
		current_aliens = spawner->NUM_ALIENS_TO_SPAWN;
	}

	void ShowWaveScreen() {
		system->drawRect(0, 0, WIDTH, HEIGHT, 0, 0, 0);
		char msg[48];
		sprintf_s(msg, "WAVE  %d  CLEARED", waveNumber - 1);
		system->drawText(WIDTH / 2 - 110, HEIGHT / 2 - 100, msg, 255, 255, 255);
	}

	void ShowStartScreen() {
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);

		system->drawRect(0, 0, WIDTH, HEIGHT, 0, 0, 0);
		char msg[48];
		sprintf_s(msg, "DEFENDER");
		system->drawText(WIDTH / 2 - 110, HEIGHT / 2 - 100, msg, 255, 255, 255);
		sprintf_s(msg, "START BY PRESSING SPACE");
		system->drawText(WIDTH / 2 - 210, HEIGHT / 2 - 50, msg, 255, 255, 255);
		if (keys.fire) {
			startScreen = false;
		}
	}

	void ShowEndScreen() {
		system->drawRect(0, 0, WIDTH, HEIGHT, 0, 0, 0);
		char msg[48];
		sprintf_s(msg, "GAME OVER");
		system->drawText(WIDTH / 2 - 110, HEIGHT / 2 - 100, msg, 255, 255, 255);
		sprintf_s(msg, "SCORE: %d", player->score);
		system->drawText(WIDTH / 2 - 110, HEIGHT / 2 - 50, msg, 255, 255, 255);
	}


	bool IsGameOver()
	{
		return game_over;
	}


	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		lifeSprite->destroy();
		bombSprite->destroy();
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		
	}

};
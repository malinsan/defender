#pragma once
class SoundMaker : public GameObject
{
public:
	virtual void Init()
	{
		SDL_Log("SoundMaker::Init");
		GameObject::Init();
	}
};

class SoundComponent : public Component 
{
public:
	FMOD::Studio::System* soundSystem = NULL;

	
	//sounds
	FMOD::Studio::EventInstance* shoot;

	virtual ~SoundComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);
		
		//init soundsystem
		FMOD::System* lowlevelSystem = NULL;
		//FMOD::Studio::System* fmodSystem = NULL;

		//init fmod system
		FMOD::Studio::System::create(&soundSystem);

		//init to 5.1 sound
		soundSystem->getLowLevelSystem(&lowlevelSystem);
		lowlevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);

		soundSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

		FMOD::Studio::Bank* masterBank = NULL;
		soundSystem->loadBankFile("data/Sound/invaders.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

		FMOD::Studio::Bank* stringsBank = NULL;
		soundSystem->loadBankFile("data/Sound/invaders.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);
		//sound inited

		StartMusic();

		//create all sounds
		//player sound
		FMOD::Studio::EventDescription* shootDescription = NULL;
		soundSystem->getEvent("event:/shoot", &shootDescription);
		//FMOD::Studio::EventInstance* shoot = NULL;
		shootDescription->createInstance(&shoot);

	}

	virtual void Update(float dt)
	{
		//update the sound
		soundSystem->update();

	}

	virtual void Receive(Message m) {
		if (m == SHOOT) {
			shoot->start();
		}

	}

	void StartMusic() {
		FMOD::Studio::EventDescription* musicDescription = NULL;
		soundSystem->getEvent("event:/music", &musicDescription);
		FMOD::Studio::EventInstance* music = NULL;
		musicDescription->createInstance(&music);

		music->setParameterValue("intensity", 0.6f);
		music->start();

		SDL_Log("Music");
	}


};


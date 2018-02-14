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
	FMOD::Studio::EventInstance* soundEvent;

	//sounds
	FMOD::Studio::EventInstance* shoot;

	virtual ~SoundComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, FMOD::Studio::System* soundSystem)
	{
		Component::Create(system, go, game_objects);
		
		//create all sounds
		//player sound
		FMOD::Studio::EventDescription* shootDescription = NULL;
		soundSystem->getEvent("event:/shoot", &shootDescription);
		//FMOD::Studio::EventInstance* shoot = NULL;
		shootDescription->createInstance(&shoot);

	}

	virtual void Update(float dt)
	{

	}

	virtual void Receive(Message m) {
		if (m == SHOOT) {
			shoot->start();
		}

	}



};


#include <set>
#include "object_pool.h"
#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "game_object.h"

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * system;	// used to access the system
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

	std::vector<Component*> receivers;


public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void AddReceiver(Component *comp);
	virtual void Receive(Message m) {}
	virtual void Send(Message m);
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite* sprite;

public:


	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects
	int x; //offset to check in x direction
	int y; //offset to check in y direction

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects, int x, int y);
	virtual void Update(float dt);
};

class BumbCollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};




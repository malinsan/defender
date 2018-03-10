#pragma once
// GameObject represents objects which moves are drawn
#include <vector>


enum Message {TELEPORTED, DROPPED, ABDUCTED, GOING_BACK, GOING_LEFT, GOING_RIGHT, SHOOT, HIT, BUMP_HIT, ALIEN_HIT, GAME_OVER, LEVEL_WIN, NO_MSG };

class Component;

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	double horizontalPosition;
	double verticalPosition;
	double horizontalVelocity, verticalVelocity; //"physics"

	float angle; // angle of rotation in degrees
	bool enabled;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);

	virtual void Init();
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject *go);
	virtual void Receive(Message m) {}
	void Send(Message m);
};
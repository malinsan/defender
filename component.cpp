#include "fmod.hpp"
#include "fmod_studio.hpp"

#include "component.h"
#include "game_object.h"
#include "avancezlib.h"

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void Component::AddReceiver(Component * comp)
{
	receivers.push_back(comp);
}

void Component::Send(Message m)
{
	for (auto i = 0; i < receivers.size(); i++)
	{
		if (!(receivers[i]->go->enabled))
			continue;

		receivers[i]->Receive(m);
	}
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(system, go, game_objects);

	sprite = system->createSprite(sprite_name);
}

void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->angle);
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if ((go0->horizontalPosition > go->horizontalPosition - 10) &&
				(go0->horizontalPosition < go->horizontalPosition + 10) &&
				(go0->verticalPosition   > go->verticalPosition - 10) &&
				(go0->verticalPosition   < go->verticalPosition + 10))
			{
				go->Receive(HIT);
				go0->Receive(HIT);
			}
		}
	}
}

void BumbCollideComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}

void BumbCollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if (//time_since_bump > 3.0f &&
				(go0->horizontalPosition > go->horizontalPosition - 32) &&
				(go0->horizontalPosition < go->horizontalPosition + 64) &&
				(go0->verticalPosition   > go->verticalPosition - 32) &&
				(go0->verticalPosition   < go->verticalPosition + 32))
			{
				go0->Receive(BUMP_HIT);
				
				//move lander and player away from each other
				go0->horizontalPosition -= go0->velocity.x * dt;
				go0->verticalPosition -= go0->velocity.y * dt;
				go->horizontalPosition -= go->velocity.x * dt;
				go->verticalPosition -= go->velocity.y * dt;


				go0->velocity.x = go->velocity.x * 0.5; //send lander in the direction player is going
				go0->velocity.y = go->velocity.y * 0.5;

				go->velocity.x *= -1; //send player the other way
				go->velocity.y *= -1;
							
			}
		}
	}

}

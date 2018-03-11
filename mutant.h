#pragma once
class Mutant : public GameObject
{
public:

	bool bumped = false;

	virtual ~Mutant() { SDL_Log("Mutant::Mutant"); }

	virtual void Init(float xPos, float yPos)
	{
		SDL_Log("Mutant::Init");
		GameObject::Init();
		horizontalPosition = xPos;
		verticalPosition = yPos;
	}

	virtual void Receive(Message m) {
		if (!enabled) {
			return;
		}
		if (m == HIT) {
			Send(ALIEN_HIT);
			enabled = false;
		}
		if (m == BUMP_HIT) {
			bumped = true;
		}
	}


};


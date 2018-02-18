class Background : public GameObject 
{
public:


	virtual void Init() {

		GameObject::Init();

		horizontalPosition = -3000; //offset so we draw it in the middle
		verticalPosition = 0;

	}

	virtual void Receive(Message m) {



	}


};

class BackgroundBehaviourComponent : public Component
{
public:

	bool moveBackground = false;

	virtual void Receive(Message m) {
		if (m == EDGE_REACHED) {

			moveBackground = true;
		}
	}

	virtual void Update(float dt) 
	{
		if (moveBackground) {
			moveBackground = false;
			go->horizontalPosition += dt * PLAYER_SPEED;
		}

	}
};
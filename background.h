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

	bool moveBG = false;
	bool moveLeft = false;

	virtual void Receive(Message m) {
		if (m == L_EDGE_REACHED) {
			moveBG = true;
			moveLeft = true;
		}
		if (m == R_EDGE_REACHED) {
			moveBG = true;
		}
	}

	virtual void Update(float dt) 
	{
		if (moveLeft) {
			moveBG = false;
			moveLeft = false;
			go->horizontalPosition += dt * PLAYER_SPEED;
		}
		else if (moveBG && !moveLeft) {
			moveBG = false;
			go->horizontalPosition -= dt * PLAYER_SPEED;
		}

	}
};
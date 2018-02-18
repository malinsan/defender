class Background : public GameObject 
{
public:


	virtual void Init() {

		GameObject::Init();

		//width = 2400
		//middle = 1200
		//screen width 1200, /2 = 600
		horizontalPosition = -600; //offset so we draw it in the middle
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
	//wrapspot is the corner of 'last' window 
	int wrapSpot = -1200;

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
			Move(dt * PLAYER_SPEED);
		}
		else if (moveBG && !moveLeft) {
			moveBG = false;
			Move(- dt * PLAYER_SPEED);
		}

	}

	void Move(float move) {

		go->horizontalPosition += move;
		
		//going right wraparound
		if (go->horizontalPosition < wrapSpot) {
			go->horizontalPosition = 0;
		}
		//left wraparound
		if (go->horizontalPosition > 0) {
			go->horizontalPosition = wrapSpot;
		}

	}
};
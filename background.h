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

class BackgroundBehaviour : public Component
{
public:
	void Update(float dt) 
	{

	}
};
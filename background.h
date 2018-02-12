class Background : public GameObject 
{
public:


	virtual void Init() {

		GameObject::Init();

		horizontalPosition = -3000; //offset so we draw it in the middle
		verticalPosition = 0;

	}


};

class BackgroundBehaviour : public Component
{

};
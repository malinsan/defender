
class BoxPhysicsComponent : public Component
{
	b2World* world;
	b2Body* body;

	float size_x, size_y;
	float pos_x, pos_y;
	float og_pos_x, og_pos_y;
	b2BodyType type;

public:

	virtual void Create(AvancezLib* system, b2World* world, GameObject * go, std::set<GameObject*> * game_objects, b2BodyType type, float pos_x, float pos_y, float size_x, float size_y)
	{
		Component::Create(system, go, game_objects);

		this->world = world;
		this->size_x = size_x / 2.f;
		this->size_y = size_y / 2.f;
		this->pos_x = pos_x;
		this->pos_y = pos_y;
		og_pos_x = pos_x;
		og_pos_y = pos_y;
		this->type = type;

	}

	virtual void Init() {
		go->horizontalPosition = 640-pos_x;
		go->verticalPosition = 480-pos_y;
		//go->angle = 0;

		// define the physics object here
		//creating a ground body definition
		b2BodyDef bodyDef;
		bodyDef.position.Set(pos_x, pos_y);
		bodyDef.type = type;

		//create a polygon shape
		b2PolygonShape boxShape;
		boxShape.SetAsBox(size_x, size_y);

		//creating the body in the world
		body = world->CreateBody(&bodyDef);

		//create fixture 
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.density = 10.0f; //dynamic bodies should have non-zero density or strange behaviour will ensue
		fixtureDef.friction = 0.3f;
		body->CreateFixture(&fixtureDef);

		b2Vec2 position = body->GetPosition();
		printf("%4.2f %4.2f\n", position.x, position.y);

	}

	virtual void Update(float dt)
	{
		b2Vec2 position = body->GetPosition();
		float32 angle = body->GetAngle();

		// update the state of the game object here
		float newX = og_pos_x - position.x;
		float newY = og_pos_y - position.y;

		go->horizontalPosition = position.x - size_x;
		go->verticalPosition = 480 - (position.y + size_y);
		go->angle = (angle / 3.14f) * -180;
	}

	virtual ~BoxPhysicsComponent()
	{
		world->DestroyBody(body);
	}
};



class Box : public GameObject
{
	BoxPhysicsComponent * physics;

public:

	virtual ~Box() { SDL_Log("Box::~Box"); }

	virtual void Init()
	{
		SDL_Log("Box::Init");
		GameObject::Init();
	}
};

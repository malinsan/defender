
#include <iostream>
#include <set>

#include "Box2D\Box2D.h"

#include "avancezlib.h"

#include "object_pool.h"

#include "fmod.hpp"
#include "fmod_studio.hpp"



using namespace std;

const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 670;
const unsigned int LEVEL_CHUNKS = 7;
const unsigned int	MAX_NUM_GAME_OBJECTS = 10000;
const unsigned int	NUM_LIVES = 2;
int PLAYER_WIDTH = 32;

const unsigned int	MAX_NUM_ROCKETS = 32;
const unsigned int	MAX_NUM_BOMBS = 32;
const unsigned int	POINTS_PER_ALIEN = 100;

const float			FIRE_TIME_INTERVAL = .5f;
const float			BOMB_TIME_INTERVAL = 1.25f;
const float			PLAYER_SPEED = 160.0f;
const float			ROCKET_SPEED = 160.0f;
const float			ALIEN_SPEED = 40.0f;
const float			BOMB_SPEED = 120.0f;

float game_speed = 1.f;



#include "game_object.h"
#include "component.h"

//game objects to include

#include "background.h"
#include "soundmaker.h"
#include "rocket.h"
#include "player.h"
#include "box.h"

#include "game.h"

int main(int argc, char** argv)
{

	AvancezLib system;

	system.init(WIDTH, HEIGHT);

	Game game;
	game.Create(&system);
	game.Init();

	float lastTime = system.getElapsedTime();
	while (system.update())
	{
		//fmodSystem->update();

		float newTime = system.getElapsedTime();
		float dt = newTime - lastTime;
		dt = dt * game_speed;
		lastTime = newTime;

		game.Update(dt);

		game.Draw();
	}

	// clean up
	game.Destroy();
	system.destroy();

	return 0;
}
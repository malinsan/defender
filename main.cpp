
#include <iostream>
#include <set>

#include "avancezlib.h"

#include "object_pool.h"

#include "fmod.hpp"
#include "fmod_studio.hpp"



using namespace std;

const int WORLD_WIDTH = 3600;
const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 670;
const unsigned int	MAX_NUM_GAME_OBJECTS = 10000;

const unsigned int	NUM_LIVES = 3;
int PLAYER_WIDTH = 64;
int PLAYER_HEIGHT = 25;
int NUM_SMARTBOMBS = 3;
const int GET_SMARTBOMB_SCORE = 1000; //gain a smartbomb if you reach 1000 points
const int TOTAL_TP_FRAMES = 200;

const float PLAYER_RANGE = 400.0f;
const float HUMAN_RANGE = 35.0f;

const float PLAYER_MAX_VELOCITY = 700.0f;
const float PLAYER_ACCELERATION = 800.0f;

const unsigned int	NUM_HUMANS = 10;
int NUM_ALIENS = 5;

const unsigned int	MAX_NUM_ROCKETS = 32;
const unsigned int	MAX_NUM_BOMBS = 32;
const unsigned int	POINTS_PER_ALIEN = 100;
const unsigned int	POINTS_PER_HUMAN = 500;

const float			FIRE_TIME_INTERVAL = .2f;
const float			BOMB_TIME_INTERVAL = 1.25f;
const float			ROCKET_SPEED = 1000.0f;
const int			ROCKET_HEIGHT = 4;

const float			LANDER_MAX_SPEED = 50.0f;
const float			LANDER_ACCELERATION = 100.0f;
const float			MUTANT_MAX_SPEED = 1.6 * LANDER_MAX_SPEED;
const float			MUTANT_ACCELERATION = 1.6 * LANDER_ACCELERATION;


const float			BOMB_SPEED = 170.0f;
const float			HUMAN_SPEED = 40.0f;

float game_speed = 1.f;



#include "game_object.h"
#include "component.h"

//game objects to include

#include "soundmaker.h"
#include "rocket.h"
#include "player.h"
#include "bomb.h"
#include "background.h"
#include "human.h"
#include "lander.h"
#include "mutant.h"
#include "mutant_state_machine.h"
#include "lander_state_machine.h"
#include "human_state_machine.h"
#include "spawner.h"

#include "game.h"

#undef main
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
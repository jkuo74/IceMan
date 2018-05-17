#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
//Pass Student world as static pointer?
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), current_level_number(0)
	{
	}
	bool by_itself(const int & x_coord, const int & y_coord);//checks that no object is placed within 6 blocks apart
	virtual int init()
	{
		Hero = std::make_shared<IceMan>();//add player
		Hero->getWorld(this);
		for (int n = 0; n < 64; n++){
			std::vector<std::shared_ptr<Ice>> columns;
			for (int m = 0; m < 64; m++)
			{
				if ((n <= 30 || n > 34) || m < 4 ) {
					columns.push_back(std::make_shared<Ice>(n, m));
				}
			}
				IceBlocks.push_back(columns);
		}//add ice
		/*
		for (int n = 0; n < 4; n++)
		{
			std::vector<std::shared_ptr<Actor>> temp_object;
			Objects.push_back(temp_object);
		}

		int B = std::min((current_level_number / 2) + 2, 9);
		for (int n = 0; n < B; n++)
		{
			int x_rand = (rand() % 60);
			int y_rand = (rand() % 56);
			if(by_itself(x_rand, y_rand))
				Objects[0].push_back(std::make_shared<Boulder>(x_rand, y_rand));
		}//add boulders

		int G = std::min((5 - current_level_number / 2), 2);
		for (int n = 0; n < G; n++)
		{
			int x_rand = (rand() % 60);
			int y_rand = (rand() % 56);
			if (by_itself(x_rand, y_rand))
				Objects[1].push_back(std::make_shared<Gold_Nugget>(x_rand, y_rand));
		}//add boulders
		*/
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		if (Hero->getHealth() != 0)
		{
			Hero->move();
			return GWSTATUS_CONTINUE_GAME;
		}
		return GWSTATUS_CONTINUE_GAME;
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	virtual void cleanUp()
	{
		++current_level_number;
	}

private:
	int current_level_number;
	std::vector<std::vector<std::shared_ptr<Ice>>> IceBlocks;
	std::shared_ptr<IceMan> Hero;
	std::vector<std::vector<std::shared_ptr<Actor>>> Objects;
};

#endif // STUDENTWORLD_H_

#include "StudentWorld.h"
#include <string>
#include<iostream>
using namespace std;
StudentWorld * StudentWorld::SWP;

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir), current_level_number(0)
{
	SWP = this;
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld* StudentWorld::getInstance()
{
	return SWP;
}

int StudentWorld::init()
{
	Hero = std::make_shared<IceMan>();//add player
	for (int n = 0; n < 64; n++)
	{
		std::vector<Ice*> columns;
		for (int m = 0; m < 64; m++)
			if (((n < 30 || n > 33) || (m < 4)) && m < 60)
				columns.push_back(new Ice{ n,m });
			else
				columns.push_back(nullptr);
		IceBlocks.push_back(columns);
	}

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

bool StudentWorld::by_itself(const int & x_coord, const int & y_coord)
{
	if(Objects.size() != 0)
		for (auto it = Objects.begin(); it != Objects.end(); it++)
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				int x_dist = (*it2)->getX() - x_coord;
				int y_dist = (*it2)->getY() - y_coord;
				double distance = sqrt(pow(x_dist, 2.0) + pow(y_dist, 2.0));
				if (distance <= 6.0)
					return false;
			}
	return true;
}

int StudentWorld::move()
{
	if (Hero->getHealth() != 0)
	{
		int x_pos = Hero->getX();
		int y_pos = Hero->getY();
		for (int n = x_pos; n < x_pos + 4; n++)
			for (int m = y_pos; m < y_pos + 4; m++)
				if (IceBlocks[n][m] != nullptr)
				{
					playSound(IID_ICE);
					delete IceBlocks[n][m];
					IceBlocks[n][m] = nullptr;
				}
		Hero->doSomething();

		return GWSTATUS_CONTINUE_GAME;
	}
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	decLives();
	return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp() {
	Hero = nullptr;
	++current_level_number;
	for (std::vector<Ice*> & line : IceBlocks) {
		for (Ice* & block : line) {
			delete block;
			block = nullptr;
		}
		
	}
	for (auto & line : Objects) {
		for (auto & block : line) {
			block = nullptr;
		}
	}
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

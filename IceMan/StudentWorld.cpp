#include "StudentWorld.h"
#include <string>
#include<iostream>
using namespace std;

StudentWorld * StudentWorld::SWP;
const static int initTunnelL = 30;
const static int initTunnelR = 33;

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

int StudentWorld::init() {
	Hero = std::make_shared<IceMan>(this);//add player
	for (int n = 0; n < 64; n++) {
		std::vector<Ice*> columns;
		for (int m = 0; m < 64; m++)
			if (((n < initTunnelL || n > initTunnelR) || (m < 4)) && m < 60)
				columns.push_back(new Ice{ n,m });
			else
				columns.push_back(nullptr);
		IceBlocks.push_back(columns);
	}

	for (int n = 0; n < 4; n++) { // creating vector of pointers to each Object type
		std::vector<std::shared_ptr<Actor>> temp_object;
		Objects.push_back(temp_object);
	}

	int B = std::min((current_level_number / 2) + 2, 9);
	int n = 0;
	while (n <  B) {
		int x_rand = (rand() % 27);
		int section_rand = (rand() % 2);
		if (section_rand == 1)
			x_rand = x_rand + 34;
		int y_rand = (rand() % 56) + 1;
		if (((x_rand < (initTunnelL - 4)) || (x_rand >(initTunnelR + 4))) && by_itself(x_rand, y_rand, 6)) {
			Objects[BOULDER].push_back(std::make_shared<Boulder>(x_rand, y_rand, this));
			removeIce(x_rand, y_rand);
			n++;
		}
	}//add boulders

	 int G = std::min((5 - current_level_number / 2), 2); //num of gold  nuggets
	 n = 0;
	 while (n < G)  {
		 int x_rand = (rand() % 60);
		 int y_rand = (rand() % 56);
		 if (((x_rand < (initTunnelL - 4)) || (x_rand > (initTunnelR + 4))) && by_itself(x_rand, y_rand, 6)) {
			 Objects[GOLD].push_back(std::make_shared<Gold_Nugget>(x_rand, y_rand, this));
			 n++;
			cerr << "x : " << x_rand << "y : " << y_rand << endl;
		 }
	 } // add gold
	 
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::by_itself(const int & x_coord, const int & y_coord, const int & radius) /////////////////////
{
	if (Objects.size() != 0)
		for (auto it = Objects.begin(); it != Objects.end(); it++)
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				int x_dist = (*it2)->getX() - x_coord;
				int y_dist = (*it2)->getY() - y_coord;
				double distance = sqrt(pow(x_dist, 2.0) + pow(y_dist, 2.0));
				if (distance <= radius) /////////////////////
					return false;
			}
	return true;
}

void StudentWorld::removeIce(const int & x_coord, const int & y_coord)
{
	for (int n = x_coord; n < x_coord + 4; n++)
		for (int m = y_coord; m < y_coord + 4; m++)
			if (IceBlocks[n][m] != nullptr)
			{
				playSound(IID_ICE);
				delete IceBlocks[n][m];
				IceBlocks[n][m] = nullptr;
			}
}

bool StudentWorld::IceBelow(const int & x_coord, const int & y_coord)
{
	return IceBlocks[x_coord][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 1][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 2][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 3][y_coord - 1] != nullptr;
}

bool StudentWorld::BoulderBelow(const int & x_coord, const int & y_coord)
{
	for (auto it = Objects[BOULDER].begin(); it != Objects[BOULDER].end(); it++)
	{
		if ((*it)->isAlive() && abs((*it)->getX() - x_coord) < 4)
		{
			if (y_coord == ((*it)->getY() + 4))
				return true;
		}
	}
	return false;
}
bool StudentWorld::makeVisible() {
	
}
void StudentWorld::pickUpItem(const int & x_coord, const int & y_coord) {

}

int StudentWorld::move()
{
	if (Hero->getHealth() != 0)
	{
		for (auto it = Objects.begin(); it != Objects.end(); it++)
		{
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				if ((*it2)->isAlive())
					(*it2)->doSomething();
			}
		}
		Hero->doSomething();
		return GWSTATUS_CONTINUE_GAME;
	}
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	playSound(SOUND_PLAYER_GIVE_UP);
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

#include "StudentWorld.h"
#include <string>
#include<iostream>
using namespace std;
StudentWorld * StudentWorld::SWP;
const static int initTunnelL = 30;
const static int initTunnelR = 33;
StudentWorld::StudentWorld(string assetDir):
	GameWorld(assetDir), level(0), total_points(0)
{
	SWP = this;
}
GameWorld * createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
StudentWorld * StudentWorld::getInstance()
{
	return SWP;
}
int StudentWorld::init()
{
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
	for (int n = 0; n < 8; n++) {
		std::vector<std::unique_ptr<Actor>> temp_object;
		Objects.push_back(std::move(temp_object));
	}
	int B = 20;// std::min((level / 2) + 2, 9);
	int n = 0;
	while (n < B) {
		int x_rand = (rand() % 27);
		int section_rand = (rand() % 2);
		if (section_rand == 1)
			x_rand = x_rand + 34;
		int y_rand = (rand() % 36) + 20;
		if (by_itself(x_rand, y_rand, 0)) {
			Objects[BOULDER].push_back(std::make_unique<Boulder>(x_rand, y_rand, this));
			removeIce(x_rand, y_rand);
			n++;
		}
	}//add boulders
	 int G = std::min((5 - level / 2), 2); //num of gold  nuggets
	 n = 0;
	 while (n < G)  {
		 int x_rand = (rand() % 27);
		 int section_rand = (rand() % 2);
		 if (section_rand == 1)
			x_rand = x_rand + 34;
		 int y_rand = (rand() % 56);
		 if (by_itself(x_rand, y_rand, 0)) {
			Objects[GOLD].push_back(std::make_shared<Gold_Nugget>(x_rand, y_rand, this));
			n++;
			cerr << "x : " << x_rand << "y : " << y_rand << endl;
		 }
	 } // add gold
	 
	return GWSTATUS_CONTINUE_GAME;
}
bool StudentWorld::by_itself(const int & x_coord, const int & y_coord, const int & ID)
{
	//ID == 0: set up to check if any other object is within 6 pixels away
	//ID == 2: checks if any person is within 3 pixels away
	//ID == 4: checks if only IceMan is within 3 pixels away
	//ID == 3: checks if only IceMan is within 4 pixels away
	if((ID != 4 && ID != 3) && Objects.size() != 0)
		for (auto it = Objects.begin() + 6*(ID); it != Objects.end(); it++)
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
				if (sqrt(pow((*it2)->getX() - x_coord, 2.0) + pow((*it2)->getY() - y_coord, 2.0)) <= (6.0 - (3.0 * ID))) // distance is less than 6.0 or 3.0
					return false;
	if ((ID == 2 || ID == 3 || ID == 4) && sqrt(pow(Hero->getX() - x_coord, 2.0) + pow(Hero->getY() - y_coord, 2.0)) <= ((ID % 2) +  3))
		return false;
	return true;
}
void StudentWorld::removeIce(const int & x_coord, const int & y_coord)
{
	for (int n = x_coord; n < x_coord + 4; n++)
		for (int m = y_coord; m < y_coord + 4; m++)
			if (IceBlocks[n][m] != nullptr) {
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
	for (auto it = Objects[0].begin(); it != Objects[0].end(); it++) {
		if (abs((*it)->getX() - x_coord) < 4) {
			if (y_coord == ((*it)->getY() + 4))
				return true;
		}
	}
	return false;
}
bool StudentWorld::makeVisible() {
	for (auto it = Objects[GOLD].begin(); it != Objects[GOLD].end(); it++) {
		if ((*it)->isAlive() && !by_itself(Hero->getX(),Hero->getY(), 4)) {
			(*it)->setVisible(false); ///FIX::::::::::::SET TO TRUE
			return true;
		}
	}
	return false;
}
void StudentWorld::pickUpItem() {
	for (auto it = Objects[GOLD].begin(); it != Objects[GOLD].end(); it++) {
		if ((*it)->isAlive() && !by_itself(Hero->getX(), Hero->getY(), 0)) {
			(*it)->setVisible(true); ///FIX::::::::::::SET TO FALSE
			(*it) = nullptr;
		}
	}
}
void StudentWorld::changePoints(const int & points)
{
	total_points += points;
}
int StudentWorld::move()
{
	if (Hero->getHealth() != 0) {
		for (auto it = Objects.begin(); it != Objects.end(); it++) {
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
					(*it2)->doSomething();
		}
		deleteDeadObjects();
		Hero->doSomething();
		return GWSTATUS_CONTINUE_GAME;
	}
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	playSound(SOUND_PLAYER_GIVE_UP);
	decLives();
	return GWSTATUS_PLAYER_DIED;
}
void StudentWorld::deleteDeadObjects()
{
	for (auto it = Objects.begin(); it != Objects.end(); it++) {
		vector<int> toDelete;
		int index = 0;
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {
			if (!(*it2)->isAlive()) {
				(*it2).reset();
				toDelete.push_back(index);
			}
			index++;
		}
		if (toDelete.size() != 0) {
			for (auto it2 = toDelete.begin(); it2 != toDelete.end(); it2++)
				it->erase((it->begin() + *it2));
			it->shrink_to_fit();
		}
	}
}
int StudentWorld::getLevel()
{
	return level;
}
void StudentWorld::cleanUp() 
{
	Hero = nullptr;
	++level;
	for (std::vector<Ice*> & line : IceBlocks) {
		for (Ice* & block : line) {
			delete block;
			block = nullptr;
		}
		line.clear();
	}
	IceBlocks.clear();
	for (auto & line : Objects) {
		for (auto & block : line) {
			block.reset(nullptr);
		}
		line.clear();
		}
	Objects.clear();
}
StudentWorld::~StudentWorld()
{
	cleanUp();
}

#include "StudentWorld.h"
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

StudentWorld * StudentWorld::SWP;
const static int initTunnelL = 30;
const static int initTunnelR = 33;
const static int sizeOfObject = 4;

StudentWorld::StudentWorld(string assetDir) :
	GameWorld(assetDir), game_ticks(0) {
	SWP = this;
}
GameWorld * createStudentWorld(string assetDir) {
	return new StudentWorld(assetDir);
}
StudentWorld * StudentWorld::getInstance() {
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
	for (int n = 0; n < 8; n++) {
		vector<unique_ptr<Actor>> temp_object;
		Objects.push_back(std::move(temp_object));
	}
	int num_boulders = min((static_cast<int>(getLevel()) / 2) + 2, 9);
	int n = 0;
	while (n < num_boulders) {
		int x_rand = (rand() % (initTunnelL - sizeOfObject + 1));
		int section_rand = (rand() % 2);
		if (section_rand == 1)
			x_rand += (initTunnelR + 1);
		int y_rand = (rand() % 36) + 20;
		if (!objectNearby(x_rand, y_rand, 6.0, BOULDER)) {
			Objects[BOULDER].push_back(std::make_unique<Boulder>(x_rand, y_rand, this));
			removeIce(x_rand, y_rand);
//			cerr << "BOULDERS (" << x_rand << "," << y_rand << ")" << endl;
			n++;
		}
	}//add boulders
	int num_gold_nuggets = min((5 - static_cast<int>(getLevel()) / 2), 2); //num of gold  nuggets
	n = 0;
	while (n < num_gold_nuggets) {
		int x_rand = (rand() % (initTunnelL - sizeOfObject + 1));
		int section_rand = (rand() % 2);
		if (section_rand == 1)
			x_rand += (initTunnelR + 1);
		int y_rand = (rand() % 56);
		if (!objectNearby(x_rand, y_rand, 6.0, BOULDER) && !objectNearby(x_rand, y_rand, 6.0, GOLD)) {
			Objects[GOLD].push_back(std::make_unique<Gold_Nugget>(x_rand, y_rand, PERMANENT, this));
			n++;
			cerr << "GOLD (" << x_rand << "," << y_rand << ")" << endl;
		}
	} // add gold
	int num_oil_barrels = min(2 + static_cast<int>(getLevel()), 21);
	n = 0;
	while (n < num_oil_barrels) {
		int x_rand = (rand() % (initTunnelL - sizeOfObject + 1));
		int section_rand = (rand() % 2); // ensures item ends up to the left or right of inital tunnel
		if (section_rand == 1)
			x_rand += (initTunnelR + 1);
		int y_rand = (rand() % 56);
		if (!objectNearby(x_rand, y_rand, 6.0, BOULDER) && !objectNearby(x_rand, y_rand, 6.0, GOLD) && !objectNearby(x_rand, y_rand, 6.0, OIL)) {
			Objects[OIL].push_back(std::make_unique<Oil_Barrel>(x_rand, y_rand, PERMANENT, this));
			n++;
			cerr << "OIL (" << x_rand << "," << y_rand << ")" << endl;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}
bool StudentWorld::personNearby(const int & x_coord, const int & y_coord, const double & radius, const int & ID1, const ObjType & ID2) {
	//ID1 == 0 check if only IceMan is nearby given coordinates
	//ID1 == 1 check if only Protesters and Hardcore Protesters are nearby
	//ID1 == 2 check if either are nearby

	if (ID1 == 1 || ID1 == 2) {
		for (auto it = Objects.begin() + 6; it != Objects.end(); it++) {//loop will look only at protesters and hardcore protesters
			for (auto it2 = it->begin(); it2 != it->end(); it2++)//iterate through all elements in sub-vector-level
				if (sqrt(pow((*it2)->getX() - x_coord, 2.0) + pow((*it2)->getY() - y_coord, 2.0)) <= radius) { // distance is less than 6.0 or 3.0
					if ((ID2 == BOULDER) && (*it2)->getY() < y_coord)//if ID equals 5 it will check for any protestors or hardcore protestors that are below the boulder that is falling
						(*it2)->annoy(100);
					if (ID2 == SQUIRT)//if ID is not five but something is within the radius, return false
						(*it2)->annoy(2);
					return true;
				}
		}
	}
	if ((ID1 == 0 || ID1 == 2) && sqrt(pow(Hero->getX() - x_coord, 2.0) + pow(Hero->getY() - y_coord, 2.0)) <= radius) {
		if ((ID2 == BOULDER) && Hero->getY() < y_coord)//if ID equals 5 it will check for any protestors or hardcore protestors that are below the boulder that is falling
			Hero->annoy(100);
		return true;
	}
	return false;
}
bool StudentWorld::objectNearby(const int & x_coord, const int & y_coord, const double & radius, const ObjType & ID2) {
	//Pass in the object's id to check if any are nearby the given coordinates
	for (auto it = Objects[ID2].begin(); it != Objects[ID2].end(); it++)
	{
		if (sqrt(pow((*it)->getX() - x_coord, 2.0) + pow((*it)->getY() - y_coord, 2.0)) <= radius) {
			if (radius != 6.0 && ID2 != BOULDER)
				(*it)->setVisibility(true);
			else
				return true;
		}
	}
	return false;
}
bool StudentWorld::emptySpace(const int & x_coord, const int & y_coord) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (IceBlocks[x_coord + i][y_coord + j] != nullptr) {
				return false;
			}
		}
	}
	for (auto it = Objects.begin(); it != Objects.end(); it++) {
		for (auto it2 = it->begin(); it2 != it->end(); it2++)
			if (sqrt(pow((*it2)->getX() - x_coord, 2.0) + pow((*it2)->getY() - y_coord, 2.0)) <= 4) { 
				return false;
			}
	}
	return true;
}
void StudentWorld::removeIce(const int & x_coord, const int & y_coord) {
	for (int n = x_coord; n < x_coord + 4; n++)
		for (int m = y_coord; m < y_coord + 4; m++)
			if (IceBlocks[n][m] != nullptr) {
				playSound(IID_ICE);
				delete IceBlocks[n][m];
				IceBlocks[n][m] = nullptr;
			}
}
bool StudentWorld::IceBelow(const int & x_coord, const int & y_coord) {
	return IceBlocks[x_coord][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 1][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 2][y_coord - 1] != nullptr ||
		IceBlocks[x_coord + 3][y_coord - 1] != nullptr;
}
bool StudentWorld::BoulderBelow(const int & x_coord, const int & y_coord) {
	for (auto it = Objects[BOULDER].begin(); it != Objects[BOULDER].end(); it++) {
		if (abs((*it)->getX() - x_coord) < 4 && y_coord == ((*it)->getY() + 4))
			return true;
	}
	return false;
}
bool StudentWorld::all_Oil_Found() {
	return Objects[OIL].empty();
}

void StudentWorld::addItem(const ObjType & ID) {
	Hero->addItem(ID);
}
void StudentWorld::dropItem(const ObjType & ID) {
	if (ID == GOLD)
		Objects[GOLD].push_back(std::make_unique<Gold_Nugget>(Hero->getX(), Hero->getY(), TEMPORARY, this));
	else if (ID == SQUIRT) {
		//Objects[SQUIRT].push_back(std::make_unique<Water>(Hero->getX(), Hero->getY(), TEMPORARY));
	}
}
int StudentWorld::move() {
	if (all_Oil_Found()) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	if (Hero->getHealth() > 0) {
		for (auto it = Objects.begin(); it != Objects.end(); it++) {
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
				(*it2)->doSomething();
		}
		deleteDeadObjects();
		Hero->doSomething();
		if (game_ticks  % 300 == 0){ //(rand() % (getLevel() * 25 + 300) == getLevel()) { 
			int sonar_or_water = rand() % 5;
			if (sonar_or_water == 0) {
				Objects[SONAR].push_back(make_unique<Sonar_Kit>(this));
				cerr << "SONAR ADDED" << endl;
			}
			else {
				int x;
				int y;
				do {
					x = rand() % 60;
					y = rand() % 60;
				} while (!emptySpace(x,y));
				Objects[WATER].push_back(make_unique<Water_Pool>(x, y, this));
				cerr << "WATER ADDED  (" << x << "," << y << ")" << endl;
			}
		}
		game_ticks++;
		//game_ticks += 20;
		updateDisplayText();
		return GWSTATUS_CONTINUE_GAME;
	}
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	playSound(SOUND_PLAYER_GIVE_UP);//does it matter that this sound plays when escape key is pressed?
	decLives();
	return GWSTATUS_PLAYER_DIED;
}
void StudentWorld::deleteDeadObjects() {
	for (auto it = Objects.begin(); it != Objects.end(); it++) {
		vector<int> toDelete;
		int index = 0;
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {
			if (!(*it2)->isAlive()) {
//				cerr << it->size() << "|unchanged|" << it->capacity() << endl;
				(*it2).reset();
//				cerr << it->size() << "|reseted|" << it->capacity() << endl;
				toDelete.push_back(index);
			}
			index++;
		}
		if (toDelete.size() != 0) {
			for (auto it2 = toDelete.begin(); it2 != toDelete.end(); it2++)
			{
//				cerr << it->size() << "|unchanged2|" << it->capacity() << endl;
				it->erase((it->begin() + *it2));
//				cerr << it->size() << "|shrunk|" << it->capacity() << endl;
			}
			it->shrink_to_fit();
		}
	}
}
void StudentWorld::updateDisplayText() {
	stringstream os;
	os << "Lvl: " << setw(2) << getLevel() << " Lives: " << getLives()
		<< " Hlth: " << setw(3) << Hero->getHealth() * 10 << "% Wtr: " << setw(2) <<  Hero->getNumItems(SQUIRT)
		<< " Gld: " << Hero->getNumItems(GOLD) << " Oil Left: " << Objects[OIL].size()
		<< " Sonar: " << Hero->getNumItems(SONAR) << " Scr: " << setfill('0') << setw(6)<< getScore();
	setGameStatText(os.str());
}
void StudentWorld::cleanUp() {
	Hero = nullptr;
	for (std::vector<Ice*> & line : IceBlocks) {
		for (Ice* & block : line) {
			delete block;
			block = nullptr;
		}
		line.clear();
	}
	IceBlocks.clear();
	cerr << "cleanup" << endl;
	for (auto & line : Objects) {
		for (auto & block : line) {
			cerr << line.size() << "|" << line.capacity() << endl;
			block.reset(nullptr);
			cerr << line.size() << "||" << line.capacity() << endl;
		}
		cerr << line.size() << "|||" << line.capacity() << endl;
		line.clear();
		cerr << line.size() << "||||" << line.capacity() << endl;
	}
	Objects.clear();
}
StudentWorld::~StudentWorld() {
	cleanUp();
}

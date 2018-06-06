#include "StudentWorld.h"
#include <string>
#include <iostream>
#include <iomanip>
//#include<thread>
using namespace std;
StudentWorld * StudentWorld::SWP;
const static int initTunnelL = 30;
const static int initTunnelR = 33;
const static int sizeOfObject = 4;
const static int maxelement = 63;
StudentWorld::StudentWorld(string assetDir) :
	GameWorld(assetDir), game_ticks(0), mapChanged(false) {
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
	for (int column = 0; column < 64; column++) {
		std::vector<Ice*> columns;
		std::vector<int> columnsInt, columnsInt1;
		for (int row = 0; row < 64; row++)
			if (((column < initTunnelL || column > initTunnelR) || (row < 4)) && row < 60) {
				columns.push_back(new Ice{ column,row });
				columnsInt.push_back(-2);
				columnsInt1.push_back(-2);
			}
			else {
				columns.push_back(nullptr);
				columnsInt.push_back(-1);
				columnsInt1.push_back(-1);
			}
		IceBlocks.push_back(std::move(columns));
		intSteps.push_back(std::move(columnsInt));
		distHero.push_back(std::move(columnsInt1));
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
			Objects[GOLD].push_back(std::make_unique<Gold_Nugget>(x_rand, y_rand, ALIVE, this));
			n++;
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
			Objects[OIL].push_back(std::make_unique<Oil_Barrel>(x_rand, y_rand, ALIVE, this));
			n++;
		}
	}
	Objects[PROTESTER].push_back(make_unique<Regular_Protester>(IID_PROTESTER, this));
	newMap = new thread(&StudentWorld::getNewMap, this, 60, 60);
	heroMap = new thread(&StudentWorld::getHeroMap, this, 60, 60);
	return GWSTATUS_CONTINUE_GAME;
}
bool StudentWorld::personNearby(const int & x_coord, const int & y_coord, const double & radius, const int & ID1, const ObjType & ID2) {
	//ID1 == 0 check if only IceMan is nearby given coordinates
	//ID1 == 1 check if only Protesters and Hardcore Protesters are nearby
	//ID1 == 2 check if either are nearby

	if (ID1 == 1 || ID1 == 2) {
		int type_of_protester = 0;
		for (auto it = Objects.begin() + PROTESTER; it != Objects.end(); it++) {//loop will look only at protesters and hardcore protesters
			for (auto it2 = it->begin(); it2 != it->end(); it2++) {//iterate through all elements in sub-vector-level
				if (sqrt(pow((*it2)->getX() - x_coord, 2.0) + pow((*it2)->getY() - y_coord, 2.0)) <= radius) { // distance is less than 6.0 or 3.0
					if ((ID2 == BOULDER) && (*it2)->getY() < y_coord) {//if ID equals 5 it will check for any protestors or hardcore protestors that are below the boulder that is falling
						(*it2)->annoy(100);
						increaseScore(500);
						playSound(SOUND_PROTESTER_ANNOYED);

					}
					if (ID2 == SQUIRT) {//if ID is not five but something is within the radius, return false
						(*it2)->annoy(2);
						(*it2)->setState(FALLING);
						playSound(SOUND_PROTESTER_ANNOYED);
					}
					if (ID2 == GOLD) {
						if (type_of_protester == 0) {
							(*it2)->setState(TEMPORARY);
							increaseScore(25);
						}
						else {
							increaseScore(50);
							(*it2)->setState(FALLING);
						}
						//printMap();
					}
					return true;
				}
			}
			++type_of_protester;
		}
	}
	if ((ID1 == 0 || ID1 == 2) && sqrt(pow(Hero->getX() - x_coord, 2.0) + pow(Hero->getY() - y_coord, 2.0)) <= radius) {
		return true;
	}
	return false;
}
bool StudentWorld::objectNearby(const int & x_coord, const int & y_coord, const double & radius, const ObjType & ID2) {
	//Pass in the object's id to check if any are nearby the given coordinates
	for (auto it = Objects[ID2].begin(); it != Objects[ID2].end(); it++)
	{
		if (sqrt(pow((*it)->getX() - x_coord, 2.0) + pow((*it)->getY() - y_coord, 2.0)) < radius)
			if (radius != 6.0 && ID2 != BOULDER)
				(*it)->setVisibility(true);
			else
				return true;
	}
	return false;
}
bool StudentWorld::emptySpace(const int & x_coord, const int & y_coord, const GraphObject::Direction & face) { //coord from  rand(), will always be within boundaries for face = none
	if (IceAround(x_coord, y_coord, face)) {
		return false;
	}
	int y = y_coord;
	int x = x_coord;
	switch (face) {
	case GraphObject::none:
		for (int obj = GOLD; obj < PROTESTER; obj++) {
			if (objectNearby(x_coord, y_coord, sizeOfObject, static_cast<ObjType>(obj))) {
				return false;
			}
		}
		break;
	case GraphObject::up:
		y++;
		break;
	case GraphObject::down:
		y--;
		break;
	case GraphObject::left:
		x--;
		break;
	case GraphObject::right:
		x++;
		break;
	}
	if (objectNearby(x, y, sizeOfObject, BOULDER)) {
		return false;
	}
	return true;
}
void StudentWorld::removeIce(const int & x_coord, const int & y_coord) {
	for (int n = x_coord; n < x_coord + 4; n++)
		for (int m = y_coord; m < y_coord + 4; m++)
			if (IceBlocks[n][m] != nullptr) {
				if (!mapChanged)
					mapChanged = true;
				playSound(IID_ICE);
				delete IceBlocks[n][m];
				IceBlocks[n][m] = nullptr;
			}
}
bool StudentWorld::IceAround(const int & x_coord, const int & y_coord, const GraphObject::Direction & face) { //not all paths reutrn a value
	switch (face) {
	case GraphObject::none:
		for (int i = 0; i < sizeOfObject; i++) {
			for (int j = 0; j < sizeOfObject; j++) {
				if (IceBlocks[x_coord + i][y_coord + j] != nullptr) {
					return true;
				}
			}
		}
		return false;
	case GraphObject::down:
		return y_coord <= 0 || (IceBlocks[x_coord][y_coord - 1] != nullptr ||
			IceBlocks[x_coord + 1][y_coord - 1] != nullptr ||
			IceBlocks[x_coord + 2][y_coord - 1] != nullptr ||
			IceBlocks[x_coord + 3][y_coord - 1] != nullptr);
	case GraphObject::up:
		return y_coord >= 60 || (IceBlocks[x_coord][y_coord + 4] != nullptr ||
			IceBlocks[x_coord + 1][y_coord + 4] != nullptr ||
			IceBlocks[x_coord + 2][y_coord + 4] != nullptr ||
			IceBlocks[x_coord + 3][y_coord + 4] != nullptr);
	case GraphObject::right:
		return x_coord >= 60 || (IceBlocks[x_coord + 4][y_coord] != nullptr ||
			IceBlocks[x_coord + 4][y_coord + 1] != nullptr ||
			IceBlocks[x_coord + 4][y_coord + 2] != nullptr ||
			IceBlocks[x_coord + 4][y_coord + 3] != nullptr);
	case GraphObject::left:
		return x_coord <= 0 || (IceBlocks[x_coord - 1][y_coord] != nullptr ||
			IceBlocks[x_coord - 1][y_coord + 1] != nullptr ||
			IceBlocks[x_coord - 1][y_coord + 2] != nullptr ||
			IceBlocks[x_coord - 1][y_coord + 3] != nullptr);
	}
}
bool StudentWorld::BoulderBelow(const int & x_coord, const int & y_coord) {
	for (auto it = Objects[BOULDER].begin(); it != Objects[BOULDER].end(); it++) {
		if (abs((*it)->getX() - x_coord) < 4 && y_coord == ((*it)->getY() + 4))
			return true;
	}
	return false;
}
bool StudentWorld::all_Oil_Found() {
	return Objects[OIL].size() == 0;
}
bool StudentWorld::clearPath(const int & x_coord, const int & y_coord, int & flag) {
	int HeroX = Hero->getX();
	int HeroY = Hero->getY();
	int start{};
	int end{};
	if (HeroY == y_coord && HeroX == x_coord) {
		flag = 0;
	}
	else if (HeroY == y_coord) {//FFIX : check if equal & make sure ice man  is fully visible
		if (x_coord < HeroX) { // hero to the right of x
			start = x_coord;
			end = HeroX;
			flag = 1;
		}
		else {
			start = HeroX; // hero to the left of x
			end = x_coord;
			flag = 2;
		}
		for (start; start != end; start++) {
			if (!emptySpace(start, HeroY, GraphObject::none)) {
				return false;
			}
		}
	}
	else if (HeroX == x_coord) {//FFIX : check if equal & make sure ice man  is fully visible
		if (y_coord < HeroY) {  // hero above y
			start = y_coord;
			end = HeroY;
			flag = 3;
		}
		else {
			start = HeroY;	// hero below y
			end = y_coord;
			flag = 4;
		}
		for (start; start != end; start++) {
			if (!emptySpace(HeroX, start, GraphObject::none)) {
				return false;
			}
		}
	}
	else if (HeroY != y_coord && HeroX != x_coord) {
		return false;
	}
	return true;
}
void StudentWorld::addItem(const ObjType & ID) {
	Hero->addItem(ID);
}
void StudentWorld::annoyHero(const int & x_coord, const int & y_coord, const ObjType & type) {
	if ((type == BOULDER) && Hero->getY() < y_coord && (abs(x_coord-Hero->getX()) < 4.0))//if ID equals 5 it will check for any protestors or hardcore protestors that are below the boulder that is falling
		Hero->annoy(100);
	if (type == PROTESTER || type == HARDCORE_PROTESTER) {
		Hero->annoy(2);
	}
}
void StudentWorld::dropItem(const ObjType & ID) {
	if (ID == GOLD)
		Objects[GOLD].push_back(std::make_unique<Gold_Nugget>(Hero->getX(), Hero->getY(), TEMPORARY, this));
	else if (ID == SQUIRT) {
		int x_pos = Hero->getX();
		int y_pos = Hero->getY();
		GraphObject::Direction face = Hero->getDirection();
		switch (face)
		{
		case GraphObject::up:
			y_pos += 3;
			break;
		case GraphObject::down:
			y_pos -= 3;
			break;
		case GraphObject::left:
			x_pos -= 3;
			break;
		case GraphObject::right:
			x_pos += 3;
			break;
		}
		if (!IceAround(x_pos, y_pos, face) && !objectNearby(x_pos, y_pos, 3.0, BOULDER))
			Objects[SQUIRT].push_back(std::make_unique<Squirt>(x_pos, y_pos, face, this));
	}
}
int StudentWorld::move() {
	if (all_Oil_Found()) {
		Hero->annoy(10);
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
		if (rand() % (getLevel() * 25 + 300) == getLevel()) { 
			int sonar_or_water = rand() % 5;
			if (sonar_or_water == 0) {
				Objects[SONAR].push_back(make_unique<Sonar_Kit>(this));
			}
			else {
				int x;
				int y;
				do {
					x = rand() % 60;
					y = rand() % 60;
				} while (!emptySpace(x, y, GraphObject::none));
				Objects[WATER].push_back(make_unique<Water_Pool>(x, y, this));
			}
		}
		if (Objects[PROTESTER].size() + Objects[HARDCORE_PROTESTER].size() < min(15, 2 + static_cast<int>(getLevel()*1.5))
			&& game_ticks != 0 && game_ticks % max(25, 200 - static_cast<int>(getLevel())) == 0) {
			int probabilityOfHardcore = min(90, static_cast<int>(getLevel()) * 10 + 30);
			int regOrHardcore = rand() % 100;
			if (regOrHardcore < 30) {
				Objects[HARDCORE_PROTESTER].push_back(make_unique<Hardcore_Protester>(this));
			}
			else {
			Objects[PROTESTER].push_back(make_unique<Regular_Protester>(IID_PROTESTER, this));
			}
		}
		game_ticks++;
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
		auto it2 = it->begin();
		while (it2 != it->end()) {
			if (!(*it2)->isAlive()) {
				(*it2).reset();
				auto b = (*it).erase(it2);
				it2 = b;
			}
			else {
				it2++;
			}
		}
		it->shrink_to_fit();
	}
}
int StudentWorld::getHeroX() {
	return Hero->getX();
}
int StudentWorld::getHeroY() {
	return Hero->getY();
}
void StudentWorld::updateDisplayText() {
	stringstream os;
	os << "Lvl: " << setw(2) << getLevel() << " Lives: " << getLives()
		<< " Hlth: " << setw(3) << Hero->getHealth() * 10 << "% Wtr: " << setw(2) << Hero->getNumItems(SQUIRT)
		<< " Gld: " << Hero->getNumItems(GOLD) << " Oil Left: " << Objects[OIL].size()
		<< " Sonar: " << Hero->getNumItems(SONAR) << " Scr: " << setfill('0') << setw(6) << getScore();
	setGameStatText(os.str());
}
void StudentWorld::cleanUp() {
	game_ticks = 0;
	newMap->join();
	delete(newMap);
	//printMap(intSteps);

	heroMap->join();
	delete(heroMap);
	//printMap(distHero);

	intSteps.clear();
	distHero.clear();
	Hero = nullptr;
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
StudentWorld::~StudentWorld() {
	cleanUp();
}

GraphObject::Direction StudentWorld::getShortPath(const int & x_coord, const int & y_coord) {
	if (x_coord > 0 && intSteps[x_coord - 1][y_coord] != -1 && intSteps[x_coord - 1][y_coord] != -2 && intSteps[x_coord - 1][y_coord] == (intSteps[x_coord][y_coord] - 1))
		return GraphObject::left;
	if (x_coord < 61 && intSteps[x_coord + 1][y_coord] != -1 && intSteps[x_coord + 1][y_coord] != -2 && intSteps[x_coord + 1][y_coord] == (intSteps[x_coord][y_coord] - 1))
		return GraphObject::right;
	if (y_coord < 61 && intSteps[x_coord][y_coord + 1] != -1 && intSteps[x_coord][y_coord + 1] != -2 && intSteps[x_coord][y_coord + 1] == (intSteps[x_coord][y_coord] - 1))
		return GraphObject::up;
	if (y_coord > 0 && intSteps[x_coord][y_coord - 1] != -1 && intSteps[x_coord][y_coord - 1] != -2 && intSteps[x_coord][y_coord - 1] == (intSteps[x_coord][y_coord] - 1))
		return GraphObject::down;
	return GraphObject::none;
}
GraphObject::Direction StudentWorld::getPathHero(const int & x_coord, const int & y_coord) {
	if (x_coord > 0 && distHero[x_coord - 1][y_coord] != -1 && distHero[x_coord - 1][y_coord] != -2 && distHero[x_coord - 1][y_coord] == (distHero[x_coord][y_coord] - 1))
		return GraphObject::left;
	if (x_coord < 61 && distHero[x_coord + 1][y_coord] != -1 && distHero[x_coord + 1][y_coord] != -2 && distHero[x_coord + 1][y_coord] == (distHero[x_coord][y_coord] - 1))
		return GraphObject::right;
	if (y_coord < 61 && distHero[x_coord][y_coord + 1] != -1 && distHero[x_coord][y_coord + 1] != -2 && distHero[x_coord][y_coord + 1] == (distHero[x_coord][y_coord] - 1))
		return GraphObject::up;
	if (y_coord > 0 && distHero[x_coord][y_coord - 1] != -1 && distHero[x_coord][y_coord - 1] != -2 && distHero[x_coord][y_coord - 1] == (distHero[x_coord][y_coord] - 1))
		return GraphObject::down;
	return GraphObject::none;
}
void StudentWorld::updateMap(int x, int y, ObjType id, GraphObject::Direction dir) {
	if (id == ICEMAN)
	{
		switch (dir)
		{
		case GraphObject::none:
			break;
		case GraphObject::up:
			if (y <= 60) {
				if (intSteps[x][y + 3] == -2){
					intSteps[x][y + 3] = -1;
					distHero[x][y + 3] = -1;
				}
				if (intSteps[x + 1][y + 3] == -2) {
					intSteps[x + 1][y + 3] = -1;
					distHero[x+1][y + 3] = -1;
				}
				if (intSteps[x + 2][y + 3] == -2) {
					intSteps[x + 2][y + 3] = -1;
					distHero[x+2][y + 3] = -1;
				}
				if (intSteps[x + 3][y + 3] == -2) {
					intSteps[x + 3][y + 3] = -1;
					distHero[x+3][y + 3] = -1;
				}
			}
			break;
		case GraphObject::down:
			if (y >= 0) {
				if (intSteps[x][y] == -2) {
					intSteps[x][y] = -1;
					distHero[x][y] = -1;
				}
				if (intSteps[x + 1][y] == -2) {
					intSteps[x + 1][y] = -1;
					distHero[x + 1][y] = -1;
				}
				if (intSteps[x + 2][y] == -2){
					intSteps[x + 2][y] = -1;
					distHero[x + 2][y] = -1;
				}
				if (intSteps[x + 3][y] == -2) {
					intSteps[x + 3][y] = -1;
					distHero[x + 3][y + 3] = -1;
				}
			}
			break;
		case GraphObject::left:
			if (x >= 0) {
				if (intSteps[x][y] == -2) {
					intSteps[x][y] = -1;
					distHero[x][y] = -1;
				}
				if (intSteps[x][y + 1] == -2) {
					intSteps[x][y + 1] = -1;
					distHero[x][y+1] = -1;
				}
				if (intSteps[x][y + 2] == -2) {
					intSteps[x][y + 2] = -1;
					distHero[x][y+2] = -1;
				}
				if (intSteps[x][y + 3] == -2) {
					intSteps[x][y + 3] = -1;
					distHero[x][y+3] = -1;
				}
			}
			break;
		case GraphObject::right:
			if (x <= 61) {
				if (intSteps[x + 3][y] == -2) {
					intSteps[x + 3][y] = -1;
					distHero[x+3][y] = -1;
				}
				if (intSteps[x + 3][y + 1] == -2) {
					intSteps[x + 3][y + 1] = -1;
					distHero[x + 3][y+1] = -1;
				}
				if (intSteps[x + 3][y + 2] == -2) {
					intSteps[x + 3][y + 2] = -1;
					distHero[x + 3][y+2] = -1;
				}
				if (intSteps[x + 3][y + 3] == -2) {
					intSteps[x + 3][y + 3] = -1;
					distHero[x + 3][y+3] = -1;
				}
				break;
			}
		default:
			break;
		}
		
	}
	else if (id == BOULDER) {
		for (int column = x; column < x + 4; column++) {
			for (int row = y; row < y + 4; row++) {
				intSteps[column][row] = -1;
				distHero[column][row] = -1;
			}
		}
	}
}
void StudentWorld::path(int x, int y, GraphObject::Direction dir, int step, std::vector<std::vector<int>> & map) {
	bool inaccessable = false;

	if (x < (maxelement - 2) && y < (maxelement - 2)) {
		if (map[x][y] == -1 || step < map[x][y]) {
			for (int row = y; row < y + 4; row++) {
				for (int column = x; column < x + 4; column++) {
					if (intSteps[column][row] == -2) {
						inaccessable = true;
						break;
					}
				}
				if (inaccessable) {
					break;
				}
			}
			if (!inaccessable) {
				map[x][y] = step;
			}
		}
		else
			return;
		if (!inaccessable) {
			if (x > 0 && dir != GraphObject::left)
				path(x - 1, y, GraphObject::right, step + 1, map);
			if (x < map.size() - 1 && dir != GraphObject::right)
				path(x + 1, y, GraphObject::left, step + 1, map);
			if (y > 0 && dir != GraphObject::down)
				path(x, y - 1, GraphObject::up, step + 1, map);
			if (y < map[x].size() - 1 && dir != GraphObject::up)
				path(x, y + 1, GraphObject::down, step + 1, map);
		}
	}
}
void StudentWorld::getNewMap(const int & x_coord, const int & y_coord) {
	bool doneOnce = false;
	while (Hero->getHealth() > 0) {
		if (game_ticks % 10 == 0 && !doneOnce) {
			if (mapChanged) {
				for (int column = 0; column < maxelement + 1; column++) {
					for (int row = 0; row < maxelement + 1; row++) {
						if (intSteps[column][row] >= 0) {
							intSteps[column][row] = -1;
						}
					}
				}
				path(x_coord, y_coord, GraphObject::none, 0, intSteps);
				mapChanged = false;
			}
			doneOnce = true;
		}
		if(game_ticks % 10 == 1)
			doneOnce = false;
	}
}
void StudentWorld::getHeroMap(const int & x_coord, const int & y_coord) {
	while (Hero->getHealth() > 0) {
		if (game_ticks % 20 == 0) {
			for (int column = 0; column < maxelement + 1; column++) {
				for (int row = 0; row < maxelement + 1; row++) {
					if (distHero[column][row] >= 0) {
						distHero[column][row] = -1;
					}
				}
			}
			path(Hero->getX(), Hero->getY(), GraphObject::none, 0, distHero);
			//printMap(distHero);
			//cout << endl;
		}
	}
}
void StudentWorld::printMap(std::vector<std::vector<int>> & map) {
	for (int row = 63; row >= 0; row--) {
		for (int column = 0; column < maxelement + 1; column++) {
			cout << setw(3) << map[column][row];
		}
		cout << endl;
	}
}

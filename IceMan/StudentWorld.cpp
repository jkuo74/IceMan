#include "StudentWorld.h"
#include <string>
#include<iostream>
using namespace std;
StudentWorld * StudentWorld::SWP;
const static int initTunnelL = 30;
const static int initTunnelR = 33;
const static int sizeOfObject = 4;

StudentWorld::StudentWorld(string assetDir) :
	GameWorld(assetDir), oilInField(0) {
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
		if (by_itself(x_rand, y_rand, 1)) {
			Objects[BOULDER].push_back(std::make_unique<Boulder>(x_rand, y_rand, this));
			removeIce(x_rand, y_rand);
			cerr << "(" << x_rand << "," << y_rand << ")" << endl;
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
		if (by_itself(x_rand, y_rand, 1)) {
			Objects[GOLD].push_back(std::make_unique<Gold_Nugget>(x_rand, y_rand, PERMANENT, this));
			n++;
			cerr << "(" << x_rand << "," << y_rand << ")" << endl;
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
		if (by_itself(x_rand, y_rand, 1)) {
			Objects[OIL].push_back(std::make_unique<Oil_Barrel>(x_rand, y_rand, PERMANENT, this));
			n++;
			cerr << " (" << x_rand << "," << y_rand << ")" << endl;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}
bool StudentWorld::by_itself(const int & x_coord, const int & y_coord, const int & ID) {
	//ID == 0: checks if only Boulder is within 3 pixels away
	//ID == 1: set up to check if any other object is within 6 pixels away
	//ID == 2: checks if any person is within 3 pixels away
	//ID == 4: checks if only IceMan is within 3 pixels away
	//ID == 3: checks if only IceMan is within 4 pixels away
	//ID == 5 checks if any Person is within 3 pixels away and below passed in coordinates
	//ID == 6 checks id any protester is within 3 pixels away

	//this checks for objects in the game that are within a radius of 3 or 6
	if ((ID != 4 && ID != 3) && Objects.size() != 0) {
		int start = 0;//start tells you what objects to check for
		int mult_radius = 1;// mult_radius will determine wheter to check for a radius of 3 or 6
		if (ID == 1) // if ID equals 1 all objects will be checked if any other objects are within a radius of 6
			mult_radius++;
		if (ID == 2 || ID == 5 || ID == 6) {//if ID equals 2 or 5 or 6 start will increment meaning of the objects in the game, not including iceman, only protestors and hardcore protestors will be checked for
			start++;
		}
		for (auto it = Objects.begin() + 6 * start; it != Objects.end(); it++) {//loop will either start at index 0 or index 6
			for (auto it2 = it->begin(); it2 != it->end(); it2++)//iterate through all elements in sub-vector-level
				if (sqrt(pow((*it2)->getX() - x_coord, 2.0) + pow((*it2)->getY() - y_coord, 2.0)) <= (mult_radius * 3.0)) { // distance is less than 6.0 or 3.0
					if ((ID == 5) && (*it2)->getY() < y_coord) { //if ID equals 5 it will check for any protestors or hardcore protestors that are below the boulder that is falling
						(*it2)->annoy(100);
						return false;
					}
					if (ID != 5)//if ID is not five but something is within the radius, return false
						return false;
				}
			if (ID == 0)//makes it so that only checks the first sub-vector which is the boulder vector
				break;
		}
	}
	//these lines of code checks if the Hero is within some radius
	if ((ID == 2 || ID == 3 || ID == 4 || ID == 5) && sqrt(pow(Hero->getX() - x_coord, 2.0) + pow(Hero->getY() - y_coord, 2.0)) <= ((ID % 2) + 3)) { // if the ID is odd, it will check if within a radius of 4 otherwise 3
		if (ID == 5 && Hero->getY() < y_coord) {
			Hero->annoy(100);
			return false;
		}
		if (ID != 5)//if ID is not five but something is within the radius, return false
			return false;
	}
	return true;//if nothing is nearby return true
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
bool StudentWorld::makeVisible(const ObjType & obj) {
	if (obj == GOLD || obj == OIL)
		for (auto it = Objects[obj].begin(); it != Objects[obj].end(); it++) {
			if ((*it)->isAlive() && !by_itself((*it)->getX(), (*it)->getY(), 3)) {
				(*it)->setVisibility(true);
				cerr << obj << " VISIBLE " << endl;
				return true;
			}
		}
	return false;
}
bool StudentWorld::allOilFound() {
	return Objects[OIL].size() == 0;
}
bool StudentWorld::pickUpItem(const ObjType & person, const ObjType & obj) {
	if (obj != BOULDER && obj != PROTESTER && obj != HARDCORE_PROTESTER)
		for (auto it = Objects[obj].begin(); it != Objects[obj].end(); it++) {
			if (person == ICEMAN && !by_itself((*it)->getX(), (*it)->getY(), 4)) {
				(*it)->setState(DEAD);
				Hero->addItem(obj);
				cerr << obj << " PICKED UP" << endl;
				switch (obj) {
				case GOLD: case SONAR: case WATER:
					playSound(SOUND_GOT_GOODIE);
					break;
				case OIL:
					playSound(SOUND_FOUND_OIL);
					cout << "NUM OIL: " << Hero->getNumItems(OIL) << endl;
				}
				return true;
			}
			///// FIX
			else if (obj == GOLD && (*it)->getState() == TEMPORARY && !by_itself((*it)->getX(), (*it)->getY(), 6)) {
				if (person == PROTESTER) {
					(*it)->setState(DEAD);
					//cerr << person << " PICKED UP " << obj << endl;
					playSound(SOUND_PROTESTER_FOUND_GOLD);
				}
				if (person == HARDCORE_PROTESTER) {}
				return true;
			}
		}
	return false;
}
int StudentWorld::move() {
	if (Hero->getHealth() > 0) {
		for (auto it = Objects.begin(); it != Objects.end(); it++) {
			for (auto it2 = it->begin(); it2 != it->end(); it2++){
				(*it2)->doSomething();
			}
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
void StudentWorld::deleteDeadObjects() {
	for (auto it = Objects.begin(); it != Objects.end(); it++) {
		vector<int> toDelete;
		int index = 0;
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {
			if (!(*it2)->isAlive()) {
				cout << it->size() << "|unchanged|" << it->capacity() << endl;
				(*it2).reset();
				cout << it->size() << "|reseted|" << it->capacity() << endl;
				toDelete.push_back(index);
			}
			index++;
		}
		if (toDelete.size() != 0) {
			for (auto it2 = toDelete.begin(); it2 != toDelete.end(); it2++)
			{
				cout << it->size() << "|unchanged2|" << it->capacity() << endl;
				it->erase((it->begin() + *it2));
				cout << it->size() << "|shrunk|" << it->capacity() << endl;
			}
			it->shrink_to_fit();
		}
	}
}
void StudentWorld::updateDisplayText()
{
	stringstream os;
	os << "Lvl: " << getLevel() << " Lives: " << getLives() 
	<< "Hlth:" << Hero->getHealth() * 10 << "% Wtr: " << Objects[SQUIRT].size() 
	<< " Gld: " << Objects[GOLD].size() << " Oil Left: " << Objects[OIL].size() 
	<< " Sonar: " << Objects[SONAR].size() << " Scr: " << getScore();
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
	cout << "cleanup" << endl;
	for (auto & line : Objects) {
		for (auto & block : line) {
			cout << line.size() << "|" << line.capacity() << endl;
			block.reset(nullptr);
			cout << line.size() << "||" << line.capacity() << endl;
		}
		cout << line.size() << "|||" << line.capacity() << endl;
		line.clear();
		cout << line.size() << "||||" << line.capacity() << endl;
	}
	Objects.clear();
}
StudentWorld::~StudentWorld() {
	cleanUp();
}

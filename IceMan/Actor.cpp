#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
Actor::Actor(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	GraphObject(ID, x_coord, y_coord, face, size, depth), SWP(swp), state(st) {
	setVisibility(true);
}
int Actor::getState() {
	return state;
}
bool Actor::isVisible() {
	return visible;
}
bool Actor::isAlive() {
	return state != DEAD;
}
Person::Person(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, st, right, size, depth, swp), health_points(10) {}
void Person::annoy(const int & damage) {
	health_points -= damage;
}
int Person::getHealth() {
	return health_points;
}

IceMan::IceMan(StudentWorld * swp) :
	Person(IID_PLAYER, 30, 60, ALIVE, right, 1.0, 0, swp) {}
void IceMan::doSomething() {
	move();
}
void IceMan::move() {
	SWP->removeIce(getX(), getY());
	int ch;
	if (SWP->getKey(ch)) {
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = getDirection();
		switch (ch) {
		case KEY_PRESS_LEFT:
			if (left != dir)
				setDirection(left);
			else if (x_pos != 0 && SWP->by_itself(x_pos - 1, y_pos, 0))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (right != dir)
				setDirection(right);
			else if (x_pos != 60 && SWP->by_itself(x_pos + 1, y_pos, 0))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (up != dir)
				setDirection(up);
			else if (y_pos != 60 && SWP->by_itself(x_pos, y_pos + 1, 0))
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (down != dir)
				setDirection(down);
			else if (y_pos != 0 && SWP->by_itself(x_pos, y_pos - 1, 0))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		}
	}
}
void IceMan::addItem(ObjType obj) {
	if (obj > BOULDER && obj <= WATER) {  // checks if obj can be picked up
		itemArr[obj]++;
	}
}
int IceMan::getNumItems(ObjType obj) {
	if (obj > WATER) // checks if obj can be picked up FIX: BOULDER = 0
		return -1;
	return itemArr[obj];
}

/*void IceMan::gotObject(const ObjType obj) {
	if (obj == GOLD)
		num_Nuggets++;
	else if (obj == SONAR)
		num_Sonars++;
	else if (obj == WATER)
		num_Waters += 5;
}*/
Thing::Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp):
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp) {}
Ice::Ice(const int & x_coord, const int & y_coord, StudentWorld * swp):
	Thing(IID_ICE, x_coord, y_coord, PERMANENT, right, 0.25, 3, swp) {}
Oil_Barrel::Oil_Barrel(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp) :
	Thing(IID_BARREL, x_coord, y_coord, st, right, 1.0, 2, swp) {
	setVisibility(false);
}
void Oil_Barrel::doSomething() {
	switch (state) {
	case DEAD:
		break;
	case PERMANENT:
		if (!visible)
			SWP->makeVisible(OIL);
		else {
			if (SWP->pickUpItem(ICEMAN, OIL))
				SWP->increaseScore(1000);
			if (SWP->allOilFound())
				cerr << "FINAL SCORE: " << SWP->getScore() << endl; // FIX : ADVANCE TO NEXT LEVEL
			break;
		}
	}
}
Temp_Thing::Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp):
	Thing(ID, x_coord, y_coord, st, face, size, depth, swp), ticks_elapsed(0), tick_limit(max_ticks) {}
Boulder::Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Temp_Thing(IID_BOULDER, x_coord, y_coord, ALIVE, right, 1.0, 1, 30, swp) {}
void Boulder::doSomething() {
	int x_coord = getX();
	int y_coord = getY();
	if (state != DEAD) {
		if (state == 0 && !SWP->IceBelow(x_coord, y_coord))
			state = TEMPORARY;
		else if (state == TEMPORARY) {
			ticks_elapsed++;
			if (ticks_elapsed == tick_limit) {
				state = FALLING;
				SWP->playSound(IID_BOULDER);
			}
		}
		else if (state == FALLING) {
			moveTo(x_coord, y_coord - 1);
			SWP->by_itself(x_coord, y_coord, 5);
			if (y_coord <= 0 || SWP->IceBelow(x_coord, y_coord) || SWP->BoulderBelow(x_coord, y_coord)) {
				state = DEAD;
				setVisibility(false);
			}
		}
	}
}
Gold_Nugget::Gold_Nugget(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp) :
	Temp_Thing(IID_GOLD, x_coord, y_coord, st, right, 1.0, 2, 100, swp) {
	if (state != TEMPORARY) 
		setVisibility(false);
}
void Gold_Nugget::doSomething() {
	switch (state) {
	case DEAD:
		break;
	case PERMANENT:
		if (!visible)
			SWP->makeVisible(GOLD);
		else if (SWP->pickUpItem(ICEMAN, GOLD))
			SWP->increaseScore(10);
		break;
		//case TEMPORARY:
		//	// FIX
		//	if (SWP->pickUpItem(PROTESTER, GOLD)) { // FOR REG/HARDCORE PROTESTERS . DO IN PROTESTER CLASSES?
		//		SWP->increaseScore(25);
		//	}
		//	break;
	}
}
Sonar_Kit::Sonar_Kit(StudentWorld * swp):
	Temp_Thing(IID_SONAR, 0, 60, TEMPORARY, right, 1.0, 2, std::max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Sonar_Kit::doSomething(){
	if (state == TEMPORARY) {
		ticks_elapsed++;
		if (!SWP->by_itself(getX(), getY(), 2)) {
			state = DEAD;
			SWP->playSound(SOUND_GOT_GOODIE);
			SWP->increaseScore(75);
		}
		if (ticks_elapsed == tick_limit)
			state = DEAD;
	}
}

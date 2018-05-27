#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
const int KEY_PRESS_z = 'z';
const int KEY_PRESS_Z = 'Z';

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
	Person(IID_PLAYER, 30, 60, ALIVE, right, 1.0, 0, swp), itemArr{ 0, 1, 5 } {}
void IceMan::doSomething() {
	int ch;
	SWP->removeIce(getX(), getY());
	if (SWP->getKey(ch)) {
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = getDirection();
		switch (ch) {
		case KEY_PRESS_LEFT:
			if (left != dir)
				setDirection(left);
			else if (x_pos != 0 && !SWP->objectNearby(x_pos - 1, y_pos, 3.0, BOULDER))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (right != dir)
				setDirection(right);
			else if (x_pos != 60 && !SWP->objectNearby(x_pos + 1, y_pos, 3.0, BOULDER))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (up != dir)
				setDirection(up);
			else if (y_pos != 60 && !SWP->objectNearby(x_pos, y_pos + 1, 3.0, BOULDER))
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (down != dir)
				setDirection(down);
			else if (y_pos != 0 && !SWP->objectNearby(x_pos, y_pos - 1, 3.0, BOULDER))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			health_points = 0;
			break;
		case KEY_PRESS_SPACE:
			if (itemArr[SQUIRT] > 0) {
				SWP->dropItem(SQUIRT);
				SWP->playSound(SOUND_PLAYER_SQUIRT);
				itemArr[SQUIRT]--;
			}
			break;
		case KEY_PRESS_TAB:
			if (itemArr[GOLD] > 0) {
				SWP->dropItem(GOLD);
				itemArr[GOLD]--;
			}
			break;
		case KEY_PRESS_z:
		case KEY_PRESS_Z:
			if (itemArr[SONAR] > 0) {
				itemArr[SONAR]--;
				SWP->objectNearby(x_pos, y_pos, 60, GOLD); // change radius to 12
				SWP->objectNearby(x_pos, y_pos, 60, OIL);
				SWP->playSound(SOUND_SONAR);
			}
		}
	}
}

void IceMan::addItem(ObjType obj) {
	if (obj == WATER || obj == SQUIRT) {
		itemArr[SQUIRT] += 5;
	}
	else if (obj < SQUIRT) {
		itemArr[obj]++;
	}
}
int IceMan::getNumItems(ObjType obj) {
	if (obj > WATER) // checks if obj can be picked up FIX: BOULDER = 0
		return -1;
	return itemArr[obj];
}
Thing::Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp) {}
Ice::Ice(const int & x_coord, const int & y_coord, StudentWorld * swp) :
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
			if (SWP->personNearby(getX(), getY(), 4.0, 0, OIL))
				setVisibility(true);
		if (visible) {
			if (SWP->personNearby(getX(), getY(), 3.0, 0, OIL)) {
				SWP->playSound(SOUND_FOUND_OIL);
				state = DEAD;
				SWP->increaseScore(1000);
				SWP->addItem(OIL);
				cerr << "OIL PICKED UP" << endl;
			}
		}
	}
}
Temp_Thing::Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp) :
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
			//ticks_elapsed += 20;
			if (ticks_elapsed == tick_limit) {
				state = FALLING;
				SWP->playSound(SOUND_FALLING_ROCK);
			}
		}
		else if (state == FALLING) {
			moveTo(x_coord, y_coord - 1);
			SWP->personNearby(getX(), getY(), 3.0, 2, BOULDER);
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
			if (SWP->personNearby(getX(), getY(), 4.0, 0, GOLD))
				setVisibility(true);
		if (visible) {
			if (SWP->personNearby(getX(), getY(), 3.0, 0, GOLD)) {
				SWP->playSound(SOUND_GOT_GOODIE);
				state = DEAD;
				SWP->increaseScore(10);
				SWP->addItem(GOLD);
				cerr << "GOLD PICKED UP" << endl;
			}
		}
		break;
	case TEMPORARY:
		if (SWP->personNearby(getX(), getY(), 3.0, 1, GOLD)) { // FOR REG/HARDCORE PROTESTERS . DO IN PROTESTER CLASSES?
			SWP->increaseScore(25);
		}
		ticks_elapsed++;
		//ticks_elapsed += 20;
		if (ticks_elapsed == tick_limit)
			state = DEAD;
		break;
	}
}
Sonar_Kit::Sonar_Kit(StudentWorld * swp) :
	Temp_Thing(IID_SONAR, 0, 60, TEMPORARY, right, 1.0, 2, std::max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Sonar_Kit::doSomething() {
	if (state == TEMPORARY) {
		ticks_elapsed++;
		if (SWP->personNearby(getX(), getY(), 3.0, 0, SONAR)) {
			state = DEAD;
			SWP->playSound(SOUND_GOT_GOODIE);
			SWP->increaseScore(75);
			SWP->addItem(SONAR);
			cerr << "SONAR PICKED UP" << endl;
		}
		if (ticks_elapsed == tick_limit)
			state = DEAD;
	}
}
Water_Pool::Water_Pool(const int & x_coord, const int & y_coord, StudentWorld * swp):
	Temp_Thing(IID_WATER_POOL, x_coord, y_coord,TEMPORARY, right, 1.0, 2, std::max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Water_Pool::doSomething() {
	if (state == TEMPORARY) {
		ticks_elapsed++;
		//ticks_elapsed += 20;
		if (SWP->personNearby(getX(), getY(), 3.0, 0, WATER)) {
			state = DEAD;
			SWP->playSound(SOUND_GOT_GOODIE);
			SWP->addItem(SQUIRT);
			cerr << "WATER PICKED UP" << endl;
		}
		if (ticks_elapsed == tick_limit)
			state = DEAD;
	}
}
Squirt::Squirt(const int & x_coord, const int & y_coord, const GraphObject::Direction & face, StudentWorld * swp) :
	Temp_Thing(IID_WATER_SPURT, x_coord, x_coord, TEMPORARY, face, 1.0, 1, 4, swp) {}
void Squirt::doSomething() {

}

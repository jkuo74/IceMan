#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
const int KEY_PRESS_z = 'z';
const int KEY_PRESS_Z = 'Z';

Actor::Actor(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	GraphObject(ID, x_coord, y_coord, face, size, depth), SWP(swp), state(st) {
	setVisibility(true);
}
STATE Actor::getState() {
	return state;
}
void Actor::setVisibility(const bool & v){
	setVisible(v); 
	visible = v;
}
StudentWorld * Actor::getSWP(){
	return SWP;
}
bool Actor::isVisible() {
	return visible;
}
bool Actor::isAlive() {
	return state != DEAD;
}
Person::Person(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & health, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp), health_points(health) {}
void Person::annoy(const int & damage) {
	health_points -= damage;
}
int Person::getHealth() {
	return health_points;
}

IceMan::IceMan(StudentWorld * swp) :
	Person(IID_PLAYER, 30, 60, ALIVE, right, 1.0, 0, 10, swp), itemArr{ 0, 1, 5 } {}
void IceMan::doSomething() {
	int ch;
	getSWP()->removeIce(getX(), getY());
	if (getSWP()->getKey(ch)) {
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = getDirection();
		switch (ch) {
		case KEY_PRESS_LEFT:
			if (left != dir)
				setDirection(left);
			else if (x_pos != 0 && !getSWP()->objectNearby(x_pos - 1, y_pos, 3.0, BOULDER))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (right != dir)
				setDirection(right);
			else if (x_pos != 60 && !getSWP()->objectNearby(x_pos + 1, y_pos, 3.0, BOULDER))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (up != dir)
				setDirection(up);
			else if (y_pos != 60 && !getSWP()->objectNearby(x_pos,y_pos + 1, 3.0, BOULDER))
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (down != dir)
				setDirection(down);
			else if (y_pos != 0 && !getSWP()->objectNearby(x_pos, y_pos - 1, 3.0, BOULDER))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			annoy(10);
			break;
		case KEY_PRESS_SPACE:
			if (itemArr[SQUIRT] > 0) {
				getSWP()->dropItem(SQUIRT);
				getSWP()->playSound(SOUND_PLAYER_SQUIRT);
				itemArr[SQUIRT]--;
			}
			break;
		case KEY_PRESS_TAB:
			if (itemArr[GOLD] > 0) {
				getSWP()->dropItem(GOLD);
				itemArr[GOLD]--;
			}
			break;
		case KEY_PRESS_z:
		case KEY_PRESS_Z:
			if (itemArr[SONAR] > 0) {
				itemArr[SONAR]--;
				getSWP()->playSound(SOUND_SONAR);
				getSWP()->objectNearby(x_pos, y_pos, 60.0, GOLD); //FIX: CHANGE RADIUS TO 12
				getSWP()->objectNearby(x_pos, y_pos, 60.0, OIL);
			}
		}
	}
}

void IceMan::addItem(ObjType obj) {
	if (obj == WATER || obj == SQUIRT){
		itemArr[SQUIRT] += 5;
	}
	else if (obj < SQUIRT) {
		itemArr[obj]++;
	}
}
int IceMan::getNumItems(ObjType obj) {
	if (obj > OIL) 
		return -1;
	return itemArr[obj];
}
Regular_Protester:: Regular_Protester(StudentWorld * swp) : 
	Person(IID_PROTESTER, 60, 60, ALIVE, left, 1.0, 0, 5, swp) {

}
void Regular_Protester::doSomething(){
	int dir = rand() % 4;
	switch (dir) {

	}
	moveTo(getX() - 1, getY());
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
	switch (getState()) {
	case DEAD:
		break;
	case PERMANENT:
		if (!isVisible())
			if (getSWP()->personNearby(getX(), getY(), 4.0, 0, OIL))
				setVisibility(true);
		if (isVisible()) {
			if (getSWP()->personNearby(getX(), getY(), 3.0, 0, OIL)) {
				getSWP()->playSound(SOUND_FOUND_OIL);
				setState(DEAD);
				getSWP()->increaseScore(1000);
				getSWP()->addItem(OIL);
			}
		}
	}
}
Temp_Thing::Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp) :
	Thing(ID, x_coord, y_coord, st, face, size, depth, swp), ticks_elapsed(0), tick_limit(max_ticks) {}
int Temp_Thing::getTicksElapsed(){
	return ticks_elapsed;
}
int Temp_Thing::getTicksLimit() {
	return tick_limit;
}
void Temp_Thing::incrementTick() {
	ticks_elapsed++;
}
bool Temp_Thing::timeUp() {
	return ticks_elapsed == tick_limit;
}
Boulder::Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Temp_Thing(IID_BOULDER, x_coord, y_coord, ALIVE, right, 1.0, 1, 30, swp) {}
void Boulder::doSomething() {
	int x_coord = getX();
	int y_coord = getY();
	STATE st = getState();
	if (st != DEAD) {
		if (st == ALIVE && !getSWP()->IceAround(x_coord, y_coord, down))
			setState(TEMPORARY);
		else if (st == TEMPORARY) {
			incrementTick();
			if (timeUp()) {
				setState(FALLING);
				getSWP()->playSound(SOUND_FALLING_ROCK);
			}
		}
		else if (st == FALLING) {
			moveTo(x_coord, y_coord - 1);
			getSWP()->personNearby(getX(), getY(), 4.0, 2, BOULDER);
			if (y_coord <= 0 || getSWP()->IceAround(x_coord, y_coord, down) || getSWP()->BoulderBelow(x_coord, y_coord)) {
				setState(DEAD);
				setVisibility(false);
			}
		}
	}
}
Gold_Nugget::Gold_Nugget(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp) :
	Temp_Thing(IID_GOLD, x_coord, y_coord, st, right, 1.0, 2, 100, swp) {
	if (st != TEMPORARY)
		setVisibility(false);
}
void Gold_Nugget::doSomething() {
	switch (getState()) {
	case DEAD:
		break;
	case PERMANENT:
		if (!isVisible())
			if (getSWP()->personNearby(getX(), getY(), 4.0, 0, GOLD))
				setVisibility(true);
		if (isVisible()) {
			if (getSWP()->personNearby(getX(), getY(), 3.0, 0, GOLD)) {
				getSWP()->playSound(SOUND_GOT_GOODIE);
				setState(DEAD);
				getSWP()->increaseScore(10);
				getSWP()->addItem(GOLD);
			}
		}
		break;
	case TEMPORARY:
		// FIX
		if (getSWP()->personNearby(getX(), getY(), 3.0, 1, GOLD)) { // FOR REG/HARDCORE PROTESTERS . DO IN PROTESTER CLASSES?
			getSWP()->increaseScore(25);
		}
		incrementTick();
		if (timeUp())
			setState(DEAD);
			break;
	}
}
Sonar_Kit::Sonar_Kit(StudentWorld * swp):
	Temp_Thing(IID_SONAR, 0, 60, TEMPORARY, right, 1.0, 2, max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Sonar_Kit::doSomething(){
	if (getState() == TEMPORARY) {
		incrementTick();
	if (getSWP()->personNearby(getX(), getY(), 3.0, 0, SONAR)) {
		setState(DEAD);
		getSWP()->playSound(SOUND_GOT_GOODIE);
		getSWP()->increaseScore(75);
		getSWP()->addItem(SONAR);
	}
	if (timeUp())
		setState(DEAD);
	}
}
Water_Pool::Water_Pool(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Temp_Thing(IID_WATER_POOL, x_coord, y_coord, TEMPORARY, right, 1.0, 2, max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Water_Pool::doSomething() {
	if (getState() == TEMPORARY) {
		incrementTick();
		//ticks_elapsed += 20;
		if (getSWP()->personNearby(getX(), getY(), 3.0, 0, WATER)) {
			setState(DEAD);
			getSWP()->playSound(SOUND_GOT_GOODIE);
			getSWP()->addItem(SQUIRT);
			cerr << "WATER PICKED UP" << endl;
		}
		if (timeUp())
			setState(DEAD);
	}
}
Squirt::Squirt(const int & x_coord, const int & y_coord, const GraphObject::Direction & face, StudentWorld * swp):
	Temp_Thing(IID_WATER_SPURT, x_coord, y_coord, TEMPORARY, face, 1.0, 1, 4, swp) {}
void Squirt::doSomething() {
	int x_pos = getX();
	int y_pos = getY();
	GraphObject::Direction face = getDirection();
	if (!getSWP()->IceAround(x_pos, y_pos, face) && !getSWP()->personNearby(x_pos, y_pos, 3.0, 1, SQUIRT) && !getSWP()->objectNearby(x_pos, y_pos, 3.0, BOULDER) && !timeUp()) {
		switch (face) {
		case left:
			moveTo(x_pos - 1, y_pos);
			break;
		case right:
			moveTo(x_pos + 1, y_pos);
			break;
		case up:
			moveTo(x_pos, y_pos + 1);
			break;
		case down:
			moveTo(x_pos, y_pos - 1);
			break;
		}
		incrementTick();
	}
	else
		setState(DEAD);
}

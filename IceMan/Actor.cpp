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
	health_points = health_points - damage;
}
int Person::getHealth() {
	return health_points;
}

IceMan::IceMan(StudentWorld * swp):
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
Thing::Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp):
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp), tick(0) {}
Ice::Ice(const int & x_coord, const int & y_coord, StudentWorld * swp):
	Thing(IID_ICE, x_coord, y_coord, PERMANENT, right, 0.25, 3, swp) {}
Boulder::Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp):
	Thing(IID_BOULDER, x_coord, y_coord, ALIVE, right, 1.0, 1, swp) {}
void Boulder::doSomething() {
	int x_coord = getX();
	int y_coord = getY();
	if (state != DEAD) {
		if (state == 0 && !SWP->IceBelow(x_coord, y_coord))
				state = TEMPORARY;
		else if (state == TEMPORARY) {
			tick++;
			if (tick == 30) {
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
	Thing(IID_GOLD, x_coord, y_coord, st, right, 1.0, 2, swp) {
	if (state != TEMPORARY) {
		setVisibility(false);
	}
}
void Gold_Nugget::doSomething() {
	if (state == DEAD) {
		return;
	}
	if (!visible && state == PERMANENT){
		SWP->makeVisible(GOLD); //nugget is invisible ** FIX ::: !VISIBLE
		return;
	}
	if (visible) {
		SWP->pickUpItem(GOLD);
		SWP->changePoints(10);
		return;
	}
}
Temp_Thing::Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp):
	Thing(ID, x_coord, y_coord, st, face, size, depth, swp), tick_limit(max_ticks) {}
Sonar_Kit::Sonar_Kit(StudentWorld * swp):
	Temp_Thing(IID_SONAR, 0, 60, TEMPORARY, right, 1.0, 2, std::max(100, 300-10*swp->getLevel()), swp) {}
void Sonar_Kit::doSomething(){
	if (state == TEMPORARY) {
		tick++;
		if (!SWP->by_itself(getX(), getY(), 2)) {
			state = DEAD;
			SWP->playSound(SOUND_GOT_GOODIE);
			SWP->changePoints(75);
		}
		if (tick == tick_limit)
			state = DEAD;
	}
}

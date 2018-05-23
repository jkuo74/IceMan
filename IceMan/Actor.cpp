#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	GraphObject(ID, x_coord, y_coord, face, size, depth), SWP(swp),state(ALIVE) {
	setVisible(true);
	visible = true;
}
int Actor::getState()
{
	return state;
}
bool Actor::isVisible() {
	return visible;
}
bool Actor::isAlive()
{
	return state != DEAD;
}
Person::Person(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, right, size, depth, swp), health_points(10) {}
void Person::annoy()
{
	health_points = health_points - 2;
}
int Person::getHealth()
{
	return health_points;
}


IceMan::IceMan(StudentWorld * swp) :
	Person(IID_PLAYER, 30, 60, right, 1.0, 0, swp) {}
void IceMan::doSomething()
{
	move();
}

void IceMan::move()
{
	SWP->removeIce(getX(), getY());
	int ch;
	if (SWP->getKey(ch))
	{
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = getDirection();
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (left != dir)
				setDirection(left);
			else if (x_pos != 0)
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (right != dir)
				setDirection(right);
			else if (x_pos != 60)
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (up != dir)
				setDirection(up);
			else if (y_pos != 60)
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (down != dir)
				setDirection(down);
			else if (y_pos != 0)
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		}
	}
}

Thing::Thing(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, face, size, depth, swp), tick(0) {
	state = PERMANENT;
}

Ice::Ice(int x_coord, int y_coord, StudentWorld * swp) : Thing(IID_ICE, x_coord, y_coord, right, 0.25, 3, swp) {}

Boulder::Boulder(int x_coord, int y_coord, StudentWorld * swp) :
	Thing(IID_BOULDER, x_coord, y_coord, right, 1.0, 1, swp){}
void Boulder::doSomething() {
	int x_coord = getX();
	int y_coord = getY();
	if (state != DEAD) {
		if (state == PERMANENT && !SWP->IceBelow(x_coord, y_coord)) {
			state = TEMPORARY;
		}
		else if (state == TEMPORARY) {
			tick++;
			if (tick == 30) {
				state = FALLING;
				SWP->playSound(IID_BOULDER);
			}
		}
		else if (state == FALLING) {
			moveTo(x_coord, y_coord - 1);
			if (getY() <= 0 || SWP->IceBelow(x_coord, y_coord) || SWP->BoulderBelow(x_coord, y_coord)) {
				state = DEAD;
				setVisible(false);
				visible = false;
			}
		}
	}
}
Gold_Nugget::Gold_Nugget(const int & x_coord, const int & y_coord, StudentWorld * swp):
	Actor(IID_GOLD, x_coord, y_coord, right, 1.0, 2, swp)  {
	//if (state != TEMPORARY) {
	//	setVisible(false);
	//}
}
void Gold_Nugget::doSomething() {
	if (state == DEAD) {
		return;
	}
	//if (visible && SWP->makeVisible()) { //nugget is invisible ** FIX ::: !VISIBLE
	//	visible = false;
	//}
	SWP->makeVisible();
	SWP->pickUpItem();
}

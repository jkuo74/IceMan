#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth) : GraphObject(ID, x_coord, y_coord, face, size, depth)
{
	setVisible(true);
}
int Actor::getState()
{
	return state;
}
bool Actor::isVisible()
{
	return visible;
}


Person::Person(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth) : Actor(ID, x_coord, y_coord, right, 1.0, 0), health_points(10)
{

}
void Person::annoy()
{
	health_points = health_points - 2;
}
int Person::getHealth()
{
	return health_points;
}


IceMan::IceMan(): Person(IID_PLAYER, 30, 60, right)
{
}
void IceMan::doSomething()
{
	move();
}
void IceMan::getWorld(const StudentWorld * sw)
{
	SW = sw;
}

void IceMan::move()
{
	int ch;
	if (SW->getKey(ch) == true)
	{
		int x_pos = getX();
		int y_pos = getY();
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if(x_pos != 0)
				moveTo(getX() + 1, getY());
		case KEY_PRESS_RIGHT:
			if(x_pos != 60)
				moveTo(getX() - 1, getY());
		case KEY_PRESS_UP:
			if(y_pos != 60)
				moveTo(getX(), getY() + 1);
		case KEY_PRESS_DOWN:
			if(y_pos != 0)
				moveTo(getX(), getY() - 1);
		}
	}
}


Ice::Ice(int x_coord, int y_coord) : Actor(IID_ICE, x_coord, y_coord, right, 0.25, 3)
{
	if (y_coord >= 60)
		setVisible(false);
}
/*Boulder::Boulder(int x_coord, int y_coord) : Actor(IID_BOULDER, x_coord, y_coord, right, 1.0, 1)
{
}
Gold_Nugget::Gold_Nugget(int x_coord, int y_coord) : Actor(IID_GOLD, x_coord, y_coord, right, 1.0, 2)
{

}*/

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


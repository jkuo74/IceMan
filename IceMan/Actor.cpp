
#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int ID, int x_coord, int y_coord, Direction face, double size, unsigned int depth):
	GraphObject(ID, x_coord, y_coord, face, size, depth)
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
Actor::~Actor(){}
Person::Person(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth): 
	Actor(ID, x_coord, y_coord, right, size, depth), health_points(10)
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
Person::~Person(){}

IceMan::IceMan():
	Person(IID_PLAYER, 30, 60, right, 1.0, 0)
{
}
void IceMan::doSomething()
{
	move();
}

void IceMan::move()
{
	StudentWorld* swp = StudentWorld::getInstance();
	int ch;
	if (swp->getKey(ch))
	{
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = getDirection();
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (left != dir)
				setDirection(left);
			else if(x_pos != 0)
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (right != dir)
				setDirection(right);
			else if(x_pos != 60)
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (up != dir)
				setDirection(up);
			else if(y_pos != 60)
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (down != dir)
				setDirection(down);
			else if(y_pos != 0)
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		}
	}
}

IceMan::~IceMan() {}

Ice::Ice(int x_coord, int y_coord):
	Actor(IID_ICE, x_coord, y_coord, right, 0.25, 3)
{

}

Thing::Thing(int ID, int x_coord, int y_coord, Direction face, double size, unsigned int depth, StudentWorld * swp):
	Actor(ID, x_coord, y_coord, face, size, depth, swp), tick(0)
{

}

Ice::Ice(int x_coord, int y_coord, StudentWorld * swp):
	Thing(IID_ICE, x_coord, y_coord, right, 0.25, 3, swp)
{

}

Boulder::Boulder(int x_coord, int y_coord, StudentWorld * swp):
	Thing(IID_BOULDER, x_coord, y_coord, right, 1.0, 1, swp)
{
}
void Boulder::doSomething()
{
	if (state == 0)
	{
		if (!SWP->checkBelow(getX(), getY()))
		{
			SWP->playSound(IID_BOULDER);

			state = 1;
		}
	}
	else if (state == 1)
	{
		tick++;
		if (tick == 30)
		{
			state = 2;
			SWP->playSound(IID_BOULDER);
		}
	}
	else if (state == 2)
	{

	}

}


/*Protester::Protester() : Person(IID_PROTESTER, 40,60,left) {

}
void Protester::move(){}
Protester::~Protester(){}
Gold_Nugget::Gold_Nugget(int x_coord, int y_coord) : Actor(IID_GOLD, x_coord, y_coord, right, 1.0, 2)
{

}*/

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


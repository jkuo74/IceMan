#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth) : GraphObject(ID, x_coord, y_coord, face, size, depth)
{
	setVisible(true);
}
Person::Person(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth) : Actor(ID, x_coord, y_coord, right, 1.0, 0)
{

}
IceMan::IceMan(): Person(IID_PLAYER, 30, 60, right)
{
}
void IceMan::move()
{
	
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

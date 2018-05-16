#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class Actor : public GraphObject
{
public:
	Actor(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth);
};

class Person : public Actor
{
public:
	int health;
	Person(int ID, int x_coord, int y_coord, GraphObject::Direction face = right, double size = 1.0, unsigned int depth = 0);
	virtual void move() = 0;
};
class IceMan : public Person
{
public:
	IceMan();
	void move();
private:
	int hp;

};
class Regular_Protester : public Person
{
public:
};
class Hardcore_Protester : public Regular_Protester
{
public:
};





class Ice : public Actor
{
public:
	Ice(int x, int y);
};
/*
class Boulder : public Actor
{
public:
	Boulder(int x_coord, int y_coord);
};
class Gold_Nugget : public Actor
{
public:	
	Gold_Nugget(int x_coord, int y_coord);
};
class Oil_Barrel : public Actor
{
public:
	Oil_Barrel(int)
};*/
#endif // ACTOR_H_

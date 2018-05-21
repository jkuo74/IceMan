#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;


class Actor : public GraphObject
{
public:
	Actor(int ID, int x_coord, int y_coord, GraphObject::Direction face, double size, unsigned int depth);
	int getState();
	void doSomething();
	bool isVisible();
	~Actor();
private:
	int state;
	bool visible;
};

class Person : public Actor
{
public:
	Person(int ID, int x_coord, int y_coord, GraphObject::Direction face = right, double size = 1.0, unsigned int depth = 0);
	virtual void move() = 0;
	void annoy();
	int getHealth();
	~Person();
private:
	int health_points;
};
class IceMan : public Person
{
public:
	IceMan();
	void move();
	void doSomething();
	~IceMan();
};
/*class Protester : public Person {
public:
	Protester();
	void move();
	~Protester();
};
class Hardcore_Protester : public Regular_Protester
{
public:
};*/

class Thing :public Actor
{
public:
	Thing(int ID, int x_coord, int y_coord, Direction face, double size, unsigned int depth, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
protected:
	int tick;
};

class Ice : public Thing
{
public:
	Ice(int x, int y, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
};

class Boulder : public Thing
{
public:
	Boulder(int x_coord, int y_coord, StudentWorld * swp);
	virtual void doSomething();
};
/*
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

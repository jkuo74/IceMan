#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp);
	int getState();
	virtual void doSomething() = 0;
	bool isVisible();
	bool isAlive();
	virtual ~Actor() { SWP = nullptr; };

protected:
	StudentWorld * SWP;
	int state;
	bool visible;
};

class Person : public Actor
{
public:
	Person(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face = right, const double & size = 1.0, const unsigned int & depth = 0, StudentWorld * swp = nullptr);
	virtual void move() = 0;
	void annoy();
	int getHealth();
	virtual ~Person() {};

protected:
	int health_points;
};
class IceMan : public Person
{
public:
	IceMan(StudentWorld * swp = nullptr);
	void move();
	virtual void doSomething();
	virtual ~IceMan() {};
};
/*class Regular_Protester : public Person
{
public:
};
class Hardcore_Protester : public Regular_Protester
{
public:
};*/
class Thing :public Actor
{
public:
	Thing(const int & ID, const int & x_coord, const int & y_coord, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	virtual ~Thing() {};
protected:
	int tick;
};
class Ice : public Thing
{
public:
	Ice(int x, int y, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	virtual ~Ice() {};

};

class Boulder : public Thing
{
public:
	Boulder(int x_coord, int y_coord, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Boulder() {};

};
/*class Gold_Nugget : public Actor
{
public:	
	Gold_Nugget(int x_coord, int y_coord, StudentWorld * swp);
	virtual void doSomething();

};*/
/*class Oil_Barrel : public Actor
{
public:
	Oil_Barrel(int)
};*/
#endif // ACTOR_H_

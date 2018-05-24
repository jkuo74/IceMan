#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
enum STATE { ALIVE, PERMANENT, TEMPORARY, FALLING, DEAD};
class Actor : public GraphObject {
public:
	Actor(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp);
	int getState();
	virtual void doSomething() = 0;
	bool isVisible();
	bool isAlive();
	virtual void annoy(const int & damage) = 0;
	void setState(STATE st) { state = st; };
	void setVisibility(bool v) { setVisible(v); visible = v; };
	virtual ~Actor() { SWP = nullptr; };
protected:
	StudentWorld * SWP;
	STATE state;
	bool visible;
};
class Person : public Actor {
public:
	Person(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face = right, const double & size = 1.0, const unsigned int & depth = 0, StudentWorld * swp = nullptr);
	virtual void move() = 0;
	virtual void annoy(const int & damage);
	int getHealth();
	virtual ~Person() {};

protected:
	int health_points;
};
class IceMan : public Person {
public:
	IceMan(StudentWorld * swp = nullptr);
	void move();
	virtual void doSomething();
	virtual ~IceMan() {};
private:
	int num_Nuggets;
	int num_Sonars;
	int num_Waters;
};
/*class Regular_Protester : public Person
{
public:
};
class Hardcore_Protester : public Regular_Protester
{
public:
};*/
class Thing : public Actor {
public:
	Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	virtual void annoy(const int & damage) {};
	virtual ~Thing() {};
protected:
	int tick;
};
class Ice : public Thing {
public:
	Ice(const int & x_coord, const int & y_coord, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	virtual ~Ice() {};
};
class Boulder : public Thing {
public:
	Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Boulder() {};
};
class Gold_Nugget : public Thing {
public:	
	Gold_Nugget(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Gold_Nugget() {};
};
/*class Oil_Barrel : public Thing
{
public:
	Oil_Barrel(int)
};*/

class Temp_Thing : public Thing {
public:
	Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp = nullptr);
protected:
	int tick_limit;
};
class Sonar_Kit :public Temp_Thing {
public:
	Sonar_Kit(StudentWorld * swp);
	virtual void doSomething();
};
#endif // ACTOR_H_

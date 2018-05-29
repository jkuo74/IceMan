#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
enum ObjType;
enum STATE { ALIVE, PERMANENT, TEMPORARY, FALLING, DEAD };
class Actor : public GraphObject {
public:
	Actor(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp);
	STATE getState();
	virtual void doSomething() = 0;
	bool isVisible();
	bool isAlive();
	virtual void annoy(const int & damage) = 0;
	void setState(const STATE & st) { state = st; };
	void setVisibility(const bool & v);
	StudentWorld * getSWP();
	virtual ~Actor() { SWP = nullptr;};
private:
	StudentWorld * SWP;
	STATE state;
	bool visible;
};
class Person : public Actor {
public:
	Person(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size = 1.0, const unsigned int & depth = 0, const int & health = 0, StudentWorld * swp = nullptr);
	virtual void annoy(const int & damage);
	int getHealth();
	virtual ~Person() {};
private:
	int health_points;
};
class IceMan : public Person {
public:
	IceMan(StudentWorld * swp = nullptr);
	virtual void doSomething();
	void addItem(ObjType item);
	int getNumItems(ObjType item);
	virtual ~IceMan() {};
private:
	int itemArr[3]; // 0=GOLD, 1=SONAR, 2=SQUIRT
};

class Regular_Protester : public Person {
public:
	Regular_Protester(StudentWorld * swp);
	virtual void doSomething();
	virtual ~Regular_Protester() {};
};
//class Hardcore_Protester : public Regular_Protester
//{
//public:
//};
class Thing : public Actor {
public:
	Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	virtual void annoy(const int & damage) {};
	virtual ~Thing() {};
};
class Ice : public Thing {
public:
	Ice(const int & x_coord, const int & y_coord, StudentWorld * swp = nullptr);
	virtual void doSomething() {}
	~Ice() {};
};
class Oil_Barrel : public Thing {
public:
	Oil_Barrel(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Oil_Barrel() {};
};
class Temp_Thing : public Thing {
public:
	Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st,
		const GraphObject::Direction & face, const double & size, const unsigned int & depth,
		const int & max_ticks, StudentWorld * swp = nullptr);
	virtual void doSomething() {};
	int getTicksElapsed();
	int getTicksLimit();
	void incrementTick();
	bool timeUp();
	virtual ~Temp_Thing() {}
private:
	int ticks_elapsed;
	int tick_limit;
};
class Boulder : public Temp_Thing {
public:
	Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Boulder() {};
};
class Gold_Nugget : public Temp_Thing {
public:
	Gold_Nugget(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Gold_Nugget() {};
};
class Sonar_Kit :public Temp_Thing {
public:
	Sonar_Kit(StudentWorld * swp);
	virtual void doSomething();
	virtual ~Sonar_Kit() {}
};
class Water_Pool : public Temp_Thing {
public:
	Water_Pool::Water_Pool(const int & x_coord, const int & y_coord, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Water_Pool() {};
};
class Squirt : public Temp_Thing {
public:
	Squirt(const int & x_coord, const int & y_coord, const GraphObject::Direction & face, StudentWorld * swp);
	virtual void doSomething();
	virtual ~Squirt() {};
};
#endif // ACTOR_H_

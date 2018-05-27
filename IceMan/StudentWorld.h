#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include<sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
enum ObjType { GOLD, SONAR, SQUIRT, OIL, WATER, BOULDER, PROTESTER, HARDCORE_PROTESTER, ICEMAN };
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	bool personNearby(const int & x_coord, const int & y_coord, const double & radius, const int & ID1, const ObjType & ID2);
	bool objectNearby(const int & x_coord, const int & y_coord, const double & radius, const ObjType & ID2);
	virtual int init();
	virtual int move();
	void deleteDeadObjects();
	void updateDisplayText();
	void removeIce(const int & x_coord, const int & y_coord);
	bool IceBelow(const int & x_coord, const int & y_coord);
	bool BoulderBelow(const int & x_coord, const int & y_coord);
	bool allOilFound();
	void addItem(const ObjType & ID);
	void dropItem(const ObjType & ID); ///NOT COMPLETE
	virtual void cleanUp();
	static StudentWorld* getInstance();
	~StudentWorld();
private:
	static StudentWorld * SWP;
	std::vector<std::vector<Ice*>> IceBlocks;
	std::shared_ptr<IceMan> Hero;
	std::vector<std::vector<std::unique_ptr<Actor>>> Objects;
};

#endif // STUDENTWORLD_H_

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

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
enum ObjType { BOULDER, GOLD, OIL, SONAR, SQUIRT, WATER, PROTESTOR, HARDCORE_PROTESTOR };
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	bool by_itself(const int & x_coord, const int & y_coord, const int & ID);//checks that no object is placed within 6 blocks apart
	virtual int init();
	virtual int move();
	void deleteDeadObjects();
	void updateDisplayText();
	int getLevel();
	void removeIce(const int & x_coord, const int & y_coord);
	bool IceBelow(const int & x_coord, const int & y_coord);
	bool BoulderBelow(const int & x_coord, const int & y_coord);
	void changePoints(const int & points);
	bool makeVisible(ObjType obj);
	bool pickUpItem(ObjType obj);
	virtual void cleanUp();
	static StudentWorld* getInstance();
	std::shared_ptr<IceMan> getHero() { return Hero; };
	~StudentWorld();
private:
	int total_points;
	int level;
	int count;
	static StudentWorld * SWP;
	std::vector<std::vector<Ice*>> IceBlocks;
	std::shared_ptr<IceMan> Hero;
	std::vector<std::vector<std::unique_ptr<Actor>>> Objects;
};

#endif // STUDENTWORLD_H_

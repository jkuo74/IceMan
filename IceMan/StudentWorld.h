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
//Pass Student world as static pointer?
enum ObjType {BOULDER, GOLD, OIL, SONAR, WATER};
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	bool by_itself(const int & x_coord, const int & y_coord, const  int & radius);//checks that no object is placed within a radius
	virtual int init();
	virtual int move();
	//void updateDisplayText();
	void removeIce(const int & x_coord, const int & y_coord);
	bool IceBelow(const int & x_coord, const int & y_coord);
	bool BoulderBelow(const int & x_coord, const int & y_coord);
	bool makeVisible();
	void pickUpItem(const int & x_coord, const int & y_coord);

	virtual void cleanUp();
	static StudentWorld* getInstance();
	std::shared_ptr<IceMan> getHero() { return Hero; };
	~StudentWorld();
private:
	int current_level_number;
	static StudentWorld * SWP;
	std::vector<std::vector<Ice*>> IceBlocks;
	std::shared_ptr<IceMan> Hero;
	std::vector<std::vector<std::shared_ptr<Actor>>> Objects;
};

#endif // STUDENTWORLD_H_

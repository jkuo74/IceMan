#include "StudentWorld.h"
#include <string>
#include<iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
bool StudentWorld::by_itself(const int & x_coord, const int & y_coord)
{
	if(Objects.size() != 0)
		for (auto it = Objects.begin(); it != Objects.end(); it++)
		{
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				int x_dist = (*it2)->getX() - x_coord;
				int y_dist = (*it2)->getY() - y_coord;
				double distance = sqrt(pow(x_dist, 2.0) + pow(y_dist, 2.0));
				if (distance <= 6.0)
					return false;
			}
		}
	return true;

}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

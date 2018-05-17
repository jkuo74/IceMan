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

void StudentWorld::cleanUp() {
	++current_level_number; //???????????????????
	Hero = nullptr;
	//	int sum = 0;
	for (std::vector<std::shared_ptr<Ice>> & line : IceBlocks) {
		for (std::shared_ptr<Ice> & block : line) {
			block = nullptr;
			//			sum++;
		}
	}
	//	cerr << "sum " << sum << endl;
	for (auto & line : Objects) {
		for (auto & block : line) {
			block = nullptr;
			//			sum++;
		}
	}
}
StudentWorld::~StudentWorld() {
	cleanUp();
}

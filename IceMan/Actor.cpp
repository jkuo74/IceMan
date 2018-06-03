#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
const int KEY_PRESS_z = 'z';
const int KEY_PRESS_Z = 'Z';

Actor::Actor(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	GraphObject(ID, x_coord, y_coord, face, size, depth), SWP(swp), state(st) {
	setVisibility(true);
}
STATE Actor::getState() {
	return state;
}
void Actor::setVisibility(const bool & v) {
	setVisible(v);
	visible = v;
}
StudentWorld * Actor::getSWP() {
	return SWP;
}
bool Actor::isVisible() {
	return visible;
}
bool Actor::isAlive() {
	return state != DEAD;
}
Person::Person(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & health, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp), health_points(health) {}
void Person::annoy(const int & damage) {
	health_points -= damage;
}
int Person::getHealth() {
	return health_points;
}
void Person::moveInDirection(const GraphObject::Direction & dir) {
	switch (dir) {
	case up:
		setDirection(up);
		if (getY() != 60) {
			moveTo(getX(), getY() + 1);
		}
		else {
			moveTo(getX(), getY());
		}
		break;
	case down:
		setDirection(down);
		if (getY() != 0) {
			moveTo(getX(), getY() - 1);
		}
		else {
			moveTo(getX(), getY());
		}
		break;
	case left:
		setDirection(left);
		if (getX() != 0) {
			moveTo(getX() - 1, getY());
		}
		else {
			moveTo(getX(), getY());
		}
		break;
	case right:
		setDirection(right);
		if (getX() != 60) {
			moveTo(getX() + 1, getY());
		}
		else {
			moveTo(getX(), getY());
		}
		break;
	case none: default:
		moveTo(getX(), getY());
		break;
	}
}
IceMan::IceMan(StudentWorld * swp) :
	Person(IID_PLAYER, 30, 60, ALIVE, right, 1.0, 0, 10, swp), itemArr{ 10, 1, 50 } {} // FIX: ( 0,1,5)
void IceMan::doSomething() {
	int ch;
	getSWP()->removeIce(getX(), getY());
	if (getSWP()->getKey(ch)) {
		int x_pos = getX();
		int y_pos = getY();
		Direction dir = none;
		switch (ch) {
		case KEY_PRESS_LEFT:
			dir = left;
			x_pos--;
			break;
		case KEY_PRESS_RIGHT:
			dir = right;
			x_pos++;
			break;
		case KEY_PRESS_UP:
			dir = up;
			y_pos++;
			break;
		case KEY_PRESS_DOWN:
			dir = down;
			y_pos--;
			break;
		case KEY_PRESS_ESCAPE:
			annoy(10);
			break;
		case KEY_PRESS_SPACE:
			if (itemArr[SQUIRT] > 0) {
				getSWP()->dropItem(SQUIRT);
				getSWP()->playSound(SOUND_PLAYER_SQUIRT);
				itemArr[SQUIRT]--;
			}
			break;
		case KEY_PRESS_TAB:
			if (itemArr[GOLD] > 0) {
				getSWP()->dropItem(GOLD);
				itemArr[GOLD]--;
			}
			break;
		case KEY_PRESS_z:
		case KEY_PRESS_Z:
			if (itemArr[SONAR] > 0) {
				itemArr[SONAR]--;
				getSWP()->playSound(SOUND_SONAR);
				getSWP()->objectNearby(x_pos, y_pos, 60.0, GOLD); //FIX: CHANGE RADIUS TO 12
				getSWP()->objectNearby(x_pos, y_pos, 60.0, OIL);
			}
		}
		if (dir != none && !getSWP()->objectNearby(x_pos, y_pos, 3.0, BOULDER)) {
			moveInDirection(dir);
		}
	}
}

void IceMan::addItem(ObjType obj) {
	if (obj == WATER || obj == SQUIRT) {
		itemArr[SQUIRT] += 5;
	}
	else if (obj < SQUIRT) {
		itemArr[obj]++;
	}
}
int IceMan::getNumItems(ObjType obj) {
	if (obj > OIL)
		return -1;
	return itemArr[obj];
}
Regular_Protester::Regular_Protester(StudentWorld * swp) :
	Person(IID_PROTESTER, 60, 60, ALIVE, left, 1.0, 1, 5, swp), stepsToTake(0), ticksToTurn(200),
	ticksToMove(max(0, 3 - static_cast<int>(swp->getLevel() / 4))), ticks_elapsed(0), ticksLeftToAnnoy(15) {}
void Regular_Protester::doSomething() {
	//	GraphOjbect:  enum Direction { none, up, down, left, right };
	int x_coord = getX();
	int y_coord = getY();
	bool heroNear = getSWP()->personNearby(x_coord, y_coord, 4.0, 0, PROTESTER);
	bool turned = false;
	Direction ogDirection = getDirection();
	if (getHealth() <= 0) {
		setState(DEAD); //setState(TEMPORARY);
		return;
	}
	if (heroNear) {
		int x_dist = getSWP()->getHeroX() - x_coord;
		int y_dist = getSWP()->getHeroY() - y_coord;
		if (abs(x_dist) <= abs(y_dist) || getY() == 60 || getY() == 0) {  // only done when heroNear?
			if (x_dist < 0) {
				setDirection(left);
			}
			else {
				setDirection(right);
			}
		}
		else {
			if (y_dist < 0) {
				setDirection(down);
			}
			else {
				setDirection(up);
			}
		}
	}
	if (ticks_elapsed % ticksToMove == 0) {
		if (ticksLeftToAnnoy == 15 && heroNear) {
			getSWP()->playSound(SOUND_PROTESTER_YELL);
			getSWP()->annoyHero(x_coord, y_coord, PROTESTER);
			ticksLeftToAnnoy--;
		}
		else if (ticksLeftToAnnoy == 0) {
			ticksLeftToAnnoy = 15;
		}
		else {
			ticksLeftToAnnoy--;
		}
		int flag = -1;
		if (getSWP()->clearPath(x_coord, y_coord, flag)) {
			if (flag == 1) {
				setDirection(right);
			}
			else if (flag == 2) {
				setDirection(left);
			}
			else if (flag == 3) {
				setDirection(up);
			}
			else if (flag == 4) {
				setDirection(down);
			}
		} 
		if (!heroNear) {
			Direction  updated_direction = getDirection();
			if (ticksToTurn <= 0) {
				int rand_direction = 0;
				int new_direction = 0;
				if (ogDirection == left || ogDirection == right) {
					if (getSWP()->emptySpace(x_coord, y_coord, up)) {
						rand_direction++;
					}
					if (getSWP()->emptySpace(x_coord, y_coord, down)) {
						rand_direction += 2;
					}
					if (rand_direction == 3)
						new_direction = (rand() % 2) + 1;
					if (rand_direction == 1 || new_direction == 1)
						setDirection(up);
					else if (rand_direction == 2 || new_direction == 2)
						setDirection(down);
				}
				else {
					if (getSWP()->emptySpace(x_coord, y_coord, left)) {
						rand_direction++;
					}
					if (getSWP()->emptySpace(x_coord, y_coord, right)) {
						rand_direction += 2;
					}
					if (rand_direction == 3)
						new_direction = (rand() % 2) + 1;
					if (rand_direction == 1 || new_direction == 1)
						setDirection(left);
					else if (rand_direction == 2 || new_direction == 2)
						setDirection(right);
				}
				if (rand_direction != 0) {
					stepsToTake = (rand() % 53) + 8;
					ticksToTurn = 200;
					turned = true;
					updated_direction = getDirection();
				}
			}
			else cout << "cannot change" << endl;
			if (!turned && (stepsToTake == 0 || !getSWP()->emptySpace(x_coord, y_coord, getDirection()))) {
				stepsToTake = (rand() % 53) + 8;
				do {
					updated_direction = static_cast<Direction>(rand() % 4 + 1);
				} while (!getSWP()->emptySpace(getX(), getY(), updated_direction));
			}
			cout << "new steps" << endl;
			if (stepsToTake > 0) {
				moveInDirection(updated_direction);
				stepsToTake--;
				ticksToTurn--;
			}
		}
	}
	if (getState() == TEMPORARY) {
		//
	}
	ticks_elapsed++;

}
Thing::Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, StudentWorld * swp) :
	Actor(ID, x_coord, y_coord, st, face, size, depth, swp) {}
Ice::Ice(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Thing(IID_ICE, x_coord, y_coord, ALIVE, right, 0.25, 3, swp) {}
Oil_Barrel::Oil_Barrel(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp) :
	Thing(IID_BARREL, x_coord, y_coord, st, right, 1.0, 2, swp) {
	setVisibility(false);
}
void Oil_Barrel::doSomething() {
	switch (getState()) {
	case DEAD:
		break;
	case ALIVE:
		if (!isVisible())
			if (getSWP()->personNearby(getX(), getY(), 4.0, 0, OIL))
				setVisibility(true);
		if (isVisible()) {
			if (getSWP()->personNearby(getX(), getY(), 3.0, 0, OIL)) {
				getSWP()->playSound(SOUND_FOUND_OIL);
				setState(DEAD);
				getSWP()->increaseScore(1000);
				getSWP()->addItem(OIL);
			}
		}
	}
}
Temp_Thing::Temp_Thing(const int & ID, const int & x_coord, const int & y_coord, const STATE & st, const GraphObject::Direction & face, const double & size, const unsigned int & depth, const int & max_ticks, StudentWorld * swp) :
	Thing(ID, x_coord, y_coord, st, face, size, depth, swp), ticks_elapsed(0), tick_limit(max_ticks) {}
int Temp_Thing::getTicksElapsed() {
	return ticks_elapsed;
}
int Temp_Thing::getTicksLimit() {
	return tick_limit;
}
void Temp_Thing::incrementTick() {
	ticks_elapsed++;
}
bool Temp_Thing::timeUp() {
	return ticks_elapsed == tick_limit;
}
Boulder::Boulder(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Temp_Thing(IID_BOULDER, x_coord, y_coord, ALIVE, right, 1.0, 1, 30, swp) {}
void Boulder::doSomething() {
	int x_coord = getX();
	int y_coord = getY();
	STATE st = getState();
	if (st != DEAD) {
		if (st == ALIVE && !getSWP()->IceAround(x_coord, y_coord, down))
			setState(TEMPORARY);
		else if (st == TEMPORARY) {
			incrementTick();
			if (timeUp()) {
				setState(FALLING);
				getSWP()->playSound(SOUND_FALLING_ROCK);
			}
		}
		else if (st == FALLING) {
			moveTo(x_coord, y_coord - 1);
			if (getSWP()->personNearby(x_coord, y_coord, 4.0, 2, BOULDER)) {
				getSWP()->annoyHero(x_coord, y_coord, BOULDER);
			}
			if (y_coord <= 0 || getSWP()->IceAround(x_coord, y_coord, down) || getSWP()->BoulderBelow(x_coord, y_coord)) {
				setState(DEAD);
				setVisibility(false);
			}
		}
	}
}
Gold_Nugget::Gold_Nugget(const int & x_coord, const int & y_coord, const STATE & st, StudentWorld * swp) :
	Temp_Thing(IID_GOLD, x_coord, y_coord, st, right, 1.0, 2, 100, swp) {
	if (st != TEMPORARY)
		setVisibility(false);
}
void Gold_Nugget::doSomething() {
	switch (getState()) {
	case DEAD:
		break;
	case ALIVE:
		if (!isVisible())
			if (getSWP()->personNearby(getX(), getY(), 4.0, 0, GOLD))
				setVisibility(true);
		if (isVisible()) {
			if (getSWP()->personNearby(getX(), getY(), 3.0, 0, GOLD)) {
				getSWP()->playSound(SOUND_GOT_GOODIE);
				setState(DEAD);
				getSWP()->increaseScore(10);
				getSWP()->addItem(GOLD);
			}
		}
		break;
	case TEMPORARY:
		// FIX
		if (getSWP()->personNearby(getX(), getY(), 3.0, 1, GOLD)) { // FOR REG/HARDCORE PROTESTERS . DO IN PROTESTER CLASSES?
			getSWP()->increaseScore(25);
			setState(DEAD);
		}
		incrementTick();
		if (timeUp())
			setState(DEAD);
		break;
	}
}
Sonar_Kit::Sonar_Kit(StudentWorld * swp) :
	Temp_Thing(IID_SONAR, 0, 60, TEMPORARY, right, 1.0, 2, max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Sonar_Kit::doSomething() {
	if (getState() == TEMPORARY) {
		incrementTick();
		if (getSWP()->personNearby(getX(), getY(), 3.0, 0, SONAR)) {
			setState(DEAD);
			getSWP()->playSound(SOUND_GOT_GOODIE);
			getSWP()->increaseScore(75);
			getSWP()->addItem(SONAR);
		}
		if (timeUp())
			setState(DEAD);
	}
}
Water_Pool::Water_Pool(const int & x_coord, const int & y_coord, StudentWorld * swp) :
	Temp_Thing(IID_WATER_POOL, x_coord, y_coord, TEMPORARY, right, 1.0, 2, max(100, 300 - 10 * static_cast<int>(swp->getLevel())), swp) {}
void Water_Pool::doSomething() {
	if (getState() == TEMPORARY) {
		incrementTick();
		//ticks_elapsed += 20;
		if (getSWP()->personNearby(getX(), getY(), 3.0, 0, WATER)) {
			setState(DEAD);
			getSWP()->playSound(SOUND_GOT_GOODIE);
			getSWP()->addItem(SQUIRT);
			cerr << "WATER PICKED UP" << endl;
		}
		if (timeUp())
			setState(DEAD);
	}
}
Squirt::Squirt(const int & x_coord, const int & y_coord, const GraphObject::Direction & face, StudentWorld * swp) :
	Temp_Thing(IID_WATER_SPURT, x_coord, y_coord, TEMPORARY, face, 1.0, 1, 4, swp) {}
void Squirt::doSomething() {
	int x_pos = getX();
	int y_pos = getY();
	GraphObject::Direction face = getDirection();
	if (getSWP()->emptySpace(x_pos, y_pos, face) && !getSWP()->personNearby(x_pos, y_pos, 3.0, 1, SQUIRT) && !timeUp()) {
		switch (face) {
		case left:
			moveTo(x_pos - 1, y_pos);
			break;
		case right:
			moveTo(x_pos + 1, y_pos);
			break;
		case up:
			moveTo(x_pos, y_pos + 1);
			break;
		case down:
			moveTo(x_pos, y_pos - 1);
			break;
		}
		incrementTick();
	}
	else
		setState(DEAD);
}

#pragma once

#ifndef _MY_STRATEGY_H_
#define _MY_STRATEGY_H_

#include <list>
#include <map>
#include <math.h>

#include "Strategy.h"


/**
* Вспомогательный класс для хранения позиций на карте.
*/
class Point2D {
	double x;
	double y;

public:
	Point2D() {
		this->x = 0;
		this->y = 0;
	}

	Point2D(double _x, double _y) {
		this->x = _x;
		this->y = _y;
	}

	double getX() const {
		return x;
	}

	double getY() const {
		return y;
	}

	double getDistanceTo(double x, double y) const {
		return sqrt((this->x - x)*(this->x - x) + (this->y - y)*(this->y - y));
	}

	double getDistanceTo(Point2D point) {
		return getDistanceTo(point.x, point.y);
	}

	double getDistanceTo(model::Unit  unit) {
		return getDistanceTo(unit.getX(), unit.getY());
	}
	Point2D operator+ (const Point2D& p) const
	{
		return Point2D(x + p.x, y + p.y);
	}
};


class MyStrategy : public Strategy {
protected:
	// critical values
	double WAYPOINT_RADIUS;
	double LOW_HP_FACTOR;
	double cooldown;

	bool bonusChecked;
	int bonusCheckTicks;

	int STRAFE_FACTOR;
	int strafeTicks;
	int lastStrafeDirection;

	std::map<model::LaneType, std::vector<Point2D>> waypointsByLane;

	//Random random;

	model::LaneType lane;
	std::vector<Point2D> waypoints;

	model::Wizard self;
	model::World world;
	model::Game game;
	model::Move my_move;

	///////////////////////////
	model::LivingUnit * closestFriend;
	
	//enemies
	model::LivingUnit * closestEnemy;	
	model::LivingUnit * closestWizard;
	model::LivingUnit * closestBuilding;
	model::LivingUnit * closestMinion;
	model::LivingUnit * weakestEnemy;

	Point2D myLastPos;
	Point2D posBeforeBonus;
	bool returnToLastPos;


	void initializeStrategy(const model::Wizard& _self, const model::Game& _game);
	void initializeTick(const model::Wizard& _self, const model::World& _world, const model::Game& _game, const model::Move& _move);
	Point2D getNextWaypoint();
	Point2D getPreviousWaypoint();
	void goTo(const Point2D & point, model::Move& _move);
	void goBackwardTo(const Point2D & point, model::Move& _move);
	void goBackwardFrom(const Point2D & point, model::Move& _move);
	void goTangentialFrom(const Point2D & point, model::Move& _move);
	int getCloseToBonus(model::Move& _move);
	bool getBonus(model::Move& _move);
	model::LivingUnit&  getNearestTarget();
	model::LivingUnit&  getNearestBuilding();
	model::LivingUnit&  getNearestWizard();
	model::LivingUnit&  getNearestMinion();
	model::LivingUnit&  getNearestFriend();
	model::LivingUnit&  getCloseAndWeakTarget();
	void getTargets();
	void setStrafe(model::Move& _move);

	void attackEnemy(const model::Wizard& _self, const model::World& _world, const model::Game& _game, model::Move& _move, const model::LivingUnit& enemy);

public:
    MyStrategy();

    void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) override;
};

#endif

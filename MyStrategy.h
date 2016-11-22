#pragma once

#ifndef _MY_STRATEGY_H_
#define _MY_STRATEGY_H_

#include <list>
#include <map>

#include "Strategy.h"
#include "Point2D.h"


class MyStrategy : public Strategy {
protected:
	// critical values
	double WAYPOINT_RADIUS;
	double LOW_HP_FACTOR;
	double cooldown;

	std::map<model::LaneType, std::vector<Point2D>> waypointsByLane;

	//Random random;

	model::LaneType lane;
	std::vector<Point2D> waypoints;

	model::Wizard self;
	model::World world;
	model::Game game;
	model::Move my_move;

	void initializeStrategy(const model::Wizard& _self, const model::Game& _game);
	void initializeTick(const model::Wizard& _self, const model::World& _world, const model::Game& _game, const model::Move& _move);
	Point2D getNextWaypoint();
	Point2D getPreviousWaypoint();
	void goTo(Point2D & point);
	model::LivingUnit&  getNearestTarget();
	model::LivingUnit&  getCloseAndWeakTarget();

public:
    MyStrategy();

    void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) override;
};

#endif

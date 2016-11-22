#pragma once

#ifndef _MY_STRATEGY_H_
#define _MY_STRATEGY_H_

#include <list>

#include "Strategy.h"
#include "Point2D.h"


class MyStrategy : public Strategy {
protected:
	// critical values
	double criticalHealth;
	double cooldown;

	//final Map<LaneType, Point2D[]> waypointsByLane = new EnumMap<>(LaneType.class);

	//Random random;

	//LaneType lane;
	Point2D waypoints;

	model::Wizard self;
	model::World world;
	model::Game game;
	model::Move _move;

	void goTo(Point2D & point);
	model::LivingUnit&  getNearestTarget();
	model::LivingUnit&  getCloseAndWeakTarget();

public:
    MyStrategy();

    void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) override;
};

#endif

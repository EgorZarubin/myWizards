#pragma once

#ifndef _MY_STRATEGY_H_
#define _MY_STRATEGY_H_

#include <list>
#include <map>
#include <math.h>
#include <memory>

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
	double SPEED_BONUS_FACTOR;
	bool ALLOW_PREDICTION;
	
	bool isSkillsEnable;
	int numOfLearnedSkills;
	int nextSkill;
	model::SkillType skillToLearn;

	//bonus
	bool underBonus;
	model::Bonus bonus;
	bool bonusChecked;
	int lastBonusCheck;
	int prevLife;	

	int STRAFE_FACTOR;
	int strafeTicks;
	int lastStrafeDirection;

	int lastDodgeDir;

	model::LaneType changeLaneTo;
	std::map<model::LaneType, std::vector<Point2D>> waypointsByLane;
	model::LaneType lane;
	std::vector<Point2D> waypoints;

	//////////////////////////////
	std::vector<std::vector<int>> myMap;
	//////////////////////////

	

	model::Wizard self;
	model::World world;
	model::Game game;
	model::Move my_move;

	///////////////////////////
	std::shared_ptr <model::LivingUnit> closestFriend;
	
	//enemies
	std::shared_ptr <model::LivingUnit> closestEnemy;
	std::shared_ptr <model::LivingUnit> closestWizard;
	std::shared_ptr<model::LivingUnit> closestBuilding;
	std::shared_ptr<model::LivingUnit> closestMinion;
	std::shared_ptr<model::LivingUnit> closestNeutral;
	std::shared_ptr<model::LivingUnit> weakestEnemy;
	double d_f, d_e, d_w, d_b, d_m, d_wt, d_n;

	Point2D myLastPos;
	Point2D posBeforeBonus;
	bool returnToLastPos;

	void clearValues();

	void initializeStrategy(const model::Wizard& _self, const model::Game& _game);
	void initializeTick(const model::Wizard& _self, const model::World& _world, const model::Game& _game, const model::Move& _move);
	Point2D getNextWaypoint();
	Point2D getPreviousWaypoint();
	
	void goTo(const Point2D & point, model::Move& _move);
	void goToAdv(const Point2D & point, model::Move& _move);
	void goBackwardTo(const Point2D & point, model::Move& _move);
	void goBackwardToAdv(const Point2D & point, model::Move& _move);
	void goBackwardFrom(const Point2D & point, model::Move& _move);
	void goTangentialFrom(const Point2D & point, const Point2D & nextPoint, model::Move& _move);
	
	
	int getCloseToBonus(model::Move& _move);
	bool getBonus(model::Move& _move);
	void getTargets();
	model::Tree getClosestTree();	

	void attackEnemy(const model::Wizard& _self, const model::World& _world, const model::Game& _game, model::Move& _move, const model::LivingUnit& enemy);
	void attackEnemyAdv(const model::Wizard& _self, const model::World& _world, const model::Game& _game, model::Move& _move, const model::LivingUnit& enemy);
	void dodgeFrom(const model::Wizard& _self, const model::World& _world, const model::Game& _game, model::Move& _move, const model::LivingUnit& enemy);
	void setStrafe(model::Move& _move);
	
	void learnSkills(const model::Wizard& _self, model::Move& _move);
	

	void setMessage(model::Move& _move);
	void getMessage();

public:
    MyStrategy();

    void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) override;
};

#endif

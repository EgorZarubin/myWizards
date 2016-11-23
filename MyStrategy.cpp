#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;


// todo разделить цели по приоритетам : тип юнита, здоровье
// сделать  хорошие опорные точки (возможно подходить к ним ближе)
// улучшить движение задним ходом

void MyStrategy::move(const Wizard& _self, const World& _world, const Game& _game, Move& _move) {
  	
	initializeStrategy(_self, _game);
	initializeTick(_self, _world, _game, _move);

	// ѕосто€нно двигаемс€ из-стороны в сторону, чтобы по нам было сложнее попасть.
	// —читаете, что сможете придумать более эффективный алгоритм уклонени€? ѕопробуйте! ;)
	_move.setStrafeSpeed( rand()%2 ? _game.getWizardStrafeSpeed() : -_game.getWizardStrafeSpeed());

	
	
	
	getTargets();
	double d_f, d_e, d_w, d_b, d_m, d_wt;
	d_f = d_e = d_w = d_b = d_m = d_wt = 6000;
	if (closestFriend != nullptr) d_f = _self.getDistanceTo(*closestFriend);
	if (closestEnemy != nullptr) d_e = _self.getDistanceTo(*closestEnemy);
	if (closestBuilding != nullptr) d_b = _self.getDistanceTo(*closestBuilding);
	if (closestWizard != nullptr) d_w = _self.getDistanceTo(*closestWizard);
	if (closestMinion != nullptr) d_m = _self.getDistanceTo(*closestMinion);
	if (weakestEnemy != nullptr) d_wt = _self.getDistanceTo(*weakestEnemy);

	LivingUnit nearestTarget = getCloseAndWeakTarget();// getNearestTarget();


	//// приоритет атаки
	//if (d_e < _self.getCastRange()) //если хоть кто то в пределах дос€гаемости
	//{		
	//	_move.setSpeed(0);
	//	LivingUnit enemy = *closestEnemy;
	//	if (d_wt < _self.getCastRange())
	//	{
	//		enemy = *weakestEnemy;//
	//	}
	//	else
	//	if (d_e < 70 || closestEnemy->getLife() <  24)
	//	{
	//		//enemy = *closestEnemy;
	//	}
	//	else if(d_b < _self.getCastRange())
	//	{
	//		enemy = *closestBuilding;//
	//	}
	//	else if (d_w < _self.getCastRange())
	//	{
	//		enemy = *closestWizard;//
	//	}
	//	else if (d_m < _self.getCastRange())
	//	{
	//		enemy = *closestMinion;//
	//	}
	//	attackEnemy(_self, _world, _game, _move, enemy);
	//	return;
	//}

	// ≈сли видим противника ...
	if (nearestTarget.getId() != self.getId())
	{

		double distance = _self.getDistanceTo(nearestTarget);

		if (d_wt != distance)
		{
			int i = 0;
		}
		// ... и он в пределах дос€гаемости наших заклинаний, ...
		if (distance <= _self.getCastRange())
		{
			//return;
			_move.setSpeed(0);

			double angle = _self.getAngleTo(nearestTarget);

			// ... то поворачиваемс€ к цели.
			_move.setTurn(angle);

			// ≈сли цель перед нами, ...
			if (_self.getRemainingActionCooldownTicks() == 0)
			{
				if (fabs(angle) < game.getStaffSector() / 2.0)
				{
					if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance < 70)
						_move.setAction(ActionType::ACTION_STAFF);
					else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE > 10])
						_move.setAction(ActionType::ACTION_FIREBALL);
					else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
					{
						_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
						_move.setCastAngle(angle);
						_move.setMinCastDistance(distance - nearestTarget.getRadius() + game.getMagicMissileRadius());
					}
				}
			}
			else if (_self.getRemainingActionCooldownTicks() < 20)
				return;
			else if (distance < 400)
			{
				goBackward(getPreviousWaypoint(), _move);
			}
			return;
		}
	}
	
	// ≈сли осталось мало жизненной энергии, отступаем задом к предыдущей ключевой точке на линии.
	if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR)
	{
		goBackward(getPreviousWaypoint(), _move);
	}
	else if (d_e < 600)// ≈сли нет других действий, просто продвигаемс€ вперЄд.
		goTo(Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
	else if (d_f > 400 && d_f< 6000 && closestFriend->getRadius() < 100)// бежим к друзь€м, если они далеко b и это не база // надо бы избегать деревьев
		goTo(Point2D(closestFriend->getX(), closestFriend->getY()), _move);
	else
		goTo(getNextWaypoint(), _move);
	return;
}


/**
* »нциализируем стратегию.
* <p>
* ƒл€ этих целей обычно можно использовать конструктор, однако в данном случае мы хотим инициализировать генератор
* случайных чисел значением, полученным от симул€тора игры.
*/
void MyStrategy::initializeStrategy(const Wizard& _self, const Game& _game) {
	
	//if (random == null) {
	//	random = new Random(game.getRandomSeed());

		double mapSize = _game.getMapSize();
		
		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_MIDDLE, vector<Point2D>{
			Point2D(100.0, mapSize - 600),
			Point2D(600.0, mapSize - 500),
			Point2D(1110.0, mapSize - 1000),
			Point2D(mapSize - 700.0, 700.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_TOP, vector<Point2D>{
			 Point2D(100.0, mapSize * 0.9),
			 Point2D(200.0, mapSize * 0.75),
			 Point2D(200.0, mapSize * 0.5),
			 Point2D(200.0, mapSize * 0.25),			
			 Point2D(mapSize * 0.25, 200.0),
			 Point2D(mapSize * 0.5, 200.0),
			 Point2D(mapSize * 0.75, 200.0),
			 Point2D(mapSize - 200.0, 200.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_BOTTOM, vector<Point2D>{
			 Point2D(100.0, mapSize - 100.0), // go down
			 Point2D(600.0, mapSize - 200.0),
			 Point2D(mapSize * 0.25, mapSize - 200.0),
			 Point2D(mapSize * 0.5, mapSize - 200.0),
			 Point2D(mapSize * 0.75, mapSize - 200.0),
			 Point2D(mapSize - 200.0, mapSize - 200.0),
			 Point2D(mapSize - 200.0, mapSize * 0.75),
			 Point2D(mapSize - 200.0, mapSize * 0.5),
			 Point2D(mapSize - 200.0, mapSize * 0.25),
			 Point2D(mapSize - 200.0, 200.0)
		}));

		switch (static_cast<int>(_self.getId())) 
		{
		case 1:
		case 2:
		case 6:
		case 7:
			lane = LaneType::LANE_TOP;
			break;
		case 3:
		case 8:
			lane = LaneType::LANE_MIDDLE;
			break;
		case 4:
		case 5:
		case 9:
		case 10:
			lane = LaneType::LANE_BOTTOM;
			break;
		default: break;
		}

		waypoints = waypointsByLane[lane];

		// Ќаша стратеги€ исходит из предположени€, что заданные нами ключевые точки упор€дочены по убыванию
		// дальности до последней ключевой точки. —ейчас проверка этого факта отключена, однако вы можете
		// написать свою проверку, если решите изменить координаты ключевых точек.

		/*Point2D lastWaypoint = waypoints[waypoints.length - 1];

		Preconditions.checkState(ArrayUtils.isSorted(waypoints, (waypointA, waypointB) -> Double.compare(
		waypointB.getDistanceTo(lastWaypoint), waypointA.getDistanceTo(lastWaypoint)
		)));*/
}


/**
* —охран€ем все входные данные в пол€х класса дл€ упрощени€ доступа к ним.
*/
void MyStrategy::initializeTick(const Wizard& _self, const World& _world, const Game& _game, const Move& _move) {
	this->self = _self;
	this->world = _world;
	this->game = _game;
	this->my_move = _move;
}

 Point2D MyStrategy::getNextWaypoint() {
	int lastWaypointIndex = waypoints.size() - 1;
	Point2D lastWaypoint = waypoints[lastWaypointIndex];

	for (int waypointIndex = 0; waypointIndex < lastWaypointIndex; ++waypointIndex) {
		Point2D waypoint = waypoints[waypointIndex];

		if (waypoint.getDistanceTo(self) <= WAYPOINT_RADIUS) {
			return waypoints[waypointIndex + 1];
		}

		if (lastWaypoint.getDistanceTo(waypoint) < lastWaypoint.getDistanceTo(self)) {
			return waypoint;
		}
	}

	return lastWaypoint;
}

/**
* ƒействие данного метода абсолютно идентично действию метода {@code getNextWaypoint}, если перевернуть массив
* {@code waypoints}.
*/
Point2D MyStrategy::getPreviousWaypoint() {
	Point2D firstWaypoint = waypoints[0];

	for (int waypointIndex = waypoints.size() - 1; waypointIndex > 0; --waypointIndex) {
		Point2D waypoint = waypoints[waypointIndex];

		if (waypoint.getDistanceTo(self) <= WAYPOINT_RADIUS) {
			return waypoints[waypointIndex - 1];
		}

		if (firstWaypoint.getDistanceTo(waypoint) < firstWaypoint.getDistanceTo(self)) {
			return waypoint;
		}
	}

	return firstWaypoint;
}

void MyStrategy::goTo(const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(point.getX(), point.getY());

	_move.setTurn(angle);

	//if (fabs(angle) < game.getStaffSector() / 4.0) {
		_move.setSpeed(game.getWizardForwardSpeed());
	//}
}

void MyStrategy::goBackward(const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(point.getX(), point.getY());

	_move.setTurn(angle+PI);

	//if (fabs(angle) < game.getStaffSector() / 4.0) {
	_move.setSpeed(-game.getWizardForwardSpeed());
	//}
}

LivingUnit & MyStrategy::getNearestTarget()
{		
	std::vector<LivingUnit *> targets;
	targets.clear();
	//LivingUnit =
	for(unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back( new LivingUnit(world.getBuildings()[i] ));
	}
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getWizards()[i]));
	}
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getMinions()[i]));
	}	

	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;
	double minDist = 1e10;

	for (auto &u : targets)
	{
		if ( !(u->getFaction() == Faction::FACTION_NEUTRAL || u->getFaction() == self.getFaction()) &&	(u->getDistanceTo(self) < minDist))
		{
			minDist = u->getDistanceTo(self);
			unit = u;
		}
	}

	/*std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		const double _x = self.getX();
		const double _y = self.getY();
		return (u1->getDistanceTo(_x,_y) < u2->getDistanceTo(0,0));
	});*/

	return *unit;
}

LivingUnit & MyStrategy::getNearestBuilding()
{
	std::vector<LivingUnit *> targets;
	for (unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getBuildings()[i]));
	}
	
	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;
	double minDist = 1e10;

	for (auto &u : targets)
	{
		if (!(u->getFaction() == Faction::FACTION_NEUTRAL || u->getFaction() == self.getFaction()) && (u->getDistanceTo(self) < minDist))
		{
			minDist = u->getDistanceTo(self);
			unit = u;
		}
	}
	closestBuilding = unit;
	return *unit;
}

LivingUnit & MyStrategy::getNearestWizard()
{
	std::vector<LivingUnit *> targets;
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getWizards()[i]));
	}

	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;
	double minDist = 1e10;

	for (auto &u : targets)
	{
		if (!(u->getFaction() == Faction::FACTION_NEUTRAL || u->getFaction() == self.getFaction()) && (u->getDistanceTo(self) < minDist))
		{
			minDist = u->getDistanceTo(self);
			unit = u;
		}
	}
	closestWizard = unit;
	return *unit;
}

LivingUnit & MyStrategy::getNearestMinion()
{
	std::vector<LivingUnit *> targets;
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getMinions()[i]));
	}

	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;
	double minDist = 1e10;

	for (auto &u : targets)
	{
		if (!(u->getFaction() == Faction::FACTION_NEUTRAL || u->getFaction() == self.getFaction()) && (u->getDistanceTo(self) < minDist))
		{
			minDist = u->getDistanceTo(self);
			unit = u;
		}
	}
	closestMinion = unit;
	return *unit;
}

LivingUnit & MyStrategy::getNearestFriend()
{
	std::vector<LivingUnit *> targets;

	for (unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getBuildings()[i]));
	}
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getWizards()[i]));
	}
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getMinions()[i]));
	}

	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;
	double minDist = 1e10;

	for (auto &u : targets)
	{
		if (u->getFaction() == self.getFaction() && (u->getDistanceTo(self) < minDist))
		{
			minDist = u->getDistanceTo(self);
			unit = u;
		}
	}

	return *unit;
}

LivingUnit & MyStrategy::getCloseAndWeakTarget()
{
	std::vector<LivingUnit *> targets;
	for (unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getBuildings()[i]));
	}
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getWizards()[i]));
	}
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getMinions()[i]));
	}

	LivingUnit * unit = &self;
	if (targets.size() == 0) return *unit;

	std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		return (u1->getLife() < u2->getLife());
	});

	auto it = targets.begin();
	while (it != targets.end() && ((*it)->getFaction() == self.getFaction() || ((*it)->getDistanceTo(self)) > self.getCastRange() || (*it)->getFaction() == Faction::FACTION_NEUTRAL))
	{
		it++;
	}
	if (it != targets.end())
		unit = *it;
	return *unit;
}

void MyStrategy::getTargets()
{
	closestBuilding = nullptr;
	closestFriend = nullptr;
	closestEnemy = nullptr;
	closestWizard = nullptr;
	closestMinion = nullptr;
	weakestEnemy = nullptr;

	std::vector<LivingUnit *> targets;
	for (unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getBuildings()[i]));
	}
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getWizards()[i]));
	}
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(new LivingUnit(world.getMinions()[i]));
	}

	if (targets.size() == 0) return;
	double minDist = 6000;
	double minDist_b = 6000;
	double minDist_w = 6000;
	double minDist_m = 6000;
	double minDist_f = 6000;

	// упор€дочиваем цели по здоровью
	std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		return (u1->getLife() < u2->getLife());
	});

	auto it = targets.begin();
	while (it != targets.end() && ((*it)->getFaction() == self.getFaction() || ((*it)->getDistanceTo(self)) > self.getCastRange() || (*it)->getFaction() == Faction::FACTION_NEUTRAL)){
		it++;
	}
	if (it != targets.end())
		weakestEnemy = *it;

	for (auto &u : targets)
	{
		if (u->getFaction() != Faction::FACTION_NEUTRAL && u->getFaction() != self.getFaction())
		{
			if (u->getDistanceTo(self) < minDist)
			{
				minDist = u->getDistanceTo(self);
				closestEnemy = u;
			}
		
			if (u->getMaxLife() >= 1000 && u->getDistanceTo(self) < minDist_b)
			{
				minDist_b = u->getDistanceTo(self);
				closestBuilding = u;
			}
			else if (u->getRadius() >= 35 && u->getDistanceTo(self) < minDist_w)
			{
				minDist_w = u->getDistanceTo(self);
				closestWizard = u;
			}
			else //if (u->getMaxLife() >= 1000 && u->getDistanceTo(self) < minDist_m)
			{
				minDist_m = u->getDistanceTo(self);
				closestMinion = u;
			}
		}
		else if (u->getFaction() == self.getFaction())
		{
			if (u->getDistanceTo(self) < minDist_f)
			{
				minDist = u->getDistanceTo(self);
				closestFriend = u;
			}
		}
	}
	return;
}

void MyStrategy::attackEnemy(const Wizard& _self, const World& _world, const Game& _game, Move& _move, const LivingUnit& enemy)
{
	double distance = _self.getDistanceTo(enemy);	
	double angle = _self.getAngleTo(enemy);	
	_move.setTurn(angle);
	if (distance < 400)
	{
		//goTo(Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
		//goBackward(getPreviousWaypoint(), _move);
		_move.setSpeed(-game.getWizardForwardSpeed());
	}
			
	if (_self.getRemainingActionCooldownTicks() == 0)
	{
		// ≈сли цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance < 70)
				_move.setAction(ActionType::ACTION_STAFF);
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE > 10])
				_move.setAction(ActionType::ACTION_FIREBALL);
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
			{
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(distance - enemy.getRadius() + _game.getMagicMissileRadius());
			}
		}
	}
}


MyStrategy::MyStrategy() {
	LOW_HP_FACTOR = 0.25;
	WAYPOINT_RADIUS = 100.0;
}

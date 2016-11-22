#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;

void MyStrategy::move(const Wizard& self, const World& world, const Game& game, Move& _move) {
    /*move.setSpeed(game.getWizardForwardSpeed());
    move.setStrafeSpeed(game.getWizardStrafeSpeed());
    move.setTurn(game.getWizardMaxTurnAngle());
    move.setAction(ACTION_MAGIC_MISSILE);
	LivingUnit nearestEnemy = getNearestTarget();	*/
	
	initializeStrategy(self, game);
	initializeTick(self, world, game, _move);

	// ѕосто€нно двигаемс€ из-стороны в сторону, чтобы по нам было сложнее попасть.
	// —читаете, что сможете придумать более эффективный алгоритм уклонени€? ѕопробуйте! ;)
	_move.setStrafeSpeed( rand()%2 ? game.getWizardStrafeSpeed() : -game.getWizardStrafeSpeed());

	// ≈сли осталось мало жизненной энергии, отступаем к предыдущей ключевой точке на линии.
	if (self.getLife() < self.getMaxLife() * LOW_HP_FACTOR) {
		goTo(getPreviousWaypoint());
		return;
	}

	LivingUnit nearestTarget = getNearestTarget();

	// ≈сли видим противника ...
	
		double distance = self.getDistanceTo(nearestTarget);

		// ... и он в пределах дос€гаемости наших заклинаний, ...
		if (distance <= self.getCastRange()) {
			double angle = self.getAngleTo(nearestTarget);

			// ... то поворачиваемс€ к цели.
			_move.setTurn(angle);

			// ≈сли цель перед нами, ...
			if (fabs(angle) < game.getStaffSector() / 2.0) {
				// ... то атакуем.
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(distance - nearestTarget.getRadius() + game.getMagicMissileRadius());
			}

			return;
		}
	

	// ≈сли нет других действий, просто продвигаемс€ вперЄд.
	goTo(getNextWaypoint());
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
			Point2D(100.0, mapSize - 100.0),
			rand()%2
			?  Point2D(600.0, mapSize - 200.0)
			:  Point2D(200.0, mapSize - 600.0),
			 Point2D(800.0, mapSize - 800.0),
			 Point2D(mapSize - 600.0, 600.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_TOP, vector<Point2D>{
			 Point2D(100.0, mapSize - 100.0),
			 Point2D(100.0, mapSize - 400.0),
			 Point2D(200.0, mapSize - 800.0),
			 Point2D(200.0, mapSize * 0.75),
			 Point2D(200.0, mapSize * 0.5),
			 Point2D(200.0, mapSize * 0.25),
			 Point2D(200.0, 200.0),
			 Point2D(mapSize * 0.25, 200.0),
			 Point2D(mapSize * 0.5, 200.0),
			 Point2D(mapSize * 0.75, 200.0),
			 Point2D(mapSize - 200.0, 200.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_BOTTOM, vector<Point2D>{
			 Point2D(100.0, mapSize - 100.0),
			 Point2D(400.0, mapSize - 100.0),
			 Point2D(800.0, mapSize - 200.0),
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
	this->self = self;
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

void MyStrategy::goTo(Point2D & point)
{
}

LivingUnit & MyStrategy::getNearestTarget()
{
	std::vector<LivingUnit *> targets;
	for (auto i : world.getBuildings())
		targets.push_back(&i);
	for (auto i : world.getWizards())
		targets.push_back(&i);
	for (auto i : world.getMinions())
		targets.push_back(&i);

	LivingUnit * unit = NULL;

	double minDist = targets.front()->getDistanceTo(self);

	for (auto &u : targets)
	{
		if (u->getDistanceTo(self) < minDist)
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

LivingUnit & MyStrategy::getCloseAndWeakTarget()
{
	std::vector<LivingUnit *> targets;
	for (auto i : world.getBuildings())
		targets.push_back(&i);
	for (auto i : world.getWizards())
		targets.push_back(&i);
	for (auto i : world.getMinions())
		targets.push_back(&i);

	LivingUnit * unit = NULL;

	std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		return (u1->getLife() < u2->getLife());
	});

	auto it = targets.begin();
	while ( ((*it)->getDistanceTo(self)) > self.getCastRange()  )
	{
		it++;
	}
	unit = *it;
	return *unit;
}

MyStrategy::MyStrategy() { }

#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;


// 
// сделать  хорошие опорные точки (возможно подходить к ним ближе)
// улучшить движение задним ходом

void MyStrategy::move(const Wizard& _self, const World& _world, const Game& _game, Move& _move) {
  	
	myLastPos = Point2D(self.getX(), self.getY());
	if (self.getDistanceTo(posBeforeBonus.getX(), posBeforeBonus.getY()) < 50) returnToLastPos = false;
	initializeStrategy(_self, _game);
	initializeTick(_self, _world, _game, _move);

	// Постоянно двигаемся из-стороны в сторону, чтобы по нам было сложнее попасть.
	// Считаете, что сможете придумать более эффективный алгоритм уклонения? Попробуйте! ;)
	_move.setStrafeSpeed( rand()%2 ? _game.getWizardStrafeSpeed() : -_game.getWizardStrafeSpeed());
	
	if (getBonus(_move)) return;	
	
	getTargets();
	double d_f, d_e, d_w, d_b, d_m, d_wt;
	d_f = d_e = d_w = d_b = d_m = d_wt = 6000;
	if (closestFriend != nullptr) d_f = _self.getDistanceTo(*closestFriend);
	if (closestEnemy != nullptr) d_e = _self.getDistanceTo(*closestEnemy);
	if (closestBuilding != nullptr) d_b = _self.getDistanceTo(*closestBuilding);
	if (closestWizard != nullptr) d_w = _self.getDistanceTo(*closestWizard);
	if (closestMinion != nullptr) d_m = _self.getDistanceTo(*closestMinion);
	if (weakestEnemy != nullptr) d_wt = _self.getDistanceTo(*weakestEnemy);

	int closeToBonus = getCloseToBonus(_move);
	switch (closeToBonus)
	{
		// если мы в определенных квадратах - идем в точку, где виден бонус
		// не забавая атаковать врагов
	case 0: break;
	case 1:
		if (d_wt < _self.getCastRange())
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
			goBackwardTo(Point2D(800, 800), _move);
		return;
	case 11:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		else 
			goBackwardTo(Point2D(750, 300), _move); return;
	case 2:
		if (d_wt < _self.getCastRange())
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10 )
			goBackwardTo(Point2D(1600, 1600), _move); return;
	case 21:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		goBackwardTo(Point2D(2000, 2000), _move); return;
	case 3:
		if (d_wt < _self.getCastRange())
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
		goBackwardTo(Point2D(3200, 3200), _move); return;
	case 31:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		goBackwardTo(Point2D(3700, 750), _move); return;
	default: break;
	}

	LivingUnit nearestTarget = getCloseAndWeakTarget();// getNearestTarget();


	// приоритет атаки
	if (d_e < _self.getCastRange()) //если хоть кто то в пределах досягаемости
	{		
		_move.setSpeed(0);
		LivingUnit enemy = *closestEnemy;
		if (d_e < 100 || closestEnemy->getLife() <  24)
		{
			enemy = *closestEnemy;
		}
		else if (d_wt < _self.getCastRange())
		{
			enemy = *weakestEnemy;//
		}		
		else if(d_b < _self.getCastRange())
		{
			enemy = *closestBuilding;//
		}
		else if (d_w < _self.getCastRange())
		{
			enemy = *closestWizard;//
		}
		else if (d_m < _self.getCastRange())
		{
			enemy = *closestMinion;//
		}
		attackEnemy(_self, _world, _game, _move, enemy);		
		return;
	}
			
	// Если осталось мало жизненной энергии, отступаем задом к предыдущей ключевой точке на линии.
	if (d_f <= self.getRadius() + closestFriend->getRadius() + 1) //застряли изза друга
		goTangentialFrom(Point2D(closestFriend->getX(), closestFriend->getY()), _move);
	else if (_self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) // застряли хрен пойми почему
	{
		_move.setAction(ActionType::ACTION_STAFF); // не работает, переделать, учесть деревья
		_move.setSpeed(-game.getWizardForwardSpeed());
	}
	else if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR)
		goBackwardTo(getPreviousWaypoint(), _move);
	else if (d_e < 600)                                            // враг близко - идем к нему
		goTo(Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
	else if (returnToLastPos)
		goTo(posBeforeBonus, _move);
	else if (d_f > 400 && d_f < 6000 && closestFriend->getRadius() < 100)// бежим к друзьям, если они далеко b и это не база // надо бы избегать деревьев
		goTo(Point2D(closestFriend->getX(), closestFriend->getY()), _move);
	
	else // Если нет других действий, просто продвигаемся вперёд.
		
		goTo(getNextWaypoint(), _move);	
	return;
}


/**
* Инциализируем стратегию.
* <p>
* Для этих целей обычно можно использовать конструктор, однако в данном случае мы хотим инициализировать генератор
* случайных чисел значением, полученным от симулятора игры.
*/
void MyStrategy::initializeStrategy(const Wizard& _self, const Game& _game) {
	
	//if (random == null) {
	//	random = new Random(game.getRandomSeed());

		double mapSize = _game.getMapSize();
		
		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_MIDDLE, vector<Point2D>{
			Point2D(100.0, mapSize - 600),
				Point2D(800.0, mapSize - 800),
				Point2D(900.0, mapSize - 1000),
				Point2D(1800.0, mapSize - 1600),
				Point2D(2000, 1900.0),
				Point2D(2400, 1600.0),
				Point2D(mapSize - 700.0, 700.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_TOP, vector<Point2D>{
				Point2D(100.0, mapSize * 0.9),
				Point2D(200.0, mapSize * 0.75),
				Point2D(200.0, mapSize * 0.5),
				Point2D(200.0, 700),				
				//Point2D(800.0, 800.0),
				//Point2D(600.0, 600.0),
				Point2D(mapSize * 0.2, 200.0),
				Point2D(mapSize * 0.5, 200.0),
				Point2D(mapSize * 0.75, 200.0),
				Point2D(mapSize - 700.0, 700.0)
		}));

		waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_BOTTOM, vector<Point2D>{
			 Point2D(100.0, mapSize - 100.0), // go down
			 Point2D(600.0, mapSize - 200.0),
			 Point2D(mapSize * 0.25, mapSize - 150.0),
			 Point2D(mapSize * 0.5, mapSize - 200.0),
			 Point2D(mapSize * 0.75, mapSize - 200.0),
			 Point2D(3300.0, mapSize - 200.0),			 
			 Point2D(mapSize - 200.0, 3300),
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

		// Наша стратегия исходит из предположения, что заданные нами ключевые точки упорядочены по убыванию
		// дальности до последней ключевой точки. Сейчас проверка этого факта отключена, однако вы можете
		// написать свою проверку, если решите изменить координаты ключевых точек.

		/*Point2D lastWaypoint = waypoints[waypoints.length - 1];

		Preconditions.checkState(ArrayUtils.isSorted(waypoints, (waypointA, waypointB) -> Double.compare(
		waypointB.getDistanceTo(lastWaypoint), waypointA.getDistanceTo(lastWaypoint)
		)));*/
}


/**
* Сохраняем все входные данные в полях класса для упрощения доступа к ним.
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
* Действие данного метода абсолютно идентично действию метода {@code getNextWaypoint}, если перевернуть массив
* {@code waypoints}.
*/
Point2D MyStrategy::getPreviousWaypoint() {
	Point2D firstWaypoint = waypoints[0];

	for (int waypointIndex = waypoints.size() - 1; waypointIndex > 0; --waypointIndex) {
		Point2D waypoint = waypoints[waypointIndex] + (rand()%2? Point2D(35,35) : Point2D(-35, -35));

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

	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setSpeed(game.getWizardForwardSpeed());

}

void MyStrategy::goBackwardTo(const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(2 * self.getX() - point.getX(), 2 * self.getY() - point.getY());

	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setSpeed(-game.getWizardForwardSpeed());

}

void MyStrategy::goBackwardFrom (const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(point.getX(), point.getY());
	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(-game.getWizardForwardSpeed());
	}
}

void MyStrategy::goTangentialFrom(const Point2D & point, Move& _move)
{
	double angle1 = self.getAngleTo(self.getX() - (point.getY() - self.getY()), self.getY() + (point.getX() - self.getX()));
	double angle2 = self.getAngleTo(self.getX() + (point.getY() - self.getY()), self.getY() - (point.getX() - self.getX()));

	_move.setStrafeSpeed(0);
	double angle = std::min(angle1, angle2);
	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(game.getWizardForwardSpeed());
	}
}

int MyStrategy::getCloseToBonus(model::Move & _move)
{
	bonusCheckTicks++;
	if (bonusCheckTicks > 500) // если бонус проверяли давно, то он не проверен
		bonusChecked = false;  
	if (bonusChecked) return 0; // если недавно - уходим

	double mapSize = game.getMapSize();
	double x = self.getX();
	double y = self.getY();
	posBeforeBonus = Point2D(x, y);
	int i = (game.getBonusAppearanceIntervalTicks() - world.getTickIndex() % game.getBonusAppearanceIntervalTicks());
	if (i > 500) return 0;
	
	
	// разделяем зоны на близость к бонусам
	if (x < 820 && y < 820 ) return 1;
	if (x < 1600 && y < 200) return 11;
	if ((x > mapSize - 820 && y > mapSize - 820)) return 3;
	if ((x > mapSize - 200 && y > mapSize - 1600)) 31;
	if (abs(x - y) < 300 && x < 2400) return 2;
	if (x > 2000 && x < 3000 && fabs(mapSize - x + y < 300)) return 21;
	
    //	держимся ближе к бонусу	 
	return 0;
}

bool MyStrategy::getBonus(model::Move & _move)
{
	double mapSize = game.getMapSize();
	double d1 = self.getDistanceTo(mapSize*0.3, mapSize*0.3);
	double d2 = self.getDistanceTo(mapSize*0.7, mapSize*0.7);

	vector<Bonus> bonuses = world.getBonuses();
	
	int i = (game.getBonusAppearanceIntervalTicks() - world.getTickIndex()%game.getBonusAppearanceIntervalTicks());
	if (bonuses.size() != 0)
	{
		returnToLastPos = true;
	
		double distance = 6000;
		if(closestWizard != nullptr) distance = self.getDistanceTo(*closestWizard);
		if (fabs(self.getX() - self.getY()) < 400 && (d1 < 800 || d2 < 800))
			if (d1 < d2)
			{
				for (auto & i : bonuses)

					if (fabs(self.getDistanceTo(i) < d1) < 50)
					{
						goTo(Point2D(mapSize*0.3, mapSize*0.3), _move);
						//атакуем, если видим вражеского волшебника
						if (distance < self.getCastRange() && fabs(self.getAngleTo(*closestWizard)) < game.getStaffSector() / 2.0)
						{
							double distance = self.getDistanceTo(*closestWizard);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE > 10])
								_move.setAction(ActionType::ACTION_FIREBALL);
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
							{
								_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
								_move.setCastAngle(self.getAngleTo(*closestWizard));
								_move.setMinCastDistance(distance - (*closestWizard).getRadius() + game.getMagicMissileRadius());
							}
						}
							return true;
					}
			}
			else
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) < d2) < 50)
					{
						goTo(Point2D(mapSize*0.7, mapSize*0.7), _move);	
						//атакуем, если видим вражеского волшебника
						if (distance < self.getCastRange() && fabs(self.getAngleTo(*closestWizard)) < game.getStaffSector() / 2.0)
						{
							double distance = self.getDistanceTo(*closestWizard);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE > 10])
								_move.setAction(ActionType::ACTION_FIREBALL);
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
							{
								_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
								_move.setCastAngle(self.getAngleTo(*closestWizard));
								_move.setMinCastDistance(distance - (*closestWizard).getRadius() + game.getMagicMissileRadius());
							}
						}
						return true;
					}
			}
		
	}
	else if ( (d1 < 600 || d2 < 600 ) && i > 500)
	{
		bonusChecked = true;
		bonusCheckTicks = 0;
	}
	
	return false;
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
		return (u1->getLife()/u1->getMaxLife() < u2->getLife()/u1->getMaxLife());
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

	// упорядочиваем цели по здоровью
	std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		return (u1->getLife() / u1->getMaxLife() < u2->getLife() / u1->getMaxLife());
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
		else if (u->getFaction() == self.getFaction() && u->getId() != self.getId())
		{
			if (u->getDistanceTo(self) < minDist_f)
			{
				minDist_f = u->getDistanceTo(self);
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
			
	if (_self.getRemainingActionCooldownTicks() == 0)
	{
		// Если цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 70)
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
	else if (distance < 100)
		goBackwardFrom(Point2D(enemy.getX(), enemy.getY()), _move);
	else if (distance < 400)
	{
		goBackwardTo(getPreviousWaypoint(), _move);
	}	
}


MyStrategy::MyStrategy() {

	bonusCheckTicks = 0;
	bonusChecked = true;
	returnToLastPos = true;
	LOW_HP_FACTOR = 0.25;
	WAYPOINT_RADIUS = 100.0;
}

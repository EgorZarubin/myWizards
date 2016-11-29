#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;


// улучшить уклонение
// предсказывать положение врагов

// сделать  хорошие опорные точки (возможно подходить к ним ближе)
// улучшить движение задним ходом
// пробегать через деревья, средний бонус из двух вариантов
// заливка 

void MyStrategy::move(const Wizard& _self, const World& _world, const Game& _game, Move& _move) {
  	
	if (_world.getTickIndex() < 300) return; //стратуем чуть чуть позже, чтобы сразу не помереть
	clearValues();

	
	myLastPos = Point2D(self.getX(), self.getY());
	prevLife = self.getLife();

	initializeStrategy(_self, _game);
	initializeTick(_self, _world, _game, _move);
	
	if (self.getDistanceTo(posBeforeBonus.getX(), posBeforeBonus.getY()) < 50 || fabs(self.getLife() - prevLife) > 50) returnToLastPos = false;
	
	// определяем тип игры
	isSkillsEnable = game.isSkillsEnabled();	
	if (isSkillsEnable)
	{		
		if (_self.isMaster()) setMessage(_move);
		else getMessage();
		learnSkills(_self, _move);
	}
	
	//если видим бонус - бежим к нему и все
	if (getBonus(_move)) return;	
	
	//распределяем цели по типам: слабые, ближние, волшебники, миньоны
	getTargets();	
	
	//если время близко к бонусу, надо его взять
	int closeToBonus = getCloseToBonus(_move);  // определяем близость к бонусу и сектор
	
	switch (closeToBonus)
	{
		// если мы в определенных квадратах - идем в точку, где виден бонус
		// не забавая атаковать врагов
	case 0: break;
	case 1:
		if (d_wt < _self.getCastRange())
		{
			//attackEnemy(_self, _world, _game, _move, *weakestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goToAdv(Point2D(800, 800), _move);//Backward
		}
		else
			goToAdv(Point2D(800, 800), _move);
		return;
	case 11:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		else
			goBackwardToAdv(Point2D(750, 300), _move);
		return;
	case 21:	
		if (d_wt < _self.getCastRange())
		{
			//attackEnemy(_self, _world, _game, _move, *weakestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goToAdv(Point2D(1400, 1750), _move); //Backward
		}
		else goToAdv(Point2D(1600, 1600), _move);
		return;
	case 22:
		if (d_wt < _self.getCastRange())
		{
			//attackEnemy(_self, _world, _game, _move, *weakestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goToAdv(Point2D(2400, 2400), _move);//Backward
		}
		else goToAdv(Point2D(2400, 2400), _move);
		return;
	case 23:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		goBackwardToAdv(Point2D(2000, 2000), _move); return;
	case 3:
		if (d_wt < _self.getCastRange())
		{
			//attackEnemy(_self, _world, _game, _move, *weakestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goToAdv(Point2D(3200, 3200), _move);//Backward
		}
		else goToAdv(Point2D(3200, 3200), _move);
		return;
	case 31:
		if (d_wt < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *weakestEnemy);
		goToAdv(Point2D(3700, 750), _move); return; // Backward
	case 41:
		if (d_w < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *closestWizard);
		goToAdv(Point2D(1150, 1250), _move);
		return;
	case 42:
		if (d_w < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *closestWizard);
		goToAdv(Point2D(2750, 2850), _move);
		return;
	default: break;
	}

	//если мало жизней- отбегаем назад (стоит ли?) и все
	//if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR && _self.getDistanceTo(0,0) > 500)
	//{
	//	goBackwardTo(getPreviousWaypoint(), _move);
	//	return;
	//}
	
	// приоритет атаки. сейчас это: совсем близкие чуваки, волшебники, самые слабые типы, башни, миньоны
	if (d_e < _self.getCastRange()) //если хоть кто то в пределах досягаемости
	{		
		_move.setSpeed(0);
		LivingUnit enemy = *closestEnemy;
		
		if (d_e < 100 || closestEnemy->getLife() <  24)
		{
			enemy = *closestEnemy;
		}
		else if (d_w < _self.getCastRange())
		{
			enemy = *closestWizard;//
		}
		else if (d_wt < _self.getCastRange())
		{
			enemy = *weakestEnemy;//
		}		
		else if(d_b < _self.getCastRange())
		{
			enemy = *closestBuilding;//
		}		
		else if (d_m < _self.getCastRange())
		{
			enemy = *closestMinion;//
		}
				
		attackEnemy(_self, _world, _game, _move, enemy);		
		return;
	}

	else if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR) // Если осталось мало жизненной энергии, отступаем задом к предыдущей ключевой точке на линии.
		goBackwardToAdv(getPreviousWaypoint(), _move);

	else if (d_e < 600)                                            // враг близко - идем к нему
		goToAdv(Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);

	else if (returnToLastPos)
		goToAdv(posBeforeBonus, _move);

	else if (d_f > 400 && d_f < 6000 && closestFriend->getRadius() < 100)// бежим к друзьям, если они далеко b и это не база // надо бы избегать деревьев
		goToAdv(Point2D(closestFriend->getX(), closestFriend->getY()), _move);
	
	else // Если нет других действий, просто продвигаемся вперёд.		
		goToAdv(getNextWaypoint(), _move);	
	return;
}


void MyStrategy::clearValues()
{
	d_f = d_e = d_w = d_b = d_m = d_wt = d_n = 6000;
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

		if (changeLaneTo != LaneType::_LANE_UNKNOWN_)
			waypoints = waypointsByLane[changeLaneTo];
		else
			waypoints = waypointsByLane[lane];

		//if (!self.isMaster())
		//	if (self.getMessages()[_self.getId()].getLane()  != LaneType::_LANE_UNKNOWN_)
		//		waypoints = waypointsByLane[self.getMessages()[_self.getId()].getLane()];
		
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
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	double angle = self.getAngleTo(point.getX(), point.getY());

	_move.setTurn(angle);

	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setSpeed(game.getWizardForwardSpeed());

}

void MyStrategy::goBackwardTo(const Point2D & point, Move& _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	double angle = self.getAngleTo(2 * self.getX() - point.getX(), 2 * self.getY() - point.getY());

	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setSpeed(-game.getWizardForwardSpeed());

}

void MyStrategy::goBackwardToAdv(const Point2D & point, Move& _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;


	if (self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1 && d_f <= self.getRadius() + closestFriend->getRadius() + 10) //застряли изза друга - обойдем его
	{
		double  dA = fabs(self.getAngleTo(*closestFriend) - self.getAngleTo(point.getX(), point.getY()));
		if (dA< PI / 2)
			goTangentialFrom(Point2D(closestFriend->getX(), closestFriend->getY()), point, _move);
		else
			goTo(point, _move);
	}
	else if (self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) // застряли хрен пойми почему
	{
		Tree tree = getClosestTree();
		if (self.getDistanceTo(tree) < self.getRadius() + tree.getRadius() + 10)
		{
			goTo(Point2D(tree.getX(), tree.getY()), _move);
			_move.setAction(ActionType::ACTION_STAFF);
		}
		else if (d_n < 6000 && d_n < self.getRadius() + closestNeutral->getRadius() + 5)
		{
			goTangentialFrom(Point2D(closestNeutral->getX(), closestNeutral->getY()), point, _move);
			setStrafe(_move);
		}
		else
			_move.setSpeed(rand() % 2 ? -game.getWizardForwardSpeed() : game.getWizardForwardSpeed()); // не работает, переделать, учесть деревья
	}
	else
	{
		double angle = self.getAngleTo(2 * self.getX() - point.getX(), 2 * self.getY() - point.getY());
		_move.setTurn(angle);
		if (fabs(angle) < game.getWizardMaxTurnAngle())
			_move.setSpeed(-game.getWizardForwardSpeed());
	}
}

void MyStrategy::goBackwardFrom (const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(point.getX(), point.getY());
	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(-game.getWizardForwardSpeed());
	}
}

void MyStrategy::goTangentialFrom(const Point2D & point, const Point2D & nextPoint,  Move& _move)
{
	double angle1 = self.getAngleTo(self.getX() - (point.getY() - self.getY()), self.getY() + (point.getX() - self.getX()));
	double angle2 = self.getAngleTo(self.getX() + (point.getY() - self.getY()), self.getY() - (point.getX() - self.getX()));
	double angle3 = self.getAngleTo(nextPoint.getX(), nextPoint.getY());
	_move.setStrafeSpeed(0);
	double angle = fabs(angle1) < fabs(angle2)? angle1 : angle2;
	
	if (fabs(angle - angle3) < game.getWizardMaxTurnAngle()) // иначе выйдем на орбиту
	{
		_move.setTurn(angle3);
		angle = angle3;
	}
	else
		_move.setTurn(angle);
	
	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(game.getWizardForwardSpeed());
	}
}

int MyStrategy::getCloseToBonus(model::Move & _move)
{ 
	if (world.getTickIndex() - lastBonusCheck < game.getBonusAppearanceIntervalTicks() - 350 ) return 0;

	//bonusCheckTicks++;
	//if (bonusCheckTicks > 500) // если бонус проверяли давно, то он не проверен
	//	bonusChecked = false;  
	//if (bonusChecked) return 0; // если недавно - уходим

	double mapSize = game.getMapSize();
	double x = self.getX();
	double y = self.getY();

	posBeforeBonus = Point2D(x, y);	//поменяется когда заберем бонус
	
	if (world.getTickIndex() - lastBonusCheck > game.getBonusAppearanceIntervalTicks() - 250)
		if (self.getDistanceTo(1200, 1200) < self.getVisionRange()+50)
			return 41;
		else if (self.getDistanceTo(2800, 2800) < self.getVisionRange()+50)
			return 42;

	// разделяем зоны на близость к бонусам
	if (x < 820 && y < 820 ) return 1;
	if (x < 1600 && y < 400) return 11;
	if ((x > mapSize - 820 && y > mapSize - 820)) return 3;
	if ((x > mapSize - 400 && y > mapSize - 1600)) return 31;
	if (abs(x - y) < 300 && (x < mapSize - y) && x > mapSize - y - 1000) return 21;
	if (abs(x - y) < 300 && (x > mapSize - y) && x < mapSize - y + 1000) return 22;
	if (x > 2000 && x < 3000 && fabs(x + y - mapSize)< 300) return 23;
	
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
		// бонус проверен, либо мы его захаваем либо не мы, точно пропадет(или меня завалят)
		returnToLastPos = true;
		bonusChecked = true;
		lastBonusCheck =  world.getTickIndex() - world.getTickIndex()%2500; //кратная 2500 штука
	
		double distance = 6000;
		double c_dist = 6000;// расстояние до ближайшего врага
		double e_d = 6000;
		if(closestWizard != nullptr) distance = self.getDistanceTo(*closestWizard);
		if (closestEnemy != nullptr) c_dist = self.getDistanceTo(*closestEnemy);
		LivingUnit *enemy;
		if (fabs(self.getX() - self.getY()) < 400 && (d1 < 800 || d2 < 800))
			if (d1 < d2)
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) - d1) < 50) // если это тот бонус, который ближний
					{
						if (lane == LaneType::LANE_MIDDLE)
						{
							changeLaneTo = LaneType::LANE_TOP;
							posBeforeBonus = Point2D(800, 800); // so hardcoded
						}
						else 
							posBeforeBonus = Point2D(800, 800); // so hardcoded

						if ((c_dist < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) //если враг близко и бонус с другой стороны
						{
							goBackwardToAdv(Point2D(mapSize*0.3, mapSize*0.3), _move);
							enemy = closestEnemy;
							e_d = c_dist;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.3, mapSize*0.3), _move);
							enemy = closestWizard;
							e_d = distance;
						}
						//атакуем, если видим вражеского волшебника
						if (self.getRemainingActionCooldownTicks() == 0 && e_d < self.getCastRange() && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
						{	
							//setStrafe(_move);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && e_d <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if ( isSkillsEnable && numOfLearnedSkills > 9 && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
							{
								_move.setAction(ActionType::ACTION_FROST_BOLT);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFrostBoltRadius());
							}
							else if (isSkillsEnable && numOfLearnedSkills > 14 && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
							{
								_move.setAction(ActionType::ACTION_FIREBALL);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFireballRadius());
							}
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
							{
								_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getMagicMissileRadius());
							}
						}
							return true;
					}
			}
			else
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) - d2) < 50) // если это тот бонус, который ближний
					{
						if (lane == LaneType::LANE_MIDDLE)
						{							
							posBeforeBonus = Point2D(2400, 2400); // so hardcoded
						}
						else
							posBeforeBonus = Point2D(3200, 3200); // so hardcoded

						if ((c_dist < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) // враг близко и бонус с другой стороны, то основной враг - он
						{
							goBackwardToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							enemy = closestEnemy;
							e_d = c_dist;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							enemy = closestWizard;
							e_d = distance;
						}

						//атакуем, если видим вражеского волшебника					
						if (self.getRemainingActionCooldownTicks() == 0 && e_d < self.getCastRange() && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
						{
							//setStrafe(_move);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && e_d <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if (isSkillsEnable && numOfLearnedSkills>9 && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
							{
								_move.setAction(ActionType::ACTION_FROST_BOLT);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFrostBoltRadius());
							}
							else if (isSkillsEnable && numOfLearnedSkills>14 && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
							{
								_move.setAction(ActionType::ACTION_FIREBALL);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFireballRadius());
							}
							else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
							{
								_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getMagicMissileRadius());
							}
						}
						return true;
					}
			}		
	}
	else if ( (d1 < self.getVisionRange() - 10 || d2 < self.getVisionRange() - 10) && world.getTickIndex() - lastBonusCheck > game.getBonusAppearanceIntervalTicks())
	{
		bonusChecked = true;
		lastBonusCheck = world.getTickIndex() - world.getTickIndex() % 2500;
		bonusCheckTicks = 0;
	}
	
	return false;
}

void MyStrategy::getTargets()
{
	closestBuilding = nullptr;
	closestFriend = nullptr;
	closestEnemy = nullptr;
	closestWizard = nullptr;
	closestMinion = nullptr;
	weakestEnemy = nullptr;
	closestNeutral = nullptr;

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
	double minDist_n = 6000;

	// упорядочиваем цели по здоровью
	std::sort(targets.begin(), targets.end(),
		[](LivingUnit* u1, LivingUnit* u2) {
		return ( double(u1->getLife())/double(u1->getMaxLife())  <   double(u2->getLife()) / double(u2->getMaxLife()));
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
		else if(u->getFaction() == Faction::FACTION_NEUTRAL)
		{
			if (u->getDistanceTo(self) < minDist_n)
			{
				minDist_n = u->getDistanceTo(self);
				closestNeutral = u;
			}
		}
	}

	if (closestFriend != nullptr) d_f   = self.getDistanceTo(*closestFriend);
	if (closestEnemy != nullptr) d_e    = self.getDistanceTo(*closestEnemy);
	if (closestBuilding != nullptr) d_b = self.getDistanceTo(*closestBuilding);
	if (closestWizard != nullptr) d_w   = self.getDistanceTo(*closestWizard);
	if (closestMinion != nullptr) d_m   = self.getDistanceTo(*closestMinion);
	if (weakestEnemy != nullptr) d_wt   = self.getDistanceTo(*weakestEnemy);
	if (closestNeutral != nullptr) d_n  = self.getDistanceTo(*closestNeutral);

	return;
}

model::Tree MyStrategy::getClosestTree()
{	
	auto trees = world.getTrees();
	double dist = sqrt(2)*game.getMapSize();
	
	if (trees.size() == 0) return model::Tree();
	Tree tree = trees[0];
	for (auto &i : trees)
	{
		if (self.getDistanceTo(i) < dist)
		{
			dist = self.getDistanceTo(i);
			tree = i;
		}
	}
	return tree;
}

void MyStrategy::setStrafe(model::Move & _move)
{
	if (strafeTicks >= STRAFE_FACTOR)
	{
		strafeTicks = 0;
		lastStrafeDirection = rand() % 2 ? 1 : -1;
	}
	_move.setStrafeSpeed(lastStrafeDirection * game.getWizardStrafeSpeed());
	strafeTicks++;
}

void MyStrategy::attackEnemy(const Wizard& _self, const World& _world, const Game& _game, Move& _move, const LivingUnit& enemy)
{
	if (isSkillsEnable)
	{
		attackEnemyAdv(_self,_world, _game, _move, enemy);
		return;
	}

	double distance = _self.getDistanceTo(enemy);	
	double angle = _self.getAngleTo(enemy);	
	//setStrafe(_move);
			
	if (_self.getRemainingActionCooldownTicks() == 0)
	{
		// Если цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 80)
			{
				goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
				_move.setAction(ActionType::ACTION_STAFF);
				return;
			}		
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(distance - enemy.getRadius() + _game.getMagicMissileRadius());
				lastDodgeDir *= -1;
			}
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12)
				_move.setTurn(angle); 
			else if (distance > self.getCastRange() - 50) dodgeFrom(_self, _world, _game, _move, enemy);
		}
		else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12)
			_move.setTurn(angle);
		else dodgeFrom(_self, _world, _game, _move, enemy);
	}
	else if (distance < self.getCastRange() - 50)
	{
		goBackwardTo(getPreviousWaypoint(), _move); //ADV?
	}	
	else dodgeFrom(_self, _world, _game, _move, enemy);
}

void MyStrategy::attackEnemyAdv(const model::Wizard & _self, const model::World & _world, const model::Game & _game, model::Move & _move, const model::LivingUnit & enemy)
{
	double distance = _self.getDistanceTo(enemy);
	double angle = _self.getAngleTo(enemy);
	//setStrafe(_move);
	

	if (_self.getRemainingActionCooldownTicks() == 0)
	{		
		// Если цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			double life = double(enemy.getLife()) / double(enemy.getMaxLife());
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 80)
			{
				goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
				_move.setAction(ActionType::ACTION_STAFF);
				return;
			}
			else if ((numOfLearnedSkills > 9) && (enemy.getLife() > 0.50) && enemy.getRadius() <= 35 &&
				(self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0))
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_FROST_BOLT);
				_move.setCastAngle(self.getAngleTo(enemy));
				_move.setMinCastDistance(distance - (enemy).getRadius() + game.getFrostBoltRadius());
				lastDodgeDir *= -1;
			}
			else if ((numOfLearnedSkills > 14) && (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10) &&
				(self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FIREBALL] == 0))
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_FIREBALL);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(distance - enemy.getRadius() + _game.getFireballRadius());
				lastDodgeDir *= -1;
			}
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(distance - enemy.getRadius() + _game.getMagicMissileRadius());
				lastDodgeDir *= -1;
			}
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12)
				_move.setTurn(angle);
			else if (distance > self.getCastRange() - 50) dodgeFrom(_self, _world, _game, _move, enemy);
		}
		else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12)
			_move.setTurn(angle);
		else dodgeFrom(_self, _world, _game, _move, enemy);
	}
	//else if (distance < 100)
	//	goBackwardFrom(Point2D(enemy.getX(), enemy.getY()), _move);
	else if (distance < self.getCastRange() - 50)
	{
		goBackwardTo(getPreviousWaypoint(), _move); //Adv?
	}
	else dodgeFrom(_self, _world, _game, _move, enemy);
}

void MyStrategy::dodgeFrom(const model::Wizard & _self, const model::World & _world, const model::Game & _game, model::Move & _move, const model::LivingUnit & enemy)
{
	if (self.getLife() == self.getMaxLife()) return;
	if (numOfLearnedSkills > 4)
	{		
		_move.setStrafeSpeed(lastDodgeDir * game.getWizardStrafeSpeed());		
		return;
	}
	Point2D point = Point2D(enemy.getX(), enemy.getY());

	double angle1 = self.getAngleTo(self.getX() - (point.getY() - self.getY()), self.getY() + (point.getX() - self.getX()));
	double angle2 = self.getAngleTo(self.getX() + (point.getY() - self.getY()), self.getY() - (point.getX() - self.getX()));
	
	_move.setStrafeSpeed(0);
	double angle = lastDodgeDir > 0 ? angle1 : angle2;

	_move.setTurn(angle);

	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(game.getWizardForwardSpeed());
	}
}

void MyStrategy::goToAdv(const Point2D & point, model::Move & _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;


	if ((self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) && (d_f <= self.getRadius() + closestFriend->getRadius() + 10)) //застряли изза друга - обойдем его
	{
		double  dA = fabs(self.getAngleTo(*closestFriend) - self.getAngleTo(point.getX(), point.getY()));
		if (dA< PI / 2)
			goTangentialFrom(Point2D(closestFriend->getX(), closestFriend->getY()), point, _move);
		else
			goTo(point, _move);
	}
	else if (self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) // застряли хрен пойми почему
	{
		Tree tree = getClosestTree();
		if (self.getDistanceTo(tree) < self.getRadius() + tree.getRadius() + 10)
		{
			goTo(Point2D(tree.getX(), tree.getY()), _move);
			_move.setAction(ActionType::ACTION_STAFF);
		}
		else if ((d_n < 6000) && (d_n < self.getRadius() + closestNeutral->getRadius() + 5))
		{			
			goTangentialFrom(Point2D(closestNeutral->getX(), closestNeutral->getY()), point, _move);
			setStrafe(_move);
		}
		else
			_move.setSpeed(rand() % 2 ? -game.getWizardForwardSpeed() : game.getWizardForwardSpeed()); // не работает, переделать, учесть деревья
	}
	else
		goTo(point, _move);
}


void MyStrategy::learnSkills(const model::Wizard & _self, model::Move& _move)
{
	if (skillToLearn != SkillType::_SKILL_UNKNOWN_) // указание свыше
	{
		_move.setSkillToLearn(skillToLearn);
		return;
	}

	SkillType skill = SkillType::_SKILL_UNKNOWN_;
	switch (nextSkill)
	{
	case 0:	skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_1; break;
	case 1: skill = SkillType::SKILL_RANGE_BONUS_AURA_1; break;
	case 2: skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_2; break;
	case 3: skill = SkillType::SKILL_RANGE_BONUS_AURA_2; break;
	case 4: skill = SkillType::SKILL_ADVANCED_MAGIC_MISSILE; break;
	//next 
	
	case 5: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1; break;
	case 6: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1; break;
	case 7: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2; break;
	case 8: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2; break;
	case 9: skill = SkillType::SKILL_FROST_BOLT; break;
	//next
	case 10: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_1; break;
	case 11: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_AURA_1; break;
	case 12: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_2; break;
	case 13: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_AURA_2; break;
	case 14: skill = SkillType::SKILL_FIREBALL; break;
	default:break;
	}
	numOfLearnedSkills = _self.getSkills().size();
	if (numOfLearnedSkills > nextSkill)
		nextSkill++;
	_move.setSkillToLearn(skill);
	// _move.getSkillTolearn()
	// getAuraSkillRange()
}

void MyStrategy::setMessage(model::Move& _move)
{
	std::vector<Message> messages;
	messages.push_back(Message(LaneType::LANE_TOP, SkillType::_SKILL_UNKNOWN_, std::vector<signed char>(0)));
	messages.push_back(Message(LaneType::LANE_MIDDLE, SkillType::_SKILL_UNKNOWN_, std::vector<signed char>(0)));
	messages.push_back(Message(LaneType::LANE_MIDDLE, SkillType::_SKILL_UNKNOWN_, std::vector<signed char>(0)));
	messages.push_back(Message(LaneType::LANE_BOTTOM, SkillType::_SKILL_UNKNOWN_, std::vector<signed char>(0)));
}

void MyStrategy::getMessage()
{
	std::vector<Message> messages = self.getMessages();
	skillToLearn = SkillType::_SKILL_UNKNOWN_;
	if (messages.size())
	{
		Message lastmessage = messages.back();

		// скилы слишком захардкожены
		//if ( (self.getSkills().size() == 0) ||  (lastmessage.getSkillToLearn() != self.getSkills().back()))
		//	skillToLearn = lastmessage.getSkillToLearn();

		if (lastmessage.getLane() != LaneType::_LANE_UNKNOWN_)
			changeLaneTo = lastmessage.getLane();		
	}
}

MyStrategy::MyStrategy() {

	changeLaneTo = LaneType::_LANE_UNKNOWN_;
	lastBonusCheck = 0;
	bonusCheckTicks = 0;
	bonusChecked = true;
	returnToLastPos = true;
	LOW_HP_FACTOR = 0.25;
	WAYPOINT_RADIUS = 100.0;

	bonusCheckTicks = 0;

	STRAFE_FACTOR = 5;
	strafeTicks = 0;
	lastStrafeDirection = 0;

	lastDodgeDir = 1;

	d_f = d_e = d_w = d_b = d_m = d_wt = d_n = 6000;

	isSkillsEnable = false;
	numOfLearnedSkills = 0;
	//learnedSkills.resize(15);
	//for (auto &i : learnedSkills)
	//{
	//	i = false;
	//}
	nextSkill = 0;
	skillToLearn = SkillType::_SKILL_UNKNOWN_;

	double mapSize = 4000;

	waypointsByLane.insert(std::pair<LaneType, vector<Point2D>>(LaneType::LANE_MIDDLE, vector<Point2D>{
		Point2D(100.0, mapSize - 300),
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
}

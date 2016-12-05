#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;


// улучшить уклонение 
// предсказывать положение врагов +-

// сделать  хорошие опорные точки (возможно подходить к ним ближе)
// улучшить движение задним ходом
// пробегать через деревья, средний бонус из двух вариантов
// заливка 
// перевести на шаред птры

void MyStrategy::move(const Wizard& _self, const World& _world, const Game& _game, Move& _move) {

	if (_world.getTickIndex() < 300) return; //стратуем чуть чуть позже, чтобы сразу не помереть
	clearValues();


	myLastPos = Point2D(self.getX(), self.getY());
	prevLife = self.getLife();

	if (_self.getLife() - prevLife > 50) changeLaneTo = LaneType::_LANE_UNKNOWN_;
	if (_self.getDistanceTo(posBeforeBonus.getX(), posBeforeBonus.getY()) < 50 || fabs(_self.getLife() - prevLife) > 50) returnToLastPos = false;

	initializeStrategy(_self, _game);
	initializeTick(_self, _world, _game, _move);
	
	// определяем тип игры
	isSkillsEnable = game.isSkillsEnabled();	
	if (isSkillsEnable)
	{		
		if (_self.isMaster()) setMessage(_move);
		else getMessage();
		learnSkills(_self, _move);
	}

	if (underBonus)
	{
		if (bonus.getType() == BonusType::BONUS_HASTE)
			SPEED_BONUS_FACTOR = game.getHastenedMovementBonusFactor();
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
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goTo(Point2D(800, 800), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);;//Backward
		}
		else
			goToAdv(Point2D(800, 800), _move);
		return;
	case 11:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			goTo(Point2D(750, 300), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);			
		}
		else
			goTo_wow(Point2D(750, 300), _move);//goToAdv(Point2D(750, 300), _move);
		return;
	case 21:	
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goTo(Point2D(1400, 1750), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move); //Backward
		}
		else 
			goTo_wow(Point2D(1400, 1750), _move);//goToAdv(Point2D(1400, 1750), _move);
		return;
	case 22:
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goTo(Point2D(2400, 2400), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);//Backward
		}
		else goToAdv(Point2D(2400, 2400), _move);
		return;
	case 23:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			goTo(Point2D(2000, 2000), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
		}
		else goToAdv(Point2D(2000, 2000), _move); return;
	case 3:
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goTo(Point2D(3200, 3200), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);//Backward
		}
		else goToAdv(Point2D(3200, 3200), _move);
		return;
	case 31:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
			attackEnemy(_self, _world, _game, _move, *closestEnemy);
		goToAdv(Point2D(3700, 750), _move); return; // Backward
	case 41:
		if (d_w < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_self, _world, _game, _move, *closestWizard);
			goTo(Point2D(1200, 1200), Point2D(closestWizard->getX(), closestWizard->getY()), _move);
		}
		else
			goToAdv(Point2D(1200, 1200), _move);		
		return;
	case 410:
		return;
	case 411: 
		goBackwardFrom(Point2D(1200, 1200), _move);
		return;
	case 42:
		if (d_w < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_self, _world, _game, _move, *closestWizard);
			goTo(Point2D(2800, 2800), Point2D(closestWizard->getX(), closestWizard->getY()), _move);
		}
		else goToAdv(Point2D(2800, 2800), _move);
		return;
	case 420:
		return;
	case 421:
		goBackwardFrom(Point2D(2800, 2800), _move);
		return;
	case 666:
		return;

	default: break;
	}

	//если мало жизней- отбегаем назад (стоит ли?) и все
	if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR && _self.getDistanceTo(0,0) > 500)
	{
		goTo_wow(getPreviousWaypoint(), _move);
		return;
	}
	
	// приоритет атаки. сейчас это: совсем близкие чуваки, волшебники, самые слабые типы, башни, миньоны
	if (d_e < _self.getCastRange()) //если хоть кто то в пределах досягаемости
	{		
		_move.setSpeed(0);
		LivingUnit enemy = *closestEnemy;
		
		if (d_e < self.getRadius() + closestEnemy->getRadius() + 200)
		{
			enemy = *closestEnemy;
		}
		else if (d_w < _self.getCastRange() + game.getWizardRadius()) 
		{
			enemy = *closestWizard;//
		}		
		else if(d_b < _self.getCastRange() + game.getGuardianTowerRadius())
		{
			enemy = *closestBuilding;//
		}
		else if (d_wt < _self.getCastRange() + weakestEnemy->getRadius())
		{
			enemy = *weakestEnemy;//
		}			
		else if (d_m < _self.getCastRange() + game.getMinionRadius())
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
	if (underBonus && world.getTickIndex() - lastBonusCheck > 2400)
		underBonus = false;
}

/**
* Инциализируем стратегию.
* <p>
* Для этих целей обычно можно использовать конструктор, однако в данном случае мы хотим инициализировать генератор
* случайных чисел значением, полученным от симулятора игры.
*/
void MyStrategy::initializeStrategy(const Wizard& _self, const Game& _game)
{
	
	/*if (lane == LaneType::_LANE_UNKNOWN_)
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
		}*/
	lane = LaneType::LANE_MIDDLE; // будем ходить по центру

	double distanceToLane = 6000;

	if ((lane == LaneType::LANE_TOP && (_self.getX() > 1000 && _self.getY() > 1000)) ||
		(lane == LaneType::LANE_MIDDLE && fabs(game.getMapSize() - (_self.getX() + _self.getY())) > 600) ||
		(lane == LaneType::LANE_BOTTOM && (_self.getX() < 3000 && _self.getY() < 3000)))
		// если оказались не там, где надо
		{
			for (auto & route : waypointsByLane)
			{
				for (Point2D & point : route.second)
					if (distanceToLane > self.getDistanceTo(point.getX(), point.getY()))
					{
						distanceToLane = self.getDistanceTo(point.getX(), point.getY());
						changeLaneTo = route.first;
					}
			}
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
Point2D MyStrategy::getPreviousWaypoint()
{
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
		_move.setSpeed(SPEED_BONUS_FACTOR + game.getWizardForwardSpeed());
}

void MyStrategy::goTo_wow(const Point2D & point, Move& _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	
	

	if (pathFinfder)
	{
		fillTheMap(); // заполняем карту видимыми объектами
		
		if (self.getDistanceTo(point.getX(), point.getY()) < 200)
		{
			int X = int(self.getX() / scale);
			int Y = int(self.getY() / scale);
			int x_to = (point.getX()) / scale;
			int y_to = (point.getY()) / scale;

			way = myWay(X, Y, x_to, y_to);
			followWay(_move);
		}
		else
		{
			double x1 = point.getX() - self.getX();
			double y1 = point.getY() - self.getY();
			double module = sqrt(x1*x1 + y1*y1);
			x1 = x1* 200.0 / module;
			y1 = y1* 200.0 / module;

			int X = int(self.getX() / scale);
			int Y = int(self.getY() / scale);
			int x_to = X + x1 / scale;
			int y_to = Y + y1 / scale;

			way = myWay(X, Y, x_to, y_to);
			followWay(_move);
		}
	}
	else goToAdv(point, _move);
}

void MyStrategy::goTo(const Point2D & point, const Point2D & lookAt, Move& _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	double angle = self.getAngleTo(lookAt.getX(), lookAt.getY());
	_move.setTurn(angle);
	if (angle > game.getWizardMaxTurnAngle()) return;

	double angleToPoint = self.getAngleTo(point.getX(), point.getY());

	if (angleToPoint > game.getWizardMaxTurnAngle())
		_move.setStrafeSpeed(game.getWizardStrafeSpeed());
	else if (angleToPoint < -game.getWizardMaxTurnAngle())
	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setStrafeSpeed(-game.getWizardStrafeSpeed());
	else _move.setStrafeSpeed(0);

	if (fabs(angleToPoint) < (PI / 4.0) - game.getWizardMaxTurnAngle())
		_move.setSpeed(SPEED_BONUS_FACTOR + game.getWizardForwardSpeed());
	else if (fabs(angleToPoint) > (PI / 4.0) + game.getWizardMaxTurnAngle())
		_move.setSpeed(-SPEED_BONUS_FACTOR - game.getWizardForwardSpeed());
	else
		_move.setSpeed(0);
}

void MyStrategy::goBackwardTo(const Point2D & point, Move& _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	double angle = self.getAngleTo(2 * self.getX() - point.getX(), 2 * self.getY() - point.getY());

	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle())
		_move.setSpeed(-SPEED_BONUS_FACTOR -game.getWizardForwardSpeed());

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
			_move.setSpeed(-game.getWizardForwardSpeed()- SPEED_BONUS_FACTOR);
	}
}

void MyStrategy::goBackwardFrom (const Point2D & point, Move& _move)
{
	double angle = self.getAngleTo(point.getX(), point.getY());
	_move.setTurn(angle);
	if (fabs(angle) < game.getWizardMaxTurnAngle()) {
		_move.setSpeed(-game.getWizardForwardSpeed()-SPEED_BONUS_FACTOR);
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
	Point2D p1(1200, 1200);
	Point2D p2(2800, 2800);
	double d1 = self.getDistanceTo(1200, 1200);
	double d2 = self.getDistanceTo(2800, 2800);
	bool isFirstClosest = d1 < d2;

	double d = std::min(self.getDistanceTo(2800, 2800), self.getDistanceTo(1200, 1200));
	int t = (PI / game.getWizardMaxTurnAngle()) + (d / (game.getWizardForwardSpeed()));// время, необходимое чтобы добраться до бонуса

	if ((world.getTickIndex() - lastBonusCheck < game.getBonusAppearanceIntervalTicks() - t) ||
		(world.getTickIndex() > 17500 && lastBonusCheck == 17500))
		return 0;

	//if  (pathFinfder && d < 1000) {			 
	//	goTo_wow((isFirstClosest ? Point2D(1200, 1200) : Point2D(2800, 2800)), _move);		
	//	return 666;
	//}
	
	const double mapSize = game.getMapSize();
	const double x = self.getX();
	const double y = self.getY();
	
	posBeforeBonus = Point2D(x, y);	//поменяется когда заберем бонус
	
	if (world.getTickIndex() - lastBonusCheck > game.getBonusAppearanceIntervalTicks() - t)
		if ((self.getDistanceTo(1200, 1200) < self.getVisionRange() + 50))
			if (d > self.getRadius() + game.getBonusRadius() + 10) return 41;
			else if (d > self.getRadius() + game.getBonusRadius()+1) return 410;
			else return 411;
		else if ((self.getDistanceTo(2800, 2800) < self.getVisionRange()+50) && d > 65)
			if (d > self.getRadius() + game.getBonusRadius() + 10) return 42;
			else if (d > self.getRadius() + game.getBonusRadius()+1) return 420;
			else return 421;

	// разделяем зоны на близость к бонусам
	if (x < 820 && y < 820) return 1;
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
		if (d1 < self.getRadius() + game.getBonusRadius() + 4.0 || d1 < self.getRadius() + game.getBonusRadius() + 4.0) {
			underBonus = true;
			if (bonuses.size() == 1) bonus = bonuses[0];
			else if (self.getDistanceTo(bonuses[0]) < self.getDistanceTo(bonuses[1]))
				bonus = bonuses[0];
			else 
				bonus = bonuses[1];
		}
		// бонус проверен, либо мы его захаваем либо не мы, точно пропадет(или меня завалят)
		returnToLastPos = true;
		bonusChecked = true;
		lastBonusCheck =  world.getTickIndex() - world.getTickIndex()%2500; //кратная 2500 штука
		
		double e_d = 6000;		
	
		shared_ptr<LivingUnit> enemy;
		if (fabs(self.getX() - self.getY()) < 400 && (d1 < 800 || d2 < 800))
			if (d1 < d2)
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) - d1) < 50) // если это тот бонус, который ближний
					{
						if (lane == LaneType::LANE_MIDDLE && !isSkillsEnable) // если скилы отключены, убегаем от башни
						{
							changeLaneTo = LaneType::LANE_TOP;
							posBeforeBonus = Point2D(800, 800); // so hardcoded
						}
						else 
							posBeforeBonus = Point2D(800, 800); // so hardcoded

						if ((d_e < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) //если враг близко и бонус с другой стороны
						{
							goBackwardToAdv(Point2D(mapSize*0.3, mapSize*0.3), _move);
							enemy = closestEnemy;
							e_d = d_e;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.3, mapSize*0.3), _move);
							enemy = closestWizard;
							e_d = d_w;
						}
						//атакуем, если видим вражеского волшебника
						if (self.getRemainingActionCooldownTicks() == 0 && e_d < self.getCastRange() && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
						{	
							//setStrafe(_move);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && e_d <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if ( isSkillsEnable && numOfLearnedSkills > frostBoltSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
							{
								_move.setAction(ActionType::ACTION_FROST_BOLT);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFrostBoltRadius());
							}
							else if (isSkillsEnable && numOfLearnedSkills > fireballSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
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

						if ((d_e < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) // враг близко и бонус с другой стороны, то основной враг - он
						{
							goBackwardToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							enemy = closestEnemy;
							e_d = d_e;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							enemy = closestWizard;
							e_d = d_w;
						}

						//атакуем, если видим вражеского волшебника					
						if (self.getRemainingActionCooldownTicks() == 0 && e_d < self.getCastRange() && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
						{
							//setStrafe(_move);
							if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && e_d <= 70)
								_move.setAction(ActionType::ACTION_STAFF);
							else if (isSkillsEnable && numOfLearnedSkills > frostBoltSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
							{
								_move.setAction(ActionType::ACTION_FROST_BOLT);
								_move.setCastAngle(self.getAngleTo(*enemy));
								_move.setMinCastDistance(e_d - (*enemy).getRadius() + game.getFrostBoltRadius());
							}
							else if (isSkillsEnable && numOfLearnedSkills > fireballSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
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

	//LivingUnit * unit = nullptr;
	//Building * b = nullptr;
	//if (world.getBuildings().size() != 0)
	//	unit = static_cast<LivingUnit*> (&(world.getBuildings()[0]));

	std::vector<shared_ptr<LivingUnit>> targets;
	for (unsigned int i = 0; i < world.getBuildings().size(); i++)
	{
		targets.push_back(shared_ptr<LivingUnit>(new Building(world.getBuildings()[i])));
	}
	for (unsigned int i = 0; i < world.getWizards().size(); i++)
	{
		targets.push_back(shared_ptr<LivingUnit>(new Wizard(world.getWizards()[i])));
	}
	for (unsigned int i = 0; i < world.getMinions().size(); i++)
	{
		targets.push_back(shared_ptr<LivingUnit>(new Minion(world.getMinions()[i])));
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
		[](shared_ptr<LivingUnit> u1, shared_ptr<LivingUnit> u2) {
		return ( double(u1->getLife())/double(u1->getMaxLife())  <   double(u2->getLife()) / double(u2->getMaxLife()));
	});

	auto it = targets.begin();
	while (it != targets.end() && ((*it)->getFaction() == self.getFaction() || 
		  ((*it)->getDistanceTo(self) > self.getCastRange()  + (*it)->getRadius()) ||
		   ((*it)->getFaction() == Faction::FACTION_NEUTRAL) && (*it)->getLife() == (*it)->getMaxLife())){
		it++;
	}
	if (it != targets.end())
		weakestEnemy = *it;

	for (auto &u : targets)
	{
		if (u->getFaction() != Faction::FACTION_NEUTRAL && u->getFaction() != self.getFaction() || (u->getFaction() == Faction::FACTION_NEUTRAL && u->getLife() != u->getMaxLife()))
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

void MyStrategy::fillTheMap()
{	
	int mapSize = 4000;
	for (unsigned int i = 0; i < (mapSize / scale); i++)
	{
		for (unsigned int j = 0; j < (mapSize / scale); j++)
			myMap[i][j] = 0;
	}
	__int16 X = 0;
	__int16 Y = 0;
	__int16 R = 0;
	for (auto &u : world.getBuildings())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 45) / scale;
		fillCircle(X, Y, R);
	}
	for (auto &u : world.getWizards())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 45) / scale;
		if (!u.isMe())fillCircle(X, Y, R);
	}
	for (auto &u : world.getMinions())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 45) / scale;
		fillCircle(X, Y, R);

	}
	for (auto &u : world.getTrees())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 45) / scale;
		fillCircle(X, Y, R);
	}
}

void MyStrategy::fillCircle(int x, int y, int r)
{
	int size = 4000 / scale;
	if (x >= size || y >= size || x + r >= size || y + r >= size || x - r < 0 || y - r < 0) return;
	myMap[x][y] = 1;
	for (int i = x - r; i < x + r; i++)
		for (int j = y - r; j < y + r; j++)
		{
			if ((i - x)*(i - x) + (j - y)*(j - y) < r*r)
				myMap[i][j] = 1;
		}
}

std::vector<Point2D> MyStrategy::myWay(int x, int y, int x_to, int y_to)
{
	way.clear();
	// будем использовать арифметику целых чисел
	const int size = 100;
	int step;
	
	int presicion = 100;
	
	bool added = true, result = true;
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			if (myMap[i][j] != 0)
			{
				matrix[i][j][0] = -2;// занято
			}
			else
			{
				matrix[i][j][0] = -1;// Мы еще нигде не были
			}
			matrix[i][j][3] = size*scale*sqrt(2);
		}
	}
	matrix[x_to][y_to][0] = 0;// До финиша ноль шагов - от него будем разбегаться
	matrix[x_to][y_to][3] = 0;// До финиша расстояние 0
	step = 0; // Изначально мы сделали ноль шагов

			  // Пока вершины добаляются и мы не дошли до старта

	while (added && matrix[x][y][0] == -1)
	{
		added = false;// Пока что ничего не добавили
		step++;// Увеличиваем число шагов

		for (int i = 0; i<size; i++)// Пробегаем по всей карте
		{
			for (int j = 0; j<size; j++)
			{
				// Если (i, j) была добавлена на предыдущем шаге
				// Пробегаем по всем четырем сторонам
				if (matrix[i][j][0] == step - 1) // <------------------
				{
					double distance = matrix[i][j][3];
					int _i, _j;

					_i = i + 1; _j = j;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, или можно добраться быстрее то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion;
							added = true; // Что-то добавили
						}
					}
					_i = i - 1; _j = j;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion;
							added = true; // Что-то добавили
						}
					}
					_i = i; _j = j + 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion;
							added = true; // Что-то добавили
						}
					}
					_i = i; _j = j - 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion;
							added = true; // Что-то добавили
						}
					}
					// Добавляем диагонали
					_i = i + 1; _j = j + 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion * sqrt(2) ;
							added = true; // Что-то добавили
						}
					}
					_i = i - 1; _j = j + 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // Что-то добавили
						}
					}
					_i = i + 1; _j = j - 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // Что-то добавили
						}
					}
					_i = i - 1; _j = j - 1;
					// Если не вышли за пределы карты -  обрабатываем
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// Если (_i, _j) уже добавлено или непроходимо, то не обрабатываем 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // Добав-
							matrix[_i][_j][1] = i; // ля-
							matrix[_i][_j][2] = j; // ем
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // Что-то добавили
						}
					}
				}
			}
		}
	}

	if (matrix[x][y][0] == -1)
	{
		result = false; // то пути не существует
	}

	if (result)
	{
		int _i = x, _j = y;
		int li, lj;
		while (matrix[_i][_j][0] != 0)
		{
			// тут ваш код где  записываем значение клеток и шагаем дальше к началу
			// записывать надо _i _J	
			way.push_back(Point2D(_i*scale + scale/2, _j*scale + scale / 2));
			li = matrix[_i][_j][1];
			lj = matrix[_i][_j][2];
			_i = li; _j = lj;
		}
	}

	return way;
}

void MyStrategy::followWay(model::Move & _move)
{
	/*int lastWaypointIndex = way.size() - 1;
	
	Point2D lastWaypoint = way[lastWaypointIndex];
	Point2D nextPoint = lastWaypoint;
	for (int waypointIndex = 0; waypointIndex < lastWaypointIndex; ++waypointIndex) {
		Point2D waypoint = way[waypointIndex];

		if (waypoint.getDistanceTo(self) <= 20) {
			nextPoint =  way[waypointIndex + 1];
		}

		if (lastWaypoint.getDistanceTo(waypoint) < lastWaypoint.getDistanceTo(self)) {
			nextPoint = waypoint;
		}
	}*/
	if (way.size() <= 1) return;
		Point2D point = way[1];
	
	Tree tree = getClosestTree();
	if (self.getAngleTo(tree) <  game.getStaffSector() / 2.0)
	{
		goTo(Point2D(tree.getX(), tree.getY()), _move);
		_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
		_move.setCastAngle(self.getAngleTo(tree));
	}
	goTo(point, _move);//goToAdv(point, _move);
}

void MyStrategy::attackEnemy(const Wizard& _self, const World& _world, const Game& _game, Move& _move, const LivingUnit& enemy)
{
	if (isSkillsEnable)
	{
		attackEnemyAdv(_self, _world, _game, _move, enemy);
		return;
	}

	double distance = _self.getDistanceTo(enemy) - enemy.getRadius() ;
	double angle = _self.getAngleTo(enemy);

	//////////////////предсказываем где будет юнит через премя которое летит ракета

	if (ALLOW_PREDICTION)
	{
		double tRocket = distance / game.getMagicMissileSpeed();
		Point2D enemyPrediction(enemy.getX(), enemy.getY());
		if (enemy.getAngleTo(self) > PI / 4 && (enemy.getSpeedX() > 0 || enemy.getSpeedY() > 0))
			enemyPrediction = enemyPrediction + Point2D(+enemy.getSpeedX()*tRocket, enemy.getSpeedY()*tRocket);
		angle = _self.getAngleTo(enemyPrediction.getX(), enemyPrediction.getY());
	}
	bool keepGoing = false;

	// advanced dodge
	//if (enemy.getRadius() == 35) // then it is wizard
	//{
	//	if (closestWizard->getAngleTo(self) < _game.getStaffSector() / 2.0)
	//	{
	//		//Wizard* badGuy = dynamic_cast<Wizard*> (closestWizard.get()); //не работает
	//		Wizard* badGuy = static_cast<Wizard*> (closestWizard.get());
	//		double tRocket = distance / game.getMagicMissileSpeed();
	//		if (badGuy != nullptr && badGuy != NULL)
	//			if ((badGuy->getDistanceTo(self) < badGuy->getCastRange()) && (badGuy->getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE]) < 60 - tRocket)
	//				keepGoing = false;// true;
	//	}
	//		
	//}
			
	if (_self.getRemainingActionCooldownTicks() == 0 || distance < 100) //уж если близко, то деремся
	{
		// Если цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 100)
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
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
				_move.setTurn(angle); 
			else dodgeFrom(_self, _world, _game, _move, enemy); //если есть шанс уйти
		}
		else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
			_move.setTurn(angle);
		else dodgeFrom(_self, _world, _game, _move, enemy);
	}
	else if (distance < self.getCastRange() - 50)
	{
		goTo(getPreviousWaypoint(), Point2D(enemy.getX(), enemy.getY()), _move); //ADV?
	}	
	else dodgeFrom(_self, _world, _game, _move, enemy);
}

void MyStrategy::attackEnemyAdv(const model::Wizard & _self, const model::World & _world, const model::Game & _game, model::Move & _move, const model::LivingUnit & enemy)
{
	double distance = _self.getDistanceTo(enemy) - enemy.getRadius();
	double angle = _self.getAngleTo(enemy);
	//setStrafe(_move);
	
	if (ALLOW_PREDICTION)
	{
		double tRocket = distance / game.getMagicMissileSpeed();
		Point2D enemyPrediction(enemy.getX(), enemy.getY());
		if (enemy.getAngleTo(self) > PI / 4 && (enemy.getSpeedX() > 0 || enemy.getSpeedY() > 0))
			enemyPrediction = enemyPrediction + Point2D(+enemy.getSpeedX()*tRocket, enemy.getSpeedY()*tRocket);
		angle = _self.getAngleTo(enemyPrediction.getX(), enemyPrediction.getY());
	}

	bool keepGoing = false;

	//advanced dodge
	//if (enemy.getRadius() == 35) // then it is wizard
	//{
	//	if (closestWizard->getAngleTo(self) < _game.getStaffSector() / 2.0)
	//	{
	//		Wizard* badGuy = static_cast<Wizard*> (closestWizard.get());
	//		double tRocket = distance / game.getMagicMissileSpeed();
	//		if (badGuy != nullptr && badGuy != NULL)
	//			if ((badGuy->getDistanceTo(self) < badGuy->getCastRange()) && (badGuy->getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE]) < 60 - tRocket)
	//				keepGoing = false;// true;
	//	}
	//}

	if (_self.getRemainingActionCooldownTicks() == 0)
	{		
		// Если цель перед нами, ...
		if (fabs(angle) < _game.getStaffSector() / 2.0)
		{
			double life = double(enemy.getLife()) / double(enemy.getMaxLife());
			if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 100)
			{
				goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
				_move.setAction(ActionType::ACTION_STAFF);
				return;
			}
			else if ((numOfLearnedSkills > frostBoltSkill) && (enemy.getLife() > 0.50) && enemy.getRadius() <= 35 &&
				(self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0))
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_FROST_BOLT);
				_move.setCastAngle(self.getAngleTo(enemy));
				_move.setMinCastDistance(distance - (enemy).getRadius() + game.getFrostBoltRadius());
				lastDodgeDir *= -1;
			}
			else if ((numOfLearnedSkills > fireballSkill) && (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10) &&
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
			else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
				_move.setTurn(angle);
			else dodgeFrom(_self, _world, _game, _move, enemy); // если есть шанс уйти
		}
		else if (_self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
			_move.setTurn(angle);
		else dodgeFrom(_self, _world, _game, _move, enemy);
	}
	//else if (distance < 100)
	//	goBackwardFrom(Point2D(enemy.getX(), enemy.getY()), _move);
	else if (distance < self.getCastRange() - 50)
	{
		goTo(getPreviousWaypoint(), Point2D(enemy.getX(), enemy.getY()) ,_move); //Adv?
	}
	else dodgeFrom(_self, _world, _game, _move, enemy);
}

void MyStrategy::dodgeFrom(const model::Wizard & _self, const model::World & _world, const model::Game & _game, model::Move & _move, const model::LivingUnit & enemy)
{
	if (self.getLife() == self.getMaxLife()) return;
	
	if ((numOfLearnedSkills > advancedMagicMissileSkill) || (enemy.getRadius() != 35) || (self.getDistanceTo(enemy) < self.getCastRange() - 100))
	{		
		double angle = self.getAngleTo(enemy);
		_move.setTurn(angle);
		if (angle < game.getWizardMaxTurnAngle())
			_move.setStrafeSpeed(lastDodgeDir * (game.getWizardStrafeSpeed() + SPEED_BONUS_FACTOR));		
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
	case 0: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1; break;
	case 1: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1; break;
	case 2: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2; break;
	case 3: skill = SkillType::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2; break;
	case 4: skill = SkillType::SKILL_FROST_BOLT; break;

	//next
	case 5:	skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_1; break;
	case 6: skill = SkillType::SKILL_RANGE_BONUS_AURA_1; break;
	case 7: skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_2; break;
	case 8: skill = SkillType::SKILL_RANGE_BONUS_AURA_2; break;
	case 9: skill = SkillType::SKILL_ADVANCED_MAGIC_MISSILE; break;

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
	_move.setMessages(messages);
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

	WAYPOINT_RADIUS = 100.0;
	LOW_HP_FACTOR = 0.25;	
	SPEED_BONUS_FACTOR = 0.0;
	ALLOW_PREDICTION = true;

	isSkillsEnable = false;
	numOfLearnedSkills = 0;
	nextSkill = 0;
	skillToLearn = SkillType::_SKILL_UNKNOWN_;

	frostBoltSkill = 4;
	advancedMagicMissileSkill = 9;
	fireballSkill = 14;

	
	
	underBonus = true;
	bonus = Bonus();
	bonusChecked = true;	
	lastBonusCheck = 0;
	
	returnToLastPos = true;	

	STRAFE_FACTOR = 5;
	strafeTicks = 0;
	lastStrafeDirection = 0;

	lastDodgeDir = 1;
	
	pathFinfder = true;
	double mapSize = 4000;
	scale = 40;
	for (unsigned int i = 0; i < (mapSize/scale); i++)
	{
		for (unsigned int j = 0; j < (mapSize / scale); j++)
			myMap[i][j] = 0;
	}
	//myMap.resize(400);
	//for (auto &raw : myMap)
	//{
	//	raw.assign(400, 0); // 0 - empty raw, -1 no way, value - distance to this point(if needed)
	//}

	lane = LaneType::_LANE_UNKNOWN_;
	changeLaneTo = LaneType::_LANE_UNKNOWN_;

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

	waypoints = waypointsByLane[LANE_TOP];

	closestBuilding = nullptr;
	closestFriend = nullptr;
	closestEnemy = nullptr;
	closestWizard = nullptr;
	closestMinion = nullptr;
	weakestEnemy = nullptr;
	closestNeutral = nullptr;

	d_f = d_e = d_w = d_b = d_m = d_wt = d_n = 6000;

	myLastPos = Point2D(0.0, 0.0);
	posBeforeBonus = Point2D(0.0, 0.0);
}

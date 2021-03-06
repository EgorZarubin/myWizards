#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;

/////////////////////////////////////////
#ifndef localMachine
//#define localMachine
#endif

#ifdef localMachine
#include <iostream>
#include <fstream>
ofstream mapAndPath;
#endif
//////// 
 

// �������  ������� ������� ����� (�������� ��������� � ��� �����)


// ��������� ����� �������, ������� ����� �� ���� ���������
// ������������ � ��� ����� �����, ������ � ����� �� �����
// �������� �������� �� ���������

void MyStrategy::move(const Wizard& _self, const World& _world, const Game& _game, Move& _move) {

	if (_world.getTickIndex() < 100) return; //�������� ���� ���� �����, ����� ����� �� ��������
	clearValues();

	//���������� ��������� ������ 5 ���
	myLastPos = Point2D(self.getX(), self.getY());
	if (_world.getTickIndex() % 5 == 0) myLastPos_5 = Point2D(self.getX(), self.getY());
	prevLife = self.getLife();

	if (_self.getLife() - prevLife > 50) changeLaneTo = LaneType::_LANE_UNKNOWN_;
	if (_self.getDistanceTo(posBeforeBonus.getX(), posBeforeBonus.getY()) < 50 || fabs(_self.getLife() - prevLife) > 50) returnToLastPos = false;


	//��� ���� ������� ����������� � ����
	if (_self.getDistanceTo(returnPoint.getX(), returnPoint.getY()) < 50 || fabs(_self.getLife() - prevLife) > 50) returnToBattle = false;

	//���� ������ ��� ����� �������� ����� �� ������� ����
	//if (fabs(_self.getLife() - prevLife) > 50) {}

	initializeStrategy(_self, _game);
	initializeTick(_self, _world, _game, _move);
	
	// ���������� ��� ����
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

	//������������ ���� �� �����: ������, �������, ����������, �������
	getTargets();

	getBattleField();	
	
	//���� ����� ����� - ����� � ���� � ���
	if (getBonus(_move)) return;		
	
	//���� ����� ������ � ������, ���� ��� �����
	int closeToBonus = getCloseToBonus(_move);  // ���������� �������� � ������ � ������
	
	switch (closeToBonus)
	{
		// ���� �� � ������������ ��������� - ���� � �����, ��� ����� �����
		// �� ������� ��������� ������
	case 0: break;
	case 1:
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_move, *closestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goTo(Point2D(800, 800), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);;//Backward
		}
		else
			goToAdv(Point2D(800, 800), _move);
		return;
	case 11:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_move, *closestEnemy);
			goTo(Point2D(750, 300), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);			
		}
		else
			goToThroughThrForest(Point2D(1200, 1200), _move);//goToAdv(Point2D(750, 300), _move);
		return;
	case 21:	
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_move, *closestEnemy);
			if (self.getDistanceTo(1200, 1200) > self.getVisionRange() - 10)
				goTo(Point2D(1400, 1750), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move); //Backward
		}
		else 
			goTo_wow(Point2D(1400, 1750), _move);//goToAdv(Point2D(1400, 1750), _move);
		return;
	case 22:
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_move, *closestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goTo(Point2D(2400, 2400), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);//Backward
		}
		else goToAdv(Point2D(2400, 2400), _move);
		return;
	case 23:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_move, *closestEnemy);
			goTo(Point2D(2000, 2000), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
		}
		else goToAdv(Point2D(2000, 2000), _move); return;
	case 3:
		if (d_e < _self.getCastRange())
		{
			attackEnemy(_move, *closestEnemy);
			if (self.getDistanceTo(2800, 2800) > self.getVisionRange() - 10)
				goTo(Point2D(3200, 3200), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);//Backward
		}
		else goToAdv(Point2D(3200, 3200), _move);
		return;
	case 31:
		if (d_e < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0) {
			attackEnemy(_move, *closestEnemy);
			goTo(Point2D(3700, 750), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
		}
		else
			goToThroughThrForest(Point2D(2800, 2800), _move); //goToAdv(Point2D(3700, 750), _move);
		return; // Backward
	case 41:
		if (d_w < _self.getCastRange() && _self.getRemainingActionCooldownTicks() == 0)
		{
			attackEnemy(_move, *closestWizard);
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
			attackEnemy(_move, *closestWizard);
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

	//���� ���� ������- �������� ����� (����� ��?) � ���
	if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR && _self.getDistanceTo(0,0) > 500)
	{
		goToThroughThrForest(getPreviousWaypoint(), _move);
		return;
	}
	
	// ��������� �����. ������ ���: ������ ������� ������, ����������, ����� ������ ����, �����, �������
	if (d_e < _self.getCastRange() + game.getMagicMissileRadius()-1) //���� ���� ��� �� � �������� ������������
	{
		_move.setSpeed(0);
		LivingUnit enemy = *closestEnemy;

		if (d_e < self.getRadius() + 100)
		{
			enemy = *closestEnemy;
		}
		else if (d_w < _self.getCastRange() + game.getMagicMissileRadius() - 1)
		{
			enemy = *closestWizard;//
		}
		else if (d_b < _self.getCastRange() + game.getMagicMissileRadius() - 1)
		{
			enemy = *closestBuilding;//
		}
		else if (d_wt < _self.getCastRange() + game.getMagicMissileRadius() - 1)
		{
			enemy = *weakestEnemy;//
		}
		else if (d_m < _self.getCastRange() + game.getMagicMissileRadius() - 1)
		{
			enemy = *closestMinion;//
		}

		attackEnemy(_move, enemy);
		return;
	}

	else if (_self.getLife() < _self.getMaxLife() * LOW_HP_FACTOR) // ���� �������� ���� ��������� �������, ��������� ����� � ���������� �������� ����� �� �����.
		goBackwardToAdv(getPreviousWaypoint(), _move);

	else if (d_e < self.getVisionRange())                                            // ���� ������ - ���� � ����
		goToAdv(Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);


	else if (returnToBattle)
	{
		goToThroughThrForest(returnPoint, _move);
	}

	//else if (returnToLastPos)
	//	if (testingStrategy && self.getDistanceTo(battlePoint.getX(), battlePoint.getY()) > self.getVisionRange())
	//		goTo_wow(battlePoint, _move);
	//	else 
	//		goToAdv(posBeforeBonus, _move);

	else if (d_f > 400 && d_f < 2000 && closestFriend->getRadius() < 100)// ����� � �������, ���� ��� ������ b � ��� �� ���� // ���� �� �������� ��������
		goToAdv(Point2D(closestFriend->getX(), closestFriend->getY()), _move);
	else // ���� ��� ������ ��������, ������ ������������ �����.		
		goToAdv(getNextWaypoint(), _move);	
	return;
}


void MyStrategy::clearValues()
{
	d_f = d_e = d_w = d_b = d_m = d_wt = d_n = 6000;
	if (underBonus && world.getTickIndex() - lastBonusCheck > 2400)
		underBonus = false;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			isBattle[i][j] = false;
}

/**
* ������������� ���������.
* <p>
* ��� ���� ����� ������ ����� ������������ �����������, ������ � ������ ������ �� ����� ���������������� ���������
* ��������� ����� ���������, ���������� �� ���������� ����.
*/
void MyStrategy::initializeStrategy(const Wizard& _self, const Game& _game)
{

	if (lane == LaneType::_LANE_UNKNOWN_) {
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
	}
	//lane = LaneType::LANE_MIDDLE; // ����� ������ �� ������

	double distanceToLane = 6000;

	if ((lane == LaneType::LANE_TOP && (_self.getX() > 1000 && _self.getY() > 1000)) ||
		(lane == LaneType::LANE_MIDDLE && fabs(game.getMapSize() - (_self.getX() + _self.getY())) > 600) ||
		(lane == LaneType::LANE_BOTTOM && (_self.getX() < 3000 && _self.getY() < 3000)))
		// ���� ��������� �� ���, ��� ����
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
	{
		lane = changeLaneTo;
		waypoints = waypointsByLane[changeLaneTo];
	}
	else
			waypoints = waypointsByLane[lane];

		//if (!self.isMaster())
		//	if (self.getMessages()[_self.getId()].getLane()  != LaneType::_LANE_UNKNOWN_)
		//		waypoints = waypointsByLane[self.getMessages()[_self.getId()].getLane()];
		
		// ���� ��������� ������� �� �������������, ��� �������� ���� �������� ����� ����������� �� ��������
		// ��������� �� ��������� �������� �����. ������ �������� ����� ����� ���������, ������ �� ������
		// �������� ���� ��������, ���� ������ �������� ���������� �������� �����.

		/*Point2D lastWaypoint = waypoints[waypoints.length - 1];

		Preconditions.checkState(ArrayUtils.isSorted(waypoints, (waypointA, waypointB) -> Double.compare(
		waypointB.getDistanceTo(lastWaypoint), waypointA.getDistanceTo(lastWaypoint)
		)));*/
}


/**
* ��������� ��� ������� ������ � ����� ������ ��� ��������� ������� � ���.
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
* �������� ������� ������ ��������� ��������� �������� ������ {@code getNextWaypoint}, ���� ����������� ������
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

void MyStrategy::getBattleField()
{
	battlePoint = Point2D(self.getX(), self.getY());
	double value = 0.0;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{
			if (isBattle[i][j])
				if (isBattle[i][j] && value < BattleFieldPriority[i][j] / self.getDistanceTo(i * 800, j * 800))
				{
					value = BattleFieldPriority[i][j] / self.getDistanceTo(i * 400, j * 400);
					battlePoint = Point2D(i * 400 + 200, j * 400 + 200);
				}
		}
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
		if (false)
		{
#ifdef localMachine
			mapAndPath.open("map.txt");
#endif
			fillTheMap(); // ��������� ����� �������� ���������
#ifdef localMachine
			for (int i = 0; i < 4000 / scale; i++)
			{
				for (int j = 0; j < 4000 / scale; j++)
					if (myMap[j][i] == 0)
						mapAndPath << 0;
					else if (myMap[j][i] == 2)
						mapAndPath << "#";
					else
						mapAndPath << "*";
				mapAndPath << endl;
			}
			mapAndPath.close();
#endif
		}

		if (/*self.getDistanceTo(point.getX(), point.getY()) < 500*/ true)
		{
			int X = int(self.getX() / scale);
			int Y = int(self.getY() / scale);
			int x_to = (point.getX()) / scale;
			int y_to = (point.getY()) / scale;

			way = myWay(X, Y, x_to, y_to);
			if (way.size() <= 5) goToAdv(point, _move);
			followWay(_move);
		}
		else
		{
			double x1 = point.getX() - self.getX();
			double y1 = point.getY() - self.getY();
			double module = sqrt(x1*x1 + y1*y1);
			x1 = x1* 500.0 / module;
			y1 = y1* 500.0 / module;

			int X = int(self.getX() / scale);
			int Y = int(self.getY() / scale);
			int x_to = X + x1 / scale;
			int y_to = Y + y1 / scale;

			way = myWay(X, Y, x_to, y_to);
			if (way.size() <= 5) goToAdv(point, _move);
			followWay(_move);
		}
	}
	else goToAdv(point, _move);
	
}

void MyStrategy::goToThroughThrForest(const Point2D & point, model::Move & _move)
{
	if (self.getDistanceTo(point.getX(), point.getY()) < 10) return;
	
	fillTheMap(); // ��������� ����� �������� ���������

	int X = int(self.getX() / scale);
	int Y = int(self.getY() / scale);
	int x_to = (point.getX()) / scale;
	int y_to = (point.getY()) / scale;

	way = myWay(X, Y, x_to, y_to);
	if (way.size() <= 5) goToAdv(point, _move);
	followWay(_move);
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


	if (self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1 && d_f <= self.getRadius() + 10) //�������� ���� ����� - ������� ���
	{
		double  dA = fabs(self.getAngleTo(*closestFriend) - self.getAngleTo(point.getX(), point.getY()));
		if (dA< PI / 2)
			goTangentialFrom(Point2D(closestFriend->getX(), closestFriend->getY()), point, _move);
		else
			goTo(point, _move);
	}
	else if (self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) // �������� ���� ����� ������
	{
		Tree tree = getClosestTree();
		if (self.getDistanceTo(tree) < self.getRadius() + tree.getRadius() + 10)
		{
			goTo(Point2D(tree.getX(), tree.getY()), _move);
			_move.setAction(ActionType::ACTION_STAFF);
		}
		else if (d_n < 6000 && d_n < self.getRadius() + 5)
		{
			goTangentialFrom(Point2D(closestNeutral->getX(), closestNeutral->getY()), point, _move);
			setStrafe(_move);
		}
		else  if (self.getDistanceTo(myLastPos_5.getX(), myLastPos_5.getY()) < 0.1)// ���� ������ ��������
		{
			_move.setTurn(game.getWizardMaxTurnAngle());
			_move.setSpeed( -game.getWizardForwardSpeed()); // �� ��������, ����������, ������ �������
		}
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
	
	if (fabs(angle - angle3) < 1e-10) // ����� ������ �� ������
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
	if (d_b < self.getVisionRange() + 50) return 0; //���� ������� ������ - ������ �� ��������

	Point2D p1(1200, 1200);
	Point2D p2(2800, 2800);
	double d1 = self.getDistanceTo(1200, 1200);
	double d2 = self.getDistanceTo(2800, 2800);
	bool isFirstClosest = d1 < d2;

	double d = std::min(self.getDistanceTo(2800, 2800), self.getDistanceTo(1200, 1200));
	int t = (PI / game.getWizardMaxTurnAngle()) + (d / (game.getWizardForwardSpeed()));// �����, ����������� ����� ��������� �� ������

	if ((world.getTickIndex() - lastBonusCheck < game.getBonusAppearanceIntervalTicks() - t) ||
		(world.getTickIndex() > 17500 && lastBonusCheck == 17500))
		return 0;

	if  (pathFinfder && d < 1000) {			 
		goTo_wow((isFirstClosest ? Point2D(1200, 1200) : Point2D(2800, 2800)), _move);		
		return 666;
	}
	
	const double mapSize = game.getMapSize();
	const double x = self.getX();
	const double y = self.getY();
	
	posBeforeBonus = Point2D(x, y);	//���������� ����� ������� �����
	
	if (world.getTickIndex() - lastBonusCheck > game.getBonusAppearanceIntervalTicks() - t)
		if ((self.getDistanceTo(1200, 1200) < self.getVisionRange() + 50))
			if (d > self.getRadius() + game.getBonusRadius() + 10) return 41;
			else if (d > self.getRadius() + game.getBonusRadius()+1) return 410;
			else return 411;
		else if ((self.getDistanceTo(2800, 2800) < self.getVisionRange()+50) && d > 65)
			if (d > self.getRadius() + game.getBonusRadius() + 10) return 42;
			else if (d > self.getRadius() + game.getBonusRadius()+1) return 420;
			else return 421;

	// ��������� ���� �� �������� � �������
	if (x < 820 && y < 820) return 1;
	if (x < 1600 && y < 800) return 11; //400

	if ((x > mapSize - 820 && y > mapSize - 820)) return 3;
	if ((x > mapSize - 800 && y > mapSize - 1600)) return 31; //400

	if (abs(x - y) < 300 && (x < mapSize - y) && x > mapSize - y - 1000) return 21;
	if (abs(x - y) < 300 && (x > mapSize - y) && x < mapSize - y + 1000) return 22;
	if (x > 2000 && x < 2500 && fabs(x + y - mapSize)< 300) return 23;
	
    //	�������� ����� � ������	 
	return 0;
}

bool MyStrategy::getBonus(model::Move & _move)
{
	double mapSize = game.getMapSize();
	double d1 = self.getDistanceTo(mapSize*0.3, mapSize*0.3);
	double d2 = self.getDistanceTo(mapSize*0.7, mapSize*0.7);

	vector<Bonus> bonuses = world.getBonuses();

	int i = (game.getBonusAppearanceIntervalTicks() - (world.getTickIndex() % game.getBonusAppearanceIntervalTicks()));
	if (bonuses.size() != 0)
	{
		//���� �� ������ � ������, ������ ����� �� ��� � ���������
		if (d1 < self.getRadius() + game.getBonusRadius() + 4.0 || d2 < self.getRadius() + game.getBonusRadius() + 4.0) 
			underBonus = true;

		if (bonuses.size() == 1)
			bonus = bonuses[0];
		else if (self.getDistanceTo(bonuses[0]) < self.getDistanceTo(bonuses[1]))
			bonus = bonuses[0];
		else
			bonus = bonuses[1];

		if (self.getDistanceTo(bonus) > self.getVisionRange()) return false; //���� ���� ����� ����� �� ��, �� ����� ����� ������

		// ����� ��������, ���� �� ��� �������� ���� �� ��, ����� ��������(��� ���� �������)
		returnToLastPos = true;
		bonusChecked = true;
		lastBonusCheck =  world.getTickIndex() - world.getTickIndex()%2500; //������� 2500 �����
		
		//���������� ���� ��� ��������� 
		defineReturnPoint();

		double e_d = 6000;		
	
		shared_ptr<LivingUnit> enemy;
		if (fabs(self.getX() - self.getY()) < 400 && (d1 < 800 || d2 < 800))
		{
			if (d1 < d2)
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) - d1) < 50) // ���� ��� ��� �����, ������� �������
					{						
						if ((d_e < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) //���� ���� ������ � ����� � ������ �������
						{						
							goTo(Point2D(mapSize*0.3, mapSize*0.3), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
							enemy = closestEnemy;
							e_d = d_e;
						}
						if (d_w < self.getCastRange() + game.getMagicMissileRadius() - 1)
						{
							goTo(Point2D(mapSize*0.3, mapSize*0.3), Point2D(closestWizard->getX(), closestWizard->getY()), _move);
							enemy = closestWizard;
							e_d = d_w;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.3, mapSize*0.3), _move);
							enemy = closestWizard;
							e_d = d_w;
						}						
						if (enemy == nullptr) return true;
						//�������, ���� ����� ���������� ���������� ��� ����� ����� ������
						if (e_d < self.getCastRange() + game.getMagicMissileRadius()-1)
							if (self.getRemainingActionCooldownTicks() == 0 && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
							{
								double castDistance = ((e_d + game.getMagicMissileRadius()) > self.getCastRange()) ? (self.getCastRange() - 1) : (e_d + game.getMagicMissileRadius());
								//setStrafe(_move);
								if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && (e_d <= self.getRadius() + game.getStaffRange()))
									_move.setAction(ActionType::ACTION_STAFF);
								else if (isSkillsEnable && numOfLearnedSkills > frostBoltSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
								{
									_move.setAction(ActionType::ACTION_FROST_BOLT);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
								else if (isSkillsEnable && numOfLearnedSkills > fireballSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
								{
									_move.setAction(ActionType::ACTION_FIREBALL);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
								else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
								{
									_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
							}
						return true;
					}
			}
			else
			{
				for (auto & i : bonuses)
					if (fabs(self.getDistanceTo(i) - d2) < 50) // ���� ��� ��� �����, ������� �������
					{
						if ((d_e < 80) && (fabs(fabs(self.getAngleTo(mapSize*0.3, mapSize*0.3) - PI) < game.getStaffSector()))) // ���� ������ � ����� � ������ �������, �� �������� ���� - ��
						{
							//goBackwardToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							goTo(Point2D(mapSize*0.7, mapSize*0.7), Point2D(closestEnemy->getX(), closestEnemy->getY()), _move);
							enemy = closestEnemy;
							e_d = d_e;
						}
						if (d_w < self.getCastRange() + game.getMagicMissileRadius()-1)
						{
							goTo(Point2D(mapSize*0.7, mapSize*0.7), Point2D(closestWizard->getX(), closestWizard->getY()), _move);
							enemy = closestWizard;
							e_d = d_w;
						}
						else
						{
							goToAdv(Point2D(mapSize*0.7, mapSize*0.7), _move);
							enemy = closestWizard;
							e_d = d_w;
						}

						if (enemy == nullptr) return true;
						//�������, ���� ����� ���������� ����������	
						if (e_d < self.getCastRange() + game.getMagicMissileRadius())
							if (self.getRemainingActionCooldownTicks() == 0 && fabs(self.getAngleTo(*enemy)) < game.getStaffSector() / 2.0)
							{
								double castDistance = ((e_d + game.getMagicMissileRadius()) > self.getCastRange()) ? (self.getCastRange() - 1) : (e_d + game.getMagicMissileRadius());
								//setStrafe(_move);
								if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && (e_d <= self.getRadius() + game.getStaffRange()))
									_move.setAction(ActionType::ACTION_STAFF);
								else if (isSkillsEnable && numOfLearnedSkills > frostBoltSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)
								{
									_move.setAction(ActionType::ACTION_FROST_BOLT);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
								else if (isSkillsEnable && numOfLearnedSkills > fireballSkill && self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10)
								{
									_move.setAction(ActionType::ACTION_FIREBALL);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
								else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
								{
									_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
									_move.setCastAngle(self.getAngleTo(*enemy));
									_move.setMinCastDistance(castDistance);
								}
							}
						return true;
					}
			}
		}
		else if( d1 < 800 ) // ���� ����� ���
			goToThroughThrForest(Point2D(1200,1200),_move);
		else if (d2 < 800) // ���� ����� ���
			goToThroughThrForest(Point2D(2800,2800), _move);
	}
	else if ( (d1 < self.getVisionRange() - 10 || d2 < self.getVisionRange() - 10) && world.getTickIndex() - lastBonusCheck > game.getBonusAppearanceIntervalTicks())
	{
		bonusChecked = true;
		lastBonusCheck = world.getTickIndex() - world.getTickIndex() % 2500;	
	}
	
	return false;
}

void MyStrategy::defineReturnPoint()
{
	Point2D p1(800, 800);
	Point2D p2(1000, 200);
	Point2D p3(200, 1000);

	Point2D p4(2200, 2200);
	Point2D p5(2600, 1800);

	Point2D p6(3800, 3000);
	Point2D p7(3000, 3800);
	Point2D p8(3200, 3200);

	double d1 = self.getDistanceTo(4000 * 0.3, 4000 * 0.3);
	double d2 = self.getDistanceTo(4000 * 0.7, 4000 * 0.7);
	if (d1 > 400 && d2 > 400) return;
	returnToBattle = true; //

	if (changeLaneTo == LaneType::LANE_TOP || (changeLaneTo == LaneType::_LANE_UNKNOWN_ && lane == LaneType::LANE_TOP))
	{
		for (int i = 0; i < 2; i++)
			for (int j = 9; j > 1; j--)
				if (isBattle[i][j]) {
					returnPoint = p3;
					changeLaneTo = LaneType::LANE_TOP;
					return;
				}

		if (isBattle[0][0] || isBattle[0][1] || isBattle[1][0] || isBattle[1][1]) {
			returnPoint = p1;
			changeLaneTo = LaneType::LANE_TOP;
			return;
		}

		for (int i = 5; i < 10; i++)
			for (int j = 0; j < 2; j++)
				if (isBattle[i][j]) {
					returnPoint = p2;
					changeLaneTo = LaneType::LANE_TOP;
					return;
				}
		returnPoint = p2;
	}
	else if (changeLaneTo == LaneType::LANE_MIDDLE || (changeLaneTo == LaneType::_LANE_UNKNOWN_ && lane == LaneType::LANE_MIDDLE))
	{

		for (int i = 1; i < 4; i++)
			if (isBattle[i][9 - i]) {
				returnPoint = p4;
				changeLaneTo = LaneType::LANE_MIDDLE;
				return;
			}

		if (lane == LaneType::LANE_MIDDLE) {
			for (int i = 6; i < 9; i++)
				if (isBattle[i][9 - i]) {
					returnPoint = p5;
					changeLaneTo = LaneType::LANE_MIDDLE;
					return;
				}
		}
		returnPoint = p4;
	}
	else if (changeLaneTo == LaneType::LANE_BOTTOM || (changeLaneTo == LaneType::_LANE_UNKNOWN_ && lane == LaneType::LANE_BOTTOM))
	{
		for (int i = 0; i < 8; i++)
			for (int j = 9; j > 7; j--)
				if (isBattle[i][j]) {
					returnPoint = p7;
					changeLaneTo = LaneType::LANE_BOTTOM;
					return;
				}

		if (isBattle[9][9] || isBattle[9][8] || isBattle[8][9] || isBattle[8][8]) {
			returnPoint = p8;
			changeLaneTo = LaneType::LANE_BOTTOM;
			return;
		}

		for (int i = 8; i < 10; i++)
			for (int j = 0; j < 6; j++)
				if (isBattle[i][j]) {
					returnPoint = p6;
					changeLaneTo = LaneType::LANE_BOTTOM;
					return;
				}

		returnPoint = p8;
	}
	else
		returnPoint = p4;
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
	double minHealth = 1.0;

	double minDist = 6000;
	double minDist_b = 6000;
	double minDist_w = 6000;
	double minDist_m = 6000;
	double minDist_f = 6000;
	double minDist_n = 6000;

	// ������������� ���� �� ��������
	
	/*std::sort(targets.begin(), targets.end(),
		[](shared_ptr<LivingUnit> u1, shared_ptr<LivingUnit> u2) {
		return ( double(u1->getLife())/double(u1->getMaxLife())  <   double(u2->getLife()) / double(u2->getMaxLife()));
	});*/

	//auto it = targets.begin();
	//while (  it != targets.end() )
	//	if ((*it)->getFaction() == self.getFaction() ||
	//		((*it)->getDistanceTo(self) > self.getCastRange() + (*it)->getRadius() /*+ game.getMagicMissileRadius()*/) ||
	//		(((*it)->getFaction() == Faction::FACTION_NEUTRAL) && (*it)->getSpeedX() == 0 && (*it)->getSpeedY() == 0) && (*it)->getLife() == (*it)->getMaxLife()) {
	//		it++;
	//	}
	//	else 
	//		break;

	//if (it != targets.end())
	//	weakestEnemy = *it;

	for (auto &u : targets)
	{
		if ( u->getFaction() != Faction::FACTION_NEUTRAL && u->getFaction() != self.getFaction() ||
		    (u->getFaction() == Faction::FACTION_NEUTRAL && ( u->getLife() != u->getMaxLife() || u->getSpeedX() != 0 || u->getSpeedY() != 0)))
		{
			if (u->getDistanceTo(self) < self.getCastRange() + u->getRadius() + game.getMagicMissileRadius() && minHealth >= double(u->getLife()) / double(u->getMaxLife()))
			{
				minHealth = double(u->getLife()) / double(u->getMaxLife());
				weakestEnemy = u;
			}

			if (u->getDistanceTo(self) < minDist)
			{
				minDist = u->getDistanceTo(self);
				closestEnemy = u;
			}
		
			if (u->getRadius() >= 50 && u->getDistanceTo(self) < minDist_b)
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

			__int16 x = int(u->getX() / 400);
			__int16 y = int(u->getY() / 400);
			if (x >= 0 && y >= 0 && x < 10 && y < 10)
				isBattle[x][y] = true;
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

	if (closestFriend != nullptr)   d_f = self.getDistanceTo(*closestFriend) - closestFriend->getRadius();
	if (closestEnemy != nullptr)    d_e = self.getDistanceTo(*closestEnemy) - closestEnemy->getRadius();
	if (closestBuilding != nullptr) d_b = self.getDistanceTo(*closestBuilding) - closestBuilding->getRadius();
	if (closestWizard != nullptr)   d_w = self.getDistanceTo(*closestWizard) - closestWizard->getRadius();
	if (closestMinion != nullptr)   d_m = self.getDistanceTo(*closestMinion) - closestMinion->getRadius();
	if (weakestEnemy != nullptr)   d_wt = self.getDistanceTo(*weakestEnemy) - weakestEnemy->getRadius();
	if (closestNeutral != nullptr)  d_n = self.getDistanceTo(*closestNeutral) - closestNeutral->getRadius();
		
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
			if (i<35/scale || j<35/scale || i> (mapSize - 35) / scale || j> (mapSize - 35) / scale)
				myMap[i][j] = 1;
			else 
				myMap[i][j] = 0;
	}

	__int16 X = 0;
	__int16 Y = 0;
	__int16 R = 0;
	for (auto &u : world.getBuildings())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 40) / scale;
		fillCircle(X, Y, R);
	}
	for (auto &u : world.getWizards())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 40) / scale;
		if (!u.isMe())fillCircle(X, Y, R);
		else myMap[X][Y] = 2;
	}
	for (auto &u : world.getMinions())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 40) / scale;
		fillCircle(X, Y, R);

	}
	for (auto &u : world.getTrees())
	{
		X = static_cast<int>(u.getX() / scale);
		Y = static_cast<int>(u.getY() / scale);
		R = (u.getRadius() + 40) / scale;
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
	// ����� ������������ ���������� ����� �����
	const int size = 400;
	int step;
	
	int presicion = 100;
	
	bool added = true, result = true;
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			if (myMap[i][j] != 0 && myMap[i][j] != 2)
			{
				matrix[i][j][0] = -2;// ������
			}
			else
			{
				matrix[i][j][0] = -1;// �� ��� ����� �� ����
			}
			matrix[i][j][3] = size*scale*sqrt(2);
		}
	}
	matrix[x_to][y_to][0] = 0;// �� ������ ���� ����� - �� ���� ����� �����������
	matrix[x_to][y_to][3] = 0;// �� ������ ���������� 0
	step = 0; // ���������� �� ������� ���� �����

			  // ���� ������� ���������� � �� �� ����� �� ������

	while (added && matrix[x][y][0] == -1 && step < 1000)
	{
		added = false;// ���� ��� ������ �� ��������
		step++;// ����������� ����� �����

		for (int i = 0; i<size; i++)// ��������� �� ���� �����
		{
			for (int j = 0; j<size; j++)
			{
				// ���� (i, j) ���� ��������� �� ���������� ����
				// ��������� �� ���� ������� ��������
				if (matrix[i][j][0] == step - 1) // <------------------
				{
					double distance = matrix[i][j][3];
					int _i, _j;

					_i = i + 1; _j = j;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, ��� ����� ��������� ������� �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion;
							added = true; // ���-�� ��������
						}
					}
					_i = i - 1; _j = j;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion;
							added = true; // ���-�� ��������
						}
					}
					_i = i; _j = j + 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion;
							added = true; // ���-�� ��������
						}
					}
					_i = i; _j = j - 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion;
							added = true; // ���-�� ��������
						}
					}
					// ��������� ���������
					_i = i + 1; _j = j + 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion * sqrt(2) ;
							added = true; // ���-�� ��������
						}
					}
					_i = i - 1; _j = j + 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // ���-�� ��������
						}
					}
					_i = i + 1; _j = j - 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // ���-�� ��������
						}
					}
					_i = i - 1; _j = j - 1;
					// ���� �� ����� �� ������� ����� -  ������������
					if (_i >= 0 && _j >= 0 && _i<size && _j<size)
					{
						// ���� (_i, _j) ��� ��������� ��� �����������, �� �� ������������ 
						if ((matrix[_i][_j][0] == -1 && matrix[_i][_j][0] != -2) || (matrix[_i][_j][3] > distance + presicion*sqrt(2) && matrix[_i][_j][0] != -2))
						{
							matrix[_i][_j][0] = step; // �����-
							matrix[_i][_j][1] = i; // ��-
							matrix[_i][_j][2] = j; // ��
							matrix[_i][_j][3] = distance + presicion * sqrt(2);
							added = true; // ���-�� ��������
						}
					}
				}
			}
		}
	}

	if (matrix[x][y][0] == -1)
	{
		result = false; // �� ���� �� ����������
	}

	if (result)
	{
		int _i = x, _j = y;
		int li, lj;
		while (way.size() < 1000 && _i > 0 && _i < size && _j >0 && _j < size && matrix[_i][_j][0] != 0)
		{
			// ��� ��� ��� ���  ���������� �������� ������ � ������ ������ � ������
			// ���������� ���� _i _J	
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
	
	
	//if (self.getDistanceTo(point.getX(), point.getY()) < 10)
	//		point = way[2];

	//��������� ����
	Tree tree = getClosestTree();
	if (fabs(self.getAngleTo(tree)) <  game.getStaffSector() / 2.0 && self.getDistanceTo(tree) < self.getCastRange())
	{
		_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
		_move.setCastAngle(self.getAngleTo(tree));
	}

	if (way.size() < 5) return;
	int i = 4;
	Point2D point = way[4];
	double d = self.getDistanceTo(point.getX(), point.getY());
	while  (way.size() < i+1 && ( d < 10) )
	{	
		i++;	
		point = way[i];
		
		//_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
		//return;
	}
	//Point2D point = way[4];

	goToAdv(point, _move);//goToAdv(point, _move);
}

void MyStrategy::attackEnemy(Move& _move, const LivingUnit& enemy)
{
	if (isSkillsEnable)
	{
		attackEnemyAdv(_move, enemy);
		return;
	}

	double distance = self.getDistanceTo(enemy) - enemy.getRadius() ;
	double castDistance = ((distance + game.getMagicMissileRadius()) > self.getCastRange()) ? (self.getCastRange() - 1) : (distance + game.getMagicMissileRadius());
 	double angle = self.getAngleTo(enemy);

	//////////////////������������� ��� ����� ���� ����� ����� ������� ����� ������
	if (ALLOW_PREDICTION)
	{
		double tRocket = distance / game.getMagicMissileSpeed();
		Point2D enemyPrediction(enemy.getX(), enemy.getY());
		if (fabs(enemy.getAngleTo(self)) > PI / 4 && (fabs(enemy.getSpeedX()) > 0 || fabs(enemy.getSpeedY()) > 0))
			enemyPrediction = enemyPrediction + Point2D(+enemy.getSpeedX()*tRocket, enemy.getSpeedY()*tRocket);
		angle = self.getAngleTo(enemyPrediction.getX(), enemyPrediction.getY());
	}
	bool keepGoing = false;

	// advanced dodge
	//if (enemy.getRadius() == 35) // then it is wizard
	//{
	//	if (closestWizard->getAngleTo(self) < _game.getStaffSector() / 2.0)
	//	{
	//		//Wizard* badGuy = dynamic_cast<Wizard*> (closestWizard.get()); //�� ��������
	//		Wizard* badGuy = static_cast<Wizard*> (closestWizard.get());
	//		double tRocket = distance / game.getMagicMissileSpeed();
	//		if (badGuy != nullptr && badGuy != NULL)
	//			if ((badGuy->getDistanceTo(self) < badGuy->getCastRange()) && (badGuy->getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE]) < 60 - tRocket)
	//				keepGoing = false;// true;
	//	}
	//		
	//}
			
	if (self.getRemainingActionCooldownTicks() == 0 || distance < 100) //�� ���� ������, �� �������
	{
		// ���� ���� ����� ����, ...
		if (fabs(angle) < game.getStaffSector() / 2.0)
		{
			if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= 100)
			{
				goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
				_move.setAction(ActionType::ACTION_STAFF);
				return;
			}		
			else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(castDistance); //������ ���������� �������� ������ �����
				lastDodgeDir *= -1;
			}
			else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
				_move.setTurn(angle); 
			else dodgeFrom(_move, enemy); //���� ���� ���� ����
		}
		else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 12 && !keepGoing)
			_move.setTurn(angle);
		else dodgeFrom(_move, enemy);
	}
	else if (distance < self.getCastRange()+enemy.getRadius())
	{
		goTo(getPreviousWaypoint(), Point2D(enemy.getX(), enemy.getY()), _move); //ADV?
	}	
	else dodgeFrom( _move, enemy);
}

void MyStrategy::attackEnemyAdv(model::Move & _move, const model::LivingUnit & enemy)
{
	double distance = self.getDistanceTo(enemy) - enemy.getRadius();
	double castDistance = ((distance + game.getMagicMissileRadius()) > self.getCastRange()) ? (self.getCastRange() - 1) : (distance + game.getMagicMissileRadius());
	double angle = self.getAngleTo(enemy);
	//setStrafe(_move);
	
	if (ALLOW_PREDICTION)
	{
		double tRocket = distance / game.getMagicMissileSpeed();
		Point2D enemyPrediction(enemy.getX(), enemy.getY());
		if (fabs(enemy.getAngleTo(self)) > PI / 4 && (fabs(enemy.getSpeedX()) > 0 || fabs(enemy.getSpeedY()) > 0))
			enemyPrediction = enemyPrediction + Point2D(enemy.getSpeedX()*tRocket, enemy.getSpeedY()*tRocket);
		angle = self.getAngleTo(enemyPrediction.getX(), enemyPrediction.getY());
	}

	bool keepGoing = false;

	//if (enemy.getRadius() > 50) // ��� ����� ����������� ����������� ��� �����
	//{
	//}
	
	if (self.getRemainingActionCooldownTicks() == 0)
	{		
		// ���� ���� ����� ����, ...
		if (fabs(angle) < game.getStaffSector() / 2.0)
		{
			double life = double(enemy.getLife()) / double(enemy.getMaxLife());
			//���� �� ������ � ����, ���� �� �������
			if ((distance < self.getRadius() + game.getStaffRange() + 20) ) 
				goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
			
			if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_STAFF] == 0 && distance <= self.getRadius() + game.getStaffRange())
			{
				//goToAdv(Point2D(enemy.getX(), enemy.getY()), _move);
				_move.setAction(ActionType::ACTION_STAFF);
				return;
			}
			else if ((numOfLearnedSkills > frostBoltSkill) && (enemy.getLife() > 0.50) && enemy.getRadius() <= 35 &&
				(self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FROST_BOLT] == 0)&&
				(self.getMana() > game.getMagicMissileManacost()))
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_FROST_BOLT);
				_move.setCastAngle(self.getAngleTo(enemy));
				_move.setMinCastDistance(castDistance);
				lastDodgeDir *= -1;
			}
			else if ((numOfLearnedSkills > fireballSkill) && (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] > 10) &&
				(self.getRemainingCooldownTicksByAction()[ActionType::ACTION_FIREBALL] == 0) &&
				(self.getMana() > game.getFireballManacost()) &&
				distance > 300)
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_FIREBALL);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(castDistance);
				lastDodgeDir *= -1;
			}
			else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] == 0)
			{
				_move.setTurn(angle);
				_move.setAction(ActionType::ACTION_MAGIC_MISSILE);
				_move.setCastAngle(angle);
				_move.setMinCastDistance(castDistance);
				lastDodgeDir *= -1;
			}
			else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 15 && !keepGoing)
				_move.setTurn(angle);
			else dodgeFrom( _move, enemy); // ���� ���� ���� ����
		}		
		else if (self.getRemainingCooldownTicksByAction()[ActionType::ACTION_MAGIC_MISSILE] < 15 && !keepGoing)
			_move.setTurn(angle);
		else dodgeFrom( _move, enemy);
	}
	//else if (distance < 100)
	//	goBackwardFrom(Point2D(enemy.getX(), enemy.getY()), _move);
	else if (distance < self.getCastRange() + enemy.getRadius())
	{
		goTo(getPreviousWaypoint(), Point2D(enemy.getX(), enemy.getY()) ,_move); //Adv?
	}
	else dodgeFrom(_move, enemy);
}

void MyStrategy::dodgeFrom(model::Move & _move, const model::LivingUnit & enemy)
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
	
	//if (d_e < 150)
	//	goTo(point, Point2D(closestEnemy->getX(), closestEnemy->getY()),_move);

	if ((self.getDistanceTo(myLastPos.getX(), myLastPos.getY()) < 0.1) ) // �������� ���� ����� ������
	{
		Tree tree = getClosestTree();
		if (self.getDistanceTo(tree) < self.getRadius() + tree.getRadius() + 4 && fabs(self.getAngleTo(tree)) < PI / 2)
		{
			goTo(Point2D(tree.getX(), tree.getY()), _move);
			_move.setAction(ActionType::ACTION_STAFF);
		}
		else if (d_f <= self.getRadius() + 10) //�������� ���� ����� - ������� ���
		{
			double  dA = fabs(self.getAngleTo(*closestFriend) - self.getAngleTo(point.getX(), point.getY()));
			if (dA< PI / 2)
				goTangentialFrom(Point2D(closestFriend->getX(), closestFriend->getY()), point, _move);
			else
				goTo(point, _move);
		}
		else if (d_n < self.getRadius() + 5)
		{
			goTangentialFrom(Point2D(closestNeutral->getX(), closestNeutral->getY()), point, _move);
			setStrafe(_move);
		}
		else if (self.getDistanceTo(myLastPos_5.getX(), myLastPos_5.getY()) < 0.1) //���� ������ ������ ��������
		{
			_move.setTurn(game.getWizardMaxTurnAngle());
			_move.setSpeed(/*rand() % 2 ? -game.getWizardForwardSpeed() :*/ game.getWizardForwardSpeed()); // �� ��������, ����������, ������ �������
		}
		else
			_move.setSpeed(game.getWizardForwardSpeed()); // �� ��������, ����������, ������ �������
	}	
	
	else
		goTo(point, _move);
}


void MyStrategy::learnSkills(const model::Wizard & _self, model::Move& _move)
{
	if (skillToLearn != SkillType::_SKILL_UNKNOWN_) // �������� �����
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
	case 5: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_1; break;
	case 6: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_AURA_1; break;
	case 7: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_2; break;
	case 8: skill = SkillType::SKILL_STAFF_DAMAGE_BONUS_AURA_2; break;
	case 9: skill = SkillType::SKILL_FIREBALL; break;

	//next
	case 10:	skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_1; break;
	case 11: skill = SkillType::SKILL_RANGE_BONUS_AURA_1; break;
	case 12: skill = SkillType::SKILL_RANGE_BONUS_PASSIVE_2; break;
	case 13: skill = SkillType::SKILL_RANGE_BONUS_AURA_2; break;
	case 14: skill = SkillType::SKILL_ADVANCED_MAGIC_MISSILE; break;
			
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

		// ����� ������� ������������
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

	// skills options
	isSkillsEnable = false;
	numOfLearnedSkills = 0;
	nextSkill = 0;
	skillToLearn = SkillType::_SKILL_UNKNOWN_;

	// sequence for learning skills
	frostBoltSkill = 4;
	advancedMagicMissileSkill = 14;
	fireballSkill = 9;


	// bonus variables
	underBonus = false;
	bonus = Bonus();
	bonusChecked = true;
	lastBonusCheck = 0;

	returnToLastPos = false;

	STRAFE_FACTOR = 5;
	strafeTicks = 0;
	lastStrafeDirection = 0;

	lastDodgeDir = 1; // direction to avoif rockets


	pathFinfder = false; // activale advanced path searcing
	double mapSize = 4000;
	scale = 10;
	for (unsigned int i = 0; i < (mapSize / scale); i++)
	{
		for (unsigned int j = 0; j < (mapSize / scale); j++)
			myMap[i][j] = 0;
	}

	testingStrategy = false; // activate battle points detection
	// priority of battle points
	__int16 battlefield[10][10] = {
		{ 2, 2, 3, 3, 4, 4, 5, 5, 5, 5 },
		{ 2, 2, 3, 3, 4, 4, 4, 4, 5, 5 },
		{ 3, 1, 1, 1, 1, 1, 4, 5, 4, 5 },
		{ 3, 2, 1, 1, 1, 1, 4, 4, 4, 5 },
		{ 4, 3, 1, 1, 2, 3, 1, 1, 4, 4 },
		{ 4, 4, 1, 2, 3, 2, 1, 1, 4, 4 },
		{ 5, 4, 4, 4, 2, 1, 1, 1, 3, 3 },
		{ 5, 5, 5, 4, 1, 1, 1, 1, 3, 3 },
		{ 5, 5, 5, 4, 4, 3, 2, 1, 2, 2 },
		{ 5, 5, 5, 5, 4, 4, 3, 3, 2, 2 }
	};
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			BattleFieldPriority[i][j] = battlefield[i][j];

	returnToBattle = false;
	returnPoint = Point2D(0, 0);


	// initialize waypoints
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
	myLastPos_5 = Point2D(0.0, 0.0);
	posBeforeBonus = Point2D(0.0, 0.0);
}

#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace model;
using namespace std;

void MyStrategy::move(const Wizard& self, const World& world, const Game& game, Move& move) {
    move.setSpeed(game.getWizardForwardSpeed());
    move.setStrafeSpeed(game.getWizardStrafeSpeed());
    move.setTurn(game.getWizardMaxTurnAngle());
    move.setAction(ACTION_MAGIC_MISSILE);
	LivingUnit nearestEnemy = getNearestTarget();	
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

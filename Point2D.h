#pragma once

#include <math.h>
/**
* Вспомогательный класс для хранения позиций на карте.
*/
class Point2D {
	double x;
	double y;

public:
	Point2D(){
		this->x = 0;
		this->y = 0;
	}

	Point2D(double _x, double _y) {
		this->x = _x;
		this->y = _y;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getDistanceTo(double x, double y) {
		return sqrt((this->x - x)*(this->x - x) + (this->y - y)*(this->y - y));
	}

	double getDistanceTo(Point2D point) {
		return getDistanceTo(point.x, point.y);
	}

	double getDistanceTo(model::Unit  unit) {
		return getDistanceTo(unit.getX(), unit.getY());
	}
};

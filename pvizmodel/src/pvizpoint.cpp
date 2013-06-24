#include "pvizpoint.h"

PvizPoint::PvizPoint()
{
}

PvizPoint::PvizPoint(unsigned int id) : id(id)
{
}

std::string PvizPoint::toString()
{
	return format("Point %d (%f, %f, %f)", id, position[0], position[1], position[2]);
}

void PvizPoint::setPosition(double* p)
{
	position[0] = p[0];
	position[1] = p[1];
	position[2] = p[2];
}

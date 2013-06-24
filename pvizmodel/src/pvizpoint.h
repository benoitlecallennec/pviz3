#ifndef PVIZPOINT_H
#define PVIZPOINT_H

#include <string>
#include "pvizcomm.h"

class PvizPoint
{
public:
	PvizPoint();
	PvizPoint(unsigned int id);
	
	void setPosition(double* p);
	
public:	
	unsigned int id;
	std::string label;
	double position[3];
	
	std::string toString();
};

#endif // PVIZPOINT_H

#ifndef PVIZLINE_H
#define PVIZLINE_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "pvizcomm.h"
#include "pvizpoint.h"

class PvizLine
{
public:
	typedef std::vector<PvizPoint *> PvizPointVec_t;
public:
	PvizLine();
	PvizLine(unsigned int id);
	
    unsigned int id;
	
	PvizPointVec_t points;
	
	std::string toString();
};

#endif // PVIZLINE_H

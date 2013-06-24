#include "pvizline.h"

PvizLine::PvizLine()
{
}

PvizLine::PvizLine(unsigned int id) : id(id)
{
}

std::string PvizLine::toString()
{
	return format("Line %d", id);
}

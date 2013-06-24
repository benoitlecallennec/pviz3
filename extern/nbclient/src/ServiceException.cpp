/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include "ServiceException.h"

ServiceException::ServiceException(const string msg = ""): data(msg) 
{
	
}
ServiceException::~ServiceException()
{
	
}
const string 
ServiceException::getMessage()
{
	return data;
}

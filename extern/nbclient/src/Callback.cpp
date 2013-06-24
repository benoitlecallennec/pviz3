/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif

#include "Callback.h"
#include <iostream>
Callback::Callback()
{
}

Callback::~Callback()
{
}
void
Callback::onEvent(NBEvent *nbEvent){
	//Override
};

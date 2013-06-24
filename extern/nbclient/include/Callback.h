/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef CALLBACK_H
#define CALLBACK_H
#ifdef WIN32
#include "nbcppdll.h" 
#endif
#include "NBEvent.h"


#ifdef WIN32
class NBCPPDLL_API Callback
#else
class  Callback
#endif
{
	public:
		Callback();
		virtual ~Callback();
		void virtual onEvent(NBEvent *nbEvent);
		
};
#endif /* CALLBACK_H */

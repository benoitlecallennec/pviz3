/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef CLARENSCALLBACK_H_
#define CLARENSCALLBACK_H_

#include <iostream>
#include <string>
#ifdef WIN32
#include "nbcppdll.h" 
#endif

#include <pthread.h>
#include "Callback.h"
#include "TCallback.h"

#ifdef WIN32
class NBCPPDLL_API ROOTCallback : public Callback
#else
class ROOTCallback : public Callback
#endif
{
	private:
		TCallback *tCallback;
		pthread_mutex_t t_mutex;
	public: 
		ROOTCallback(TCallback *tCallback);
		virtual ~ROOTCallback();
		void onEvent(NBEvent *nbEvent);
	
};
#endif /*CLARENSCALLBACK_H_*/

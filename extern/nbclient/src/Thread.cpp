/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#else
#include <pthread.h>
#endif
#include <iostream>
#include "Thread.h"

using namespace std;

Thread::Thread() {}
Thread::~Thread() {}

int Thread::start(void * arg)
{
   setRunnable(arg); // store user data
  int code= pthread_create( &thread, NULL, Thread::entryPoint,(void *)getRunnable());
   //int code = thread_create(Thread::EntryPoint, this, & ThreadId_);
   return code;
}

void Thread::run(void * arg)
{
	 Thread * pt = (Thread*)arg;
     pt->setup();
     pt->execute( arg );
}

/*static */
void * Thread::entryPoint(void * pthis)
{
   Thread * pt = (Thread*)pthis;
   pt->run( pt->getRunnable());  
   return NULL;
}

void Thread::setup()
{
     //override, but call this if override
}


void Thread::execute(void* arg)
{	
	//override
}

void Thread::join()
{
	#ifdef VERBOSE
	cout<<"Coming to JOIN "<<endl;
	#endif
	pthread_join( thread, NULL);
}

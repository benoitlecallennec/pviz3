/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef THREAD_H
#define THREAD_H
 
#include <pthread.h>

class Thread
{
   public:
      Thread();
      virtual ~Thread();
      int start(void * arg);
      void join();
   protected:
      void run(void * arg);
      static void * entryPoint(void*);
      virtual void setup();
      virtual void execute(void*);
      void * getRunnable() const {return runnable_;}
      void setRunnable(void* a){runnable_ = a;}
	  
	  
   private:
     pthread_t thread;
      void * runnable_;

};

#endif /*THREAD_H*/

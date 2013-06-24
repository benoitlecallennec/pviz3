/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
 #ifndef RECEIVER_THREAD_H
#define RECEIVER_THREAD_H
 
 #include <map>	
 #include "Connection.h"
 #include "Thread.h"
 #include "Callback.h"

 
class ReceiverThread : public Thread
{
			public:
				ReceiverThread();
			  virtual ~ReceiverThread();
				bool init(Connection *conn);
			    void addCallback(string topic,Callback *callback);
                virtual void execute(void*);
	    	   void setStop();
			
			private :
			 std::map<string,Callback * > callbacks;           
			Connection  *_conn;
	        Callback *callback;
			int numSubscriptions;
			bool stop;
			bool initialized;
};

#endif /*RECEIVER_THREAD_H*/

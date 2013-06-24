/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include <iostream>
#include "ReceiverThread.h"
#include "ServiceException.h"

using namespace std;

ReceiverThread::ReceiverThread()
 :initialized(false)
{
};

ReceiverThread::~ReceiverThread(){};

bool 
ReceiverThread::init(Connection *conn)
{
	  if(this->initialized){
		 #if defined(VERBOSE)
		  	cout<<"ReceiverThread is already initialized "<<endl;
	     #endif		  
		 return false	;  
	  }else{
		 this->_conn=conn;
	     this->numSubscriptions=0;
	     this->stop=false;
		  return true;
	  }

};

void 
ReceiverThread::addCallback(string topic,Callback *callback){
	 this->callbacks.insert(make_pair(topic,callback));
}
	
void ReceiverThread::execute(void* arg)
{
	while(!stop){
		#if defined(VERBOSE)
		    cout<<"ReceiverThread is reading"<<endl;
		#endif
		
		NBEvent event;
		int errorCode=0;		
		try{
			if(_conn->readEvent(&event,&errorCode)){
				 
				 std::map<string,Callback * >::iterator iter=callbacks.find(event.getContentSynopsis());
				 if(iter != callbacks.end()){
					 Callback  *callback=iter->second;								 
					 callback->onEvent(&event);
				 }      
	     	}else{
				#if defined(VERBOSE)
				     cout<<"Recevied a control event"<<endl;	
				#endif
			}
		}catch(ServiceException& e){
			if(stop!=true){
				cerr<<"Receiver Thread is Exiting due to "+e.getMessage()<<endl;
			}
			break;
		}
	}
};


void 
ReceiverThread::setStop()
{
	this->stop=true;
	
}

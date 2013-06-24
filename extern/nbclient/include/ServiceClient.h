/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef SERVICE_CLIENT_H
#define SERVICE_CLIENT_H

#ifdef WIN32
#include "nbcppdll.h" 
#endif
#include "Callback.h"
#include "ReceiverThread.h"
#include "Connection.h"
#include "Profile.h"
#include "NBEventGenerator.h"
#include <string>


#ifdef WIN32
class NBCPPDLL_API ServiceClient
#else
class  ServiceClient
#endif
{
	public:
		ServiceClient();
		~ServiceClient();
	
		void init(string host,short port,int entityId,int templateId);
	    void subscribe(string topic,Callback *callback);
	    void unsubscribe(string topic);
		void publish(string topic,char* bytes,int length);		
	    void cleanup();
	    
	    /*Stream support*/
	    void publishStream(string topic, string pipename);	
	
	protected:		
		 void sendInitPubMessage(int entityId);
         void sendProfileRequestMessage(Profile *profile);
         void getProfileRequestMessage(Profile *profile,bool addProfile,char *bytes, int size,int * bytesWritten);	
		
	private:   	 
	 	 bool initialized;
	     bool receiverInitialized;
	
		 int _entityId;
		 int _templateId;	
	
	     const char * host;
	     short port;
	     
	    NBEventGenerator *eventGen;	    
		Connection *conn;
	    ReceiverThread *receiver;
	
		//Profile profile;
	    std::map<string,Profile * > profiles;
  
};

#endif /*SERVICE_CLIENT_H*/

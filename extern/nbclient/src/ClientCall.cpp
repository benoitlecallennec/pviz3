/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#include "ClientCall.h"
#include "ServiceException.h"
#include "Utils.h"

#include <iostream>
using namespace std;

/*
 * Populates the arrary of char pointers using the
 * response messages. Caller should deserialize the message
 * according to its type.
 * */

class ClientCallCallback : public Callback
{
	private:
		char **res;
		int *resLen;
		bool complete;
	
	public:
		
	ClientCallCallback(char** res,int *resLen):res(res),resLen(resLen),complete(false){}
		
	void onEvent(NBEvent *nbEvent){
					
		//cout<<endl<<"Event Rececived with for the topic - "<<nbEvent->getContentSynopsis()<<endl;
		/*
		 * We create a new char array in the heap for the payload. 
		 * So we can pass this pointer to the client. It should be deleted at the caller.
		 * */
		(*res)=nbEvent->getContentPayload();
		(*resLen)=nbEvent->getPayloadLength();
		complete=true;
		
		//string str(nbEvent->getContentPayload(),payloadSize);
		//cout<<str<<endl;		
	}
	
	bool isComplete(){
		return complete;
	}
};


ClientCall::ClientCall(){}
ClientCall::~ClientCall(){}

/*
 * Initialize the ServiceClient used for the call.
 * */
void 
ClientCall::init(string host,short port,int entityId,int templateId)
{	
	try{
		this->serviceClient.init(host,port,entityId,templateId);
	}catch(ServiceException& e){
		throw e;
	}
}

/*
 * Blocking invocation based on the underlying publish/subscribe API.
 * This will wait for maximum of CLIENT_CALL_MAX_WAIT to receive a 
 * response or responses specified by noOfResponse
 * Number of received responses is passed back using numReturn value.
 * 
 * */
bool
ClientCall::sendReceive(string pubTopic, string subTopic, char* data,int size_of_data,char** res,int *resLen)
{	
	ClientCallCallback ccCallback(res,resLen);
	this->serviceClient.subscribe(subTopic,&ccCallback);	
	
    this->serviceClient.publish(pubTopic,data,size_of_data); 	
	
	long long start=Utils::getCurrentTimeMicroseconds();
	while(!ccCallback.isComplete())
	{
		
		long long end=Utils::getCurrentTimeMicroseconds();
		if((end-start)>CLIENT_CALL_MAX_WAIT)
		{	
			break;
		}				
		usleep(CLIENT_CALL_SLEEP);
	}
	

	/*
	 * Should unsubscribe from the topics. 
	 * */
	this->serviceClient.unsubscribe(subTopic);	
	
	return false;
}

void 
ClientCall::send(string pubTopic, char* data)
{
	
}

void ClientCall::close() 
{
	
}

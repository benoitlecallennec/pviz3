/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TSERVICECLIENT_H_
#define TSERVICECLIENT_H_
#include <iostream>
#include <string>

#include "TObject.h"
#include "TCallback.h"


using namespace std;

class  TServiceClient : public TObject{

protected:
	void *client;
	
public:
	TServiceClient();
	virtual ~TServiceClient();
	
	void Init(string host,short port,int entityId,int templateId);
	void Subscribe(string topic,TCallback *callback);
	void Unsubscribe(string topic);
	void Publish(string topic,char* bytes,int length);		
	void Cleanup();
	
	ClassDef(TServiceClient,1)

};
#endif /*TSERVICECLIENT_H_*/

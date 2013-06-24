#ifndef TCALL_H_
#define TCALL_H_

#include <iostream>
#include <string>

#include "TObject.h"

using namespace std;

class TCall :public TObject
{

protected:
	void *client;

public:
	TCall();
	virtual ~TCall();
	
	void Init(string host,short port,int entityId,int templateId);
	void Cleanup();
	
	bool SendReceive(string pubTopic,string subTopic, char* data,int size_of_data, char** res, int *resLen);
	
	ClassDef(TCall,1)

};

#endif /*TCALL_H_*/

/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef CLIENTCALL_H_
#define CLIENTCALL_H_

#include <map>	
#include <string>
#include <iostream>
#include "ServiceClient.h"

using namespace std;

class ClientCall
{
	public:
		ClientCall();
		~ClientCall();
		
		void init(string host,short port,int entityId,int templateId);
		//bool sendReceive(string pubTopic, string subTopic, char* data,int size_of_data,int noOfResponse,char** responses,int *numReturn);
		bool sendReceive(string pubTopic, string subTopic, char* data,int size_of_data,char** response,int *resLen);
		void send(string pubTopic, char* data);
		void close() ;
	
	private:
		ServiceClient serviceClient;
		//ServiceClient subServiceClient;
		

};


#endif /*CLIENTCALL_H_*/

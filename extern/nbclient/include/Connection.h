/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef CONNECTION_H
#define CONNECTION_H

#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#else
#include <Windows.h>
#include <stdlib.h>
#endif

#include "NBEvent.h"


class Connection
{
	public:
		Connection();
		~Connection();
		
		void init(const char* hostName,short port,char socketType);
		bool sendData(const char* data,int length);
		bool closeConnection();
	
	    bool readEvent(NBEvent *event,int *errorCode);
	
	private:
		int socketDescriptor;
		unsigned short int serverPort;
 		struct sockaddr_in serverAddress;
 		struct hostent *hostInfo;

};

#endif /*CONNECTION_H*/

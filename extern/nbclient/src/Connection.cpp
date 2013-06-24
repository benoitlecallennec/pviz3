/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Connection.h"


#include "NBEntity.h"
#include "Utils.h"
#include "constants.h"
#include "NBEventGenerator.h"
#include "NBEventIDGenerator.h"
#include "ServiceException.h"

#ifdef WIN32
#include <io.h>
#endif

using namespace std;

Connection::Connection()
{	
}

Connection::~Connection()
{
}


void
Connection::init(const char* hostName,short port,char socketType)
{
	#ifdef WIN32
	WSADATA wsaData;
    int iResult;
	 // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
		throw ServiceException("WSAStartup failed");	
    }
	#endif

	 //Get the host info
	 hostInfo = gethostbyname(hostName);	 
	 if (hostInfo == NULL)	 {
	 	 string strHostName=hostName;
    	 throw ServiceException("problem interpreting host: " + strHostName);    	
	 }
	 
	 /*Set the port*/
	 serverPort=port;
	 
	/*Currently ceating a TCP Socket.*/
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0) {
		throw ServiceException("Could not create a socket");		
	}
	
	// Connect to server.  First we have to set some fields in the
    // serverAddress structure.  The system will assign me an arbitrary
    // local port that is not in use.
    serverAddress.sin_family = hostInfo->h_addrtype;
    memcpy((char *) &serverAddress.sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
    serverAddress.sin_port = htons(serverPort);
				
    if (connect(socketDescriptor,(struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    	throw ServiceException("Could not connect to the socket");	
  	} 

}
/*
 * Send length amount of bytes using the openned connection.
 * */
bool 
Connection::sendData(const char* data,int length)
{	
	//5=one byte +one integer i.e. the initial byte and the length of the data
	//JJB
	//char buff[length+INT_LENGTH+1];
	char *buff = (char *) malloc(length+INT_LENGTH+1);
	long long offset=0;
	//Utils::writeByte(buff+offset,DATA,offset);

	//set the length of this message.
    Utils::writeInt(buff+offset,length,offset);
	Utils::writeBytes(buff+offset,data,length,offset);
	
	//int ret=write(socketDescriptor,buff,offset);
	int ret=send(socketDescriptor,buff,offset,0);
	if (ret < 0){
	 	throw ServiceException("Error writing to socket.");
	}	
	return 0;
};


/*
 * Close the existing connection
 * */
bool 
Connection::closeConnection()
{	
	shutdown(socketDescriptor, 0);
	#ifdef WIN32
		closesocket(socketDescriptor);	
	#else
		close(socketDescriptor);
	#endif
	return true;
}

/*
 * Read an NBEvent from the openned socket. Here we don't have a fixed
 * length of events to read, so the reading is a incremental reading
 * process byte by byte.
 * 
 * */
bool 
Connection::readEvent(NBEvent *event,int *errorCode)
{ 
	int bytesThisTime=0;	
	
	/*Always create a buffer larger than one for last null terminator.*/
	/*
	char b[2];
	b[0]=0;
	while(bytesThisTime<1){
		bytesThisTime=recv(socketDescriptor,b+bytesThisTime,1,0); 
		if(bytesThisTime==-1)		{
			throw ServiceException("Socket Read Error");
		}else if(bytesThisTime==0){
			throw ServiceException("Socket Read Error EOF");
		}
   }
	
	
	if(b[0]!=DATA){
		#if defined(VERBOSE)
				cout<<"Recived a non compliant message. Ignore further processing. ";				
		#endif
		return 0;
	}else{	
		
	*/	
		/*
		 * This needs to be tuned up for different endianness
		 * 4 byts for integer is only for 32 bit machines.
		 * 
		 * 
		 * */	
		char buf[INT_LENGTH+1];
		memset(buf, 0x0, INT_LENGTH+1);
		//for(int i=0;i<INT_LENGTH;i++){buf[i]=0;}
	
		bytesThisTime=0;
		int totalSoFar=0;
		
		while(totalSoFar<INT_LENGTH){			
			bytesThisTime=0;
			bytesThisTime=recv(socketDescriptor,buf+totalSoFar,(INT_LENGTH-totalSoFar),0); 
			if(bytesThisTime==-1){
				throw ServiceException("Socket Read Error");
			}else if(bytesThisTime==0){
				throw ServiceException("Socket Read Error EOF");
			}
				totalSoFar+=bytesThisTime;
		}
			
	    /*Size of the message */	
		int res=Utils::readInt(buf);
		//char msg[res];
		//JJB
		//char *msg = (char *) malloc(res);
		char *msg=new char[res];
		//char msg[res+1];
		memset(msg, 0x0, res);
			
		bytesThisTime=0;
		totalSoFar=0;
		/*Now read the message from the socket*/
		while(totalSoFar<res){
			bytesThisTime=0;
			//bytesThisTime=recv(socketDescriptor+totalSoFar,msg-totalSoFar,res,0); 				
			bytesThisTime=recv(socketDescriptor,msg+totalSoFar,res-totalSoFar,0);
			
			if(bytesThisTime==-1){
				throw ServiceException("Socket Read Error");
			}else if(bytesThisTime==0){
				throw ServiceException("Socket Read Error EOF");
			}
			
			totalSoFar+=bytesThisTime;
		}
			
		#if defined(VERBOSE)
			cout<<"The bytes read >>>>>>>>>>>>>>>>>>>>>>>"<<endl;
		    for(int i=0;i<totalSoFar;i++){
				printf("%d ",msg[i]);
			}
		  	cout<<endl<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
		#endif
			
		/*Create the event */	
				
		if(event->fromBytes(msg)){
		   return true;
		}else{
			return false;
		}				
    //}	
}


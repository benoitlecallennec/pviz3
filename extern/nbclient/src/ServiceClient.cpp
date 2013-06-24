/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#else
// Added for unix pipes
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>

#include <unistd.h>

#endif
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <assert.h>

#include "ServiceClient.h"
#include "ServiceException.h"

using namespace std;

#define INIT_BUFF_SIZE 8
//const int HEADER_BYTES_MAX=512;

ServiceClient::ServiceClient() :initialized(false) ,  receiverInitialized(false),eventGen(new NBEventGenerator()),conn(new Connection()),receiver(new ReceiverThread())

{}
ServiceClient::~ServiceClient(){
  delete eventGen;
  delete receiver;
  delete conn;
  
  map<string,Profile *>::iterator iter;
  for( iter = profiles.begin(); iter != profiles.end(); ++iter ) {
    delete iter->second;
  }
}

/*
 * Initialize the service client. This will establish a 
 * socket connection with the borker and establish a 
 * connection by sending initial publish message.
 * */
void
ServiceClient::init(string host,short port,int entityId,int templateId)
{

	//Not synchronized. TODO synchronize this.
	if(!this->initialized){
		this->_entityId=entityId;
		this->_templateId=templateId;
		
		try{
			this->conn->init(host.c_str(),port,'a');
		}catch(ServiceException& e){
			throw ServiceException("Error initializing the connection "+e.getMessage());
		}
		
		this->host=host.c_str();
		this->port=port;
		
		try{
			sendInitPubMessage(entityId);
		}catch(ServiceException& e){
			throw ServiceException("Error establishing the connection "+e.getMessage());		
		}
		/*
		cout<<"Info:ServiceClient Initialized with entityId "<<entityId<<"host:port "<<host<<":"<<port<<endl;	
		cout<<"Info:Initializing the publisher "<<entityId<<endl;
		*/
		this->initialized=true;
		
	}
	else{
		throw  ServiceException("ServiceClient is already initilized");
	}		
	
}

/*
 * Close the connection and the ReceiverThread. 
 * */
void 
ServiceClient::cleanup()
{
	this->receiver->setStop();
	this->conn->closeConnection();
}


/*
 * Subscribe to a given topic. Use the callback object to 
 * hand over the receiving events.
 * 
 * */
void
ServiceClient::subscribe(string topic,Callback *callback)
{
	
	#if defined(VERBOSE)
		cout<<"Subscribing to this topic "<<topic<<endl;
	#endif

	//INT2STR
    stringstream ss;
	ss<<topic;
	ss<<"<>";
	ss<< this->_entityId;
	
	//string profileId= topic + "<>" + ss.str();
	string profileId= ss.str();
	
	Profile *profile=new Profile(topic,this->_entityId,profileId);
		
	this->profiles.insert( make_pair(profileId,profile));
	
	//start the listening thread if not already started.
	if(!receiverInitialized){
		this->receiver->init(this->conn);
		this->receiver->start(this->receiver);
		receiverInitialized=true;
	}
	this->receiver->addCallback(topic,callback);
	
	//Now we need to register this profile with the broker./
	try{
		sendProfileRequestMessage(profile);	
	}catch(ServiceException &e){
		throw ServiceException("Error subscirbing to topic " + topic+" due to "+e.getMessage());
	}

	cout<<"Info:Subscribed to the topic "<<topic<<endl;
		
}

/**
 * Unsubscribe from a given topic.
 */
void
ServiceClient::unsubscribe(string topic){
	/*INT2STR*/
    stringstream ss;
    ss << this->_entityId;
	///////////////	
	string profileId= topic + "<>" + ss.str();
	
	map<string,Profile *>::iterator iter=this->profiles.find(profileId);
	if( iter != this->profiles.end() ) {    	
    	Profile *profile=iter->second;
    	int est_prof_req_size=256;
		//char *bytes = (char *) malloc(est_prof_req_size);
		//char bytes[est_prof_req_size];
		char *bytes = new char[est_prof_req_size];
	
		int bytesWritten=0;
	
		getProfileRequestMessage(profile,false,bytes,est_prof_req_size,&bytesWritten);
		try{
			this->conn->sendData(bytes,bytesWritten);
		}catch(ServiceException& e){		
			throw ServiceException("Error Unsubscribing due to "+e.getMessage());
			free(bytes);
		} 
			
		/*remove the entry in the profiles as well.*/
		this->profiles.erase(iter);  
		delete []bytes;
    	
 	}else{
 		throw  ServiceException("No such profile to unsubscribe");
 	}
	
}

/*
 * Publish an event using the bytes passed to the given topic.
 * */
void
ServiceClient::publish(string  topic,char* bytes,int length)
{
	NBEvent event;
	event.init();
	
	try{
    this->eventGen->generateEvent(&event,_templateId,_entityId,true,true,0,false,NULL,false,0,STRING_CONTENT_SYNOPSIS,topic.c_str(),bytes,length);
	}catch(ServiceException& ex){
		throw ServiceException("Error in publishing the event dur to "+ex.getMessage());
	}
	
	//512 bytes for the header.
	//JJB
	//char *event_buff = (char *) malloc(HEADER_BYTES_MAX+length);
	char *event_buff = new char[HEADER_BYTES_MAX+length];
	//char event_buff[HEADER_BYTES_MAX+length];	
	int actualLength=0;
	event.getBytes(event_buff,&actualLength);	
	
	//Sanity Check
	assert(actualLength<=(HEADER_BYTES_MAX+length));

	try{
		this->conn->sendData(event_buff,actualLength);
		//free(event_buff);
		delete [] event_buff;
	}catch(ServiceException& e){
		string strTopic=topic;
		throw ServiceException("Error in publishing data to the topic " +strTopic+" due to "+e.getMessage());
	}
	
}

#ifndef WIN32
void 
ServiceClient::publishStream(string topic, string pipename){
	
//  int pipefd=mkfifo(pipename.c_str(),S_IRUSR|S_IWUSR);
//  if (pipefd==-1){
//    string err_msg("Error creating FIFO ");
//    err_msg+=pipename;
//    switch (errno){
//      case (EEXIST): err_msg+="File exists";
//        break;
//      case (EACCES): err_msg+="Access denied";
//        break;
//      case (ENAMETOOLONG): err_msg+="Name too long";
//        break;
//      case (ENOENT): err_msg+="Directory component doesn't exist, or dangling symlink";
//        break;
//      case (ENOSPC): err_msg+="No space left on file system";
//        break;
//      case (ENOTDIR): err_msg+="Part of pathname is not a directory";
//        break;
//      case (EROFS): err_msg+="Read-only file system";
//        break;
//      default: break;
//    }
//    if (errno!=EEXIST){
//      cerr<<err_msg<<", exiting"<<endl;    
//      exit(errno);
//    }
//    cerr<<err_msg<<endl;      
//  }
//  else close(pipefd);
	
	
	
	FILE *pipestream;
	string str="";
	char *msg;
	 cout<<"Coming to the method in ServiceClient Before open"<<endl;
	int pipefd=open(pipename.c_str(),O_RDONLY);
	cout<<"Coming to the method in ServiceClient After Open"<<endl;
	
	
    if (pipefd==-1) {
      cerr<<"Opening file "<<pipename<<" failed!"<<endl;
      exit(0);
    }
    else cerr<<"Opening file "<<pipename<<" succeeded.."<<endl;

    pipestream=fdopen(pipefd,"r");
    cerr<<pipename<<" opened successfully: fd="<<pipefd<<endl;

	
	while (str!=".") {
        cout<<"Waiting for input.."<<endl;

        size_t n;
        ssize_t bytes_read;
        msg=NULL;
        bytes_read=getline(&msg, &n, pipestream);
        //cout<<"Bytes read this time = "<<bytes_read<<endl;

        if (bytes_read==-1){
          cerr<<"Error reading from pipe "<<pipename<<". Clearing..."<<endl;
          fclose(pipestream);
          close(pipefd);
          break;
        }

        //if (bytes_read>=1 & msg[0]!='.') {
        if (bytes_read>=1) {
            publish(topic,msg,bytes_read-1);
            msg[bytes_read-1]='\0';
            //cout<<"published message: '"<<msg<<"'"<<endl;
            if (msg) delete[] msg;//free(msg); 
          }
        else {
          if (msg) delete [] msg;//free(msg);
          //goto close_tag;
        }
    }

}
#endif


void 
ServiceClient::sendInitPubMessage(int entityId)
{
	//Now initialize the publisher as well.
	//char *buff = (char *) malloc(INIT_BUFF_SIZE);
	
	char *buff = new char[INIT_BUFF_SIZE];

	//char buff[INIT_BUFF_SIZE];
	long long offset=0;
	
	Utils::writeByte(buff+offset,DATA,offset);
	Utils::writeByte(buff+offset,NODE_ADDRESS,offset);
	Utils::writeByte(buff+offset,CLIENT_SETUP,offset);
	Utils::writeInt(buff+offset,entityId,offset);
	

	//The initilization message's length is 12 bytes.
	try{
		this->conn->sendData(buff,offset);
		delete[] buff;
	}catch(ServiceException& e){
		delete[] buff;
		throw ServiceException("Error in sending data due to "+e.getMessage());
	}
	
}



void 
ServiceClient::sendProfileRequestMessage(Profile *profile){
	int est_prof_req_size=256;
	char *bytes = new char[est_prof_req_size];

	int bytesWritten=0;
	
	getProfileRequestMessage(profile,true,bytes,est_prof_req_size,&bytesWritten);
	try{
		this->conn->sendData(bytes,bytesWritten);
		free(bytes);
	}catch(ServiceException& e){
		throw ServiceException("Error in sending data due to "+e.getMessage());
	}	
}


 void 
ServiceClient::getProfileRequestMessage(Profile *profile,bool addProfile,char *bytes, int size,int * bytesWritten)
{
	
	long long offset=0;
	
	Utils::writeByte(bytes+offset,DATA,offset);
	
	Utils::writeByte(bytes+offset,PROFILE_PROPAGATION,offset);

    /** Marshall the profile bytes next */
	Utils::writeInt(bytes+offset,profile->getProfileType(),offset);
 
	char profBytes[256]; 
    int profLen=0;
    profile->getBytes(profBytes,&profLen);	
	
	Utils::writeInt(bytes+offset,profLen,offset);
	
	Utils::writeBytes(bytes+offset,profBytes,profLen,offset);

    /** Marshall the addProfile next */
    Utils::writeBool(bytes+offset,addProfile,offset);
 	 
    /** Marshall whether it is from client or not */
    Utils::writeBool(bytes+offset,true,offset);
	
    /*isFromClient - Not expecting this for the client*/
 
    /*Security is not suppored yet*/
	Utils::writeBool(bytes+offset,false,offset);
     
    /*Sanity check*/
    assert(offset<=size);
    
    (*bytesWritten)=offset; 	
}


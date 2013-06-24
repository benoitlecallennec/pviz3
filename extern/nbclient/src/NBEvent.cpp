/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include <string>
#include <sstream>
#include <iostream>
#include "NBEvent.h"
#include "constants.h"
#include "NBEventHeaders.h"
#include "Utils.h"

using namespace std;

NBEvent::NBEvent():distributionTrace(new DistributionTrace())
{

}

NBEvent::~NBEvent()
{
	delete this->distributionTrace;
}

void
NBEvent::init()
{
	/*This is what we support currently*/
	contentSynopsisType=STRING_CONTENT_SYNOPSIS;
	containsDistributionTraces=false;
	containsEventProperties=false;
	containsContentType=false;
	eventType=0;
	
	header.init();	
}

bool 
NBEvent::fromBytes(char *bytes)
{
	long long offset=0;
	
	if(DATA!=Utils::readByte(bytes+offset,offset)) {return 0;} ;
	if(NB_EVENT!=Utils::readByte(bytes+offset,offset)) {return 0;} ;
	//Just read this, this is the event type.
    Utils::readByte(bytes+offset,offset); 
		 
	this->containsContentType=Utils::readBool(bytes+offset,offset);
    #if defined(VERBOSE)
		cout<<"Contains con  containsContentType ="<<this->containsContentType<<endl;
	 #endif
	if (this->containsContentType) {
         //   int _contentTypeLength = din.readShort();
         //   byte[] _contentTypeBytes = new byte[_contentTypeLength];
         //   din.readFully(_contentTypeBytes);
         //   contentType = new String(_contentTypeBytes);
    }
	
	 /** Deal with the event headers */
	this->containsEventHeaders	= Utils::readBool(bytes+offset,offset);
	#if defined(VERBOSE)
		cout<<"Contains con  containsEventHeaders ="<<this->containsEventHeaders<<endl;
    #endif 
    if (this->containsEventHeaders) {
           int headerLength = Utils::readInt(bytes+offset,offset);
		  //SKIP THE HEADER FOR NOW
		  offset+=  headerLength;
	}

    bool hasPropertiesToRead =  Utils::readBool(bytes+offset,offset);
	#if defined(VERBOSE)
		cout<<"Contains   hasPropertiesToRead ="<<hasPropertiesToRead<<endl;
	#endif
    if (hasPropertiesToRead) {
       	//Nothing       
    }
		 
	this->contentSynopsisType=  Utils::readByte(bytes+offset,offset);
	#if defined(VERBOSE)
    	cout<<"Contains   contentSynopsisType ="<< this->contentSynopsisType<<endl;
	#endif
		 
	int synopsisBytes = Utils::readInt(bytes+offset,offset );
	//char temp[synopsisBytes];
	//JJB
	//char *temp = (char *) malloc(synopsisBytes);
	char *temp=new char[synopsisBytes];
	Utils::copyBytes((bytes+offset),temp,synopsisBytes,offset);
	this->contentSynopsis.assign(temp,synopsisBytes);
	
	//Just read this but does not use it.			 
	Utils::readInt(bytes+offset,offset);
	
    int payloadSize = Utils::readInt(bytes+offset,offset);
	this->payloadLength=payloadSize;
		 
	if (payloadSize != 0) {
		this->contentPayload  =new char[payloadSize];
		Utils::copyBytes(bytes+offset,this->contentPayload,payloadSize,offset);		
    }
    
    //We are not processing the distribution traces yet.
    
	//free(temp);
	delete [] temp;
    return true;			
}

/** Indicates the type of the event */
int
NBEvent::getEventType()
{
	return eventType;
}

void
NBEvent::setEventType(int eventType) 
{
	this->eventType=eventType;
}


/** Indicates the type of the content */
char*
NBEvent::getContentType()
{
	return contentType;
}

/** Indicates if this event has any event headers */
bool
NBEvent::hasEventHeaders()
{
	return containsEventHeaders;
}

void 
NBEvent::setEventHeaders(NBEventHeaders *headers)
{
	this->header=(*headers);
	this->containsEventHeaders=true;
}

/** Header information pertaining to the event */
/* EventHeaders are not yet supported */ 
//EventHeaders getEventHeaders();
  
  
/** Indicates if this event has any user-defined properties */
bool
NBEvent::hasEventProperties()
{
	return containsEventProperties;
}

/** EventProperties associated with the event */
/*Not supported yet */
//public EventProperties getEventProperties();

/** Returns the type of the content synopsis */
int
NBEvent::getContentSynopsisType()
{
	return contentSynopsisType;
}

void
NBEvent::setContentSynopsisInfo(int type,string synopsis)
{
	this->contentSynopsisType=type;
	this->contentSynopsis=synopsis;
}

/** Retrieves the synopis associated with the content */
/*Only string content synopsises are supported */
string
NBEvent::getContentSynopsis()
{
	return contentSynopsis;
}
  
/** Get the payload size */
int
NBEvent::getContentPayloadSize()
{
	return payloadLength;
}

/** Get the content Payload */
char*
NBEvent::getContentPayload()
{
	return contentPayload;
}


void
NBEvent::setContentPayload(char *payload,int payloadLength)
{
	if(NULL==payload)
	{
		return;		
	}
	else
	{
		this->contentPayload=payload;	
		this->payloadLength=payloadLength;
	}
}

int 
NBEvent::getPayloadLength()
{	  
	return this->payloadLength;  
}


/** Indicates if the event has distribution traces */
bool
NBEvent::hasDistributionTraces()
{
	return containsDistributionTraces;
}
  
  NBEventID *
  NBEvent::getEventID()
{	
	return &id;
}
  
  NBEventHeaders * 
  NBEvent::getEventHeaders()
{
	return &header;
}


/** Gets the distribution Traces associated with the event */
/*Currently no destribution traces */

/** Get a byte stream representation of the event */
void 
NBEvent::getBytes(char *buff,int *length){
	
   long long offset = 0;
	
	Utils::writeByte(buff+offset,(char)DATA,offset);
 	Utils::writeByte(buff+offset,(char)NB_EVENT,offset);
	Utils::writeByte(buff+offset,(char)eventType,offset);
		
    Utils::writeBool(buff+offset,(char)containsContentType,offset);
   
	if(containsContentType){
	  		//Not supported yet.
	}

   /** Deal with the event headers */
	   
	Utils::writeBool(buff+offset,containsEventHeaders,offset);
        
    if (containsEventHeaders) {
		int len=0;
		char temp[512]; /*Header is 512 bytes if more we need to increase it ;) */
		header.getBytes(temp,&len);
		Utils::writeInt(buff+offset,len,offset);
    	Utils::writeBytes(buff+offset,temp,len,offset);				 
	}
		 
	bool willWriteProperties=false;
	Utils::writeBool(buff+offset,willWriteProperties,offset);		 
	Utils::writeByte(buff+offset,(char)contentSynopsisType,offset);
	Utils::writeInt(buff+offset,contentSynopsis.length(),offset);
	Utils::writeBytes(buff+offset,contentSynopsis.c_str(),contentSynopsis.length(),offset);
	int _payloadOffset = offset + 2*INT_LENGTH;//This is used in the java implementation.
	Utils::writeInt(buff+offset,_payloadOffset,offset);
	Utils::writeInt(buff+offset,payloadLength,offset);
	Utils::writeBytes(buff+offset,contentPayload,payloadLength,offset);
	bool  containsDistributionTraces=false;//We only send a place holder.
	Utils::writeBool(buff+offset,containsDistributionTraces,offset);
	
	int dLen=0;
	char dBuff[512];
	this->distributionTrace->getBytes(dBuff,&dLen);
	Utils::writeShort(buff+offset,dLen,offset);
    Utils::writeBytes(buff+offset,dBuff,dLen,offset);	
		
	(*length)=offset;
	
}

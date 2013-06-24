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
#include "constants.h"
#include "NBEventHeaders.h"
#include "ServiceException.h"
#include "Utils.h"

using namespace std;

NBEventHeaders::NBEventHeaders()
{
}
NBEventHeaders::~NBEventHeaders()
{
}

void 
NBEventHeaders::init()
{
 			containsTemplateId =false;
		    containsTimestamp=false;
		    containsSource =false;
		    supressDistributionToSource=false;
		    containsEventId=false;
		    correlated=false;
            containsTimeToLive=false;
		    secure=false;
		    containsIntegrity=false;
		    fragmented=false;
		    sliced=false;
		    containsPriorityInfo=false;
		    containsApplicationInfo=false;
		    persistent=false;
		    transient=false;
		    compressed=false;
		    containsAssertions=false;

			templateId=0 ;
			timestamp=0;
			source=0;
			timeToLive=0;
			priority=0;
			snapshot=0;
			numOfHeaders=0;
}


NBEventHeaders::NBEventHeaders(char *)
{
}
/** ********************************************************************** */
/** Implementation of methods in the Event Headers interface */
/** ********************************************************************** */

/** Indicates the templateId which this event conforms to */
int
NBEventHeaders::getTemplateId() 
{
	return this->templateId;
}

void 
NBEventHeaders::setTemplateId(int templateId) 
{
	this->containsTemplateId=true;
	this->templateId=templateId;
}

/**
* Returns the timestamp for this event. This timestamp corresponds to the
* value assigned by the time service
*/
long long
NBEventHeaders::getTimeStamp() 
{
	return this->timestamp;
}

void 
NBEventHeaders::setTimeStamp(long long timestamp)
{
	this->timestamp=timestamp;
	this->containsTimestamp=true;
}
 
 /** Returns the generator of this message */
int 
NBEventHeaders::getSource() 
{
	return this->source;
}

/**
 * Indicates that the message should not be rerouted to it source, that is
 * contained in the event headers
 */
bool 
NBEventHeaders::setSupressDistributionToSource() 
{
	return this->supressDistributionToSource;
}

void
NBEventHeaders::setSourceInformation(int source,bool supressDistributionToSource)
{
	this->source=source;
	this->containsSource=true;
	this->supressDistributionToSource=supressDistributionToSource;
}

/** Indicates if there is an event id associated with this message */
bool 
NBEventHeaders::hasEventId() 
{
	return this->containsEventId;
}
    
   /** Get the event identifier */
NBEventID* 
NBEventHeaders::getEventId() 
 {
	 return this->eventId;
 }
   
 void
 NBEventHeaders::setEventId(NBEventID * eventId)
 {
	 this->eventId=eventId;
	 this->containsEventId=true;
 }
   /**
    * Indicates if this event is correlated with any other event. Casual
    * constraints may entail that the correlated event be delivered prior to the
    * delivery of this event.
    */
bool 
NBEventHeaders::isCorrelated() 
 {
	 return this->correlated;
 }

 /** Returns the correlation identifier associated with the event */
 NBEventID* 
 NBEventHeaders::getCorrelationIdentifier() 
 {
	 return this->correlationId;
 }
 
void
 NBEventHeaders::setCorrelationId(NBEventID *correlationId)
 {
	  this->correlationId=correlationId;
	 this->correlated=true;
 }
 
 bool 
 NBEventHeaders::hasTimeToLive(){
	 return this->containsTimeToLive;
 }

 /** Returns the time to live */
 int 
NBEventHeaders::getTimeToLive() 
 {
	 return this->timeToLive;
 }
 
void 
 NBEventHeaders::setTimeToLive(int timeToLive)
 {
	 this->timeToLive=timeToLive;
	 this->containsTimeToLive=true;
 }

 /** Indicates if this event is a secure event */
 bool 
NBEventHeaders::isSecure() 
 {
	 return this->secure;
 }

/** Indicates if the event has integerity check information. */
bool 
NBEventHeaders::hasIntegrity()
{
	return this->containsIntegrity;
}

/** Indicates if this event is a fragment of a larger event */
bool 
NBEventHeaders::isFragmented()
{
	return this->fragmented;
}

 /*
 * Indicates if this is an event slice which contains a part of the orginal
 * transmitted payload. These are specifically used in cases where the device
 * might not prefer the retransmission of the entire event
 */
bool 
NBEventHeaders::isSliced() 
{
	return this->sliced;
}

 /** Indicates if this event includes priority information */
 bool 
 NBEventHeaders::hasPriorityInfo() 
{
	return this->containsPriorityInfo;
}

/** Gets the priority associated with this event */
int 
NBEventHeaders::getPriority() 
{
	return this->priority;
}

 void 
NBEventHeaders::setPriorityInformation(int priority)
{
	this->priority=priority;	
}


  bool 
NBEventHeaders::hasApplicationInfo()
{
	return this->containsApplicationInfo;
}

/** Gets the application type associated with this event */
char *
NBEventHeaders::getApplicationType() 
{
	return this->application;
}

/**
* Indicates if the event is a persistent one. If it is, the event needs to
* be archived on storage. It is of course assumed that the event corresponds
* to a template that supports reliable delivery
*/
bool 
NBEventHeaders::isPersistent() 
{
	return this->persistent;
}

void 
NBEventHeaders::setPersistent()
{
		this->persistent=true;
}
   
/**
* Indicates if this event is a transient event. If this is a transient event
* it need not be archived on any storage
*/
bool 
NBEventHeaders::isTransient() 
{
	return this->transient;
}

/**
* Indicates if the payload has been compressed. Such events are specifically
* used in cases where the device's network utilization costs might be at a
* premium.
*/
bool 
NBEventHeaders::isCompressed() 
{
	return this->compressed;
}


/** Generate the serialized representation of the event headers */
void
NBEventHeaders::getBytes(char *buff,int * length) 
{
	long long offset=0;
	
	snapshot = createHeaderSnapshot();
	//cout<<"Snapshot ==== "<<snapshot<<endl;
	Utils::writeInt(buff+offset,snapshot,offset);
	//offset+=4;
  
	if (containsTemplateId) {
		//  cout<<"Template ID =  "<<templateId<<endl;
	     Utils::writeInt(buff+offset,templateId,offset);
	     //offset+=4;
    }

	 if (containsTimestamp) {
        //  cout<<"TimeStamp =  "<<timestamp<<endl;
	      Utils::writeLong(buff+offset,timestamp,offset);
	      //offset+=8;
     }
	 
	  if (containsSource) {
       //  cout<<"Source  =  "<<source<<endl;
	      Utils::writeInt(buff+offset,source,offset);
	     //offset+=4;
      }
	 
	  
      if (containsEventId) {
		  int len;
		  char temp[32];
		  eventId->getBytes(temp,&len);
          // cout<<"Length of Event ID = "<<len<<endl;
	    
		  Utils::writeInt(buff+offset,len,offset);
		 // offset+=4;
		  Utils::writeBytes(buff+offset,temp,len,offset);
		  //offset+=len;
     }
	
	//cout<<"Coming to this point" <<endl;
	 
     if (correlated) {
             int len; 
             correlationId->getBytes(buff+offset,&len);
			// cout<<"Length of Correlation ID = "<<len<<endl;			
           
     		 offset+=len;
     }
		 


         if (containsTimeToLive) {
              // cout<<"containsTimeToLive  =  "<<timeToLive<<endl;
			    Utils::writeInt(buff+offset,timeToLive,offset);
			 //offset+=4;
         }
		 
		 if (secure) {
			  throw  ServiceException("Security is not supported ");
		 }
		 
		 if (containsIntegrity) {
			  throw  ServiceException("Integrity checking is not supported ");
		 }
		 
		  if (fragmented) {
         	 throw  ServiceException("Fragmenting is not supported ");
         }

		 //By default this is false. We don't support it yet.
		 /*
         if (hasTotalNumOfFragmentsInfo) {
	    }
		*/
		 
         if (containsPriorityInfo) {
          //    cout<<"contains Priority  =  "<<priority<<endl;
			  Utils::writeInt(buff+offset,priority,offset);
			 //offset+=4;
         }

		 	 //By default this is false. We don't support it yet.
		 /*
         if (containsApplicationInfo) {
         }
		 */
		 
		 //By default this is false. We don't support it yet.
		 /*
         if (compressed) {
         }
		 */
		
	//ss>>str;
	//strncat(buff,str.c_str(),str.length());
	(*length)=offset;
}

  /** Creates a snapshot of the headers contained in the event headers */
int
NBEventHeaders::createHeaderSnapshot() {
      snapshot = 0;
      numOfHeaders = 0;

      snapshot = updateSnapshot(snapshot, false); //Default Value
      snapshot = updateSnapshot(snapshot, compressed);
      snapshot = updateSnapshot(snapshot, persistent);
      snapshot = updateSnapshot(snapshot, containsApplicationInfo);
      snapshot = updateSnapshot(snapshot, containsPriorityInfo);
      snapshot = updateSnapshot(snapshot, false);//Default Value
      snapshot = updateSnapshot(snapshot, false);//Default Value
      snapshot = updateSnapshot(snapshot, fragmented);
      snapshot = updateSnapshot(snapshot, containsIntegrity);
      snapshot = updateSnapshot(snapshot, secure);
      snapshot = updateSnapshot(snapshot, containsTimeToLive);
      snapshot = updateSnapshot(snapshot, correlated);
      snapshot = updateSnapshot(snapshot, containsEventId);
      snapshot = updateSnapshot(snapshot, supressDistributionToSource);
      snapshot = updateSnapshot(snapshot, containsSource);
      snapshot = updateSnapshot(snapshot, containsTimestamp);
      snapshot = updateSnapshot(snapshot, containsTemplateId);

     return snapshot;
   }



int 
NBEventHeaders::updateSnapshot(int snapshot, bool subHeader) {
      int orWith = 1;
      /**
       * If the widget's value is true, we need to first update the snapshot to
       * contain the subHeader's trace
       */
      if (subHeader) {
         snapshot |= orWith;
      }
      snapshot = snapshot << 1;
      numOfHeaders++;
      if (numOfHeaders == MAX_NUM_HEADERS) {
		  throw  ServiceException("Num of headers exceeded the maximum ");
      }
      return snapshot;
   }

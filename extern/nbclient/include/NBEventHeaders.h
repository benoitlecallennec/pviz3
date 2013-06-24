/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifndef _NBEVENT_HEADERS_H
#define _NBEVENT_HEADERS_H

	
#include "NBEventID.h"
#include "Utils.h"
#include <sstream>
	
	
class NBEventHeaders
{
 
   public:

		   NBEventHeaders(); 
           ~NBEventHeaders(); 
   
   			//Initilize the headers to defaults.
   			void init();
		
		   NBEventHeaders(char *);
		   /** ********************************************************************** */
		   /** Implementation of methods in the Event Headers interface */
		   /** ********************************************************************** */
		
		   /** Indicates the templateId which this event conforms to */
			int getTemplateId() ;
   
   			void setTemplateId(int templateId) ;
		
		   /**
			* Returns the timestamp for this event. This timestamp corresponds to the
			* value assigned by the time service
			*/
			long long getTimeStamp() ;
			
			void setTimeStamp(long long timestamp);
		
		   /** Returns the generator of this message */
			int getSource() ;
   
           /**
			* Indicates that the message should not be rerouted to it source, that is
			* contained in the event headers
			*/
			bool setSupressDistributionToSource() ;
			
			void setSourceInformation(int source,bool supressDistributionToSource);
			
			/** Indicates if there is an event id associated with this message */
		   bool hasEventId() ;
   
           /** Get the event identifier */
			NBEventID *getEventId() ;
			
			void setEventId(NBEventID * eventId);
			
			 /**
			* Indicates if this event is correlated with any other event. Casual
			* constraints may entail that the correlated event be delivered prior to the
			* delivery of this event.
			*/
			bool isCorrelated() ;
			
			/** Returns the correlation identifier associated with the event */
			NBEventID *getCorrelationIdentifier() ;
			
			void setCorrelationId(NBEventID *correlationId);
			
			bool hasTimeToLive();

		    /** Returns the time to live */
			int getTimeToLive() ;
			
			void setTimeToLive(int timeToLive);
		
				   /** Indicates if this event is a secure event */
			bool isSecure() ;
			
		   /** Indicates if the event has integerity check information. */
		   bool hasIntegrity();
		   
		   		   /** Indicates if this event is a fragment of a larger event */
			bool isFragmented();
		   
		   		   /*
			* Indicates if this is an event slice which contains a part of the orginal
			* transmitted payload. These are specifically used in cases where the device
			* might not prefer the retransmission of the entire event
			*/
			bool isSliced() ;
		   
		     /** Indicates if this event includes priority information */
		   bool hasPriorityInfo() ;
   
		   /** Gets the priority associated with this event */
		   int getPriority() ;
		   
		   void setPriorityInformation(int priority);
		
		   bool hasApplicationInfo();
		   
		   /** Gets the application type associated with this event */
		   char *getApplicationType() ;
		   
		   		   /**
			* Indicates if the event is a persistent one. If it is, the event needs to
			* be archived on storage. It is of course assumed that the event corresponds
			* to a template that supports reliable delivery
			*/
		   bool isPersistent() ;
		   
		   void setPersistent();
		   
		   /**
			* Indicates if this event is a transient event. If this is a transient event
			* it need not be archived on any storage
			*/
		   bool isTransient() ;
		
		   /**
			* Indicates if the payload has been compressed. Such events are specifically
			* used in cases where the device's network utilization costs might be at a
			* premium.
			*/
		   bool isCompressed() ;
 
		   /** Generate the serialized representation of the event headers */
	      void getBytes(char *buff,int * length) ;
		
   
   private:

		   bool containsTemplateId ;
		   bool containsTimestamp;
		   bool containsSource ;
		   bool supressDistributionToSource;
		   bool containsEventId;
		   bool correlated;
		   bool containsTimeToLive;
		   bool secure;
		   bool containsIntegrity;
		   bool fragmented;
		   bool sliced;
		   bool containsPriorityInfo;
		   bool containsApplicationInfo;
		   bool persistent;
		   bool transient;
		   bool compressed;
		   bool containsAssertions;

			int templateId ;
			long long timestamp;
			int source;
			NBEventID *eventId;
			NBEventID *correlationId;
			int timeToLive;
			int priority;
			char *application;
			
			int snapshot;
			int numOfHeaders;
			
			int createHeaderSnapshot() ;
			int updateSnapshot(int snapshot, bool subHeader);
	
};


#endif /* _NBEVENT_HEADERS_H */

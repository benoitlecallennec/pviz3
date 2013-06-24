/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include <iostream>
#include "Utils.h"
#include "NBEventGenerator.h"
#include "ServiceException.h"


NBEventGenerator::NBEventGenerator()
{
}

NBEventGenerator::~NBEventGenerator()
{
}

void
NBEventGenerator:: generateEvent(NBEvent *event,int templateId, int source,
            bool suppressDistToSource, bool timestamp, int timeToLive,
            bool isCorrelated, NBEventID *correlationId,
            bool persistent, int priority, int contentSynopsisType,
            string contentSynopsis, char* contentPayload,int payloadLength)
{
		
		NBEventID *id=event->getEventID();
		NBEventHeaders *header=event->getEventHeaders();
        
	    //Make sure that the header is properly initilized.	
 	    header->init();

		//Always generates an EventID
	     this->idGen.generateEventId(id,source);	     
	  	
		 header->setEventId(id);
	   
		 header->setTemplateId(templateId);
	 	
	     header->setSourceInformation(source,suppressDistToSource);	
	
		if(timestamp)
		{
			header->setTimeStamp(Utils::getCurrentTimeMicroseconds());	
		}
		
		header->setTimeToLive(timeToLive);
		
		if(isCorrelated)
		{
			header->setCorrelationId(correlationId);	
		}
			
		 if (persistent)
		 {
            header->setPersistent();
         }
		 
		 header->setPriorityInformation(priority);
		 

		if (contentSynopsis.empty()) {
            throw new ServiceException("Specified content synopsis empty");
        }

        if (contentPayload == NULL) {
           throw new ServiceException("Specified content payload = NULL");
        }
	
		event->setEventHeaders(header);
		
		event->setContentSynopsisInfo(contentSynopsisType,contentSynopsis);
		
		event->setContentPayload(contentPayload,payloadLength);
		
		
		/**
		We are generating an ID always.
		 if (generateId) {
            if (source == 0) {
                throw new ServiceException("Specified entity id is 0"
                        + "Generation of event id is not possible");
            }
            headers->setEventId(this->idGen.generateEventId(id,source));
        }
		*/
		
			
}

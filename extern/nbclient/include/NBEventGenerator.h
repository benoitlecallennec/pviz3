/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef NBEVENT_GENERATOR_H
#define NBEVENT_GENERATOR_H	
	
#include "NBEvent.h"
#include "NBEventID.h"
#include "NBEventIDGenerator.h"
#include <string>

using namespace std;

	class NBEventGenerator
	{
		public:
			NBEventGenerator();
		~NBEventGenerator();
			
		void  generateEvent(NBEvent *event,int templateId, int source,
            bool suppressDistToSource, bool timestamp, int timeToLive,
            bool isCorrelated, NBEventID *correlationId,
            bool persistent, int priority, int contentSynopsisType,
            string contentSynopsis, char* contentPayload,int payloadLength);		
		
		private:
			NBEventIDGenerator idGen;
	};

#endif /*NBEVENT_GENERATOR_H*/

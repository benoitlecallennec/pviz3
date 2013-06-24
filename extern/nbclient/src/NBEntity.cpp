/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include "constants.h"
#include "Utils.h"
#include "NBEntity.h"
#include "NBEventID.h"
#include <climits>
#include <iostream>
using namespace std;

NBEntity::~NBEntity()
{
}

 NBEntity::NBEntity(int entityId) {
      this->entityId = entityId;
      reset();
    }

void  
NBEntity::generateEventId(NBEventID *id) {
      if (seqNum == INT_MAX) {
	reset();
      }
      seqNum++;	  
	  id->init(entityId, timestamp, seqNum);
	 	
    }
    

    /** Reset the timestamp and sequence number */
	void
    NBEntity::reset() {
      timestamp = Utils::getCurrentTimeMicroseconds();
      seqNum = 0;
    }

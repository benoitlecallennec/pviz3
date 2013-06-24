/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifndef _NB_EVENTID_GENERATOR_H
#define _NB_EVENTID_GENERATOR_H


	#include <map>	
	#include <string>
	#include "NBEntity.h"
	#include "NBEventID.h"

class NBEventIDGenerator {

  public:
	  NBEventIDGenerator();
  	~ NBEventIDGenerator();
      void generateEventId(NBEventID *id,int entityId);

  private:
  	 std::map<int,NBEntity * > managedEntities;
     std::map<int,NBEntity * >::iterator iter;
   	 int numOfEntities;
     int firstEntityId ;
     NBEntity *firstEntity;
  };


#endif /* _NB_EVENTID_GENERATOR_H */

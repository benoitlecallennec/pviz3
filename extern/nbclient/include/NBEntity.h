/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifndef _NBENTITY_H
#define _NBENTITY_H

#include "NBEventID.h"
	
class NBEntity 
	{
   public:

     ~NBEntity();
     NBEntity(int entityId) ;
     void generateEventId(NBEventID *id) ;
   
	private:
	int entityId;
    long long timestamp;
    int seqNum;
	
	void reset() ;
  };


#endif /* _NBENTITY_H */

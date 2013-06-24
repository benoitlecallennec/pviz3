/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "StdAfx.h"
#endif
#include "NBEventIDGenerator.h"
#include "NBEntity.h"
#include "NBEventID.h"
#include <iostream>
using namespace std;

NBEventIDGenerator::NBEventIDGenerator()
	:numOfEntities(0),
	firstEntityId(0)
{	}

NBEventIDGenerator::~NBEventIDGenerator()
{
  for(iter = managedEntities.begin(); iter != managedEntities.end(); iter++)
  {
	   	delete iter->second;
  }
   managedEntities.clear();
   firstEntity=NULL;
}

void
NBEventIDGenerator::generateEventId(NBEventID *id,int entityId)
{
 	 if (this->managedEntities.size() == 0) {
	     this->numOfEntities++;
         this->firstEntityId = entityId;
 		
		 //Have to keep the entities in the Map for future IDs.
		 //Deletion is handled in the destructor of the NBEventIDGenerator
 		 this->firstEntity=new NBEntity( entityId);
     	 this->managedEntities[entityId]=this->firstEntity;
 		
         this->firstEntity->generateEventId(id);		 
        } else{
			this->iter=managedEntities.find(entityId);
	
		if( this->iter != managedEntities.end()){
			 NBEntity  *entity=this->iter->second;
			 entity->generateEventId(id);
		 }
	else{
			
		NBEntity *newEntity= new NBEntity(entityId);
		 this-> managedEntities[entityId]=newEntity;
		 newEntity->generateEventId(id);
 	}

}
}

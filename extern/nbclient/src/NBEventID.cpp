/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifdef WIN32
#include "StdAfx.h"
#endif
#include <iostream>
#include <string>
#include "NBEventID.h"
#include "Utils.h"
#include <limits>

NBEventID::NBEventID()
{
}

NBEventID::~NBEventID()
{
}

/*This is to unmarshell the bytes*/
NBEventID::NBEventID(char *bytes)
{
}
     
  /** Constructs a unique EventID based on the entityID, timeStamp,
      sequenceNumber and the incarnation associated with the publishing
      entity.
      @param _entityID The entityID
      @param _timeStamp The timestamp associated with the ID.
      @param _seqNum The sequence number
  */
void
NBEventID::init(int entityID, long long timeStamp, int seqNum)
{
	this->entityID=entityID;
	this->timeStamp=timeStamp;
	this->seqNum=seqNum;
	
	/*FIX THIS - CURRENTLY JUST THE MEMORY LOCATION */
	this->eventHashcode=this->hashCode();
	
}
  
  /** Returns the entity ID contained in the eventID 
      @return int entityID */
int
NBEventID::getEntityID()
{
	return this->entityID;
}
  
  /** This method returns the timeStamp that is associated with the eventID.
      @return long timeStamp  */
long long
NBEventID::getTimeStamp()
{
	return this->timeStamp;
}
  
  /** This method returns the sequence number associated with the eventID
      instance.
      @return int sequenceNumber */
int
NBEventID::getSequenceNumber()
{
	return this->seqNum;
}
  
  /* Returns a string representation of the EventID object.
     @return String stringRepOfObject */
char *
NBEventID::toString(	char *buffer)
{
	stringstream buff;
    buff<<"Event ID:: entityID -> ";
    buff<<entityID;
    buff<<"   timeStamp -> ";
    buff<<timeStamp;
    buff<<" seqNum -> ";
 	buff<<seqNum;
 	sprintf(buffer,buff.str().c_str());
	//sprintf (buffer, "Event ID:: entityID -> %d   timeStamp -> %d seqNum -> %d", entityID,timeStamp,seqNum);
	return buffer;
}
  
  /** This method provides a marshalling of the eventID bytes. 
      @return Byte stream representation of the eventID.
  */

void 
NBEventID::getBytes(char *buff,int * length)
{
	
	*length=0;
	long long offset=0;
	Utils::writeInt(buff+(*length),entityID,offset);
	//(*length)+=4;
	Utils::writeLong(buff+(*length),timeStamp,offset);
	//(*length)+=8;
	Utils::writeInt(buff+(*length),seqNum,offset);
	//(*length)+=4; /* entityId time stamp sequence number*/
	(*length)+=offset;
	//cout<<"NBEventID : entityID "<<entityID<<"  timeStamp "<<timeStamp<<"   seqNum "<<seqNum<<endl;
	
}

  /**
     Compares two objects for equality. Returns a boolean that indicates
     whether this object is equivalent to the specified object. This method
     is used when an object is stored in a hashtable.
     @param obj the Object to compare with
     @return true if these Objects are equal; false otherwise.
     @see java.util.Hashtable */
bool 
NBEventID::equals(NBEventID id)
{
	//TODO:Implement this
	return false;
}

  /**Generates a hash code for the receiver. This method is supported 
     primarily for hash tables, such as those provided in java.util.
     If two objects are equal (equals(Object) returns true) they must have the 
     same hash code
     @return an integer hash code for the receiver */
int
NBEventID::hashCode()
{
	//TODO;Implement this
	return 0;
}

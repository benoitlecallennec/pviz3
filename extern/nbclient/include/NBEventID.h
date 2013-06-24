/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifndef _NBEVENTID_H
#define _NBEVENTID_H

#include <sstream>
using namespace std;
class NBEventID
{
	
  public:
  
  NBEventID();
  ~NBEventID();
  /** This constructor provides for the unmarshalling of the eventID bytes
      that are received over a link
      @param idBytes A byte stream representation of the event ID.
  */
  NBEventID(char*);
     
  /** Constructs a unique EventID based on the entityID, timeStamp,
      sequenceNumber and the incarnation associated with the publishing
      entity.
      @param _entityID The entityID
      @param _timeStamp The timestamp associated with the ID.
      @param _seqNum The sequence number
  */
    void init(int , long long , int );
  
  /** Returns the entity ID contained in the eventID 
      @return int entityID */
  int getEntityID();
  
  /** This method returns the timeStamp that is associated with the eventID.
      @return long timeStamp  */
  long long getTimeStamp();
  
  /** This method returns the sequence number associated with the eventID
      instance.
      @return int sequenceNumber */
  int getSequenceNumber();
  
  /* Returns a string representation of the EventID object.
     @return String stringRepOfObject */
  char *toString(char *buffer);  
  
  /** This method provides a marshalling of the eventID bytes. 
      @return Byte stream representation of the eventID.
  */
void getBytes(char *buff,int * length);

  /**
     Compares two objects for equality. Returns a boolean that indicates
     whether this object is equivalent to the specified object. This method
     is used when an object is stored in a hashtable.
     @param obj the Object to compare with
     @return true if these Objects are equal; false otherwise.
     @see java.util.Hashtable */
  bool equals(NBEventID id);

  /**Generates a hash code for the receiver. This method is supported 
     primarily for hash tables, such as those provided in java.util.
     If two objects are equal (equals(Object) returns true) they must have the 
     same hash code
     @return an integer hash code for the receiver */
  int hashCode();
 
 
  private:
	    /** Every issuing entity is assigned a unique ID. This reduces the uniqueness
      problem to a point in space */
   int entityID;

  /** The time in micro-seconds  */ 
   long long timeStamp;
  /** We use this so that our issue rate isn't determined by the granularity
      of the clock in the underlying system. The sequence numbers start from
      zero and continue till it reaches Short.MAX_VALUE
  */
   int seqNum;

   int eventHashcode;
	  
};


#endif /* _NBEVENTID_H */

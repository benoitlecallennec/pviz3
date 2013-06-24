/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifdef WIN32
#include "StdAfx.h"
#endif
#include "Profile.h"
#include "constants.h"
#include "Utils.h"


Profile::Profile()
{
}

Profile::~Profile()
{
}


Profile::Profile(string subscription, int destinations, string profileId):subscription(subscription),destinations(destinations),profileId(profileId)
{
	profileType=STRING_PROFILE_SYNOPSIS_TYPE;
}

// void
//Profile::init(string subscription, int destinations, string profileId)
//{
//	 this->profileType = STRING_PROFILE_SYNOPSIS_TYPE;
//     this->moduleName = "StringProfile: ";
//	 this->subscription = subscription;
//     this->destinations = destinations;
//     this->profileId = profileId;
//}
   
   
    /** This method serves to unmarshall a stringProfile stream received over the
      link. The corresponding marshalling method for the stringProfile is 
      contained within the getBytes() method.
      @param stringProfileBytes The marshalled stream of stringProfile bytes 
      that are received over a link. */
void 
Profile::fromBytes(char *stringProfileBytes) 
{
}
  
  
/** This method returns the subscription associated with the stringProfile.
      @return The subscription.
*/
void 
Profile::getSubscription(string *subscription) 
 {
 }
  
/** This method returns the destinations assocaited with the subscription.
      @return The destinations.
*/
int 
Profile::getDestination()
{
	return 0;  //TODO Not supported yet.
}
  
void 
Profile::getProfileId(string *profileId)
{
}
	
int 
Profile::getProfileType()
{
     return this->profileType;
}
  
      
/** This method returns the stringProfile as a stream of bytes, the 
      unmarshalling counterpart to this marshalling scheme is contained within
      one of the StringProfile constructors 
      @return The byte stream representation of the stringProfile */
void 
Profile::getBytes(char *buff,int * length)
{
	  
*length=0;
long long offset=0;
	  Utils::writeInt(buff+offset,subscription.length(),offset);
	 // (*length)+=4;
	  
      Utils::writeBytes(	  buff+offset,subscription.c_str(),subscription.length(),offset);
	  //(*length)+=subscription.length();

	    Utils::writeInt(buff+offset,destinations,offset);
	 // (*length)+=4;

	   Utils::writeInt(buff+offset,profileId.length(),offset);
	  //(*length)+=4;

       Utils::writeBytes(	  buff+offset,profileId.c_str(),profileId.length(),offset);
	  //(*length)+=profileId.length();
	  (*length)+=offset;
	  
}

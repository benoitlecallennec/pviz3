/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
 #ifndef PROFILE_H
#define PROFILE_H
 
 #include <string>
 #include <iostream>
 
 using namespace std;
 
 class Profile{
  
  public:
  
  Profile();
 ~Profile();
 

  
  /** A StringProfile is comprised of both the subscription as well as the 
      destination of the unit/super-unit that has expressed an interest in it.
      @param subscription The string subscription
      @param destinations The hierarchial destinations associated with this
      stringProfile */
	
 Profile(string subscription, int destinations, string profileId);
	  
// void init(string subscription, int destinations, string profileId);
   
   
    /** This method serves to unmarshall a stringProfile stream received over the
      link. The corresponding marshalling method for the stringProfile is 
      contained within the getBytes() method.
      @param stringProfileBytes The marshalled stream of stringProfile bytes 
      that are received over a link. */
  void fromBytes(char *stringProfileBytes) ;
  
  
   /** This method returns the subscription associated with the stringProfile.
      @return The subscription.
  */
  void getSubscription(string *subscription) ;
  
   void getProfileId(string *profileId);
	
  int getProfileType();
  
  /** This method returns the destinations assocaited with the subscription.
      @return The destinations.
  */
  int getDestination();
  
    
  /** This method returns the stringProfile as a stream of bytes, the 
      unmarshalling counterpart to this marshalling scheme is contained within
      one of the StringProfile constructors 
      @return The byte stream representation of the stringProfile */
  void getBytes(char *buff,int * length); 
  
  
 private:
  
  string subscription;
  int destinations;
  string profileId;
  int profileType;
 
 };

    
#endif /*#define PROFILE_H*/

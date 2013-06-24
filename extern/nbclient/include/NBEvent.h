/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef NBEVENT_H
#define NBEVENT_H


#ifdef WIN32
#include "nbcppdll.h" 
#endif
#include "constants.h"
#include "NBEventHeaders.h"
#include "NBEventID.h"
#include "DistributionTrace.h"
#include <string>

using namespace std;

#ifdef WIN32
class NBCPPDLL_API NBEvent
#else
class  NBEvent
#endif
{

public:

	NBEvent();
	~NBEvent();

	void init();

   bool fromBytes(char *bytes);

  /** Indicates the type of the event */
  int getEventType();

  void setEventType(int); 

   /** Indicates the type of the content */
  char* getContentType();

  /** Indicates if this event has any event headers */
  bool hasEventHeaders();

   void setEventHeaders(NBEventHeaders *headers);

  /** Header information pertaining to the event */
  /* EventHeaders are not yet supported */ 
  //EventHeaders getEventHeaders();
  
  
  /** Indicates if this event has any user-defined properties */
  bool hasEventProperties();


  /** EventProperties associated with the event */
  /*Not supported yet */
  //public EventProperties getEventProperties();

  /** Returns the type of the content synopsis */
  int getContentSynopsisType();


  /** Retrieves the synopis associated with the content */
  /*Only string content synopsises are supported */
  string getContentSynopsis();
  
  void setContentSynopsisInfo(int,string);

  
  /** Get the payload size */
  int getContentPayloadSize();


  /** Get the content Payload */
  char* getContentPayload();
    
   int getPayloadLength();
   
  void setContentPayload(char *,int);
   
   /** Indicates if the event has distribution traces */
  bool hasDistributionTraces();
  
  /** Gets the distribution Traces associated with the event */
  /*Currently no destribution traces */
  //public DistributionTraces getDistributionTraces();

  /** Get a byte stream representation of the event */
  void getBytes(char *buff,int *length);
  
  NBEventID *  getEventID();
  
  NBEventHeaders * 
  getEventHeaders();
  
  private:
	int eventType;
  	char *contentType;
  	bool containsEventHeaders;
  	bool containsEventProperties;
    int contentSynopsisType;
    string contentSynopsis;
    char *contentPayload;
    int payloadLength;
    bool containsDistributionTraces;
    bool containsContentType;
    DistributionTrace *distributionTrace;
  
    NBEventID id;
    NBEventHeaders header;
  
};

#endif /*NBEVENT_H*/

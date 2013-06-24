/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#include <string>
using namespace std;

/*Integer Lengths in Different Platforms*/
/*32 bit platforms*/
const int SHORT_LENGTH=2;
const int INT_LENGTH=4;
const int LONG_LENGTH=8;
//Always one, but prefer to have them without magic numbers
const int BOOL_LENGTH=1;
const int CHAR_LENGTH=1;


/*Mappings from  TemplateProfileAndSynopsisTypes in the NB java implementation*/
const int STRING_CONTENT_SYNOPSIS=1;

const int STRING_PROFILE_SYNOPSIS_TYPE=1;
	
	
/*START: NB Event Types*/
	 /** For most cases this would be true */
 const int NORMAL_EVENT = 0;

  /** This would indicate that the payload is carrying a reliable delivery
      protocol exchange message */
 const int RELIABLE_DELIVERY_EXCHANGE = 1;

  /** This would indicate that the payload is carrying a warning exchange 
      message */
 const  int RELIABLE_DELIVERY_WARNINGS = 2;

  /** This would indicate that the payload is carrying a replay exchange 
      message */
 const  int REPLAY_EXCHANGE = 3;

  /** This would indicate that the payload is carrying a record exchange 
      message */
const  int RECORD_EXCHANGE = 4;
/*END*/	


 const int MAX_NUM_HEADERS=31;
 const int HEADER_BYTES_MAX=512;

 const char DATA = (char) 6;
 const char NB_EVENT = (char)98;
 const char NODE_ADDRESS  = (char) 99;
 const char CLIENT_SETUP=(char)100;
 const char PROFILE_PROPAGATION            = (char) 97;
 
 
 /*ERROR CODES*/
 const int SOCKET_READ_ERROR=1;
 const int SOCKET_EOF=2;
	
/*Constants related to the agents and discovery of services*/

const string DISCOVER_AGENT="Clarens/Service/LocatorAgent";
const string GOSSIP_AGENT="Clarens/Service/LocatorAgent/Gossips";
const string DISCOVER_ROOTLET="Clarens/Service/LocatorAgent/Discover/Service";							 
const string CLIENT_TEMP_RESPONSE_TPOIC="Clarens/Service/Client/Response";

//const string DISCOVER_ROOTLET_ALL_AGENT="Clarens/Service/LocatorAgent/Discover";
const string ADVERTISE_ROOTLET ="Clarens/Service/Rootlet/Advertise";
const string ROOTLET_HEART_BEAT="Clarens/Service/Rootlet/Heartbeat/PULL/Rootlet-ID";
const string ROOTLET_SHUT_DOWN="Clarens/Service/Rootlet/Shutdown";
const string RESPONSE_DISCOVERY="Clarens/Service/Rootlet/DiscoveryResponse/Request-ID";
const string ROOTLET_UPDATE="Clarens/Rootlet/Updates/Rootlet-ID";
const string ROOTLET_START_JOB="Clarens/Rootlet/Updates/Rootlet-ID/Start";
const string ROOTLET_FINISH_JOB="Clarens/Rootlet/Updates/Rootlet-ID/Finish";
const string ROOTLET_POLL="Clarens/Rootlet/Poll/Status/Rootlet-ID";
		
const string DISCOVER_AGENT_MSG="DISCOVER_AGENT_MSG";
			
	/*Various wait times in seconds*/
const int DISCOVER_AGENT_WAIT_TIME=2;
			
	/*Other Constants*/
const int MAX_NUM_AGENT_DISCOVERY=10;	


/**
 * Now the constants for the Clarense
 * */

const string CONF_NB_TRANSPORT = "nb_transport";
const string CONF_NB_HOST_NAME = "nb_hostname";
const string CONF_NB_PORT_NUMBER = "nb_portnumber";
const string CONF_NB_ENTITY_ID = "nb_entity_id";
const string CONF_NB_KEYSTORE = "nb_keystore";
const string CONF_NB_KEYSTORE_PWD = "nb_keystore_pwd";
const string CONF_NB_KEY_ALIAS="nb_key_alias";
const string CONF_NB_PRIVATE_KEY_PWD="nb_private_key_pwd";
const string CONF_NB_TRUSTED_CAS="nb_trusted_cas";
const string CONF_NB_PROXY_FILE="nb_proxy_file";

/*Max wait time for the AgentServiceClient
 * to retrieve responses in micro seconds*/

const long CLIENT_CALL_MAX_WAIT =30000000;//Microseconds
const long CLIENT_CALL_SLEEP=5000;//Microseconds



//Data Type
const char PROTOCOL_MESSAGE=1;

const char CLARENS_DATA =1;
const char NB_CLARENS_MESSAGE=2;	

//Exchange Type
const char AGENT_DISCOVERY=2;
const char ROOTLET_DISCOVERY=3;
const char HEART_BEAT=4;
const char ROOTLET_ADVERTISE=5;
	
//Action Type
const char REQUEST =6;
const char RESPONSE=7;
const char PRO_ACTIVE=8;

//Security Related Constants

/*Provision for increase in AES encryption*/
const int AES_INC=256;

	
#endif /* _CONSTANTS_H */

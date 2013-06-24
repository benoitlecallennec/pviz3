/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#include <iostream>
#include "ServiceClient.h"
#include "Profile.h"

using namespace std;

int main(int argc, char **argv){
	cout<<"///////////////////////TestClient////////////////////////"<<endl;
	cout<<argc<<endl;
	if(argc!=6){
		cout<<"Usage : [host][port][entityId][topic][msg]"<<endl;
		exit(1);	
	}
	/*Ignore the first argument */
	*argv++;	
	
	/*host*/
	string host=*argv++;
	
	/*port*/
	int port=0;
	std::istringstream ss_port(*argv++);
	ss_port>>port;
	
	/*entityId*/	
	int entityId=0;
	std::istringstream ss_entityId(*argv++);
    ss_entityId >> entityId;
    
	string contentSynopsis=*argv++;
	
	char *msg=*argv++;
	
	if(entityId<=0|| contentSynopsis.length()<=0 ||strlen(msg)<=0){
		cout<<"Usage : [host][port][entityId][topic][msg]"<<endl;
		exit(1);	
	}
		
	int templateId=55745;
	//string contentSynopsis="/abc/cpp/test";
			
	ServiceClient serviceClient;
	serviceClient.init(host,port,entityId,templateId);
	cout<<"Publishing = "<<contentSynopsis<<"   "<<msg<<endl;
	serviceClient.publish(contentSynopsis,(char *)msg,strlen(msg));				
	
	//usleep(500000);	//Half  a second wait.
	cout<<"Cleanup"<<endl;
	serviceClient.cleanup();
	return 0;
}

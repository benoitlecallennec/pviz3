/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifdef WIN32
#include "stdafx.h"
#endif

#include <iostream>
#include "ServiceClient.h"
#include "Profile.h"

using namespace std;

class MyCallback : public Callback{

     private:
	 int counter;

     public:
	 
	MyCallback(): counter(0){}
    
        void onEvent(NBEvent *nbEvent) {
		//cout<<"------------Event Rececived with the following information---------"<<endl;
		//cout<<"Topic  = "<<nbEvent->getContentSynopsis()<<" counter = "<<counter<<endl;
                counter++;
		char * payload=nbEvent->getContentPayload();
		int payloadSize=nbEvent->getPayloadLength();
		// cout<<"Payload Size = "<<payloadSize<<endl;
		 for(int i=0;i<payloadSize;i++)
		 {
		    printf("%c",*(payload+i));
		 }
		cout<<endl<<"----------------------------------------------------- "<<endl;
	}
};

int main(int argc, char **argv) {
	cout<<"///////////////////////TestClient////////////////////////"<<endl;

	if (argc!=3) {
		cout<<"Usage : [host][topic]"<<endl;
		exit(1);
	}
	/*Ignore the first argument */
	*argv++;
	string host=*argv++;
	string contentSynopsis=*argv++;

	if (contentSynopsis.length()<=0) {
		cout<<"Usage : [topic]"<<endl;
		exit(1);
	}

	srand((unsigned) time(0));
	int templateId=55745;
	//for (int i=0; i<12; i++) {		
		int entityID = (rand() % 10000) + 10000;
		//string contentSynopsis="/abc/cpp/test";

		ServiceClient *serviceClient = new ServiceClient();
		serviceClient->init(host, 5045, entityID, templateId);

		MyCallback *callback=new MyCallback();
		serviceClient->subscribe(contentSynopsis, callback);
	//}
	cout<<"====================Simple Chat Client============="<<endl;
	cout<<"Type any number of messages and <return key>"<<endl;
	cout<<"To exit type .<return key>"<<endl;
	cout<<"==================================================="<<endl;

	char msg[512]="";
	string str="";
	while (str!=".") {
		cin.getline(msg, 256);
		str=msg;
		if (!(str==".")) {
			//for (int i=0; i<1; i++) {
			//	cout<<i<<endl;
					serviceClient->publish(contentSynopsis,msg,strlen(msg));
			//}
		}
	}

	serviceClient->cleanup();

}

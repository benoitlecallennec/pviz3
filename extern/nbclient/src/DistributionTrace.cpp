/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#include <iostream>
#include "DistributionTrace.h"
#include "Utils.h"

using namespace std;

DistributionTrace::DistributionTrace():traversedSoFar(new char[DIST_TRAVERSDSOFAR_LEN]), toReach(new char[DIST_TOREACH_LEN])
{}

DistributionTrace::~DistributionTrace(){
	delete[] this->traversedSoFar;
	delete[] this->toReach;
}

void 
DistributionTrace::fromBytes(char *stringProfileBytes){
	cout<<"ERROR:Processing of Distribution Traces is not yet supported"<<endl; 
}

void 
DistributionTrace::getBytes(char *buff,int * length){
	 long long offset = 0;
	 
	 int flagVal = 0;

         if (this->toReach != NULL) {
            flagVal += 1;
         }

         if (this->traversedSoFar != NULL) {
            flagVal += 2;
         }

        Utils::writeByte(buff+offset,(char)flagVal,offset);
   

         if (this->toReach != NULL) {
            Utils::writeShort(buff+offset,DIST_TRAVERSDSOFAR_LEN,offset);
         	Utils::writeBytes(buff+offset,this->traversedSoFar,DIST_TRAVERSDSOFAR_LEN,offset);      
         }

         if (this->traversedSoFar != NULL) {
          	Utils::writeShort(buff+offset,DIST_TOREACH_LEN,offset);
         	Utils::writeBytes(buff+offset,this->traversedSoFar,DIST_TOREACH_LEN,offset);      
         }
	 	 
	  (*length)=offset;
	
}

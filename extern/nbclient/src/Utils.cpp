/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/

#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <Windows.h>
#endif

#include <time.h>
#include <climits>
#include <sstream>

#include "Utils.h"
#include "constants.h"

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

using namespace std;

#ifndef WIN32
long long
Utils::getCurrentTimeMicroseconds(){
    int rc;
    struct timeval time;
    struct timezone timez;
    
    rc=gettimeofday(&time, &timez);
	 if(rc==0) {	 	 	
	 	long long sec=time.tv_sec;
	 	long long usec=time.tv_usec;	 	
		return (long long)( (sec*1000000)+(usec));
    }
    else {
        cerr<<"gettimeofday() failed "<<endl;
        return -1;
    }
}

	
#else

long long Utils::getCurrentTimeMicroseconds() {

  LARGE_INTEGER ticksPerSecond;
  LARGE_INTEGER tick;   // A point in time
  //LARGE_INTEGER time;   // For converting tick into real time

  // get the high resolution counter's accuracy
  QueryPerformanceFrequency(&ticksPerSecond);

  // what time is it?
  QueryPerformanceCounter(&tick);

  return (long long)tick.QuadPart/(ticksPerSecond.QuadPart/1000000);
}

#endif

string 
Utils::getCurrentTimeMicroAsString(){
	long long time=getCurrentTimeMicroseconds();
	stringstream buff;
	buff<<time;	
	return buff.str();	
}

#ifndef WIN32
void
Utils::sleep(int millis){
	cout<<"Coming to sleep"<<endl;
	usleep(millis*1000);
	cout<<"Ending sleep"<<endl;
}
#else
void
Utils::sleep(int millis){
	//Check this
	Sleep(millis);	
}
#endif

void
Utils::writeInt(char *buff ,int integer,long long &offset){
	
    	char ch=(char)(((integer)>>24)& 0x000000FF);
		memcpy(buff,&ch,1);
		
		ch=(char)(((integer)>>16)& 0x000000FF);
		memcpy(buff+1,&ch,1);
		
		ch=(char)(((integer)>>8)& 0x000000FF);
		memcpy(buff+2,&ch,1);
		
		ch=(char)((integer)& 0x000000FF);
		memcpy(buff+3,&ch,1);	
		
		offset+=INT_LENGTH;			
}


void 
Utils::writeByte(char *buff,char char_val,long long &offset)
{
	memcpy(buff,&char_val,1);
	offset+=1;
}
 
void 
Utils::writeBool(char *buff,bool bool_val,long long &offset)
{
	memcpy(buff,&bool_val,1);
	offset+=1;
}

void
Utils::writeShort(char *buff,short short_val,long long &offset)
{
    char ch=(char)(((short_val)>>8)&0xff00);
	memcpy(buff,&ch,1);
	
	ch=(char)(short_val&0x00ff);
	memcpy(buff+1,&ch,1);
	offset+=SHORT_LENGTH;
}

void 
Utils::writeLong(char *buff, long long long_val,long long &offset)
{
	long long temp;
	temp=(((long_val)>>56)& 0x00000000000000FF);
	char ch=(char)temp;
	memcpy(buff,&ch,1);
	
	temp=(((long_val)>>48)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+1,&ch,1);
	
	temp=(((long_val)>>40)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+2,&ch,1);
	
	temp=(((long_val)>>32)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+3,&ch,1);
	
	temp=(((long_val)>>24)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+4,&ch,1);
	
	temp=(((long_val)>>16)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+5,&ch,1);
	
	temp=(((long_val)>>8)& 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+6,&ch,1);
	
	temp=(long_val & 0x00000000000000FF);
	ch=(char)temp;
	memcpy(buff+7,&ch,1);
	offset+=LONG_LENGTH;

}

void 
Utils::writeBytes(char *buff,const char *src, int length,long long &offset)
{	
		memcpy(buff,src,length);		
		offset+=length;	  
}

long long 
Utils::readLong(char *buf,long long &offset){
	long long one=(*(buf))&0x00000000000000FF;
	long long two=(*(buf+1))&0x00000000000000FF;
	long long three=(*(buf+2))&0x00000000000000FF;
	long long four=(*(buf+3))&0x00000000000000FF;	
	long long five=(*(buf+4))&0x00000000000000FF;	
	long long six=(*(buf+5))&0x00000000000000FF;	
	long long seven=(*(buf+6))&0x00000000000000FF;	
	long long eight=(*(buf+7))&0x00000000000000FF;	
	
		//Increment the offset
	offset+=LONG_LENGTH;
		
	return (long long)((one<<56)+(two<<48)+(three<<40)+(four<<32)+(five<<24)+(six<<16)+(seven<<8)+(eight));
	
}

int
Utils::readInt(char *buf,long long &offset)
{
	int one=(*(buf))&0x000000ff;
	int two=(*(buf+1))&0x000000ff;
	int three=(*(buf+2))&0x000000ff;
	int four=(*(buf+3))&0x000000ff;	
	//Increment the offset
	offset+=INT_LENGTH;
	
	return ((one<<24)+(two<<16)+(three<<8)+(four));

}

bool
Utils::readBool(char *buf,long long &offset)
{
	offset+=BOOL_LENGTH;
	return (bool)((*(buf))&0x000000ff);
}

char
Utils::readByte(char *buf,long long &offset)
{
	offset+=CHAR_LENGTH;
	return (char)((*(buf))&0x000000ff);
}

int
Utils::readInt(char *buf)
{
	int one=(*(buf))&0x000000ff;
	int two=(*(buf+1))&0x000000ff;
	int three=(*(buf+2))&0x000000ff;
	int four=(*(buf+3))&0x000000ff;		
	return ((one<<24)+(two<<16)+(three<<8)+(four));

}

bool
Utils::readBool(char *buf)
{
	return (bool)((*(buf))&0x000000ff);
}

char
Utils::readByte(char *buf)
{
	return (char)((*(buf))&0x000000ff);
}

void 
Utils::copyBytes(char *src, char* dest,int length, long long &offset){
	memcpy(dest,src,length);
	offset+=length;
}

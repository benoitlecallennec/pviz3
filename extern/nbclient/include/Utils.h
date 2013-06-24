/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef _UTILS_H
#define _UTILS_H
#ifdef WIN32
#include "nbcppdll.h" 
#endif
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;

#ifdef WIN32
class NBCPPDLL_API Utils
#else
class Utils
#endif
{
	public:
		//static long getCurrentTimeMillis();
		static string getCurrentTimeMicroAsString();
		//static long long getTimeMillis();
		static long long getCurrentTimeMicroseconds();
		
		
	    static void writeByte(char *buff,char char_val,long long &offset);
    	static void writeBool(char *buff,bool bool_val,long long &offset);
	    static void writeShort(char *buff,short short_val,long long &offset);
		static void writeInt(char *buff,int int_val,long long &offset);
	    static void writeLong(char *buff,long long long_val,long long &offset);
	    static void writeBytes(char *buff,const char *src, int length,long long &offset);
	
	    static int readInt(char *buf,long long &offset);
	    static long long readLong(char *buf,long long &offset);
	    static bool readBool(char *buf,long long &offset);
	    static char readByte(char *buf,long long &offset);  
	    
	    static int readInt(char *buf);
	    static bool readBool(char *buf);
	    static char readByte(char *buf);  	
	    
	    static void copyBytes(char *src, char* dest, int length,long long &offset);
	    
	    static void sleep(int millis);
  
		
};

#endif /* _UTILS_H */

/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TUTILS_H_
#define TUTILS_H_

#include <iostream>
#include <string>
#include "TObject.h"

//typedef void (* CALLBACK_FUNC)(void **,char*,int);

using namespace std;

class  TUtils :public TObject
{
			
	public:
		TUtils();
	    virtual ~TUtils();
	    void TestMethod(string msg);
	    static void Sleep(int millis);
	    static long long ReadLong(char *buf,long long &offset);
	    static long long GetCurrentTimeMicroseconds();
	    static void WriteLong(char *buff,long long long_val,long long &offset);
	    static int ReadInt(char *buf,long long &offset);
	    static void CopyBytes(char *src, char* dest, int length,long long &offset);
		
		ClassDef(TUtils,1)
		
};
#endif /*TUTILS_H_*/

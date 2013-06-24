/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TCALLBACK_H_
#define TCALLBACK_H_

#include <iostream>
#include <string>

#include "TObject.h"

//typedef void (* CALLBACK_FUNC)(void **,char*,int);

using namespace std;

class  TCallback :public TObject
{
	private:
		//CALLBACK_FUNC callback;
		void **args;
		string className;
		string methodName;
		
	public:
	    TCallback();
	    virtual ~TCallback();
	    
//	    CALLBACK_FUNC getCallback(){
//	    	return callback;
//	    };
	    
	    void **getArgs(){
	    	return args;
	    };
	    
	    string getClassName(){
	  	    	return className;
	  	};
	  	    
	  	string getMethodName(){
	  		    	return methodName;
	  	};
	    
	    //void registerCallback(CALLBACK_FUNC f, void **args,string className,);
	    void registerCallback( void **args,string className,string methodName);
		
		ClassDef(TCallback,1)
		
};
#endif /*TCALLBACK_H_*/

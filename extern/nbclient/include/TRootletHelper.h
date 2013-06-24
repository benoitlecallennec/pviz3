/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TROOTLETHELPER_H_
#define TROOTLETHELPER_H_

#include <iostream>
#include <string>
#include <vector>

#include "TObject.h"
#include "TLine.h"


using namespace std;

class TRootletHelper :public TObject
{
	private:
		string scriptName;
		vector<string> inputFileNames;
		string outputFileName;		
		string resposeTopic;		
		
	public:
		TRootletHelper(){
			
		}
	    virtual ~TRootletHelper();
	    
	    void setScriptName(string scriptName){
	    	this->scriptName=scriptName;
	    };
	    
	    void addInputFileName(string fileName){
	    	this->inputFileNames.push_back(fileName);
	    }
	    
	    void setOutputFileName(string fileName){
	   	   	this->outputFileName=fileName;
	   	}
	    
	    void setResponseTopic(string topic){
	    	this->resposeTopic=topic;
	    }
	    
	    bool getTLineString(char *tLine);
		
		ClassDef(TRootletHelper,1)
		
};
#endif /*TROOTLETHELPER_H_*/

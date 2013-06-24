/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TCSERVERUIMODEL_
#define TCSERVERUIMODEL_
#include <iostream>
#include <string>

#include "TObject.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include <map>

using namespace std;

class TCServerUIModel : public TObject {
private:
	TCanvas *canvas;
	
	const static double colWith=0.20;
	const static double headerHeight=0.20;
	const static double rHeight=0.70;
	const static double gapBtnServerAndR=0.05;
	
	const static int BEFORE_MERGE=1;
	const static int AFTER_MERGE=2;
	
        int columnNum;
	string serverURL;
        string serverName;
        double horizGap;
        int numFiles;
        double topY;
        int numFilesPerRootlet;

	TPaveText *lblPaveText;	
	map<string,TPaveText*> rootlets;
	
	void drawHeader();
	void drawRootlets();
	string getRootletName(int index);
	
public:
	TCServerUIModel(TCanvas *canv,double hGap, double y);
	virtual ~TCServerUIModel();
	
	void setServerURL(string url){
		this->serverURL=url;
	}
	
	string getServerURL(){
		return this->serverURL;
	}
	
	void setServerName(string name){
		this->serverName=name;
	}
	
	string getServerName(){
		return this->serverName;
	}
	
	void setNumFiles(int num){
		this->numFiles=num;
	}
	
	int getNumFiles(){
		return this->numFiles;
	}
	
	void setNumFilesPerRootlet(int num){
		this->numFilesPerRootlet=num;
	}
		
	void draw(int col);	
	void updateRootlet(int num,int status); //this should be update this rootlet

	ClassDef(TCServerUIModel,1)

};

#endif /*TCSERVERUIMODEL_*/

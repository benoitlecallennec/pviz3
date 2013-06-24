/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef TFILECANVASMANAGER_H_
#define TFILECANVASMANAGER_H_

#include <iostream>
#include <string>

#include "TObject.h"
#include "TCanvas.h"
#include "TCServerUIModel.h"

using namespace std;

class TFileCanvasManager : public TObject {
private:
    const static double lblTopGap=0.02;
    const static double lblHorizontalGap=0.02;

	TCanvas *canvas;
	
	int nextFreeColumn;
	map<string,TCServerUIModel*> servers;
		
	bool hasUpdates;	
	int numFilesPerRootlet;

public:
	TFileCanvasManager(TCanvas *canvas);
	virtual ~TFileCanvasManager();	
	
	bool isHasUpdates(){
		return this->hasUpdates;
	}
	
	void setHasUpdates(bool flag){
			this->hasUpdates=flag;
	}	
	
	void setNumFilePerRootlet(int num);
	
	TCServerUIModel *getTCServerUIModel(string url);
	
	void addServer(string url, string name, int numFiles);
	void removeServer(string url);
	void draw();	
	void updateCanvas();
	void updateRootlet(string url,int num, int status);

	ClassDef(TFileCanvasManager,1)

};


#endif /*TFILECANVASMANAGER_H_*/

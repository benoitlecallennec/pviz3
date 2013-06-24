#ifndef TCLARENSSERVER_H_
#define TCLARENSSERVER_H_

#include "TObject.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class TClarensServer : public TObject
{

private:
    string          	url;
    int					num_files;
    //void 				*server;
    bool				loaded;
    vector<string>		rootFiles;
    void 				*labelPaveText;
    map<string,void *>	rootlets;

public:
	//TClarensServer(void *server,string url);
	TClarensServer(string url);
    void setLabelPaveText(void *paveText);
    void *getLabelPaveText();
    string getUrl();
    //void * getServer();
    string getRootFile(int index);
    void addRootFile(string name);
    bool isLoaded();
    void setLoaded(bool loaded);
   
    int getNumRootFile();
    
    void addRootlet(string rootletId,void *rootlet);
    void* getRootlet(string rootletId);
    
    
    virtual ~TClarensServer();
    
    ClassDef(TClarensServer,1)

};
#endif /*TCLARENSSERVER_H_*/

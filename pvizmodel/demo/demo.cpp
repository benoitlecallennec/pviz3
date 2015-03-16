#include <iostream>
#include <vector>

#include "pvizmodel.h"
#include <stdio.h>

using namespace std;

int main( int argc, const char* argv[] )
{
	PvizModel *model = new PvizModel();
	
	if (argc < 2)
    {
        printf("USAGE: %s filename\n", argv[0]);
		return 0;
    }
	
	string filename(argv[1]);
	size_t idx = filename.rfind(".");
	string ext = filename.substr(idx, filename.size() - idx);
	cout << ext << endl;
	
	if (ext.compare(".txt") == 0)
	{
		model->loadSimpleDataFile(filename.c_str());
	}
	else if (ext.compare(".pviz") == 0)
	{
		model->loadXmlDataFile(filename.c_str());
	}

	cout << "Done." << endl;
	cout << model->toString() << endl;
    
    PvizCluster* c1 = new PvizCluster();
    c1->id = 999999;
    c1->SetAsDefault(true);
    
    PvizCluster* c2 = new PvizCluster();
    c2->id = 111111;
    c2->SetAsDefault(true);
    
    model->insertCluster(c1);
    model->insertCluster(c2);
    
    model->printPvizClusterList();
    
	cout << "Moving ..." << endl;
    //model->moveClusterToHead(c2);
    //model->ClusterListPushBack(c2);
    
    model->printPvizClusterList();
    
    delete model;
	
	return 1;
}

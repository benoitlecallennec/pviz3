#ifndef PVIZMODEL_H
#define PVIZMODEL_H

#include <map>
#include <list>
#include <string>
#include <ostream>

#include "pvizcomm.h"
#include "pvizpoint.h"
#include "pvizcluster.h"
#include "pvizline.h"

#include "rapidxml.hpp"

#include <boost/shared_ptr.hpp>

#ifdef PVIZRPC_CLIENT_ENABLE
#include <boost/asio.hpp>
#include <google/protobuf/service.h>

#include "pvizrpc.pb.h"
#include "RpcChannelImp.h"
#include "RpcControllerImp.h"
#endif

//#include "pvizmessage.pb.h"

typedef std::map<unsigned int, PvizPoint*> PvizPointMap_t;
typedef std::map<unsigned int, PvizCluster*> PvizClusterMap_t;
typedef std::list<PvizCluster*> PvizClusterList_t;
typedef std::map<unsigned int, PvizLine*> PvizLineMap_t;

struct Point3D
{
    double x;
    double y;
    double z;
};

class PvizModel {
public:
	PvizModel();
	~PvizModel();
	
	enum SortStatus_t {
		SIZE = 0, SIZE_DESCENDING, 
        ID, ID_DESCENDING, 
        LABEL, LABEL_DESCENDING, 
        NONE
	};
    
    SortStatus_t sortStatus;
	PvizPointMap_t points;
    PvizClusterMap_t clusters;
    PvizClusterList_t clusterList;
    //PvizClusterList_t clusterNondefaultList;
    //PvizClusterList_t clusterDefaultList;
	PvizLineMap_t lines;
	PvizPointMap_t gridPoints;
    
    // PlotSetting
    std::string title;
    unsigned int pointsize;
    unsigned int linewidth;
    bool glyphvisible;
    double glyphscale;
    unsigned int focusmode;
    double focus[3];
	
	void loadCompressedDataFile(const char* filename);
	void loadSimpleDataFile(const char* filename);
	void loadXmlDataFile(const char* filename);
	void saveXmlDataFile(const char* filename);
	void saveSimpleDataFile(const char* filename);
    void printPvizClusterList();
	
    void insertCluster(PvizCluster* cluster);
    //void moveClusterToHead(PvizCluster* cluster);
    void setClusterDefault(PvizCluster* cluster, bool b);
    
    //void ClusterListPushFront(PvizCluster* cluster);
    //void ClusterListPushBack(PvizCluster* cluster);
    void ClusterListMoveFront(PvizCluster* cluster);
    void ClusterListMoveBack(PvizCluster* cluster);
    int GetNumberOfNonDefaultCluster();
    
    void SortClusterBySize(bool descending);
    void SortClusterById(bool descending);
    void SortClusterByLabel(bool descending);
    
    //static void loadPvizMessage(const cgl::pviz::rpc::PvizMessage& pvizmessage);

    void load(std::vector<int>& labels, std::vector<Point3D>& positions);
    void update(std::vector<Point3D>& positions);
    
protected:
	void loadSimpleDataFileReadColors(std::ifstream& file);
	void loadSimpleDataFileReadLabels(std::ifstream& file);
	void loadSimpleDataFileReadScales(std::ifstream& file);
	void loadSimpleDataFileReadLines(std::ifstream& file);
	void loadSimpleDataFileReadPoints(std::ifstream& file);
	void loadSimpleDataFileReadGridPoints(std::ifstream& file);
	
	void saveXmlDataFile_plot(rapidxml::xml_node<>* root, rapidxml::xml_document<>& doc);
	void saveXmlDataFile_clusters(rapidxml::xml_node<>* root, rapidxml::xml_document<>& doc);
	void saveXmlDataFile_points(rapidxml::xml_node<>* root, rapidxml::xml_document<>& doc);
	void saveXmlDataFile_lines(rapidxml::xml_node<>* root, rapidxml::xml_document<>& doc);
    
#ifdef PVIZMODEL_USE_HDF5
public:
	void loadHDF5DataFile(const char* h5filename,
						  const char* h5datasetname);
#endif
	
public:
	std::string toString() const
	{
		return format("Model : clusters(%d), points(%d), lines(%d), gridpoints(%d)",
					  clusters.size(), points.size(), lines.size(), gridPoints.size());
	};
	
	
	friend std::ostream& operator<<(std::ostream& out, const PvizModel& p)
	{
		out << p.toString();
		return out;
	};
	
#ifdef PVIZRPC_CLIENT_ENABLE
public:
	void connectPvizRPCServer(std::string dest, short port);
	void loadFromPvizRPCServer();
	void updateFromPvizRPCServer();
	
private:
	google::protobuf::RpcChannel* channel;
	google::protobuf::RpcController* controller;
	cgl::pviz::rpc::PvizRpcService* service;
	
	boost::asio::io_service io_service;	
#endif
	
};

#endif // PVIZMODEL_H

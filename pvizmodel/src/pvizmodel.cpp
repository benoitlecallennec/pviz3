#include "pvizmodel.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "rapidxml_print.hpp"


//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace boost::iostreams;

#ifdef PVIZMODEL_USE_HDF5
#include "hdf5.h"
#include "H5Cpp.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "pvizutil.h"
#endif

#include <cassert>

using namespace std;

PvizModel::PvizModel() :
sortStatus(NONE),
pointsize(2),
linewidth(2),
glyphvisible(false),
glyphscale(1.0),
focusmode(0)    // AUTO(0), ORIGIN(1)
{
    focus[0] = 0.0;
    focus[1] = 0.0;
    focus[2] = 0.0;
}

PvizModel::~PvizModel()
{
	for (PvizPointMap_t::iterator it = points.begin(); it != points.end(); it++) 
	{
		delete it->second;
	}
	
	for (PvizClusterMap_t::iterator it = clusters.begin(); it != clusters.end(); it++) 
	{
		delete it->second;
	}
	
	for (PvizLineMap_t::iterator it = lines.begin(); it != lines.end(); it++) 
	{
		delete it->second;
	}
}

#ifdef PVIZMODEL_USE_HDF5
void PvizModel::loadHDF5DataFile(const char* h5filename, 
								 const char* h5datasetname)
{
	H5::Exception::dontPrint();
	
	hsize_t dimM[2] = {0, 0};
	PvizUtil::H5GetDatasetDim(h5filename, h5datasetname, 2, dimM);
	
	hsize_t N = dimM[0];
	hsize_t L = dimM[1];
	
	hsize_t dimX[2] = {0, 0};
	PvizUtil::H5GetDatasetDim(h5filename, "X.dat", 2, dimX);
	
	hsize_t K = dimX[0];
	assert(L == dimX[1]);
	
	gsl_vector_int *vlb = gsl_vector_int_alloc(N);
	gsl_vector_int *vid = gsl_vector_int_alloc(N);
	gsl_matrix *mM = gsl_matrix_alloc(N, L);
	gsl_matrix *mX = gsl_matrix_alloc(K, L);
	
	PvizUtil::H5ReadByRow(h5filename, "M.dat", H5::PredType::NATIVE_DOUBLE, 0, mM->size1, mM->size2, mM->data);
	PvizUtil::H5ReadByRow(h5filename, "X.dat", H5::PredType::NATIVE_DOUBLE, 0, mX->size1, mX->size2, mX->data);
	
	if (PvizUtil::H5CheckDataset(h5filename, "lb.dat"))
	{
		PvizUtil::H5ReadByOffset(h5filename, "lb.dat", H5::PredType::NATIVE_INT, 0, vlb->size, vlb->data);
	}
	else 
	{
		for (size_t i = 0; i < N; i++) 
		{
			gsl_vector_int_set(vlb, i, 0);
		}
	}
	
	
	if (PvizUtil::H5CheckDataset(h5filename, "ID.dat"))
	{
		PvizUtil::H5ReadByOffset(h5filename, "ID.dat", H5::PredType::NATIVE_INT, 0, vid->size, vid->data);
	}
	else 
	{
		for (size_t i = 0; i < N; i++) 
		{
			gsl_vector_int_set(vid, i, i);
		}
	}
	
	for (size_t idx = 0; idx < N; idx++) 
	{
		//std::cerr << "idx = " << idx << std::endl;
		
		unsigned int pid = gsl_vector_int_get(vid, idx);
		//std::cerr << "pid = " << pid << std::endl;
		
		PvizPoint *point;
		PvizPointMap_t::iterator iter = points.find(pid);
		if (iter != points.end())
		{
			cerr << "[Warning] duplicated point : " << iter->first << endl;
			point = iter->second;
		}
		else 
		{
			point = new PvizPoint(pid);
			
			gsl_vector_view row = gsl_matrix_row(mM, idx);
			//std::cerr << "position = " << "(" << 
			row.vector.data[0] << "," <<
			row.vector.data[1] << "," <<
			row.vector.data[2] << ")" << std::endl;
			point->setPosition(row.vector.data);
			//std::cerr << "position = " << "(" << 
			//point->position[0] << "," <<
			//point->position[1] << "," <<
			//point->position[2] << ")" << std::endl;
			
			points.insert(std::make_pair(point->id, point));
		}
		
		unsigned int cid = gsl_vector_int_get(vlb, idx);
		//std::cerr << "cid = " << cid << std::endl;
		
		PvizCluster *cluster;
		PvizClusterMap_t::iterator citer = clusters.find(cid);
		if (citer != clusters.end())
		{
			cluster = citer->second;
		}
		else 
		{
			cluster = new PvizCluster(cid);
            insertCluster(cluster);
		}		
		cluster->points.push_back(point);
	}
	
	gsl_vector_int_free(vlb);
	gsl_vector_int_free(vid);
	gsl_matrix_free(mM);
	gsl_matrix_free(mX);
	
}
#endif

void PvizModel::saveXmlDataFile_plot(rapidxml::xml_node<> *root, rapidxml::xml_document<>& doc)
{
	char *node_plot_title_name = doc.allocate_string("title");
    char *node_plot_title_value = doc.allocate_string(title.c_str());
    rapidxml::xml_node<> *node_plot_title = doc.allocate_node(rapidxml::node_element, node_plot_title_name, node_plot_title_value);
    root->append_node(node_plot_title);
    
	char *node_plot_pointsize_name = doc.allocate_string("pointsize");
    char *node_plot_pointsize_value = doc.allocate_string(to_string<unsigned int>(pointsize).c_str());
    rapidxml::xml_node<> *node_plot_pointsize = doc.allocate_node(rapidxml::node_element, node_plot_pointsize_name, node_plot_pointsize_value);    
    root->append_node(node_plot_pointsize);
    
    // Need to add linewidth
    
	char *node_plot_glyph_name = doc.allocate_string("glyph");
    rapidxml::xml_node<> *node_plot_glyph = doc.allocate_node(rapidxml::node_element, node_plot_glyph_name);
    
	char *node_plot_glyph_visible_name = doc.allocate_string("visible");
    char *node_plot_glyph_visible_value = doc.allocate_string(to_string<bool>(glyphvisible).c_str());   		
    rapidxml::xml_node<> *node_plot_glyph_visible = doc.allocate_node(rapidxml::node_element, node_plot_glyph_visible_name, node_plot_glyph_visible_value);
    node_plot_glyph->append_node(node_plot_glyph_visible);
    
	char *node_plot_glyph_scale_name = doc.allocate_string("scale");
    char *node_plot_glyph_scale_value = doc.allocate_string(to_string<double>(glyphscale).c_str()); 
    rapidxml::xml_node<> *node_plot_glyph_scale = doc.allocate_node(rapidxml::node_element, node_plot_glyph_scale_name, node_plot_glyph_scale_value);
    node_plot_glyph->append_node(node_plot_glyph_scale);
    
    root->append_node(node_plot_glyph);

	char *node_plot_camera_name = doc.allocate_string("camera");
    rapidxml::xml_node<> *node_plot_camera = doc.allocate_node(rapidxml::node_element, node_plot_camera_name);
    
	char *node_plot_camera_focusmode_name = doc.allocate_string("focusmode");
    char *node_plot_camera_focusmode_value = doc.allocate_string(to_string<unsigned int>(focusmode).c_str());   		
    rapidxml::xml_node<> *node_plot_camera_focusmode = doc.allocate_node(rapidxml::node_element, node_plot_camera_focusmode_name, node_plot_camera_focusmode_value);
    node_plot_camera->append_node(node_plot_camera_focusmode);
    
	char *node_plot_camera_focus_name = doc.allocate_string("focus");
	char *node_plot_camera_focus_x_name = doc.allocate_string("x");
	char *node_plot_camera_focus_y_name = doc.allocate_string("y");
	char *node_plot_camera_focus_z_name = doc.allocate_string("z");
    
    rapidxml::xml_node<> *node_plot_camera_focus = doc.allocate_node(rapidxml::node_element, node_plot_camera_focus_name);
    
    char *node_plot_camera_focus_x_value = doc.allocate_string(to_string<double>(focus[0]).c_str());   		
    rapidxml::xml_attribute<> *attr_plot_camera_focus_x = doc.allocate_attribute(node_plot_camera_focus_x_name, node_plot_camera_focus_x_value);
    
    char *node_plot_camera_focus_y_value = doc.allocate_string(to_string<double>(focus[1]).c_str());   		
    rapidxml::xml_attribute<> *attr_plot_camera_focus_y = doc.allocate_attribute(node_plot_camera_focus_y_name, node_plot_camera_focus_y_value);

    char *node_plot_camera_focus_z_value = doc.allocate_string(to_string<double>(focus[2]).c_str());   		
    rapidxml::xml_attribute<> *attr_plot_camera_focus_z = doc.allocate_attribute(node_plot_camera_focus_z_name, node_plot_camera_focus_z_value);

    node_plot_camera_focus->append_attribute(attr_plot_camera_focus_x);
    node_plot_camera_focus->append_attribute(attr_plot_camera_focus_y);
    node_plot_camera_focus->append_attribute(attr_plot_camera_focus_z);

    node_plot_camera->append_node(node_plot_camera_focus);
    
    root->append_node(node_plot_camera);
}

void PvizModel::saveXmlDataFile_clusters(rapidxml::xml_node<> *root, rapidxml::xml_document<>& doc)
{
	char *node_cluster_name = doc.allocate_string("cluster");
	char *node_cluster_key_name = doc.allocate_string("key");
	char *node_cluster_label_name = doc.allocate_string("label");
	char *node_cluster_size_name = doc.allocate_string("size");
	char *node_cluster_visible_name = doc.allocate_string("visible");
	char *node_cluster_isdefault_name = doc.allocate_string("default");
	char *node_cluster_shape_name = doc.allocate_string("shape");
	
	char *node_cluster_color_name = doc.allocate_string("color");
	char *node_cluster_color_r_name = doc.allocate_string("r");
	char *node_cluster_color_g_name = doc.allocate_string("g");
	char *node_cluster_color_b_name = doc.allocate_string("b");
	char *node_cluster_color_a_name = doc.allocate_string("a");
	
	for (PvizClusterList_t::iterator it = clusterList.begin(); it != clusterList.end(); it++) 
	{
        PvizCluster* cluster = *it;
        
		rapidxml::xml_node<> *node_cluster = doc.allocate_node(rapidxml::node_element, node_cluster_name);
		
		char *node_cluster_key_value = doc.allocate_string(to_string<unsigned int>(cluster->id).c_str());   		
		rapidxml::xml_node<> *node_cluster_key = doc.allocate_node(rapidxml::node_element, node_cluster_key_name, node_cluster_key_value);
		
		char *node_cluster_label_value = doc.allocate_string(cluster->label.c_str());   		
		rapidxml::xml_node<> *node_cluster_label = doc.allocate_node(rapidxml::node_element, node_cluster_label_name, node_cluster_label_value);
		
		char *node_cluster_visible_value = doc.allocate_string(to_string<bool>(cluster->visible).c_str());   		
		rapidxml::xml_node<> *node_cluster_visible = doc.allocate_node(rapidxml::node_element, node_cluster_visible_name, node_cluster_visible_value);
		
		char *node_cluster_isdefault_value = doc.allocate_string(to_string<bool>(cluster->IsDefault()).c_str());   		
		rapidxml::xml_node<> *node_cluster_isdefault = doc.allocate_node(rapidxml::node_element, node_cluster_isdefault_name, node_cluster_isdefault_value);
		
		char *node_cluster_size_value = doc.allocate_string(to_string<unsigned int>(cluster->GetScale()).c_str());   		
		rapidxml::xml_node<> *node_cluster_size = doc.allocate_node(rapidxml::node_element, node_cluster_size_name, node_cluster_size_value);
		
		char *node_cluster_shape_value = doc.allocate_string(to_string<unsigned int>(cluster->shape).c_str());   		
		rapidxml::xml_node<> *node_cluster_shape = doc.allocate_node(rapidxml::node_element, node_cluster_shape_name, node_cluster_shape_value);
        
		rapidxml::xml_node<> *node_cluster_color = doc.allocate_node(rapidxml::node_element, node_cluster_color_name);
		
		char *node_cluster_color_r_value = doc.allocate_string(to_string<unsigned int>(cluster->GetColor()->uc_color.r).c_str());   		
		rapidxml::xml_attribute<> *attr_cluster_color_r = doc.allocate_attribute(node_cluster_color_r_name, node_cluster_color_r_value);
		
		char *node_cluster_color_g_value = doc.allocate_string(to_string<unsigned int>(cluster->GetColor()->uc_color.g).c_str());   		
		rapidxml::xml_attribute<> *attr_cluster_color_g = doc.allocate_attribute(node_cluster_color_g_name, node_cluster_color_g_value);
		
		char *node_cluster_color_b_value = doc.allocate_string(to_string<unsigned int>(cluster->GetColor()->uc_color.b).c_str());   		
		rapidxml::xml_attribute<> *attr_cluster_color_b = doc.allocate_attribute(node_cluster_color_b_name, node_cluster_color_b_value);
		
		char *node_cluster_color_a_value = doc.allocate_string(to_string<unsigned int>(cluster->GetColor()->uc_color.a).c_str());   		
		rapidxml::xml_attribute<> *attr_cluster_color_a = doc.allocate_attribute(node_cluster_color_a_name, node_cluster_color_a_value);
		
		node_cluster_color->append_attribute(attr_cluster_color_r);
		node_cluster_color->append_attribute(attr_cluster_color_g);
		node_cluster_color->append_attribute(attr_cluster_color_b);
		node_cluster_color->append_attribute(attr_cluster_color_a);
		
		node_cluster->append_node(node_cluster_key);
		node_cluster->append_node(node_cluster_label);
		node_cluster->append_node(node_cluster_visible);
		node_cluster->append_node(node_cluster_isdefault);
		node_cluster->append_node(node_cluster_color);
		node_cluster->append_node(node_cluster_size);
		node_cluster->append_node(node_cluster_shape);
		root->append_node(node_cluster);
	}
}

void PvizModel::saveXmlDataFile_points(rapidxml::xml_node<> *root, rapidxml::xml_document<>& doc)
{
	char *node_point_name = doc.allocate_string("point");
	char *node_point_key_name = doc.allocate_string("key");
	char *node_point_clusterkey_name = doc.allocate_string("clusterkey");
	char *node_point_label_name = doc.allocate_string("label");
	char *node_point_location_name = doc.allocate_string("location");
	
	char *node_point_location_x_name = doc.allocate_string("x");
	char *node_point_location_y_name = doc.allocate_string("y");
	char *node_point_location_z_name = doc.allocate_string("z");
	
	for (PvizClusterList_t::iterator cit = clusterList.begin(); cit != clusterList.end(); cit++) 
	{
        PvizCluster* cluster = *cit;
        
		for (PvizCluster::PvizPointVec_t::iterator pit = cluster->points.begin(); pit != cluster->points.end(); pit++) 
		{
			rapidxml::xml_node<> *node_point = doc.allocate_node(rapidxml::node_element, node_point_name);
			
			char *node_point_key_value = doc.allocate_string(to_string<unsigned int>((*pit)->id).c_str());   		
			rapidxml::xml_node<> *node_point_key = doc.allocate_node(rapidxml::node_element, node_point_key_name, node_point_key_value);
			
			char *node_point_clusterkey_value = doc.allocate_string(to_string<unsigned int>(cluster->id).c_str());   		
			rapidxml::xml_node<> *node_point_clusterkey = doc.allocate_node(rapidxml::node_element, node_point_clusterkey_name, node_point_clusterkey_value);
			
			char *node_point_label_value = doc.allocate_string((*pit)->label.c_str());   		
			rapidxml::xml_node<> *node_point_label = doc.allocate_node(rapidxml::node_element, node_point_label_name, node_point_label_value);
			
			rapidxml::xml_node<> *node_point_location = doc.allocate_node(rapidxml::node_element, node_point_location_name);
			
			char *node_point_location_x_value = doc.allocate_string(to_string<double>((*pit)->position[0]).c_str());   		
			rapidxml::xml_attribute<> *attr_point_location_x = doc.allocate_attribute(node_point_location_x_name, node_point_location_x_value);
			
			char *node_point_location_y_value = doc.allocate_string(to_string<double>((*pit)->position[1]).c_str());   		
			rapidxml::xml_attribute<> *attr_point_location_y = doc.allocate_attribute(node_point_location_y_name, node_point_location_y_value);
			
			char *node_point_location_z_value = doc.allocate_string(to_string<double>((*pit)->position[2]).c_str());   		
			rapidxml::xml_attribute<> *attr_point_location_z = doc.allocate_attribute(node_point_location_z_name, node_point_location_z_value);
			
			node_point_location->append_attribute(attr_point_location_x);
			node_point_location->append_attribute(attr_point_location_y);
			node_point_location->append_attribute(attr_point_location_z);
			
			node_point->append_node(node_point_key);
			node_point->append_node(node_point_clusterkey);
			node_point->append_node(node_point_label);
			node_point->append_node(node_point_location);
			root->append_node(node_point);
		}
	}	
}

void PvizModel::saveXmlDataFile_lines(rapidxml::xml_node<> *root, rapidxml::xml_document<>& doc)
{
	char *node_edge_name = doc.allocate_string("edge");
	char *node_edge_key_name = doc.allocate_string("key");
 	char *node_edge_vertices_name = doc.allocate_string("vertices");
 	char *node_edge_vertex_name = doc.allocate_string("vertex");
	
	for (PvizLineMap_t::iterator lit = lines.begin(); lit != lines.end(); lit++) 
	{
		rapidxml::xml_node<> *node_edge = doc.allocate_node(rapidxml::node_element, node_edge_name);
		
		char *node_edge_key_value = doc.allocate_string(to_string<unsigned int>(lit->second->id).c_str());   		
		rapidxml::xml_node<> *node_edge_key = doc.allocate_node(rapidxml::node_element, node_edge_key_name, node_edge_key_value);
		
		rapidxml::xml_node<> *node_edge_vertices = doc.allocate_node(rapidxml::node_element, node_edge_vertices_name);
		
		for (PvizLine::PvizPointVec_t::iterator pit = lit->second->points.begin(); pit != lit->second->points.end(); pit++) 
		{
			rapidxml::xml_node<> *node_vertex = doc.allocate_node(rapidxml::node_element, node_edge_vertex_name);
			
			char *node_vertex_key_value = doc.allocate_string(to_string<unsigned int>((*pit)->id).c_str());   		
			rapidxml::xml_attribute<> *attr_vertex_key = doc.allocate_attribute(node_edge_key_name, node_vertex_key_value);
			
			node_vertex->append_attribute(attr_vertex_key);
			
			node_edge_vertices->append_node(node_vertex);
		}
		
		node_edge->append_node(node_edge_key);
		node_edge->append_node(node_edge_vertices);
		root->append_node(node_edge);
	}	
}

void PvizModel::saveXmlDataFile(const char* filename)
{
	
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<> *node_top = doc.allocate_node(rapidxml::node_element, "plotviz");
	rapidxml::xml_node<> *node_plot = doc.allocate_node(rapidxml::node_element, "plot");
	rapidxml::xml_node<> *node_clusters = doc.allocate_node(rapidxml::node_element, "clusters");
	rapidxml::xml_node<> *node_points = doc.allocate_node(rapidxml::node_element, "points");
	rapidxml::xml_node<> *node_lines = doc.allocate_node(rapidxml::node_element, "edges");
	
	saveXmlDataFile_plot(node_plot, doc);
	saveXmlDataFile_clusters(node_clusters, doc);
	saveXmlDataFile_points(node_points, doc);
	saveXmlDataFile_lines(node_lines, doc);
	
	node_top->append_node(node_plot);
	node_top->append_node(node_clusters);
	node_top->append_node(node_points);
	node_top->append_node(node_lines);
	
	doc.append_node(node_top);
	
	//char *node_name = doc.allocate_string(name);        // Allocate string and copy name into it
	//xml_node<> *node = doc.allocate_node(node_element, node_name);  // Set node name to node_name
	//string name = "test";
	//char *node_name = doc.allocate_string(name.c_str());        // Allocate string and copy name into it
	//rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, node_name, "Google");  // Set node name to node_name
	//rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, node_name);  // Set node name to node_name
	//doc.append_node(node);
	
	/*
	 rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, "a", "Google");
	 doc.append_node(node);
	 rapidxml::xml_attribute<> *attr = doc.allocate_attribute("href", "google.com");
	 node->append_attribute(attr);
	 */
	
	streambuf *psbuf, *backup;
	std::ofstream filestr;
	filestr.open (filename, std::ios::trunc);
	
	backup = std::cout.rdbuf();     // back up cout's streambuf
	
	psbuf = filestr.rdbuf();   // get file's streambuf
	std::cout.rdbuf(psbuf);         // assign streambuf to cout
	
	std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
	
	//rapidxml::print(std::cout, doc, rapidxml::print_no_indenting); 
	rapidxml::print(std::cout, doc); 
	
	cout.rdbuf(backup);        // restore cout's original streambuf
	
	filestr.close();
	
	/*
	 // Print to stream using operator <<
	 std::stringstream sStream;
	 sStream << doc.first_node();
	 
	 std::ofstream fs;
	 fs.open(filename, std::ios::trunc);
	 //rapidxml::print(std::cout, doc, 0);
	 //ofFileToWrite.open(CONF, std::ios::trunc);
	 fs << "<?xml version=\"1.0\"?>\n" << sStream.str() << '\0';
	 fs.close();
	 */
}

void PvizModel::saveSimpleDataFile(const char* filename)
{
    std::ofstream fs;
    fs.open(filename, std::ios::trunc);
    //rapidxml::print(std::cout, doc, 0);
    //ofFileToWrite.open(CONF, std::ios::trunc);

    typedef std::map<int, int> map_t;
    map_t mmap;  
    BOOST_FOREACH( map_t::value_type &i, mmap )
    i.second++;
    
    
    BOOST_FOREACH( PvizClusterMap_t::value_type &i, clusters )
    {
        BOOST_FOREACH( PvizPoint *point, i.second->points)
        {
            fs << boost::format("%1% %2% %3% %4% %5%\n") 
            % point->id 
            % point->position[0] 
            % point->position[1] 
            % point->position[2] 
            % i.second->id;
        }        
    }
    
    fs.close();
}

void PvizModel::loadXmlDataFile(const char* filename)
{
    title = boost::filesystem::basename (filename);
	std::ifstream fs(filename);
	
	std::string xml;
	std::string line;
	
	while(std::getline(fs,line))
	{
		xml += line;
	}
	
	boost::scoped_array<char> writable(new char[xml.size() + 1]);
	std::copy(xml.begin(), xml.end(), writable.get());
	writable[xml.size()] = '\0'; // don't forget the terminating 0
	
    //Parse the original document
    rapidxml::xml_document<> doc;
    doc.parse<0>(writable.get());
	
    // Plot
    rapidxml::xml_node<>* node = doc.first_node()->first_node("plot");
    
    if (node != 0)
    {
        if (node->first_node("title") != 0)
        {
            title = node->first_node("title")->value();
        }
        
        if (node->first_node("pointsize") != 0)
        {
            if (!from_string<unsigned int>(pointsize, node->first_node("pointsize")->value()))
            {
                cerr << "[Warning] pointsize is not numeric : " << node->first_node("pointsize")->value() << endl;
            }
        }
        
        if (node->first_node("glyph") != 0)
        {
            if (node->first_node("glyph")->first_node("visible") != 0)
            {
                if (!from_string<bool>(glyphvisible, node->first_node("glyph")->first_node("visible")->value()))
                {
                    cerr << "[Warning] visible is not numeric : " << node->first_node("glyph")->first_node("visible")->value() << endl;
                }
            }
            
            if (node->first_node("glyph")->first_node("scale") != 0)
            {
                if (!from_string<double>(glyphscale, node->first_node("glyph")->first_node("scale")->value()))
                {
                    cerr << "[Warning] visible is not numeric : " << node->first_node("glyph")->first_node("scale")->value() << endl;
                }
            }
        }
        
        if (node->first_node("camera") != 0)
        {
            if (node->first_node("camera")->first_node("focusmode") != 0)
            {
                if (!from_string<unsigned int>(focusmode, node->first_node("camera")->first_node("focusmode")->value()))
                {
                    cerr << "[Warning] focusmode is not numeric : " << node->first_node("camera")->first_node("focusmode")->value() << endl;
                }
            }
            
            if (node->first_node("camera")->first_node("focus") != 0)
            {
				if (!from_string<double>(focus[0], node->first_node("camera")->first_node("focus")->first_attribute("x")->value()))
				{
                    cerr << "[Warning] focus x is not numeric : " << node->first_node("camera")->first_node("focus")->first_attribute("x")->value() << endl;
				}
                
				if (!from_string<double>(focus[1], node->first_node("camera")->first_node("focus")->first_attribute("y")->value()))
				{
                    cerr << "[Warning] focus x is not numeric : " << node->first_node("camera")->first_node("focus")->first_attribute("y")->value() << endl;
				}
                
				if (!from_string<double>(focus[2], node->first_node("camera")->first_node("focus")->first_attribute("z")->value()))
				{
                    cerr << "[Warning] focus x is not numeric : " << node->first_node("camera")->first_node("focus")->first_attribute("z")->value() << endl;
				}
            }
        }
    }
    

	// Cluster
	for (rapidxml::xml_node<>* node = doc.first_node()->first_node("clusters")->first_node("cluster");
		 node; node = node->next_sibling())
	{
		unsigned int cid;
		if (node->first_node("key") != 0)
		{
			if (!from_string<unsigned int>(cid, node->first_node("key")->value()))
			{
				cerr << "[Warning] id is not numeric : " << node->first_node("key")->value() << endl;
				continue;
			}
		}
		
		if (node->first_attribute("key") != 0)
		{
			if (!from_string<unsigned int>(cid, node->first_attribute("key")->value()))
			{
				cerr << "[Warning] id is not numeric : " << node->first_attribute("key")->value() << endl;
				continue;
			}
		}
		
		
		cerr << "cid : " << cid << endl;
		
		PvizClusterMap_t::iterator citer = clusters.find(cid);
		if (citer != clusters.end())
		{
			// Already exist
			cerr << "[Warning] duplicated cluster : " << cid << endl;
			continue;
		}
		else
		{
			PvizCluster *cluster = new PvizCluster(cid);
			
			try 
			{
				cluster->label = node->first_node("label")->value();
				
				if (node->first_node("size") != 0)
				{
                    float scale;
					if (!from_string<float>(scale, node->first_node("size")->value()))
					{
						throw std::runtime_error("Parse error");
					}
                    cluster->SetScale(scale);
				}
				
				if (node->first_node("scale") != 0)
				{
                    float scale;
					if (!from_string<float>(scale, node->first_node("scale")->value()))
					{
						throw std::runtime_error("Parse error");
					}
                    cluster->SetScale(scale);
				}
				
				if (node->first_node("visible") != 0)
				{
					if (!from_string<bool>(cluster->visible, node->first_node("visible")->value()))
					{
						throw std::runtime_error("Parse error");
					}
				}
                
				if (node->first_node("default") != 0)
				{
                    bool isDefault;
					if (!from_string<bool>(isDefault, node->first_node("default")->value()))
					{
						throw std::runtime_error("Parse error");
					}
                    cluster->SetAsDefault(isDefault);
				}
                
				if (node->first_node("shape") != 0)
				{
                    int shape;
					if (!from_string<int>(shape, node->first_node("shape")->value()))
					{
						throw std::runtime_error("Parse error");
					}
                    cluster->shape = shape;
				}
				
				unsigned int rgba[4];
				if (!from_string<unsigned int>(rgba[0], node->first_node("color")->first_attribute("r")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				cluster->GetColor()->uc_color.r = rgba[0];
				
				if (!from_string<unsigned int>(rgba[1], node->first_node("color")->first_attribute("g")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				cluster->GetColor()->uc_color.g = rgba[1];
				
				if (!from_string<unsigned int>(rgba[2], node->first_node("color")->first_attribute("b")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				cluster->GetColor()->uc_color.b = rgba[2];
				
				if (!from_string<unsigned int>(rgba[3], node->first_node("color")->first_attribute("a")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				cluster->GetColor()->uc_color.a = rgba[3];
				
                insertCluster(cluster);
			}
			catch (std::exception &e) 
			{
				cerr << "[Warning] cluster (cid = " << cid << " ) error : "<< e.what() << endl;
				delete cluster;
			}
		}
	}
    
	// Point
	for (rapidxml::xml_node<>* node = doc.first_node()->first_node("points")->first_node("point");
		 node; node = node->next_sibling())
	{
		unsigned int pid;
		if (node->first_node("key") != 0)
		{
			if (!from_string<unsigned int>(pid, node->first_node("key")->value()))
			{
				cerr << "[Warning] id is not numeric : " << node->first_node("key")->value() << endl;
				continue;
			}
		}
		
		if (node->first_attribute("key") != 0)
		{
			if (!from_string<unsigned int>(pid, node->first_attribute("key")->value()))
			{
				cerr << "[Warning] id is not numeric : " << node->first_attribute("key")->value() << endl;
				continue;
			}
		}
		
		//cerr << "pid : " << pid << endl;
		
		PvizPointMap_t::iterator piter = points.find(pid);
		if (piter != points.end())
		{
			// Already exist
			cerr << "[Warning] duplicated point : " << pid << endl;
			continue;
		}
		else
		{
			PvizPoint *point = new PvizPoint(pid);
			
			try 
			{
				point->label = node->first_node("label")->value();
				
				if (!from_string<double>(point->position[0], node->first_node("location")->first_attribute("x")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				
				if (!from_string<double>(point->position[1], node->first_node("location")->first_attribute("y")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				
				if (!from_string<double>(point->position[2], node->first_node("location")->first_attribute("z")->value()))
				{
					throw std::runtime_error("Parse error");
				}
				
				unsigned int cid;
				if (node->first_node("clusterkey") != 0)
				{
					if (!from_string<unsigned int>(cid, node->first_node("clusterkey")->value()))
					{
						throw std::runtime_error("Parse error");
					}
				}
				
				if (node->first_attribute("clusterkey") != 0)
				{
					if (!from_string<unsigned int>(cid, node->first_attribute("clusterkey")->value()))
					{
						throw std::runtime_error("Parse error");
					}
				}
				
				points.insert(std::make_pair(pid, point));				
				
				if (clusters.find(cid) == clusters.end())
				{
					throw std::runtime_error("No existing cluster");
				}
				
				clusters[cid]->points.push_back(point);
				
			}
			catch (std::exception &e) 
			{
				cerr << "[Warning] point (pid = " << pid << " ) error : "<< e.what() << endl;
				delete point;
			}
		}
	}
	
	// Edge
	if (doc.first_node()->first_node("edges") == 0)
		return;
	
	for (rapidxml::xml_node<>* node = doc.first_node()->first_node("edges")->first_node("edge");
		 node; node = node->next_sibling())
	{
		unsigned int lid;
		if (!from_string<unsigned int>(lid, node->first_node("key")->value()))
		{
			cerr << "[Warning] id is not numeric : " << node->first_node("key")->value() << endl;
			continue;
		}
		
		cerr << "lid : " << lid << endl;
		
		PvizLineMap_t::iterator liter = lines.find(lid);
		if (liter != lines.end())
		{
			// Already exist
			cerr << "[Warning] duplicated line : " << lid << endl;
			continue;
		}
		else 
		{
			PvizLine *line = new PvizLine(lid);
			
			try 
			{
				//std::cerr << "vertices : " << node->first_node("vertices")->name() << std::endl;
				for (rapidxml::xml_node<>* subnode = node->first_node("vertices")->first_node("vertex");
					 subnode; subnode = subnode->next_sibling())
				{
					unsigned int pid;
					if (!from_string<unsigned int>(pid, subnode->first_attribute("key")->value()))
					{
						throw std::runtime_error("Parse error");
					}
					
					if (points.find(pid) == points.end())
					{
						throw std::runtime_error("No existing point");
					}
					
					line->points.push_back(points[pid]);
				}
				
			}
			catch (std::exception &e) 
			{
				cerr << "[Warning] line (lid = " << lid << " ) error : "<< e.what() << endl;
				delete line;
			}
			
			lines.insert(std::make_pair(lid, line));
		}
	}
	
}

void PvizModel::loadSimpleDataFileReadGridPoints(std::ifstream& file)
{
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			break;
		
		unsigned int pid;
		if (!from_string<unsigned int>(pid, tokens[0]))
		{
			cerr << "[Warning] id is not numeric : " << tokens[0] << endl;
			continue;
		}
		
		PvizPoint *point;
		PvizPointMap_t::iterator iter = gridPoints.find(pid);
		if (iter != gridPoints.end())
		{
			cerr << "[Warning] duplicated point : " << iter->first << endl;
			point = iter->second;
		}
		else 
		{
			point = new PvizPoint(pid);
			double p;
			if (from_string<double>(p, tokens[1]))
			{
				point->position[0] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			
			if (from_string<double>(p, tokens[2]))
			{
				point->position[1] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			
			if (from_string<double>(p, tokens[3]))
			{
				point->position[2] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			gridPoints.insert(std::pair<unsigned int, PvizPoint*>(point->id, point));
		}		
	}
}

void PvizModel::loadSimpleDataFileReadLabels(std::ifstream& file)
{
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);
		
		Tokenize(line, tokens, " \t\r");
		
		if ( tokens.size() < 1)
			break;
		
		try 
		{
			unsigned int pid;
			if (!from_string<unsigned int>(pid, tokens[0]))
			{
				throw std::runtime_error("Parse error");
			}
			
			string label = quote2string(tokens, 1);
			
			if (points.find(pid) == points.end())
			{
				throw std::runtime_error("No existing cluster");
			}
			
			points[pid]->label = label;
		}
		catch (std::exception &e) 
		{
			cerr << "[Warning] parsing error : " << e.what() << endl;
		}
	}
}

void PvizModel::loadSimpleDataFileReadColors(std::ifstream& file)
{
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			break;
		
		try 
		{
			unsigned int cid;
			if (!from_string<unsigned int>(cid, tokens[0]))
			{
				throw std::runtime_error("Parse error");
			}
			
			unsigned int rgba[4];
			if (!from_string<unsigned int>(rgba[0], tokens[1]))
			{
				throw std::runtime_error("Parse error");
			}
			
			if (!from_string<unsigned int>(rgba[1], tokens[2]))
			{
				throw std::runtime_error("Parse error");
			}
			
			if (!from_string<unsigned int>(rgba[2], tokens[3]))
			{
				throw std::runtime_error("Parse error");
			}
			
			rgba[3] = 255;
			if (tokens.size() > 4)
			{
				if (!from_string<unsigned int>(rgba[3], tokens[4]))
				{
					throw std::runtime_error("Parse error");
				}
			}
			
			if (clusters.find(cid) == clusters.end())
			{
				throw std::runtime_error("No existing cluster");
			}
			
			clusters[cid]->GetColor()->uc_color.r = rgba[0];
			clusters[cid]->GetColor()->uc_color.g = rgba[1];
			clusters[cid]->GetColor()->uc_color.b = rgba[2];
			clusters[cid]->GetColor()->uc_color.a = rgba[3];			
		}
		catch (std::exception &e) 
		{
			cerr << "[Warning] parsing error : " << e.what() << endl;
		}
	}
}

void PvizModel::loadSimpleDataFileReadScales(std::ifstream& file)
{
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			break;
		
		try 
		{
			unsigned int cid;
			if (!from_string<unsigned int>(cid, tokens[0]))
			{
				throw std::runtime_error("Parse error");
			}
			
			float scale;
			if (!from_string<float>(scale, tokens[1]))
			{
				throw std::runtime_error("Parse error");
			}
			
			if (clusters.find(cid) == clusters.end())
			{
				throw std::runtime_error("No existing cluster");
			}
			
			clusters[cid]->SetScale(scale);
		}
		catch (std::exception &e) 
		{
			cerr << "[Warning] parsing error : " << e.what() << endl;
		}
	}
}

void PvizModel::loadSimpleDataFileReadPoints(std::ifstream& file)
{
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			break;
		
		unsigned int pid;
		if (!from_string<unsigned int>(pid, tokens[0]))
		{
			cerr << "[Warning] id is not numeric : " << tokens[0] << endl;
			continue;
		}
		
		PvizPoint *point;
		PvizPointMap_t::iterator iter = points.find(pid);
		if (iter != points.end())
		{
			cerr << "[Warning] duplicated point : " << iter->first << endl;
			point = iter->second;
		}
		else 
		{
			point = new PvizPoint(pid);
			double p;
			if (from_string<double>(p, tokens[1]))
			{
				point->position[0] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			
			if (from_string<double>(p, tokens[2]))
			{
				point->position[1] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			
			if (from_string<double>(p, tokens[3]))
			{
				point->position[2] = p;
			}
			else 
			{
				delete point;
				continue;
			}
			points.insert(std::pair<unsigned int, PvizPoint*>(point->id, point));
		}
		
		unsigned int cid;
		if (!from_string<unsigned int>(cid, tokens[4]))
		{
			delete point;
			continue;
		}
		
		if (tokens.size() > 5)
		{
			point->label = quote2string(tokens, 5);
		}
        /*
         // No default label anymore
		else 
		{
			point->label = to_string<unsigned int>(point->id);
		}
		*/
		
		PvizCluster *cluster;
		PvizClusterMap_t::iterator citer = clusters.find(cid);
		if (citer != clusters.end())
		{
			cluster = citer->second;
		}
		else 
		{
			cluster = new PvizCluster(cid);
            insertCluster(cluster);
		}		
		cluster->points.push_back(point);
	}
}

void PvizModel::loadSimpleDataFileReadLines(std::ifstream& file)
{
	
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		getline(file, line);	
		
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			break;
		
		unsigned int lid;
		if (!from_string<unsigned int>(lid, tokens[0]))
		{
			cerr << "[Warning] id is not numeric : " << tokens[0] << endl;
			continue;
		}
		
		PvizLine *pvizline = new PvizLine(lid);
		for (size_t i = 1; i < tokens.size(); i++) 
		{
			PvizPoint *point;
			
			unsigned int pid;
			if (!from_string<unsigned int>(pid, tokens[i]))
			{
				cerr << "[Warning] id is not numeric : " << tokens[i] << endl;
				delete pvizline;
				continue;
			}
			
			PvizPointMap_t::iterator iter = points.find(pid);
			if (iter == points.end())
			{
				cerr << "[Warning] no point exists : " << iter->first << endl;
				delete pvizline;
				continue;
			}
			point = iter->second;
			pvizline->points.push_back(point);
		}
		
		lines.insert(std::make_pair(lid, pvizline));
	}
}

void PvizModel::loadCompressedDataFile(const char* filename)
{
    using namespace boost::filesystem;
    
    std::ifstream file(filename, ios_base::in | ios_base::binary);
    filtering_streambuf<input> in;
    in.push(gzip_decompressor());
    in.push(file);
    
    //title = basename(filename);
    
    path base = basename(filename);
    path newpath = temp_directory_path() / base;
    
    cout << "decompress ... " << newpath << endl;
    
    std::ofstream out(newpath.native().c_str(), ios_base::out | ios_base::binary);
    boost::iostreams::copy(in, out);
    
    file.close();
    
    string ext = base.extension().string();
    
    if (ext == ".txt")
    {
       this->loadSimpleDataFile(newpath.string().c_str());
    }
    else if (ext == ".pviz")
    {
        this->loadXmlDataFile(newpath.string().c_str());
    }
    else if (ext == ".h5")
    {
#ifdef PVIZMODEL_USE_HDF5
        this->LoadHDF5DataFile(newpath.string().c_str());
#else
        throw std::runtime_error("Under construction. Net yet supported.");
#endif
    }
    else
    {
        throw std::runtime_error("Under construction. Net yet supported.");
    }
    
    remove(newpath);
    
    return;
}

void PvizModel::loadSimpleDataFile(const char* filename)
{
    using namespace boost::filesystem;

    std::ifstream file(filename, ios_base::in | ios_base::binary);
    title = basename(filename);
    
    /*
    if (extension(filename) == ".gz")
    {
        path filename_ = basename(filename);
        
        filtering_streambuf<input> in;
        in.push(gzip_decompressor());
        in.push(file);
        
        path newpath = temp_directory_path();
        
        newpath /= filename_;
        cout << "decompress ... " << newpath << endl;
        
        std::ofstream out(newpath.native().c_str(), ios_base::out | ios_base::binary);
        boost::iostreams::copy(in, out);
        
        file.close();
        
        title = basename(basename(newpath));
        file.open(newpath.native().c_str(), ios_base::in | ios_base::binary);
    }
     */
    
	if (!file.is_open())
		throw std::runtime_error("Failed to open file");
	    
	while (!file.eof())
	{
		std::string line;
		std::vector<std::string> tokens;
		
		int pos = file.tellg();
		getline(file, line);
		
		if (line[0] == '#')
			continue;
		
		Tokenize(line, tokens, " \t\r");
		
		if (tokens.size() < 1)
			continue;
		
		
		if (tokens.size() >= 5)
		{
			file.seekg(pos, ios::beg);
			loadSimpleDataFileReadPoints(file);
		}
		
		if (tokens[0].compare("POINTS") == 0)
		{
			loadSimpleDataFileReadPoints(file);
		}
		
		if (tokens[0].compare("LINES") == 0)
		{
			loadSimpleDataFileReadLines(file);
		}
		
		if (tokens[0].compare("SCALES") == 0)
		{
			loadSimpleDataFileReadScales(file);
		}
		
		if (tokens[0].compare("COLORS") == 0)
		{
			loadSimpleDataFileReadColors(file);
		}
		
		if (tokens[0].compare("POINTLABELS") == 0)
		{
			loadSimpleDataFileReadLabels(file);
		}
		
		if (tokens[0].compare("GRIDPOINTS") == 0)
		{
			loadSimpleDataFileReadGridPoints(file);
		}
	}
	
	file.close();
}

void PvizModel::insertCluster(PvizCluster* cluster)
{
    clusters.insert(std::make_pair(cluster->id, cluster));
    
    PvizClusterList_t::iterator it = find(clusterList.begin(), clusterList.end(), cluster);
    if (it == clusterList.end())
    {
        clusterList.push_back(cluster);
        //ClusterListPushBack(cluster);
        /*
        if (cluster->IsDefault()) 
        {
            clusterList.push_back(cluster);
        }
        else
        {
            PvizClusterList_t::iterator iit = find_if(clusterList.begin(), clusterList.end(), boost::bind(&PvizCluster::IsDefault, _1));
            //std::cout << "Found first non-default" << *iit << std::endl;
            clusterList.insert(iit, cluster);
        }
        */
    }
    else
    {
        throw std::runtime_error("Duplicated cluster");
    }                
}

/*
void PvizModel::moveClusterToHead(PvizCluster* cluster)
{
    PvizClusterList_t::iterator it = find(clusterList.begin(), clusterList.end(), cluster);
    if (it != clusterList.end())
    {
        cout << "Moving ... " << cluster->id << " " << clusterList.size() << endl ;
        clusterList.erase(it);
        clusterList.push_front(cluster);
        cout << "Done :" << clusterList.size() << endl;
    }
}
 */

void PvizModel::printPvizClusterList()
{
    for (PvizClusterList_t::iterator it = clusterList.begin(); it != clusterList.end(); it++)
    {
        PvizCluster* cluster = *it;
        cout << cluster->toString() << endl;
    }
}

void PvizModel::setClusterDefault(PvizCluster* cluster, bool b)
{
    cluster->SetAsDefault(b);
}
/*
void PvizModel::ClusterListPushFront(PvizCluster* cluster)
{
    if (cluster->IsDefault())
    {
        PvizClusterList_t::iterator iit = find_if(clusterList.begin(), clusterList.end(), boost::bind(&PvizCluster::IsDefault, _1));
        clusterList.insert(iit, cluster);
    }
    else
    {
        clusterList.push_front(cluster);
    }
}

void PvizModel::ClusterListPushBack(PvizCluster* cluster)
{
    if (cluster->IsDefault())
    {
        clusterList.push_back(cluster);
    }
    else
    {
        PvizClusterList_t::iterator iit = find_if(clusterList.begin(), clusterList.end(), boost::bind(&PvizCluster::IsDefault, _1));
        clusterList.insert(iit, cluster);
    }
}
 */

void PvizModel::ClusterListMoveFront(PvizCluster* cluster)
{
    PvizClusterList_t::iterator it = find(clusterList.begin(), clusterList.end(), cluster);
    if (it != clusterList.end())
    {
        clusterList.erase(it);
        //ClusterListPushFront(cluster);
        clusterList.push_front(cluster);
    }
}

void PvizModel::ClusterListMoveBack(PvizCluster* cluster)
{
    PvizClusterList_t::iterator it = find(clusterList.begin(), clusterList.end(), cluster);
    if (it != clusterList.end())
    {
        clusterList.erase(it);
        //ClusterListPushBack(cluster);
        clusterList.push_back(cluster);
    }
}

int PvizModel::GetNumberOfNonDefaultCluster()
{
    int count = 0;
    for (PvizClusterList_t::iterator it = clusterList.begin(); it != clusterList.end(); it++)
    {
        PvizCluster* cluster = *it;
        if (!cluster->IsDefault()) 
        {
            count++;
        }
    }
    
    return count;
}

void PvizModel::SortClusterBySize(bool descending)
{
    clusterList.sort( boost::bind(&PvizCluster::CompareBySize, _1, _2, descending));
    sortStatus = descending ? SIZE_DESCENDING : SIZE;
}

void PvizModel::SortClusterById(bool descending)
{
    clusterList.sort( boost::bind(&PvizCluster::CompareById, _1, _2, descending));
    sortStatus = descending ? ID_DESCENDING : ID;
}

void PvizModel::SortClusterByLabel(bool descending)
{
    clusterList.sort( boost::bind(&PvizCluster::CompareByLabel, _1, _2, descending));
    sortStatus = descending ? LABEL_DESCENDING : LABEL;
}


#ifdef PVIZRPC_CLIENT_ENABLE
void PvizModel::connectPvizRPCServer(std::string dest, short port)
{
	channel = new cgl::protorpc::RpcChannelImp(io_service, dest, port);
	controller = new cgl::protorpc::RpcControllerImp();	
	service = new cgl::pviz::rpc::PvizRpcService::Stub(channel);
}

void PvizModel::loadFromPvizRPCServer()
{
	if (channel == NULL) 
	{
		return;
	}
	
	cgl::pviz::rpc::VoidMessage voidMessage;
	cgl::pviz::rpc::ListInt ids;
	cgl::pviz::rpc::ListInt lbs;
	cgl::pviz::rpc::ListPosition positions;
	
	service->getIds(controller, &voidMessage, &ids, google::protobuf::NewCallback(&google::protobuf::DoNothing));	
	io_service.run();
	io_service.reset();
	
	service->getLabels(controller, &voidMessage, &lbs, google::protobuf::NewCallback(&google::protobuf::DoNothing));
	io_service.run();
	io_service.reset();
	
	service->getPositions(controller, &voidMessage, &positions, google::protobuf::NewCallback(&google::protobuf::DoNothing));
	io_service.run();
	io_service.reset();
	
	service->Pause(controller, &voidMessage, &voidMessage, google::protobuf::NewCallback(&google::protobuf::DoNothing));	
	io_service.run();
	io_service.reset();
	
	//std::cerr << "All done. Now model is being updating ... " << std::endl;
	
	for (int idx = 0; idx < ids.val_size(); idx++) 
	{
		//std::cerr << "idx = " << idx << std::endl;
		
		unsigned int pid = ids.val(idx);
		//std::cerr << "pid = " << pid << std::endl;
		
		PvizPoint *point;
		PvizPointMap_t::iterator iter = points.find(pid);
		if (iter != points.end())
		{
			cerr << "[Warning] duplicated point : " << iter->first << endl;
			point = iter->second;
		}
		else 
		{
			point = new PvizPoint(pid);
			point->position[0] = positions.pos(idx).x();
			point->position[1] = positions.pos(idx).y();
			point->position[2] = positions.pos(idx).z();
			
			//std::cerr << "position = " << "(" << 
			//point->position[0] << "," <<
			//point->position[1] << "," <<
			//point->position[2] << ")" << std::endl;
			
			points.insert(std::make_pair(point->id, point));
		}
		
		unsigned int cid = lbs.val(idx);
		//std::cerr << "cid = " << cid << std::endl;
		
		PvizCluster *cluster;
		PvizClusterMap_t::iterator citer = clusters.find(cid);
		if (citer != clusters.end())
		{
			cluster = citer->second;
		}
		else 
		{
			cluster = new PvizCluster(cid);
            insertCluster(cluster);
		}		
		cluster->points.push_back(point);
	}
}

void PvizModel::updateFromPvizRPCServer()
{
	if (channel == NULL) 
	{
		return;
	}
	
	cgl::pviz::rpc::VoidMessage voidMessage;
	cgl::pviz::rpc::ListInt ids;
	cgl::pviz::rpc::ListPosition positions;
	
	service->getIds(controller, &voidMessage, &ids, google::protobuf::NewCallback(&google::protobuf::DoNothing));
	io_service.run();
	io_service.reset();
	
	service->getPositions(controller, &voidMessage, &positions, google::protobuf::NewCallback(&google::protobuf::DoNothing));
	io_service.run();
	io_service.reset();
	
	service->Pause(controller, &voidMessage, &voidMessage, google::protobuf::NewCallback(&google::protobuf::DoNothing));	
	io_service.run();
	io_service.reset();
	
	for (int idx = 0; idx < ids.val_size(); idx++) 
	{
		//std::cerr << "idx = " << idx << std::endl;
		
		unsigned int pid = ids.val(idx);
		//std::cerr << "pid = " << pid << std::endl;
		
		PvizPoint *point = points[pid];
		point->position[0] = positions.pos(idx).x();
		point->position[1] = positions.pos(idx).y();
		point->position[2] = positions.pos(idx).z();
		
		//std::cerr << "position = " << "(" << 
		//point->position[0] << "," <<
		//point->position[1] << "," <<
		//point->position[2] << ")" << std::endl;
	}
	
}

#endif

/*
void PvizModel::loadPvizMessage(const cgl::pviz::rpc::PvizMessage& pvizmessage)
{
    std::cout << "loadPvizMessage ... " << std::endl;
    
    std::cout << "pvizmessae.timestamp      : " << pvizmessage.timestamp() << std::endl;
    std::cout << "pvizmessae.stepid         : " << pvizmessage.stepid() << std::endl;
    std::cout << "pvizmessae.type           : " << pvizmessage.type() << std::endl;
    std::cout << "pvizmessae.label.size     : " << pvizmessage.labels().val_size() << std::endl;
    std::cout << "pvizmessae.positions.size : " << pvizmessage.positions().pos_size() << std::endl;    
    
    
}
*/

void PvizModel::load(std::vector<int>& labels, std::vector<Point3D>& positions)
{
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        PvizPoint *point = new PvizPoint(i);
        
        double pos[3];
        pos[0] = positions[i].x;
        pos[1] = positions[i].y;
        pos[2] = positions[i].z;
        
        point->setPosition(pos);
        
        points.insert(std::make_pair(point->id, point));
        
		PvizCluster *cluster;
		PvizClusterMap_t::iterator citer = clusters.find(labels[i]);
		if (citer != clusters.end())
		{
			cluster = citer->second;
		}
		else 
		{
			cluster = new PvizCluster(labels[i]);
            insertCluster(cluster);
		}		
		cluster->points.push_back(point);
    }
}

void PvizModel::update(std::vector<Point3D>& positions)
{
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        PvizPointMap_t::iterator it = points.find(i);
        
        if (it == points.end()) continue;
        
        PvizPoint *point = it->second;
        
        Point3D pos = positions[i];
        
        point->position[0] = pos.x;
        point->position[1] = pos.y;
        point->position[2] = pos.z;
    }
}


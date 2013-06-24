#include "pvizcluster.h"

//float PvizCluster::DefaultScale = 0.0;
//PvizColor PvizCluster::DefaultColor = PvizColor(119, 136, 153);

PvizCluster::PvizCluster() : visible(true), shape(3), isDefault(false), scale(1.0f) 
{
	color.ui_color = 0x00FFFFFF;
}

PvizCluster::~PvizCluster()
{
    //std::cout << "Destroying ... cluster " << this->id << std::endl;
}

PvizCluster::PvizCluster(unsigned int id) : id(id), visible(true), shape(3), isDefault(false), scale(1.0f)
{
	color.ui_color = 0x00FFFFFF;
}

std::string PvizCluster::toString()
{
	return format("Cluster %d", id);
}

bool PvizCluster::hasPoint(PvizPoint *p)
{
    PvizPointVec_t::iterator i = find(points.begin(), points.end(), p);
    
    if (i != points.end())
        return true;
    else
        return false;
}

bool PvizCluster::IsDefault()
{
    return isDefault;
}

void PvizCluster::SetAsDefault(bool value)
{
    isDefault = value;
}

float PvizCluster::GetScale()
{
    return scale;
    /*
    if (isDefault)
        return DefaultScale;
    else
        return scale;
     */
}

void PvizCluster::SetScale(float value)
{
    scale = value;
}

PvizColor* PvizCluster::GetColor()
{
    return &color;
    /*
    if (isDefault)
        return DefaultColor;
    else
        return color;
     */
}

void PvizCluster::SetColor(PvizColor value)
{
    color = value;
}

unsigned int PvizCluster::GetNumberOfPoints()
{
    return points.size();
}


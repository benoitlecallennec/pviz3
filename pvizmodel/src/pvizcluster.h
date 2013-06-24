#ifndef PVIZCLUSTER_H
#define PVIZCLUSTER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "pvizcomm.h"
#include "pvizpoint.h"

typedef union u_color { 
	unsigned int ui_color; 
	struct s_color { 
		unsigned char b, g, r, a;
	} uc_color;
	
    u_color() {}
    
    u_color(unsigned int rgba) : ui_color(rgba) {}
    
    u_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) 
    {
        setRgb(r, g, b, a);
    }

    void setRgb(unsigned int rgba) 
    {
        ui_color = rgba;
    }
    
    void setRgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) 
    {
		uc_color.r = r;
		uc_color.g = g;
		uc_color.b = b;
		uc_color.a = a;
    }
    
	void setRgbF(double r, double g, double b, double a = 1.0)
	{
		uc_color.r = 255.0 * r;
		uc_color.g = 255.0 * g;
		uc_color.b = 255.0 * b;
		uc_color.a = 255.0 * a;
	}
	
	std::string toString() const
	{
		return format("Color #%08x (a:%d, r:%d, g:%d, b:%d)", ui_color, uc_color.a, uc_color.r, uc_color.g, uc_color.b);
	}
	
	friend std::ostream& operator<<(std::ostream& out, const u_color& c) 
	{
		out << c.toString();
		return out;
	}
} PvizColor;

class PvizCluster
{
public:
	typedef std::vector<PvizPoint *> PvizPointVec_t;

public:
    //static PvizColor DefaultColor;
    //static float DefaultScale;
public:
	PvizCluster();
	PvizCluster(unsigned int id);
    ~PvizCluster();
public:
    //
    // Function
    bool hasPoint(PvizPoint *p);
	std::string toString();

    //
    // Member function
	bool IsDefault();
	void SetAsDefault(bool value);
    
    float GetScale();
    void SetScale(float value);
    
    PvizColor* GetColor();
    void SetColor(PvizColor value);
    
    unsigned int GetNumberOfPoints();

    static bool CompareBySize(PvizCluster* a, PvizCluster* b, bool descending)
    { 
        return (a->GetNumberOfPoints() < b->GetNumberOfPoints()) ^ descending;
    } 
    
    /*
    static bool CompareBySize(PvizCluster* a, PvizCluster* b, bool descending, bool nondefaultfirst = false)
    { 
        if ((nondefaultfirst) && (a->IsDefault() ^ b->IsDefault()))
        {
            if (a->GetNumberOfPoints() < b->GetNumberOfPoints())
            {
                return b->IsDefault() ^ descending;
            }
            else
            {
                return b->IsDefault() ^ descending;
                
            }
        }
        else
        {
            return (a->GetNumberOfPoints() < b->GetNumberOfPoints()) ^ descending;
        }
    }
     */
    
    static bool CompareById(PvizCluster* a, PvizCluster* b, bool descending)
    { 
        return (a->id < b->id) ^ descending;
    } 
    
    static bool CompareByLabel(PvizCluster* a, PvizCluster* b, bool descending)
    { 
        return (a->label < b->label) ^ descending;
    } 
    
    //
    // Member
    unsigned int id;
	std::string label;
    bool visible;
	PvizPointVec_t points;	
    unsigned int shape;
protected:
    //
    // Member
    bool isDefault;
    float scale;
	PvizColor color;
};

#endif // PVIZCLUSTER_H

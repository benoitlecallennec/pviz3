#ifndef __vtkCubeAxes2Actor_h
#define __vtkCubeAxes2Actor_h

#include "vtkCubeAxesActor.h"

class vtkCubeAxes2Actor : protected vtkCubeAxesActor
{
public:
    double GetLabelScaleFactor();
    void SetLabelScaleFactor(double);
protected:
    
private:
    double labelScaleFactor;
};


#endif

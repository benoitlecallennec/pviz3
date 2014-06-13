#include "vtkCubeAxes2Actor.h"

double vtkCubeAxes2Actor::GetLabelScaleFactor()
{
    return labelScaleFactor;
}

void vtkCubeAxes2Actor::SetLabelScaleFactor(double factor)
{
    this->labelScaleFactor = factor;
    
    double labelscalexfactor = this->LabelScale * labelScaleFactor;
    double titlescalexfactor = titlescale * labelScaleFactor;
    for (int i = 0; i < 4; i++)
    {
        this->XAxes[i]->SetLabelScale(labelscalexfactor);
        this->YAxes[i]->SetLabelScale(labelscalexfactor);
        this->ZAxes[i]->SetLabelScale(labelscalexfactor);
        this->XAxes[i]->SetTitleScale(titlescalexfactor);
        this->YAxes[i]->SetTitleScale(titlescalexfactor);
        this->ZAxes[i]->SetTitleScale(titlescalexfactor);
    }
}

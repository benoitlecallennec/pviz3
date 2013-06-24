#include "pvizinteractorstyle.h"

#include <vtkLabeledDataMapper.h>
#include <vtkDynamic2DLabelMapper.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkStringArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkPointData.h> 
#include <vtkIdTypeArray.h>

#include "pvizwidget.h"

#include <QDebug>

vtkStandardNewMacro(pvizInteractorStyle);

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

pvizInteractorStyle::pvizInteractorStyle() : 
//delta_(16), 
//pickMode_(PickWhileSelecting),
interactorMode_(Interator3D)
{
    flyToPos_[0] = 0.0;
    flyToPos_[1] = 0.0;
    flyToPos_[2] = 0.0;
}

pvizInteractorStyle::~pvizInteractorStyle()
{
}

void pvizInteractorStyle::OnChar()
{
	switch (this->Interactor->GetKeyCode())
    {
        case '2' :      
            interactorMode_ = Interator2D;
            break;
        case '3' :      
            interactorMode_ = Interator3D;
            break;
        case 'f' :      
        case 'F' :
        {
            vtkRenderWindowInteractor *rwi = this->Interactor;
            rwi->FlyTo(this->CurrentRenderer, flyToPos_);
        }
            break;
		case 'h':
		case 'H':
			HoverPick();
			break;
		default:
			// Forward events
			this->Superclass::OnChar();
    }
}

/*
void pvizInteractorStyle::SetDelta(int d)
{
	delta_ = d;
}

int pvizInteractorStyle::GetDelta()
{
	return delta_;
}
*/

void pvizInteractorStyle::HoverPick()
{
	//pickMode_ = PickWhileHovering;
	vtkRenderWindowInteractor *rwi = this->Interactor;
	int *eventPos = rwi->GetEventPosition();
	this->FindPokedRenderer(eventPos[0], eventPos[1]);
	this->StartPosition[0] = eventPos[0];
	this->StartPosition[1] = eventPos[1];
	this->EndPosition[0] = eventPos[0];
	this->EndPosition[1] = eventPos[1];
	this->Superclass::Pick();
    /*
	this->StartPosition[0] = eventPos[0] - delta_;
	this->StartPosition[1] = eventPos[1] - delta_;
	this->EndPosition[0] = eventPos[0] + delta_;
	this->EndPosition[1] = eventPos[1] + delta_;
	pickMode_ = PickWhileSelecting;
     */
}

/*
pvizInteractorStyle::PickMode_t pvizInteractorStyle::GetPickMode()
{
	return pickMode_;
}
*/

void pvizInteractorStyle::SetFlyToPos(double pos[3])
{
    flyToPos_[0] = pos[0];
    flyToPos_[1] = pos[1];
    flyToPos_[2] = pos[2];
}

double* pvizInteractorStyle::GetFlyToPos()
{
    return flyToPos_;
}

void pvizInteractorStyle::OnLeftButtonDown()
{
	if (this->CurrentMode == VTKISRBP_SELECT)
    {
		//if in rubber band mode, let the parent class handle it
		this->Superclass::OnLeftButtonDown();
		return;
    }
	
	if (!this->Interactor)
    {
		return;
    }
    
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], 
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    
    //this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetShiftKey()) 
    {
        if (this->Interactor->GetControlKey()) 
        {
            this->StartDolly();
        }
        else 
        {
            this->StartPan();
        }
    } 
    else 
    {
        if (this->Interactor->GetControlKey()) 
        {
            this->StartSpin();
        }
        else 
        {
            if (interactorMode_ == Interator2D)
            {
                this->StartPan();
            }
            else
            {
                this->StartRotate();
            }
        }
    }
    
	// Forward events
	//this->Superclass::OnLeftButtonDown();
}


/*
void pvizInteractorStyle::OnMouseMove()
{
	// Forward events
	this->Superclass::OnMouseMove();
}

void pvizInteractorStyle::OnLeftButtonUp()
{
	// Forward events
	this->Superclass::OnLeftButtonUp();
}

void pvizInteractorStyle::OnChar()
{
	// Forward events
	this->Superclass::OnChar();
}
*/
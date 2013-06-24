#ifndef PVIZINTERACTORSTYLE_H
#define PVIZINTERACTORSTYLE_H

#include <vtkObjectFactory.h>

#include "vtkInteractorStyleRubberBand2Pick.h"

class pvizInteractorStyle : public vtkInteractorStyleRubberBand2Pick
{
public:
    /*
	enum PickMode_t {
		PickWhileHovering,
		PickWhileSelecting
	};
     */
	
	enum InteractorMode_t {
		Interator2D, Interator3D
	};
    
    static pvizInteractorStyle* New();
    vtkTypeMacro(pvizInteractorStyle, vtkInteractorStyleRubberBand2Pick);
	
    pvizInteractorStyle();
	~pvizInteractorStyle();
	
	virtual void OnChar();

	virtual void HoverPick();
    
	virtual void OnLeftButtonDown();
    
    //void RedrawRubberBand();
	/*
	virtual void OnMouseMove();
	virtual void OnLeftButtonUp();
	virtual void OnChar();
	*/
	//void SetDelta(int d);
	//int GetDelta();
	//PickMode_t GetPickMode();
    void SetFlyToPos(double pos[3]);
    double* GetFlyToPos();
protected:
	//int delta_;
	//PickMode_t pickMode_;
    double flyToPos_[3];
    InteractorMode_t interactorMode_;
};

#endif //PVIZINTERACTORSTYLE_H
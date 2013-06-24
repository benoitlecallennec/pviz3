#ifndef PVIZSYNC_H
#define PVIZSYNC_H

#include <QDebug>

#include <set>
#include <vtkCallbackCommand.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>

//#include "pvizwidget.h"

class PvizWidget;

typedef std::set<PvizWidget*> PvizWidgetSet_t;

class PvizSync 
{
public:
	PvizSync();
	static void RenderEndCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	
	void AddMember(PvizWidget *pviz);
	void RemoveMember(PvizWidget *pviz);
	
	void InstallCallback(PvizWidget *pviz);
	void UninstallCallback(PvizWidget *pviz);
	void DoSync();
private:
	PvizWidget* master;
	PvizWidgetSet_t syncMembers;
	vtkSmartPointer<vtkCamera> syncCamera;
	vtkSmartPointer<vtkCallbackCommand> cb;
};

#endif // PVIZSYNC_H

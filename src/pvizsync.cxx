#include "pvizsync.h"
#include "pvizwidget.h"

PvizSync::PvizSync() :
master(NULL),
syncCamera(vtkCamera::New()),
cb(vtkCallbackCommand::New())
{
	//VTK_ASSIGN(vtkCallbackCommand, cb);
	cb->SetCallback(PvizSync::RenderEndCallBack);
	cb->SetClientData(this);
}

void PvizSync::AddMember(PvizWidget *pviz)
{
	if ((syncMembers.size() == 0) && (master == NULL))
	{
		syncCamera->ShallowCopy(pviz->GetActiveCamera());
	}
	pviz->SetAsActiveCamera(syncCamera);
	pviz->SetSync(this);
	syncMembers.insert(pviz);
	
}

void PvizSync::RemoveMember(PvizWidget *pviz)
{
	UninstallCallback(pviz);
	
	pviz->UseLocalCamera();
	
	if (pviz == master)
		master = NULL;
	
	syncMembers.erase(pviz);
}

void PvizSync::InstallCallback(PvizWidget *pviz)
{
	if ((pviz != master) && (syncMembers.find(pviz) == syncMembers.end()))
	{
		return;
	}
	
	if (master != NULL) 
		syncMembers.insert(master);
	
	for (PvizWidgetSet_t::iterator it = syncMembers.begin(); it != syncMembers.end(); it++) 
	{
		//qDebug() << "RemoveObserver ... " << (*it);
		(*it)->renderer->RemoveObserver(cb);
	}
	
	syncMembers.erase(pviz);
	master = pviz;
	
	//qDebug() << "AddObserver ... " << master;
	master->renderer->AddObserver(vtkCommand::EndEvent, cb);
	
}

void PvizSync::UninstallCallback(PvizWidget *pviz)
{
	//qDebug() << "RemoveObserver ... " << pviz;
	pviz->renderer->RemoveObserver(cb);
}

void PvizSync::DoSync()
{
	for (PvizWidgetSet_t::iterator it = syncMembers.begin(); it != syncMembers.end(); it++) 
	{
		(*it)->update();
	}
}

void PvizSync::RenderEndCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
{
	//qDebug() << "PvizSync::RenderEndCallBack - eventId : " << eventId;	
	switch (eventId) 
	{
		case vtkCommand::EndEvent:
			PvizSync* self = reinterpret_cast<PvizSync*>(clientData);
			//qDebug() << "self : " << self;
			self->DoSync();			
			break;
	}
}

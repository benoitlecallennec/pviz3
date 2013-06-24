#ifndef PVIZEXTRACTSELECTEDPOLYDATAIDS_H
#define PVIZEXTRACTSELECTEDPOLYDATAIDS_H

#include <vtkObjectFactory.h>
#include <vtkExtractSelectedPolyDataIds.h>

class pvizExtractSelectedPolyDataIds : public vtkExtractSelectedPolyDataIds
{
public:
	static pvizExtractSelectedPolyDataIds* New();
    vtkTypeMacro(pvizExtractSelectedPolyDataIds, vtkExtractSelectedPolyDataIds);
	
	pvizExtractSelectedPolyDataIds();
protected:
	// Usual data generation method
	int RequestData(vtkInformation *,
					vtkInformationVector **,
					vtkInformationVector *);	
};

#endif //PVIZEXTRACTSELECTEDPOLYDATAIDS_H
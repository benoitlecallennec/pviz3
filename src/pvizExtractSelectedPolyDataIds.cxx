#include "pvizExtractSelectedPolyDataIds.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"

#include <QDebug>

vtkStandardNewMacro(pvizExtractSelectedPolyDataIds);

pvizExtractSelectedPolyDataIds::pvizExtractSelectedPolyDataIds()
{
}

int pvizExtractSelectedPolyDataIds::RequestData(
											   vtkInformation *vtkNotUsed(request),
											   vtkInformationVector **inputVector,
											   vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *selInfo = inputVector[1]->GetInformationObject(0);
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	
	// get the input and output
	vtkPolyData *input = vtkPolyData::SafeDownCast(
												   inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(
													outInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkSelection *sel = vtkSelection::SafeDownCast(
												   selInfo->Get(vtkDataObject::DATA_OBJECT()));
	if ( ! sel )
    {
		vtkErrorMacro(<<"No selection specified");
		return 1;
    }
	
	vtkPointData *pd = input->GetPointData();
	vtkCellData *cd = input->GetCellData();
	
	vtkPointData *outputPD = output->GetPointData();
	vtkCellData *outputCD = output->GetCellData();
	
	vtkDebugMacro(<< "Extracting poly data geometry");
	
	vtkSelectionNode* node = 0;
	if (sel->GetNumberOfNodes() == 1)
    {
		node = sel->GetNode(0);
    }
	if (!node)
    {
		return 1;
    }
	if (!node->GetProperties()->Has(vtkSelectionNode::CONTENT_TYPE()) ||
		node->GetProperties()->Get(vtkSelectionNode::CONTENT_TYPE()) != vtkSelectionNode::INDICES ||
		!node->GetProperties()->Has(vtkSelectionNode::FIELD_TYPE()) ||
		node->GetProperties()->Get(vtkSelectionNode::FIELD_TYPE()) != vtkSelectionNode::CELL)
    {
		return 1;
    }
	
	vtkIdTypeArray* idArray = 
    vtkIdTypeArray::SafeDownCast(node->GetSelectionList());
	
	if (!idArray)
    {
		return 1;
    }
	
	vtkIdType numCells = 
    idArray->GetNumberOfComponents()*idArray->GetNumberOfTuples();
	
	if (numCells == 0)
    {
		return 1;
    }
	
	output->Allocate(numCells);
	output->SetPoints(input->GetPoints());
	outputPD->PassData(pd);
	outputCD->CopyAllocate(cd);
	
	// Now loop over all cells to see whether they are in the selection.
	// Copy if they are.
	
	vtkIdList* ids = vtkIdList::New();
	
	vtkIdType numInputCells = input->GetNumberOfCells();
	for (vtkIdType i=0; i < numCells; i++)
    {
		vtkIdType cellId = idArray->GetValue(i);
		if (cellId >= numInputCells)
		{
			continue;
		}

        // !!!! Warning !!!!
		// Check number of points. Skip empty cell
		// GetNumberOfPoints() should be called to avoid error in GetCellPoints()
		vtkIdType numInputCellPoints = input->GetCell(cellId)->GetNumberOfPoints();
		vtkIdType *pts, npts;		
		input->vtkPolyData::GetCellPoints(cellId, npts, pts);
		if (npts == 0) 
		{
			continue;
		}
		
		input->GetCellPoints(cellId, ids);
		vtkIdType newId = output->InsertNextCell(input->GetCellType(cellId), ids);
		outputCD->CopyData(cd, cellId, newId);
		
    }
	ids->Delete();
	output->Squeeze();
	
	return 1;
}

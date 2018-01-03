#include "myResliceCursorThickLineRepresentation.h"
#include "vtkImageSlabReslice.h"
#include "vtkImageReslice.h"
#include "myResliceCursor.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageMapToColors.h"
#include "math.h"
#include <algorithm>

#include <sstream>
#include <iostream>

vtkStandardNewMacro(myResliceCursorThickLineRepresentation);


//----------------------------------------------------------------------
myResliceCursorThickLineRepresentation::myResliceCursorThickLineRepresentation()
{
  this->CreateDefaultResliceAlgorithm();
}

//----------------------------------------------------------------------
myResliceCursorThickLineRepresentation::~myResliceCursorThickLineRepresentation()
{
}

//----------------------------------------------------------------------
void myResliceCursorThickLineRepresentation::CreateDefaultResliceAlgorithm()
{
  if (this->ResliceHiRes){
	this->ResliceHiRes->Delete();
  }
  if (this->ResliceLowRes){
	this->ResliceLowRes->Delete();
  }
  // Override superclass implementation to create a vtkImageSlabReslice here.
  std::cout << __FILE__ << " - " << __FUNCTION__ << " - Override superclass implementation to create a vtkImageSlabReslice here. É onde instancio o vtkImageSlabReslice" << std::endl;
  this->ResliceHiRes = vtkImageSlabReslice::New(); //Instancia os dois filtros, o de hires e o de lowres
  this->ResliceLowRes = vtkImageSlabReslice::New();
}

//----------------------------------------------------------------------------
void myResliceCursorThickLineRepresentation::SetResliceParameters( double outputSpacingX, double outputSpacingY, int extentX, int extentY )
{
	std::cout << __FUNCTION__ << std::endl;
	vtkImageSlabReslice *thickResliceHiRes = vtkImageSlabReslice::SafeDownCast(this->ResliceHiRes);
	if (thickResliceHiRes){
		// Set the default color the minimum scalar value
		double range[2];
		vtkImageData::SafeDownCast(thickResliceHiRes->GetInput())->GetScalarRange(range);
		thickResliceHiRes->SetBackgroundLevel(range[0]);
		// Set the usual parameters.
		this->ColorMap->SetInputConnection(thickResliceHiRes->GetOutputPort());
		thickResliceHiRes->TransformInputSamplingOff();
		thickResliceHiRes->SetResliceAxes(this->ResliceAxes);
		thickResliceHiRes->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
		thickResliceHiRes->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
		thickResliceHiRes->SetOutputExtent(0, extentX - 1, 0, extentY - 1, 0, 0);
		myResliceCursor *rc = this->GetResliceCursor();
		thickResliceHiRes->SetSlabThickness(rc->GetThickness()[0]);
		double spacing[3];
		rc->GetImageHiRes()->GetSpacing(spacing);
		// Perhaps we should multiply this by 0.5 for nyquist
		const double minSpacing = std::min(std::min(spacing[0], spacing[1]), spacing[2]);
		// Set the slab resolution the minimum spacing. Reasonable default
		thickResliceHiRes->SetSlabResolution(minSpacing);
	}
	vtkImageSlabReslice *thickResliceLowRes = vtkImageSlabReslice::SafeDownCast(this->ResliceLowRes);
	if (thickResliceLowRes){
		// Set the default color the minimum scalar value
		double range[2];
		vtkImageData::SafeDownCast(thickResliceLowRes->GetInput())->GetScalarRange(range);
		thickResliceLowRes->SetBackgroundLevel(range[0]);
		// Set the usual parameters.
		this->ColorMap->SetInputConnection(thickResliceLowRes->GetOutputPort());
		thickResliceLowRes->TransformInputSamplingOff();
		thickResliceLowRes->SetResliceAxes(this->ResliceAxes);
		thickResliceLowRes->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
		thickResliceLowRes->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
		thickResliceLowRes->SetOutputExtent(0, extentX - 1, 0, extentY - 1, 0, 0);
		myResliceCursor *rc = this->GetResliceCursor();
		thickResliceLowRes->SetSlabThickness(rc->GetThickness()[0]);
		double spacing[3];
		rc->GetImageHiRes()->GetSpacing(spacing);
		// Perhaps we should multiply this by 0.5 for nyquist
		const double minSpacing = std::min(std::min(spacing[0], spacing[1]), spacing[2]);
		// Set the slab resolution the minimum spacing. Reasonable default
		thickResliceLowRes->SetSlabResolution(minSpacing);
	}
}

//----------------------------------------------------------------------
void myResliceCursorThickLineRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

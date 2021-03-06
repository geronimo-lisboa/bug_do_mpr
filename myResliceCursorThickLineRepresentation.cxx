#include "myResliceCursorThickLineRepresentation.h"
#include "vtkImageSlabReslice.h"
#include "vtkImageReslice.h"
#include "myResliceCursor.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageMapToColors.h"
#include "math.h"
#include <algorithm>
#include "myResliceCursorPolyDataAlgorithm.h"
#include <sstream>
#include <iostream>


vtkStandardNewMacro(myResliceCursorThickLineRepresentation);


//----------------------------------------------------------------------
myResliceCursorThickLineRepresentation::myResliceCursorThickLineRepresentation()
{
  this->CreateDefaultResliceAlgorithm();
  this->UseLowRes = false;
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
  this->ResliceHiRes = vtkImageSlabReslice::New(); //Instancia os dois filtros, o de hires e o de lowres
  this->ResliceLowRes = vtkImageSlabReslice::New();
}

//----------------------------------------------------------------------------
void myResliceCursorThickLineRepresentation::SetResliceParameters( double outputSpacingX, double outputSpacingY, int extentX, int extentY )
{
	vtkImageSlabReslice *thickSlabReslice = nullptr;
	if (UseLowRes){//� para usar a low res
		thickSlabReslice = vtkImageSlabReslice::SafeDownCast(this->ResliceLowRes);
		thickSlabReslice->SetInterpolationModeToNearestNeighbor();
		int *dims = vtkImageData::SafeDownCast(thickSlabReslice->GetInput())->GetDimensions();
		//std::co ut <<"Dimensoes da imagem sendo usada no reslice = "<< dims[0] << ", " << dims[1] << ", " << dims[2] << std::endl;
	}
	else{//� pra usar a hi res
		thickSlabReslice = vtkImageSlabReslice::SafeDownCast(this->ResliceHiRes);
		thickSlabReslice->SetInterpolationModeToCubic();
		int *dims = vtkImageData::SafeDownCast(thickSlabReslice->GetInput())->GetDimensions();
		//std::cou t << "Dimensoes da imagem sendo usada no reslice = " << dims[0] << ", " << dims[1] << ", " << dims[2] << std::endl;
	}
	// Set the default color the minimum scalar value
	double range[2];
	vtkImageData::SafeDownCast(thickSlabReslice->GetInput())->GetScalarRange(range);
	thickSlabReslice->SetBackgroundLevel(range[0]);
	// Set the usual parameters.
	this->ColorMap->SetInputConnection(thickSlabReslice->GetOutputPort());

	int *dims = vtkImageData::SafeDownCast(thickSlabReslice->GetInput())->GetDimensions();
	thickSlabReslice->TransformInputSamplingOff();
	thickSlabReslice->SetResliceAxes(this->ResliceAxes);
	thickSlabReslice->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
	thickSlabReslice->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
	thickSlabReslice->SetOutputExtent(0, extentX - 1, 0, extentY - 1, 0, 0);
	myResliceCursor *rc = this->GetResliceCursor();
	thickSlabReslice->SetSlabThickness(rc->GetThickness()[0]);
	double spacing[3];
	rc->GetImageHiRes()->GetSpacing(spacing);
	// Perhaps we should multiply this by 0.5 for nyquist
	const double minSpacing = std::min(std::min(spacing[0], spacing[1]), spacing[2]);
	// Set the slab resolution the minimum spacing. Reasonable default
	thickSlabReslice->SetSlabResolution(minSpacing);
	//O que acontece se eu gravo o que t� no thickSlabReslice?
	//O callback de pegar a slab:
	const int idReslicePlaneNormal = GetCursorAlgorithm()->GetReslicePlaneNormal();
	for (auto l : resliceResultListeners){
		l->ReslicedImageCreated(idReslicePlaneNormal, thickSlabReslice);
	}
	//boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	//long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();
	//std::string filename = "c:\\" + boost::lexical_cast<std::string>(milliseconds) + ".vti";
	//vtkSmartPointer<vtkXMLImageDataWriter> debugsave = vtkSmartPointer<vtkXMLImageDataWriter>::New();
	//debugsave->SetFileName(filename.c_str());
	//debugsave->SetInputConnection(thickSlabReslice->GetOutputPort());
	//debugsave->Update();
}

//----------------------------------------------------------------------
void myResliceCursorThickLineRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

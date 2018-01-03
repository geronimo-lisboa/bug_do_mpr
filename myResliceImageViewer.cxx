/*=========================================================================

  Program:   Visualization Toolkit
  Module:    myResliceImageViewer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "myResliceImageViewer.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "myResliceCursorWidget.h"
#include "myResliceCursorLineRepresentation.h"
#include "myResliceCursorThickLineRepresentation.h"
#include "myResliceCursorActor.h"
#include "myResliceCursorPolyDataAlgorithm.h"
#include "vtkPlane.h"
#include "myResliceCursor.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"
#include "vtkScalarsToColors.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkPlane.h"
#include "vtkMath.h"
#include "myResliceImageViewerMeasurements.h"

vtkStandardNewMacro(myResliceImageViewer);

//----------------------------------------------------------------------------
// This class is used to scroll slices with the scroll bar. In the case of MPR
// view, it moves one "normalized spacing" in the direction of the normal to
// the resliced plane, provided the new center will continue to lie within the
// volume.
class myResliceImageViewerScrollCallback : public vtkCommand
{
public:
  static myResliceImageViewerScrollCallback *New()
    { return new myResliceImageViewerScrollCallback; }

  virtual void Execute(vtkObject *, unsigned long ev, void*)
    {
    if (!this->Viewer->GetSliceScrollOnMouseWheel())
      {
      return;
      }

    // Do not process if any modifiers are ON
    if (this->Viewer->GetInteractor()->GetShiftKey() ||
        this->Viewer->GetInteractor()->GetControlKey() ||
        this->Viewer->GetInteractor()->GetAltKey())
      {
      return;
      }

    // forwards or backwards
    int sign = (ev == vtkCommand::MouseWheelForwardEvent) ? 1 : -1;
    this->Viewer->IncrementSlice(sign);

    // Abort further event processing for the scroll.
    this->SetAbortFlag(1);
    }

  myResliceImageViewerScrollCallback():Viewer(0) {}
  myResliceImageViewer *Viewer;
};

//----------------------------------------------------------------------------
myResliceImageViewer::myResliceImageViewer()
{
  // Default is to not use the reslice cursor widget, ie use fast
  // 3D texture mapping to display slices.
  this->ResliceMode = myResliceImageViewer::RESLICE_AXIS_ALIGNED;

  // Set up the reslice cursor widget, should it be used.

  this->ResliceCursorWidget = myResliceCursorWidget::New();

  vtkSmartPointer< myResliceCursor > resliceCursor = vtkSmartPointer< myResliceCursor >::New();
  resliceCursor->SetThickMode(0);
  resliceCursor->SetThickness(10, 10, 10);

  vtkSmartPointer< myResliceCursorLineRepresentation > resliceCursorRep = vtkSmartPointer< myResliceCursorLineRepresentation >::New();
  resliceCursorRep->GetResliceCursorActor()->GetCursorAlgorithm()->SetResliceCursor(resliceCursor);
  resliceCursorRep->GetResliceCursorActor()->
      GetCursorAlgorithm()->SetReslicePlaneNormal(this->SliceOrientation);
  this->ResliceCursorWidget->SetRepresentation(resliceCursorRep);

  this->PointPlacer = vtkBoundedPlanePointPlacer::New();

  this->Measurements = myResliceImageViewerMeasurements::New();
  this->Measurements->SetResliceImageViewer(this);

  this->ScrollCallback = myResliceImageViewerScrollCallback::New();
  this->ScrollCallback->Viewer = this;
  this->SliceScrollOnMouseWheel = 1;

  this->InstallPipeline();
}

//----------------------------------------------------------------------------
myResliceImageViewer::~myResliceImageViewer()
{
  this->Measurements->Delete();

  if (this->ResliceCursorWidget)
    {
    this->ResliceCursorWidget->Delete();
    this->ResliceCursorWidget = NULL;
    }

  this->PointPlacer->Delete();
  this->ScrollCallback->Delete();
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetThickMode( int t )
{
	vtkSmartPointer< myResliceCursor > rc = this->GetResliceCursor();

  if (t == this->GetThickMode())
    {
    return;
    }

  vtkSmartPointer< myResliceCursorLineRepresentation >
    resliceCursorRepOld = myResliceCursorLineRepresentation::SafeDownCast(
                          this->ResliceCursorWidget->GetRepresentation());
  vtkSmartPointer< myResliceCursorLineRepresentation > resliceCursorRepNew;

  this->GetResliceCursor()->SetThickMode(t);

  if (t)
    {
    resliceCursorRepNew = vtkSmartPointer<
        myResliceCursorThickLineRepresentation >::New();
    }
  else
    {
    resliceCursorRepNew = vtkSmartPointer<
        myResliceCursorLineRepresentation >::New();
    }

  int e = this->ResliceCursorWidget->GetEnabled();
  this->ResliceCursorWidget->SetEnabled(0);

  resliceCursorRepNew->GetResliceCursorActor()->
      GetCursorAlgorithm()->SetResliceCursor(rc);
  resliceCursorRepNew->GetResliceCursorActor()->
      GetCursorAlgorithm()->SetReslicePlaneNormal(this->SliceOrientation);
  this->ResliceCursorWidget->SetRepresentation(resliceCursorRepNew);
  resliceCursorRepNew->SetLookupTable(resliceCursorRepOld->GetLookupTable());

  resliceCursorRepNew->SetWindowLevel(
      resliceCursorRepOld->GetWindow(),
      resliceCursorRepOld->GetLevel(), 1);

  this->ResliceCursorWidget->SetEnabled(e);
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetResliceCursor(myResliceCursor * rc)
{
  myResliceCursorRepresentation *rep =
    myResliceCursorRepresentation::SafeDownCast(
          this->GetResliceCursorWidget()->GetRepresentation());
  rep->GetCursorAlgorithm()->SetResliceCursor(rc);

  // Rehook the observer to this reslice cursor.
  this->Measurements->SetResliceImageViewer(this);
}

//----------------------------------------------------------------------------
int myResliceImageViewer::GetThickMode()
{
  return (myResliceCursorThickLineRepresentation::
    SafeDownCast(this->ResliceCursorWidget->GetRepresentation())) ? 1 : 0;
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetLookupTable( vtkScalarsToColors * l )
{
  if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
    {
    rep->SetLookupTable(l);
    }

  if (this->WindowLevel)
    {
    this->WindowLevel->SetLookupTable(l);
    this->WindowLevel->SetOutputFormatToRGBA();
    this->WindowLevel->PassAlphaToOutputOn();
    }
}

//----------------------------------------------------------------------------
vtkScalarsToColors * myResliceImageViewer::GetLookupTable()
{
  if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
    {
    return rep->GetLookupTable();
    }

  return NULL;
}

//----------------------------------------------------------------------------
void myResliceImageViewer::UpdateOrientation()
{
    // Set the camera position

    vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
    if (cam)
      {
      switch (this->SliceOrientation)
        {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
          cam->SetFocalPoint(0,0,0);
          cam->SetPosition(0,0,1); // -1 if medical ?
          cam->SetViewUp(0,1,0);
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
          cam->SetFocalPoint(0,0,0);
          cam->SetPosition(0,-1,0); // 1 if medical ?
          cam->SetViewUp(0,0,1);
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
          cam->SetFocalPoint(0,0,0);
          cam->SetPosition(1,0,0); // -1 if medical ?
          cam->SetViewUp(0,0,1);
          break;
        }
      }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::UpdateDisplayExtent()
{
  // Only update the display extent in axis aligned mode

  if (this->ResliceMode == RESLICE_AXIS_ALIGNED)
    {
    this->Superclass::UpdateDisplayExtent();
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::InstallPipeline()
{
  this->Superclass::InstallPipeline();

  if (this->Interactor)
    {
    this->ResliceCursorWidget->SetInteractor(this->Interactor);

    // Observe the scroll for slice manipulation at a higher priority
    // than the interactor style.
    this->Interactor->RemoveObserver(this->ScrollCallback);
    this->Interactor->AddObserver(vtkCommand::MouseWheelForwardEvent,
        this->ScrollCallback, 0.55 );
    this->Interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent,
        this->ScrollCallback, 0.55 );
    }

  if (this->Renderer)
    {
    this->ResliceCursorWidget->SetDefaultRenderer(this->Renderer);
    vtkCamera *cam = this->Renderer->GetActiveCamera();
    cam->ParallelProjectionOn();
    }

  if (this->ResliceMode == RESLICE_OBLIQUE)
    {
    this->ResliceCursorWidget->SetEnabled(1);
    this->ImageActor->SetVisibility(0);
    this->UpdateOrientation();

    double bounds[6] = {0, 1, 0, 1, 0, 1};

    vtkCamera *cam = this->Renderer->GetActiveCamera();
    double onespacing[3] = {1, 1, 1};
    double *spacing = onespacing;
	if (this->GetResliceCursor()->GetImageHiRes())
      {
		  this->GetResliceCursor()->GetImageHiRes()->GetBounds(bounds);
		  spacing = this->GetResliceCursor()->GetImageHiRes()->GetSpacing();
      }
    double avg_spacing =
      (spacing[0] + spacing[1] + spacing[2]) / 3.0;
    cam->SetClippingRange(
      bounds[this->SliceOrientation * 2] - 100 * avg_spacing,
      bounds[this->SliceOrientation * 2 + 1] + 100 * avg_spacing);
    }
  else
    {
    this->ResliceCursorWidget->SetEnabled(0);
    this->ImageActor->SetVisibility(1);
    this->UpdateOrientation();
    }

  if (this->WindowLevel)
    {
    this->WindowLevel->SetLookupTable(this->GetLookupTable());
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::UnInstallPipeline()
{
  this->ResliceCursorWidget->SetEnabled(0);

  if (this->Interactor)
    {
    this->Interactor->RemoveObserver(this->ScrollCallback);
    }

  this->Superclass::UnInstallPipeline();
}

//----------------------------------------------------------------------------
void myResliceImageViewer::UpdatePointPlacer()
{
  if (this->ResliceMode == RESLICE_OBLIQUE)
    {
    this->PointPlacer->SetProjectionNormalToOblique();
    if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
      {
      const int planeOrientation =
        rep->GetCursorAlgorithm()->GetReslicePlaneNormal();
      vtkPlane *plane = this->GetResliceCursor()->GetPlane(planeOrientation);
      this->PointPlacer->SetObliquePlane(plane);
      }
    }
  else
    {

    if (!this->WindowLevel->GetInput())
      {
      return;
      }

    vtkImageData *input = this->ImageActor->GetInput();
    if ( !input )
      {
      return;
      }

    double spacing[3];
    input->GetSpacing(spacing);

    double origin[3];
    input->GetOrigin(origin);

    double bounds[6];
    this->ImageActor->GetBounds(bounds);

    int displayExtent[6];
    this->ImageActor->GetDisplayExtent(displayExtent);

    int axis = vtkBoundedPlanePointPlacer::XAxis;
    double position = 0.0;
    if ( displayExtent[0] == displayExtent[1] )
      {
      axis = vtkBoundedPlanePointPlacer::XAxis;
      position = origin[0] + displayExtent[0]*spacing[0];
      }
    else if ( displayExtent[2] == displayExtent[3] )
      {
      axis = vtkBoundedPlanePointPlacer::YAxis;
      position = origin[1] + displayExtent[2]*spacing[1];
      }
    else if ( displayExtent[4] == displayExtent[5] )
      {
      axis = vtkBoundedPlanePointPlacer::ZAxis;
      position = origin[2] + displayExtent[4]*spacing[2];
      }

    this->PointPlacer->SetProjectionNormal(axis);
    this->PointPlacer->SetProjectionPosition(position);
    }

}

//----------------------------------------------------------------------------
void myResliceImageViewer::Render()
{
  if (!this->WindowLevel->GetInput())
    {
    return;
    }

  this->UpdatePointPlacer();

  this->Superclass::Render();
}

//----------------------------------------------------------------------------
myResliceCursor * myResliceImageViewer::GetResliceCursor()
{
  if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
    {
    return rep->GetResliceCursor();
    }

  return NULL;
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetInputData(vtkImageData *imgHiRes, vtkImageData *imgLowRes)
{
	std::cout << __FUNCTION__ << std::endl;
	vtkImageData *in = imgHiRes;
	this->WindowLevel->SetInputData(in);
	this->GetResliceCursor()->SetImages(imgHiRes, imgLowRes);
	this->GetResliceCursor()->SetCenter(in->GetCenter());
	this->UpdateDisplayExtent();
	double range[2];
	in->GetScalarRange(range);
	if (myResliceCursorRepresentation *rep = myResliceCursorRepresentation::SafeDownCast(this->ResliceCursorWidget->GetRepresentation()))
	{
		// default background color is the min value of the image scalar range
		if (vtkImageReslice *reslice = vtkImageReslice::SafeDownCast(rep->GetResliceHiRes())){
			reslice->SetBackgroundColor(range[0],range[0],range[0],range[0]);
			this->SetColorWindow(range[1]-range[0]);
			this->SetColorLevel((range[0]+range[1])/2.0);
		}
		if (vtkImageReslice *reslice = vtkImageReslice::SafeDownCast(rep->GetResliceLowRes())){
			reslice->SetBackgroundColor(range[0], range[0], range[0], range[0]);
			this->SetColorWindow(range[1] - range[0]);
			this->SetColorLevel((range[0] + range[1]) / 2.0);
		}
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetInputConnection(vtkAlgorithmOutput* input)
{
  vtkErrorMacro( << "Use SetInputData instead. " );
  this->WindowLevel->SetInputConnection(input);
  this->UpdateDisplayExtent();
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetResliceMode( int r )
{
  if (r == this->ResliceMode)
    {
    return;
    }

  this->ResliceMode = r;
  this->Modified();

  this->InstallPipeline();
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetColorWindow( double w )
{
  double rmin = this->GetColorLevel() - 0.5*fabs( w );
  double rmax = rmin + fabs( w );
  this->GetLookupTable()->SetRange( rmin, rmax );

  this->WindowLevel->SetWindow(w);
  if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
    {
    rep->SetWindowLevel(w, rep->GetLevel(), 1);
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::SetColorLevel( double w )
{
  double rmin = w - 0.5*fabs( this->GetColorWindow() );
  double rmax = rmin + fabs( this->GetColorWindow() );
  this->GetLookupTable()->SetRange( rmin, rmax );

  this->WindowLevel->SetLevel(w);
  if (myResliceCursorRepresentation *rep =
        myResliceCursorRepresentation::SafeDownCast(
          this->ResliceCursorWidget->GetRepresentation()))
    {
    rep->SetWindowLevel(rep->GetWindow(), w, 1);
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::Reset()
{
  this->ResliceCursorWidget->ResetResliceCursor();
}

//----------------------------------------------------------------------------
vtkPlane * myResliceImageViewer::GetReslicePlane()
{
  // Get the reslice plane
  if (myResliceCursorRepresentation *rep =
      myResliceCursorRepresentation::SafeDownCast(
        this->ResliceCursorWidget->GetRepresentation()))
    {
    const int planeOrientation =
      rep->GetCursorAlgorithm()->GetReslicePlaneNormal();
    vtkPlane *plane = this->GetResliceCursor()->GetPlane(planeOrientation);
    return plane;
    }

  return NULL;
}

//----------------------------------------------------------------------------
double myResliceImageViewer::GetInterSliceSpacingInResliceMode()
{
  double n[3], imageSpacing[3], resliceSpacing = 0;

  if (vtkPlane *plane = this->GetReslicePlane())
    {
    plane->GetNormal(n);
	this->GetResliceCursor()->GetImageHiRes()->GetSpacing(imageSpacing);
    resliceSpacing = fabs(vtkMath::Dot(n, imageSpacing));
    }

  return resliceSpacing;
}

//----------------------------------------------------------------------------
void myResliceImageViewer::IncrementSlice( int inc )
{
  if (this->GetResliceMode() ==
      myResliceImageViewer::RESLICE_AXIS_ALIGNED)
    {
    int oldSlice = this->GetSlice();
    this->SetSlice(this->GetSlice() + inc);
    if (this->GetSlice() != oldSlice)
      {
      this->InvokeEvent( myResliceImageViewer::SliceChangedEvent, NULL );
      this->InvokeEvent( vtkCommand::InteractionEvent, NULL );
      }
    }
  else
    {
    if (vtkPlane *p = this->GetReslicePlane())
      {
      double n[3], c[3], bounds[6];
      p->GetNormal(n);
      const double spacing =
          this->GetInterSliceSpacingInResliceMode() * inc;
      this->GetResliceCursor()->GetCenter(c);
      vtkMath::MultiplyScalar(n, spacing);
      c[0] += n[0];
      c[1] += n[1];
      c[2] += n[2];

      // If the new center is inside, put it there...
	  if (vtkImageData *image = this->GetResliceCursor()->GetImageHiRes())
        {
        image->GetBounds(bounds);
        if (c[0] >= bounds[0] && c[0] <= bounds[1] &&
            c[1] >= bounds[2] && c[1] <= bounds[3] &&
            c[2] >= bounds[4] && c[2] <= bounds[5])
          {
          this->GetResliceCursor()->SetCenter(c);

          this->InvokeEvent( myResliceImageViewer::SliceChangedEvent, NULL );
          this->InvokeEvent( vtkCommand::InteractionEvent, NULL );
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ResliceCursorWidget:\n";
  this->ResliceCursorWidget->PrintSelf(os,indent.GetNextIndent());
  os << indent << "ResliceMode: " << this->ResliceMode << endl;
  os << indent << "SliceScrollOnMouseWheel: " << this->SliceScrollOnMouseWheel << endl;
  os << indent << "Point Placer: ";
  this->PointPlacer->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Measurements: ";
  this->Measurements->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Interactor: " << this->Interactor << "\n";
  if (this->Interactor)
    {
    this->Interactor->PrintSelf(os,indent.GetNextIndent());
    }
}

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkResliceImageViewerMeasurements.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "myResliceImageViewerMeasurements.h"
#include "myResliceImageViewer.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkImageActor.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkDistanceRepresentation.h"
#include "vtkDistanceWidget.h"
#include "vtkCaptionWidget.h"
#include "vtkCaptionRepresentation.h"
#include "vtkAngleWidget.h"
#include "vtkAngleRepresentation.h"
#include "vtkHandleWidget.h"
#include "vtkBiDimensionalWidget.h"
#include "vtkBiDimensionalRepresentation.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkSeedWidget.h"
#include "vtkHandleWidget.h"
#include "vtkSeedRepresentation.h"
#include "vtkContourWidget.h"
#include "vtkContourRepresentation.h"
#include "vtkHandleRepresentation.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkPlane.h"
#include "vtkResliceCursor.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkPlane.h"
#include "vtkResliceImageViewer.h"

vtkStandardNewMacro(myResliceImageViewerMeasurements);

//----------------------------------------------------------------------------
myResliceImageViewerMeasurements::myResliceImageViewerMeasurements()
{
  this->ResliceImageViewer = NULL;
  this->WidgetCollection = vtkCollection::New();

  // Setup event processing
  this->EventCallbackCommand = vtkCallbackCommand::New();
  this->EventCallbackCommand->SetClientData(this);
  this->EventCallbackCommand->SetCallback(
    myResliceImageViewerMeasurements::ProcessEventsHandler);

  this->ProcessEvents = 1;
  this->Tolerance = 6;
}

//----------------------------------------------------------------------------
myResliceImageViewerMeasurements::~myResliceImageViewerMeasurements()
{
  // Remove any added observers
  if (this->ResliceImageViewer)
    {
    this->ResliceImageViewer->GetResliceCursor()->
        RemoveObservers(vtkResliceCursorWidget::ResliceAxesChangedEvent,
                     this->EventCallbackCommand );
    }

  this->WidgetCollection->Delete();
  this->EventCallbackCommand->Delete();
}

//----------------------------------------------------------------------------
void myResliceImageViewerMeasurements
::SetResliceImageViewer(myResliceImageViewer *i)
{
  // Weak reference. No need to delete
  this->ResliceImageViewer = i;

  if(i)
    {
    // Add the observer
    i->GetResliceCursor()
      ->AddObserver( vtkResliceCursorWidget::ResliceAxesChangedEvent,
        this->EventCallbackCommand );
    i->GetResliceCursor()
      ->AddObserver( vtkResliceCursorWidget::ResliceAxesChangedEvent,
        this->EventCallbackCommand );
    }
}

//----------------------------------------------------------------------------
void myResliceImageViewerMeasurements::Render()
{
  this->ResliceImageViewer->Render();
}

//-------------------------------------------------------------------------
void myResliceImageViewerMeasurements
::ProcessEventsHandler(vtkObject* ,
                       unsigned long,
                       void* clientdata,
                       void* )
{
	myResliceImageViewerMeasurements * self =
		reinterpret_cast<myResliceImageViewerMeasurements *>(clientdata);

  // if ProcessEvents is Off, we ignore all interaction events.
  if (!self->GetProcessEvents())
    {
    return;
    }

  self->Update();
}

//-------------------------------------------------------------------------
void myResliceImageViewerMeasurements::Update()
{
  if (this->ResliceImageViewer->GetResliceMode() !=
    vtkResliceImageViewer::RESLICE_OBLIQUE)
    {
    return; // nothing to do.
    }

  const int nItems = this->WidgetCollection->GetNumberOfItems();
  for (int i = 0; i < nItems; i++)
    {
    vtkAbstractWidget *a = vtkAbstractWidget::SafeDownCast(
                        this->WidgetCollection->GetItemAsObject(i) );

    vtkSeedWidget *s = vtkSeedWidget::SafeDownCast( a );

    // seed is handled differently since its really a collection of several
    // markers which may exist on different planes.
    if (!s)
      {
      a->SetEnabled(this->IsItemOnReslicedPlane(a));
      }
    }
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsItemOnReslicedPlane( vtkAbstractWidget * w )
{

  if (vtkDistanceWidget *dw = vtkDistanceWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(dw);
    }
  if (vtkAngleWidget *aw = vtkAngleWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(aw);
    }
  if (vtkBiDimensionalWidget *aw = vtkBiDimensionalWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(aw);
    }
  if (vtkCaptionWidget *capw = vtkCaptionWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(capw);
    }
  if (vtkContourWidget *capw = vtkContourWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(capw);
    }
  if (vtkSeedWidget *s = vtkSeedWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(s);
    }
  if (vtkHandleWidget *s = vtkHandleWidget::SafeDownCast( w ))
    {
    return this->IsWidgetOnReslicedPlane(s);
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkDistanceWidget * w )
{
  if (w->GetWidgetState() != vtkDistanceWidget::Manipulate)
    {
    return true; // widget is not yet defined.
    }

  if (vtkDistanceRepresentation *rep =
      vtkDistanceRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    return
      this->IsPointOnReslicedPlane( rep->GetPoint1Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetPoint2Representation() );
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkAngleWidget * w )
{
  if (w->GetWidgetState() != vtkAngleWidget::Manipulate)
    {
    return true; // widget is not yet defined.
    }

  if (vtkAngleRepresentation *rep =
      vtkAngleRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    return
      this->IsPointOnReslicedPlane( rep->GetPoint1Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetPoint2Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetCenterRepresentation() );
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkBiDimensionalWidget * w )
{
  if (w->GetWidgetState() != vtkBiDimensionalWidget::Manipulate)
    {
    return true; // widget is not yet defined.
    }

  if (vtkBiDimensionalRepresentation *rep =
      vtkBiDimensionalRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    return
      this->IsPointOnReslicedPlane( rep->GetPoint1Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetPoint2Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetPoint3Representation() ) &&
      this->IsPointOnReslicedPlane( rep->GetPoint4Representation() );
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkHandleWidget * w )
{
  return this->IsPointOnReslicedPlane(w->GetHandleRepresentation());
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkCaptionWidget * w )
{
  if (vtkCaptionRepresentation *rep =
      vtkCaptionRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    return
      this->IsPointOnReslicedPlane( rep->GetAnchorRepresentation() );
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkContourWidget * w )
{
  if (w->GetWidgetState() != vtkContourWidget::Manipulate)
    {
    return true; // widget is not yet defined.
    }

  if (vtkContourRepresentation *rep =
      vtkContourRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    const int nNodes = rep->GetNumberOfNodes();
    for (int i = 0; i < nNodes; i++)
      {
      double p[3];
      rep->GetNthNodeWorldPosition(i,p);
      if (this->IsPositionOnReslicedPlane(p) == false)
        {
        return false;
        }
      }
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsWidgetOnReslicedPlane( vtkSeedWidget * w )
{
  if (vtkSeedRepresentation *rep =
      vtkSeedRepresentation::SafeDownCast(w->GetRepresentation()))
    {
    const int nNodes = rep->GetNumberOfSeeds();
    for (int i = 0; i < nNodes; i++)
      {
      w->GetSeed(i)->GetHandleRepresentation()->SetVisibility(
          w->GetEnabled() && this->IsPointOnReslicedPlane(
            w->GetSeed(i)->GetHandleRepresentation()));

      }
    }

  return true;
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsPointOnReslicedPlane( vtkHandleRepresentation * h )
{
  double pos[3];
  h->GetWorldPosition(pos);
  return this->IsPositionOnReslicedPlane(pos);
}

//-------------------------------------------------------------------------
bool myResliceImageViewerMeasurements
::IsPositionOnReslicedPlane( double p[3] )
{
  if (vtkResliceCursorRepresentation *rep =
      vtkResliceCursorRepresentation::SafeDownCast(
        this->ResliceImageViewer->GetResliceCursorWidget()
                                      ->GetRepresentation()))
    {
    const int planeOrientation =
      rep->GetCursorAlgorithm()->GetReslicePlaneNormal();
    vtkPlane *plane = this->ResliceImageViewer->
        GetResliceCursor()->GetPlane(planeOrientation);
    const double d = plane->DistanceToPlane(p);
    return (d < this->Tolerance);
    }

  return true;
}

//-------------------------------------------------------------------------
void myResliceImageViewerMeasurements::AddItem( vtkAbstractWidget * w )
{
  this->WidgetCollection->AddItem(w);
}

//-------------------------------------------------------------------------
void myResliceImageViewerMeasurements::RemoveItem( vtkAbstractWidget * w )
{
  this->WidgetCollection->RemoveItem(w);
}

//-------------------------------------------------------------------------
void myResliceImageViewerMeasurements::RemoveAllItems()
{
  this->WidgetCollection->RemoveAllItems();
}

//----------------------------------------------------------------------------
void myResliceImageViewerMeasurements::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ResliceImageViewer: " << this->ResliceImageViewer << "\n";
  os << indent << "WidgetCollection: " << this->WidgetCollection << endl;
  this->WidgetCollection->PrintSelf(os,indent.GetNextIndent());

  os << indent << "ProcessEvents: "
    << (this->ProcessEvents? "On" : "Off") << "\n";

  os << indent << "Tolerance: " << this->Tolerance << endl;
}

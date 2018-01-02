/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkResliceCursorWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkResliceCursorWidget - represent a reslice cursor
// .SECTION Description
// This class represents a reslice cursor that can be used to
// perform interactive thick slab MPR's through data. It
// consists of two cross sectional hairs, with an optional thickness. The
// hairs may have a hole in the center. These may be translated or rotated
// independent of each other in the view. The result is used to reslice
// the data along these cross sections. This allows the user to perform
// multi-planar thin or thick reformat of the data on an image view, rather
// than a 3D view. The class internally uses vtkImageSlabReslice
// or vtkImageReslice depending on the modes in vtkResliceCursor to
// do its reslicing. The slab thickness is set interactively from
// the widget. The slab resolution (ie the number of blend points) is
// set as the minimum spacing along any dimension from the dataset.
// .SECTION See Also
// vtkImageSlabReslice vtkResliceCursorLineRepresentation
// vtkResliceCursor

#ifndef myResliceCursorWidget_h
#define myResliceCursorWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkAbstractWidget.h"

class vtkResliceCursorRepresentation;

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorWidget : public vtkAbstractWidget
{
public:
	bool EnableReslicing;
	bool EnableWindow;
  // Description:
  // Instantiate this class.
	static myResliceCursorWidget *New();

  // Description:
  // Standard VTK class macros.
	vtkTypeMacro(myResliceCursorWidget, vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkResliceCursorRepresentation *r)
    {this->Superclass::SetWidgetRepresentation(
        reinterpret_cast<vtkWidgetRepresentation*>(r));}

  // Description:
  // Return the representation as a vtkResliceCursorRepresentation.
  vtkResliceCursorRepresentation *GetResliceCursorRepresentation()
    {return reinterpret_cast<vtkResliceCursorRepresentation*>(this->WidgetRep);}

  // Description:
  // Create the default widget representation if one is not set.
  void CreateDefaultRepresentation();

  // Description:
  // Methods for activiating this widget. This implementation extends the
  // superclasses' in order to resize the widget handles due to a render
  // start event.
  virtual void SetEnabled(int);

  // Description:
  // Also perform window level ?
  vtkSetMacro( ManageWindowLevel, int );
  vtkGetMacro( ManageWindowLevel, int );
  vtkBooleanMacro( ManageWindowLevel, int );

  // Description:
  // Events
  enum
    {
    WindowLevelEvent = 1055,
    ResliceAxesChangedEvent,
    ResliceThicknessChangedEvent,
    ResetCursorEvent
    };

  // Description:
  // Reset the cursor back to its initial state
  virtual void ResetResliceCursor();

protected:
	myResliceCursorWidget();
	~myResliceCursorWidget();

  // These are the callbacks for this widget
  static void SelectAction(vtkAbstractWidget*);
  static void RotateAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);
  static void ResizeThicknessAction(vtkAbstractWidget*);
  static void EndResizeThicknessAction(vtkAbstractWidget*);
  static void MoveAction(vtkAbstractWidget*);
  static void ResetResliceCursorAction(vtkAbstractWidget*);

  // helper methods for cursor management
  void SetCursor(int state);

  // Start Window Level
  void StartWindowLevel();

  // Invoke the appropriate event based on state
  void InvokeAnEvent();

  // Manage the state of the widget
  int WidgetState;
  enum _WidgetState
  {
    Start=0,
    Active
  };

  // Keep track whether key modifier key is pressed
  int ModifierActive;
  int ManageWindowLevel;

private:
	myResliceCursorWidget(const myResliceCursorWidget&);  //Not implemented
	void operator=(const myResliceCursorWidget&);  //Not implemented
};

#endif
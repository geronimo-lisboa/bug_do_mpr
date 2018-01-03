
#ifndef myResliceCursorWidget_h
#define myResliceCursorWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkAbstractWidget.h"

class myResliceCursorRepresentation;

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
  void SetRepresentation(myResliceCursorRepresentation *r){
	  this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
  }

  // Description:
  // Return the representation as a myResliceCursorRepresentation.
  myResliceCursorRepresentation *GetResliceCursorRepresentation(){
	  return reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);
  }

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

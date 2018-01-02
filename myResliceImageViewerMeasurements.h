


#ifndef myResliceImageViewerMeasurements_h
#define myResliceImageViewerMeasurements_h

#include "vtkInteractionImageModule.h" // For export macro
#include "vtkObject.h"

class myResliceImageViewer;
class vtkAbstractWidget;
class vtkCallbackCommand;
class vtkCollection;
class vtkDistanceWidget;
class vtkAngleWidget;
class vtkBiDimensionalWidget;
class vtkHandleRepresentation;
class vtkHandleWidget;
class vtkCaptionWidget;
class vtkContourWidget;
class vtkSeedWidget;

class VTKINTERACTIONIMAGE_EXPORT myResliceImageViewerMeasurements : public vtkObject
{
public:

  // Description:
  // Standard VTK methods.
	static myResliceImageViewerMeasurements *New();
	vtkTypeMacro(myResliceImageViewerMeasurements, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Render the measurements.
  virtual void Render();

  // Description:
  // Add / remove a measurement widget
  virtual void AddItem(vtkAbstractWidget *);
  virtual void RemoveItem(vtkAbstractWidget *);
  virtual void RemoveAllItems();

  // Description:
  // Methods to change whether the widget responds to interaction.
  // Set this to Off to disable interaction. On by default.
  // Subclasses must overide SetProcessEvents() to make sure
  // that they pass on the flag to all component widgets.
  vtkSetClampMacro(ProcessEvents, int, 0, 1);
  vtkGetMacro(ProcessEvents, int);
  vtkBooleanMacro(ProcessEvents, int);

  // Description:
  // Tolerance for Point-in-Plane check
  vtkSetMacro( Tolerance, double );
  vtkGetMacro( Tolerance, double );

  // Description:
  // Set the reslice image viewer. This is automatically done in the class
  // myResliceImageViewer
  virtual void SetResliceImageViewer(myResliceImageViewer *);
  vtkGetObjectMacro(ResliceImageViewer, myResliceImageViewer);

  // Description:
  // Update the measurements. This is automatically called when the reslice
  // cursor's axes are change.
  virtual void Update();

protected:
	myResliceImageViewerMeasurements();
	~myResliceImageViewerMeasurements();

  // Description:
  // Check if a measurement widget is on the resliced plane.
  bool IsItemOnReslicedPlane( vtkAbstractWidget * w );
  bool IsWidgetOnReslicedPlane( vtkDistanceWidget * w );
  bool IsWidgetOnReslicedPlane( vtkAngleWidget * w );
  bool IsWidgetOnReslicedPlane( vtkBiDimensionalWidget * w );
  bool IsWidgetOnReslicedPlane( vtkCaptionWidget * w );
  bool IsWidgetOnReslicedPlane( vtkContourWidget * w );
  bool IsWidgetOnReslicedPlane( vtkSeedWidget * w );
  bool IsWidgetOnReslicedPlane( vtkHandleWidget * w );
  bool IsPointOnReslicedPlane( vtkHandleRepresentation * h );
  bool IsPositionOnReslicedPlane( double p[3] );

  // Handles the events; centralized here for all widgets.
  static void ProcessEventsHandler(vtkObject* object, unsigned long event,
                            void* clientdata, void* calldata);

  myResliceImageViewer * ResliceImageViewer;
  vtkCollection         * WidgetCollection;

  // Handle the visibility of the measurements.
  vtkCallbackCommand    * EventCallbackCommand; //

  // Flag indicating if we should handle events.
  // On by default.
  int ProcessEvents;

  // Tolerance for Point-in-plane computation
  double Tolerance;

private:
	myResliceImageViewerMeasurements(const myResliceImageViewerMeasurements&);  // Not implemented.
	void operator=(const myResliceImageViewerMeasurements&);  // Not implemented.
};

#endif

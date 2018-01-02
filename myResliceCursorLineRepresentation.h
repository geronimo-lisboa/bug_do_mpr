

#ifndef myResliceCursorLineRepresentation_h
#define myResliceCursorLineRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "myResliceCursorRepresentation.h"

class vtkPolyData;
class myResliceCursorActor;
class myResliceCursorPolyDataAlgorithm;
class myResliceCursorPicker;
class myResliceCursor;


class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorLineRepresentation : public myResliceCursorRepresentation
{
public:
  // Description:
  // Instantiate the class.
  static myResliceCursorLineRepresentation *New();

  // Description:
  // Standard VTK methods.
  vtkTypeMacro(myResliceCursorLineRepresentation,myResliceCursorRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();
  virtual int  ComputeInteractionState(int X, int Y, int modify=0);
  virtual void StartWidgetInteraction(double startEventPos[2]);
  virtual void WidgetInteraction(double e[2]);
  virtual void Highlight(int highlightOn);

  // Description:
  // Methods required by vtkProp superclass.
  virtual void ReleaseGraphicsResources(vtkWindow *w);
  virtual int RenderOverlay(vtkViewport *viewport);
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual int HasTranslucentPolygonalGeometry();

  // Description:
  // Get the bounds of this prop. This simply returns the bounds of the
  // reslice cursor object.
  virtual double * GetBounds();

  // Description:
  // Get the reslice cursor actor. You must set the reslice cursor on this
  // class
  vtkGetObjectMacro( ResliceCursorActor, myResliceCursorActor );

  // Description:
  // Get the reslice cursor.
  virtual myResliceCursor * GetResliceCursor();

  // Description:
  // Set the user matrix on all the internal actors.
  virtual void SetUserMatrix( vtkMatrix4x4 *matrix);

protected:
  myResliceCursorLineRepresentation();
  ~myResliceCursorLineRepresentation();

  virtual myResliceCursorPolyDataAlgorithm * GetCursorAlgorithm();

  double RotateAxis( double evenPos[2], int axis );

  void RotateAxis( int axis, double angle );

  void RotateVectorAboutVector( double vectorToBeRotated[3],
                                double axis[3], // vector about which we rotate
                                double angle, // angle in radians
                                double o[3] );
  int DisplayToReslicePlaneIntersection(
    double displayPos[2], double intersectionPos[3] );

  myResliceCursorActor  * ResliceCursorActor;
  myResliceCursorPicker * Picker;

  double                   StartPickPosition[3];
  double                   StartCenterPosition[3];

  // Transformation matrices. These have no offset. Offset is recomputed
  // based on the cursor, so that the center of the cursor has the same
  // location in transformed space as it does in physical space.
  vtkMatrix4x4           * MatrixReslice;
  vtkMatrix4x4           * MatrixView;
  vtkMatrix4x4           * MatrixReslicedView;

private:
  myResliceCursorLineRepresentation(const myResliceCursorLineRepresentation&);  //Not implemented
  void operator=(const myResliceCursorLineRepresentation&);  //Not implemented
};

#endif



#ifndef myResliceCursorPicker_h
#define myResliceCursorPicker_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkPicker.h"

class vtkPolyData;
class vtkGenericCell;
class myResliceCursorPolyDataAlgorithm;
class vtkIdList;
class vtkMatrix4x4;
class vtkPlane;

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorPicker : public vtkPicker
{
public:
  static myResliceCursorPicker *New();
  vtkTypeMacro(myResliceCursorPicker, vtkPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform pick operation with selection point provided. Normally the
  // first two values are the (x,y) pixel coordinates for the pick, and
  // the third value is z=0. The return value will be non-zero if
  // something was successfully picked.
  virtual int Pick(double selectionX, double selectionY, double selectionZ,
                   vtkRenderer *renderer);

  // Description:
  // Get the picked axis
  vtkGetMacro( PickedAxis1, int );
  vtkGetMacro( PickedAxis2, int );
  vtkGetMacro( PickedCenter, int );

  // Description:
  // Set the reslice cursor algorithm. One must be set
  virtual void SetResliceCursorAlgorithm(
      myResliceCursorPolyDataAlgorithm * );
  vtkGetObjectMacro( ResliceCursorAlgorithm,
                     myResliceCursorPolyDataAlgorithm );

  virtual void SetTransformMatrix( vtkMatrix4x4 * );

  // Description:
  // Overloaded pick method that returns the picked coordinates of the current
  // resliced  plane in world coordinates when given a display position
  void Pick(
    double displayPos[2], double world[3], vtkRenderer *ren );

protected:
  myResliceCursorPicker();
  ~myResliceCursorPicker();

  virtual int IntersectPolyDataWithLine(
      double p1[3], double p2[3], vtkPolyData *, double tol );
  virtual int IntersectPointWithLine(
    double p1[3], double p2[3], double X[3], double tol );

  void TransformPlane();
  void TransformPoint( double pIn[4], double pOut[4] );
  void InverseTransformPoint( double pIn[4], double pOut[4] );

private:

  vtkGenericCell *Cell; //used to accelerate picking
  myResliceCursorPolyDataAlgorithm * ResliceCursorAlgorithm;

  int PickedAxis1;
  int PickedAxis2;
  int PickedCenter;
  vtkIdList *PointIds;
  vtkMatrix4x4 * TransformMatrix;
  vtkPlane     * Plane;

private:
  myResliceCursorPicker(const myResliceCursorPicker&);  // Not implemented.
  void operator=(const myResliceCursorPicker&);  // Not implemented.
};

#endif

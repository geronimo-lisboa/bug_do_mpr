

#ifndef myResliceCursorPolyDataAlgorithm_h
#define myResliceCursorPolyDataAlgorithm_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkCutter;
class myResliceCursor;
class vtkPlane;
class vtkBox;
class vtkClipPolyData;
class vtkLinearExtrusionFilter;

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorPolyDataAlgorithm : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(myResliceCursorPolyDataAlgorithm,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static myResliceCursorPolyDataAlgorithm *New();

  // Description:
  // Which of the 3 axes defines the reslice plane normal ?
  vtkSetMacro(ReslicePlaneNormal,int);
  vtkGetMacro(ReslicePlaneNormal,int);

//BTX
  enum {XAxis=0,YAxis,ZAxis};
//ETX

  // Description:
  // Set the planes that correspond to the reslice axes.
  void SetReslicePlaneNormalToXAxis()
    { this->SetReslicePlaneNormal(XAxis); }
  void SetReslicePlaneNormalToYAxis()
    { this->SetReslicePlaneNormal(YAxis); }
  void SetReslicePlaneNormalToZAxis()
    { this->SetReslicePlaneNormal(ZAxis); }

  // Description:
  // Set the Reslice cursor from which to generate the polydata representation
  virtual void SetResliceCursor( myResliceCursor * );
  vtkGetObjectMacro( ResliceCursor, myResliceCursor );

  // Description:
  // Set/Get the slice bounds, ie the slice of this view on which to display
  // the reslice cursor.
  vtkSetVector6Macro( SliceBounds, double );
  vtkGetVector6Macro( SliceBounds, double );

  // Description:
  // Get either one of the axes that this object produces. Depending on
  // the mode, one renders either the centerline axes or both the
  // centerline axes and the slab
  virtual vtkPolyData * GetCenterlineAxis1();
  virtual vtkPolyData * GetCenterlineAxis2();
  virtual vtkPolyData * GetThickSlabAxis1();
  virtual vtkPolyData * GetThickSlabAxis2();

  // Description:
  // Get the index of the axes and the planes that they represent
  virtual int GetAxis1();
  virtual int GetAxis2();
  virtual int GetPlaneAxis1();
  virtual int GetPlaneAxis2();

  // Description:
  // Convenience method that, given one plane, returns the other plane
  // that this class represents.
  int GetOtherPlaneForAxis( int p );

  // Description:
  // Get the MTime. Check the MTime of the internal ResliceCursor as well, if
  // one has been set
  virtual unsigned long int GetMTime();

protected:
  myResliceCursorPolyDataAlgorithm();
  ~myResliceCursorPolyDataAlgorithm();

  int RequestData(vtkInformation*,
                  vtkInformationVector**,
                  vtkInformationVector*);

  void GetSlabPolyData( int axis, int planeAxis, vtkPolyData *pd );

  virtual void CutAndClip( vtkPolyData *in, vtkPolyData *out);

  // Build the reslice slab axis
  void BuildResliceSlabAxisTopology();

  int                ReslicePlaneNormal;
  myResliceCursor * ResliceCursor;
  vtkCutter        * Cutter;
  vtkPlane         * SlicePlane;
  vtkBox           * Box;
  vtkClipPolyData  * ClipWithBox;
  double             SliceBounds[6];
  bool               Extrude;
  vtkLinearExtrusionFilter *ExtrusionFilter1;
  vtkLinearExtrusionFilter *ExtrusionFilter2;
  vtkPolyData              *ThickAxes[2];

private:
  myResliceCursorPolyDataAlgorithm(const myResliceCursorPolyDataAlgorithm&);  // Not implemented.
  void operator=(const myResliceCursorPolyDataAlgorithm&);  // Not implemented.

};

#endif

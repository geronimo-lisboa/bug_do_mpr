

#ifndef myResliceCursorActor_h
#define myResliceCursorActor_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkProp3D.h"

class myResliceCursor;
class myResliceCursorPolyDataAlgorithm;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkBoundingBox;

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorActor : public vtkProp3D
{

public:

  // Description:
  // Standard VTK methods
  static myResliceCursorActor *New();
  vtkTypeMacro(myResliceCursorActor,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the cursor algorithm. The cursor must be set on the algorithm
  vtkGetObjectMacro( CursorAlgorithm, myResliceCursorPolyDataAlgorithm );

  // Description:
  // Support the standard render methods.
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);

  // Description:
  // Does this prop have some translucent polygonal geometry? No.
  virtual int HasTranslucentPolygonalGeometry();

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
  double *GetBounds();

  // Description:
  // Get the actors mtime plus consider its algorithm.
  unsigned long int GetMTime();

  // Description:
  // Get property of the internal actor.
  vtkProperty *GetCenterlineProperty( int i );
  vtkProperty *GetThickSlabProperty( int i );

  // Description:
  // Get the centerline actor along a particular axis
  vtkActor * GetCenterlineActor(int axis);

  // Description:
  // Set the user matrix on all the internal actors.
  virtual void SetUserMatrix( vtkMatrix4x4 *matrix);

protected:
  myResliceCursorActor();
  ~myResliceCursorActor();

  void UpdateViewProps( vtkViewport * v = NULL );
  void UpdateHoleSize( vtkViewport * v );

  myResliceCursorPolyDataAlgorithm * CursorAlgorithm;
  vtkPolyDataMapper                 * CursorCenterlineMapper[3];
  vtkActor                          * CursorCenterlineActor[3];
  vtkPolyDataMapper                 * CursorThickSlabMapper[3];
  vtkActor                          * CursorThickSlabActor[3];
  vtkProperty                       * CenterlineProperty[3];
  vtkProperty                       * ThickSlabProperty[3];

private:
  myResliceCursorActor(const myResliceCursorActor&);  // Not implemented.
  void operator=(const myResliceCursorActor&);  // Not implemented.
};

#endif

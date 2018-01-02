


#ifndef myResliceImageViewer_h
#define myResliceImageViewer_h

#include "vtkInteractionImageModule.h" // For export macro
#include "vtkImageViewer2.h"

class myResliceCursorWidget;
class myResliceCursor;
class vtkScalarsToColors;
class vtkBoundedPlanePointPlacer;
class myResliceImageViewerMeasurements;
class myResliceImageViewerScrollCallback;
class vtkPlane;

class VTKINTERACTIONIMAGE_EXPORT myResliceImageViewer : public vtkImageViewer2
{
public:

  // Description:
  // Standard VTK methods.
  static myResliceImageViewer *New();
  vtkTypeMacro(myResliceImageViewer, vtkImageViewer2);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Render the resulting image.
  virtual void Render();

  // Description:
  // Set/Get the input image to the viewer.
  virtual void SetInputData(vtkImageData *imgHiRes, vtkImageData *imgLowRes);
  virtual void SetInputConnection(vtkAlgorithmOutput* input);

  // Description:
  // Set window and level for mapping pixels to colors.
  virtual void SetColorWindow(double s);
  virtual void SetColorLevel(double s);

  // Description:
  // Get the internal render window, renderer, image actor, and
  // image map instances.
  vtkGetObjectMacro(ResliceCursorWidget, myResliceCursorWidget);

  // Description:
  // Set/get the slice orientation
  //BTX
  enum
  {
    RESLICE_AXIS_ALIGNED = 0,
    RESLICE_OBLIQUE = 1
  };
  //ETX

  vtkGetMacro(ResliceMode, int);
  virtual void SetResliceMode(int resliceMode);
  virtual void SetResliceModeToAxisAligned()
  {
	  this->SetResliceMode(myResliceImageViewer::RESLICE_AXIS_ALIGNED);
  };
  virtual void SetResliceModeToOblique()
  {
	  this->SetResliceMode(myResliceImageViewer::RESLICE_OBLIQUE);
  };

  // Description:
  // Set/Get the reslice cursor.
  myResliceCursor * GetResliceCursor();
  void SetResliceCursor(myResliceCursor * rc);

  // Description:
  // Set the lookup table
  virtual void SetLookupTable( vtkScalarsToColors * );
  vtkScalarsToColors * GetLookupTable();

  // Description:
  // Switch to / from thick mode
  virtual void SetThickMode( int );
  virtual int GetThickMode();

  // Description:
  // Reset all views back to initial state
  virtual void Reset();

  // Description:
  // Get the point placer.
  vtkGetObjectMacro( PointPlacer, vtkBoundedPlanePointPlacer );

  // Description:
  // Get the measurements manager
  vtkGetObjectMacro( Measurements, myResliceImageViewerMeasurements );

  // Description:
  // Get the render window interactor
  vtkGetObjectMacro( Interactor, vtkRenderWindowInteractor );

  // Description:
  // Scroll slices on the mouse wheel ? In the case of MPR
  // view, it moves one "normalized spacing" in the direction of the normal to
  // the resliced plane, provided the new center will continue to lie within
  // the volume.
  vtkSetMacro( SliceScrollOnMouseWheel, int );
  vtkGetMacro( SliceScrollOnMouseWheel, int );
  vtkBooleanMacro( SliceScrollOnMouseWheel, int );

  // Description:
  // Increment/Decrement slice by 'n' slices
  virtual void IncrementSlice( int n );

  // Description:
  enum { SliceChangedEvent = 1001 };

protected:
	myResliceImageViewer();
	~myResliceImageViewer();

  virtual void InstallPipeline();
  virtual void UnInstallPipeline();
  virtual void UpdateOrientation();
  virtual void UpdateDisplayExtent();
  virtual void UpdatePointPlacer();

  // Description:
  // Convenience methods to get the reslice plane and the normalized
  // spacing between slices in reslice mode.
  vtkPlane * GetReslicePlane();
  double GetInterSliceSpacingInResliceMode();

  myResliceCursorWidget            * ResliceCursorWidget;
  vtkBoundedPlanePointPlacer        * PointPlacer;
  int                                 ResliceMode;
  myResliceImageViewerMeasurements * Measurements;
  int                                 SliceScrollOnMouseWheel;
  myResliceImageViewerScrollCallback * ScrollCallback;

private:
	myResliceImageViewer(const myResliceImageViewer&);  // Not implemented.
	void operator=(const myResliceImageViewer&);  // Not implemented.
};

#endif

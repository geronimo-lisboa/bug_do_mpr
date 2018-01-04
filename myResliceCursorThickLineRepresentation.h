
#ifndef myResliceCursorThickLineRepresentation_h
#define myResliceCursorThickLineRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "myResliceCursorLineRepresentation.h"
#include "myAfterReslicedImageGeneratedCallback.h"
#include <vector>

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorThickLineRepresentation : public myResliceCursorLineRepresentation//myResliceCursorRepresentation
{
public:
  // Description:
  // Instantiate the class.
	static myResliceCursorThickLineRepresentation *New();

  // Description:
  // Standard VTK methods.
	vtkTypeMacro(myResliceCursorThickLineRepresentation, myResliceCursorLineRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // INTERNAL - Do not use
  // Create the thick reformat class. This overrides the superclass
  // implementation and creates a vtkImageSlabReslice instead of a
  // vtkImageReslice.
  virtual void CreateDefaultResliceAlgorithm();

  // Description:
  // INTERNAL - Do not use
  // Reslice parameters which are set from myResliceCursorWidget based on
  // user interactions.
  virtual void SetResliceParameters(
      double outputSpacingX, double outputSpacingY,
      int extentX, int extentY );

  void AddResliceResultListener(myAfterReslicedImageGeneratedCallback* l){
	  this->resliceResultListeners.push_back(l);
  }
protected:
	myResliceCursorThickLineRepresentation();
	~myResliceCursorThickLineRepresentation();
	std::vector<myAfterReslicedImageGeneratedCallback*> resliceResultListeners;
private:
	myResliceCursorThickLineRepresentation(const myResliceCursorThickLineRepresentation&);  //Not implemented
	void operator=(const myResliceCursorThickLineRepresentation&);  //Not implemented
};

#endif

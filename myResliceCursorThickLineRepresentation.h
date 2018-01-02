/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkResliceCursorLineRepresentation.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkResliceCursorThickLineRepresentation - represents a thick slab of the reslice cursor widget
// .SECTION Description
// This class respresents a thick reslice cursor, that can be used to
// perform interactive thick slab MPR's through data. The class internally
// uses vtkImageSlabReslice to do its reslicing. The slab thickness is set
// interactively from the widget. The slab resolution (ie the number of
// blend points) is set as the minimum spacing along any dimension from
// the dataset.
// .SECTION See Also
// vtkImageSlabReslice vtkResliceCursorLineRepresentation vtkResliceCursorWidget

#ifndef vtkResliceCursorThickLineRepresentation_h
#define vtkResliceCursorThickLineRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "myResliceCursorRepresentation.h"

class VTKINTERACTIONWIDGETS_EXPORT myResliceCursorThickLineRepresentation : public myResliceCursorRepresentation
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
  // Reslice parameters which are set from vtkResliceCursorWidget based on
  // user interactions.
  virtual void SetResliceParameters(
      double outputSpacingX, double outputSpacingY,
      int extentX, int extentY );

protected:
	myResliceCursorThickLineRepresentation();
	~myResliceCursorThickLineRepresentation();

private:
	myResliceCursorThickLineRepresentation(const myResliceCursorThickLineRepresentation&);  //Not implemented
	void operator=(const myResliceCursorThickLineRepresentation&);  //Not implemented
};

#endif

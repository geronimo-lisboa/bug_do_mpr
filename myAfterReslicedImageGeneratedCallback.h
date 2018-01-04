#pragma once
#include <vtkImageSlabReslice.h>

class myAfterReslicedImageGeneratedCallback{
public:
	virtual void ReslicedImageCreated(vtkImageSlabReslice *slabAlgo) = 0;
};
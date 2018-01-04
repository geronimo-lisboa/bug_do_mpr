#pragma once
#include <vtkImageSlabReslice.h>

class myAfterReslicedImageGeneratedCallback{
public:
	virtual void ReslicedImageCreated(int id, vtkImageSlabReslice *slabAlgo) = 0;
};
#pragma once
#include "myAfterReslicedImageGeneratedCallback.h"
#include <vtkSmartPointer.h>
#include "myResliceCursorCallback.h"
#include <memory>
#include <array>
#include "MPRView.h"
#include "vtkImageData.h"
#include "vtkImageSlabReslice.h"

class Sistema : public myAfterReslicedImageGeneratedCallback{
private:
	vtkSmartPointer<myResliceCursorCallback> resliceCursorCallback;
	array<shared_ptr<MPRView>, 3> mprs;
	vtkSmartPointer<vtkImageData> imagemHiRes, imagemLowRes;
public:
	Sistema(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes);
	void ReslicedImageCreated(int id, vtkImageSlabReslice *slabAlgo) override;
};
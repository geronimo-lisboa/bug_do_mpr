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

	Sistema();
	void SetImages(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes);
	void CreateView(int id);
	void CreateView(HWND handle, int id);
	void LinkEverything();
	void ResizeWindow(int qual, int w, int h);
	void SetSlabThickness(int thickness);
	void Reset();
	//O = MIP
	//1 = Composite
	void SetFuncaoDeRenderizacao(int idFn);
	//eventos do mouse
	void MouseMove(UINT nFlags, int x, int y, int qual);
	void LMouseDown(UINT nFlags, int x, int y, int qual);
	void LMouseUp(UINT nFlags, int x, int y, int qual);
	void MMouseDown(UINT nFlags, int x, int y, int qual);
	void MMouseUp(UINT nFlags, int x, int y, int qual);
	void RMouseDown(UINT nFlags, int x, int y, int qual);
	void RMouseUp(UINT nFlags, int x, int y, int qual);
};
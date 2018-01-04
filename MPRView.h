#pragma once
#include "IMPRView.h"
#include "vtkCommand.h"
#include "myResliceCursorCallback.h"
class MPRView : public IMPRView, public vtkCommand{
private:
	vtkSmartPointer<myResliceImageViewer> resliceViewer;
	vtkSmartPointer<vtkImageData> imagemHiRes, imagemLowRes;
	vtkSmartPointer<myResliceCursor> sharedCursor;
	myResliceCursorCallback *resliceCallback;
public:
	MPRView(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes, int _id);
	vtkSmartPointer<myResliceImageViewer> GetmyResliceImageViewer();
	void Atualizar();
	~MPRView(){};
	void SetCallbacks(vtkSmartPointer<myResliceCursor> _sharedCursor, 
		vtkSmartPointer<myResliceCursorCallback> rcbk);
	void Execute(vtkObject * caller, unsigned long event, void* calldata) override;
	void AddAfterResliceListener(myAfterReslicedImageGeneratedCallback* l){
		resliceViewer->AddAfterResliceListener(l);
	}
};
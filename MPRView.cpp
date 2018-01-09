#include "MPRView.h"
#include <vtkRenderer.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWidgetRepresentation.h>
#include "myResliceCursorRepresentation.h"
#include <boost\assert.hpp>
#include "myResliceImageViewer.h"
#include "myResliceCursorWidget.h"
#include "myResliceCursorLineRepresentation.h"
#include "myResliceCursorActor.h"
#include "myResliceCursorPolyDataAlgorithm.h"
#include <boost\lexical_cast.hpp>
#include <vtkInteractorStyleImage.h>
#include <vtkImageData.h>
#include "myResliceCursor.h"
#include <vtkWin32RenderWindowInteractor.h>
#include <Windows.h>

void  MPRView::Resize(int w,int  h){
	this->resliceViewer->GetRenderWindow()->SetSize(w, h);
}

void MPRView::Execute(vtkObject * caller, unsigned long event, void* calldata){
	vtkRenderer *ren = vtkRenderer::SafeDownCast(caller);
	if (ren){
		this->resliceViewer->GetRenderPassDasLetras()->Calculate(resliceViewer->GetRenderer());
	}
}



void MPRView::SetToMIP(){
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceHiRes());
	thickSlabReslice->SetBlendModeToMax();
	thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceLowRes());
	thickSlabReslice->SetBlendModeToMax();
}

void MPRView::SetToMean(){
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceHiRes());
	thickSlabReslice->SetBlendModeToMean();
	thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceLowRes());
	thickSlabReslice->SetBlendModeToMean();
}


void MPRView::SetToMINP(){
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceHiRes());
	thickSlabReslice->SetBlendModeToMin();
	thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceLowRes());
	thickSlabReslice->SetBlendModeToMin();
}

MPRView::MPRView(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes, int _id, HWND handle){
	id = _id;
	imagemHiRes = imgHiRes;
	imagemLowRes = imgLowRes;
	resliceViewer = vtkSmartPointer<myResliceImageViewer>::New();
	resliceViewer->SetThickMode(1);
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	if (handle){
		renderWindow->SetWindowId(handle);
	}
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkWin32RenderWindowInteractor> interactor = vtkSmartPointer<vtkWin32RenderWindowInteractor>::New();
	interactor->InstallMessageProcOn();
	interactor->SetRenderWindow(renderWindow);
	resliceViewer->SetRenderWindow(renderWindow);
	resliceViewer->SetupInteractor(interactor);
	resliceViewer->SetResliceModeToOblique();
	resliceViewer->SetInputData(imagemHiRes, imagemLowRes);
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	//tl->UseImageActorOff();//teste
	//tl->RestrictPlaneToVolumeOff();//teste
	vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceHiRes());
	BOOST_ASSERT((thickSlabReslice != nullptr));//sanity check do cast
	//thickSlabReslice->SetSlabModeToMax();//Seta pra mip
	//thickSlabReslice->SetSlabNumberOfSlices(1);
	myResliceCursorLineRepresentation *cursorRepresentation = myResliceCursorLineRepresentation::SafeDownCast(
		resliceViewer->GetResliceCursorWidget()->GetRepresentation());
	resliceViewer->GetResliceCursorWidget()->ManageWindowLevelOff();
	cursorRepresentation->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(id);
	string nomeDaTela = "mpr " + boost::lexical_cast<std::string>(id);
	renderWindow->SetWindowName(nomeDaTela.c_str());
	//renderWindow->Render();
	renderer->AddObserver(vtkCommand::EndEvent, this);
}

vtkSmartPointer<myResliceImageViewer> MPRView::GetmyResliceImageViewer(){
	return this->resliceViewer;
}

void MPRView::Atualizar(){
	resliceViewer->Render();

}

void MPRView::SetCallbacks(vtkSmartPointer<myResliceCursor> _sharedCursor, vtkSmartPointer<myResliceCursorCallback> rcbk){
	resliceCallback = rcbk;
	sharedCursor = _sharedCursor;
	resliceViewer->SetResliceCursor(sharedCursor);
	resliceViewer->GetResliceCursorWidget()->AddObserver(myResliceCursorWidget::ResliceThicknessChangedEvent, resliceCallback);
	resliceViewer->GetResliceCursorWidget()->AddObserver(myResliceCursorWidget::ResetCursorEvent, resliceCallback);
	resliceViewer->GetResliceCursorWidget()->AddObserver(myResliceCursorWidget::WindowLevelEvent, resliceCallback);
	resliceViewer->GetResliceCursorWidget()->AddObserver(myResliceCursorWidget::ResliceAxesChangedEvent, resliceCallback);
	resliceViewer->GetInteractorStyle()->AddObserver(vtkCommand::WindowLevelEvent, resliceCallback);
}

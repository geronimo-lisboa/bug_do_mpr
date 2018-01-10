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
#include <vtkOpenGLRenderer.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkPropCollection.h>
#include <vtkImageActor.h>

void  MPRView::Resize(int w,int  h){
	this->resliceViewer->GetRenderWindow()->SetSize(w, h);
}

void MPRView::Execute(vtkObject * caller, unsigned long event, void* calldata){
	vtkOpenGLRenderer *ren = vtkOpenGLRenderer::SafeDownCast(caller);
	if (ren){
		this->resliceViewer->GetRenderPassDasLetras()->Calculate(resliceViewer->GetRenderer());
		if (this->id == 0){
		ren->GetRenderWindow()->GetRenderers()->InitTraversal();
		vtkOpenGLRenderer *camadaDaImagem; vtkOpenGLRenderer *camadaDoWidget;
		for (auto i = 0; i < ren->GetRenderWindow()->GetRenderers()->GetNumberOfItems(); i++){
			vtkOpenGLRenderer* r = vtkOpenGLRenderer::SafeDownCast(ren->GetRenderWindow()->GetRenderers()->GetNextItemAsObject());
			if (r->GetLayer() == 0)
				camadaDaImagem = r;
			if (r->GetLayer() == 1)
				camadaDoWidget = r;
		}
		cout << camadaDaImagem->GetViewProps()->GetNumberOfItems() << endl;
		cout << camadaDoWidget->GetViewProps()->GetNumberOfItems() << endl;
		vtkPropCollection *propCollection = camadaDoWidget->GetViewProps();
		propCollection->InitTraversal();
		for (auto i = 0; i < propCollection->GetNumberOfItems(); i++){
			vtkImageActor *actor = vtkImageActor::SafeDownCast(propCollection->GetNextItemAsObject());
			if (actor){
				camadaDaImagem->AddActor2D(actor);
				ren->GetRenderWindow()->Render();
			}
		}
		}
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

	vtkSmartPointer<vtkOpenGLRenderer> rendererImagem = vtkSmartPointer<vtkOpenGLRenderer>::New();
	rendererImagem->SetBackground(1, 0, 0);
	rendererImagem->SetLayer(0);

	resliceViewer = vtkSmartPointer<myResliceImageViewer>::New();
	resliceViewer->SetThickMode(1);
	vtkSmartPointer<vtkOpenGLRenderer> renderer = vtkSmartPointer<vtkOpenGLRenderer>::New();
	renderer->SetLayer(1);
	vtkSmartPointer<vtkWin32OpenGLRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
	renderWindow->SetNumberOfLayers(2);
	if (handle){
		renderWindow->SetWindowId(handle);
	}
	renderWindow->AddRenderer(renderer);
	renderWindow->AddRenderer(rendererImagem);
	vtkSmartPointer<vtkWin32RenderWindowInteractor> interactor = vtkSmartPointer<vtkWin32RenderWindowInteractor>::New();
	interactor->InstallMessageProcOn();
	interactor->SetRenderWindow(renderWindow);
	resliceViewer->SetRenderWindow(renderWindow);
	resliceViewer->SetupInteractor(interactor);
	resliceViewer->SetResliceModeToOblique();
	resliceViewer->SetInputData(imagemHiRes, imagemLowRes);
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	tl->ShowReslicedImageOff();
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

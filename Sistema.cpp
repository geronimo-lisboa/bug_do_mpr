#include "Sistema.h"
#include "myQualityControllable.h"
#include "myResliceImageViewer.h"
#include "myResliceCursorWidget.h"
#include "MPRView.h"
#include "myResliceImageViewer.h"
#include "myResliceCursor.h"
#include <Windows.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleImage.h>

void Sistema::ReslicedImageCreated(int id, vtkImageSlabReslice *slabAlgo){
	std::cout << __FUNCTION__ << std::endl;
	//boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	//long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();
	//std::string filename = "c:\\tela "+boost::lexical_cast<std::string>(id)+"-" + boost::lexical_cast<std::string>(milliseconds) + ".vti";
	//vtkSmartPointer<vtkXMLImageDataWriter> debugsave = vtkSmartPointer<vtkXMLImageDataWriter>::New();
	//debugsave->SetFileName(filename.c_str());
	//debugsave->SetInputConnection(slabAlgo->GetOutputPort());
	//debugsave->Update();
}

void Sistema::ResizeWindow(int qual, int w, int h){
	mprs[qual]->Resize(w, h);
}

void Sistema::LinkEverything(){
	//Liga os callbacks
	for (std::shared_ptr<MPRView> m : mprs){
		m->SetCallbacks(mprs[0]->GetmyResliceImageViewer()->GetResliceCursor(), resliceCursorCallback);
		resliceCursorCallback->AddMPR(m);
	}
	//Liga os controles de qualidade
	myQualityControllable *c0 = mprs[0]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	myQualityControllable *c1 = mprs[1]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	myQualityControllable *c2 = mprs[2]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	mprs[0]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c1, c2);
	mprs[1]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c2);
	mprs[2]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c1);
	for (auto m : mprs)
		m->AddAfterResliceListener(this);
	for (auto m : mprs)
		m->GetmyResliceImageViewer()->Render();
	for (auto m : mprs)
		m->Atualizar();
}

Sistema::Sistema(){
	imagemHiRes = nullptr;
	imagemLowRes = nullptr;
	resliceCursorCallback = vtkSmartPointer<myResliceCursorCallback>::New();
}

void Sistema::SetImages(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes){
	imagemHiRes = imgHiRes;
	imagemLowRes = imgLowRes;
}

void Sistema::CreateView(int id){
	mprs[id] = make_shared<MPRView>(imagemHiRes, imagemLowRes, id);
}

void  Sistema::CreateView(HWND handle, int id){
	mprs[id] = make_shared<MPRView>(imagemHiRes, imagemLowRes, id, handle);
}

Sistema::Sistema(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes){
	imagemHiRes = imgHiRes;
	imagemLowRes = imgLowRes;
	mprs[0] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 0);
	mprs[1] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 1);
	mprs[2] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 2);
	resliceCursorCallback = vtkSmartPointer<myResliceCursorCallback>::New();
	//Liga os callbacks
	for (std::shared_ptr<MPRView> m : mprs){
		m->SetCallbacks(mprs[0]->GetmyResliceImageViewer()->GetResliceCursor(), resliceCursorCallback);
		resliceCursorCallback->AddMPR(m);
	}
	//Liga os controles de qualidade
	myQualityControllable *c0 = mprs[0]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	myQualityControllable *c1 = mprs[1]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	myQualityControllable *c2 = mprs[2]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb é do do tipo myQualityControllable
	mprs[0]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c1, c2);
	mprs[1]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c2);
	mprs[2]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c1);

	for (auto m : mprs)
		m->AddAfterResliceListener(this);
	for (auto m : mprs)
		m->GetmyResliceImageViewer()->Render();

}

void Sistema::MouseMove(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->MouseMoveEvent();
}

void Sistema::LMouseDown(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->LeftButtonPressEvent();
}

void Sistema::LMouseUp(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->LeftButtonReleaseEvent();
}
void Sistema::MMouseDown(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->MiddleButtonPressEvent();
}
void Sistema::MMouseUp(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->MiddleButtonReleaseEvent();

}
void Sistema::RMouseDown(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->RightButtonPressEvent();
}
void Sistema::RMouseUp(UINT nFlags, int x, int y, int qual){
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->SetEventInformation(x, y, 0, 0, 0, 0, 0);
	mprs[qual]->GetmyResliceImageViewer()->GetInteractor()->RightButtonReleaseEvent();
}
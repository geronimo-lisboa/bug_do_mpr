#include "Sistema.h"
#include "myQualityControllable.h"
#include "myResliceImageViewer.h"
#include "myResliceCursorWidget.h"
#include "MPRView.h"
#include "myResliceImageViewer.h"
#include "myResliceCursor.h"

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
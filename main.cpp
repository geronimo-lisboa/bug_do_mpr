#include <iostream>
#include <map>
#include <string>
#include <itkImage.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkImageImport.h>
#include "loadVolume.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <memory>
#include <vtkResliceImageViewer.h>
#include <array>
#include <vtkResliceCursor.h>
#include <vtkWidgetRepresentation.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkImageReslice.h>
#include <boost/lexical_cast.hpp>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>

using namespace std;
using boost::lexical_cast;
using boost::bad_lexical_cast;

class IMPRView{
protected:
	int id;
public:
	int GetId(){ return id; }
	virtual vtkSmartPointer<vtkResliceImageViewer> GetVtkResliceImageViewer() = 0;
	virtual void Atualizar() = 0;
	virtual ~IMPRView(){};
};

class MPRView : public IMPRView{
private:
	vtkSmartPointer<vtkResliceImageViewer> resliceViewer;
	vtkSmartPointer<vtkImageData> imagem;
	vtkSmartPointer<vtkResliceCursor> sharedCursor;
public:
	MPRView(vtkSmartPointer<vtkImageData> img, int _id);
	vtkSmartPointer<vtkResliceImageViewer> GetVtkResliceImageViewer();
	void Atualizar();
	~MPRView(){};
};

class Sistema{
private:
	array<unique_ptr<MPRView>, 3> mprs;
	vtkSmartPointer<vtkImageData> imagem;
public:
	Sistema(vtkSmartPointer<vtkImageData> img);
};

int main(int argc, char** argv){
	//Carrega a imagem.
	const std::vector<std::string> lst = GetList("C:\\meus dicoms\\mm.txt");//GetList("C:\\meus dicoms\\Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt"); //("C:\\meus dicoms\\mm.txt");//Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt");
	std::map<std::string, std::string> metadataDaImagem;
	itk::Image<short, 3>::Pointer imagemItk = LoadVolume(metadataDaImagem, lst);
	vtkSmartPointer<vtkImageImport> imagemVtk = CreateVTKImage(imagemItk);
	//Cria o sistema
	unique_ptr<Sistema> sistema = make_unique<Sistema>(imagemVtk->GetOutput());
	//A tela dummy
	vtkSmartPointer<vtkRenderer> rendererDummy = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindowDummy = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindowDummy->AddRenderer(rendererDummy);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractorDummy = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowDummy->SetInteractor(renderWindowInteractorDummy);
	renderWindowInteractorDummy->Initialize();
	renderWindowInteractorDummy->Start();
	//Fim do sistema
	return EXIT_SUCCESS;
}

MPRView::MPRView(vtkSmartPointer<vtkImageData> img, int _id){
	id = _id;
	imagem = img;
	resliceViewer = vtkSmartPointer<vtkResliceImageViewer>::New();
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);
	resliceViewer->SetRenderWindow(renderWindow);
	resliceViewer->SetupInteractor(interactor);
	resliceViewer->SetResliceModeToOblique();
	resliceViewer->SetInputData(imagem);
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	vtkResliceCursorLineRepresentation *tl = vtkResliceCursorLineRepresentation::SafeDownCast(r);
	vtkImageReslice *thickSlabReslice = vtkImageReslice::SafeDownCast(tl->GetReslice());
	thickSlabReslice->SetSlabModeToMax();
	thickSlabReslice->SetSlabNumberOfSlices(5);
	vtkResliceCursorLineRepresentation *cursorRepresentation = vtkResliceCursorLineRepresentation::SafeDownCast(
		resliceViewer->GetResliceCursorWidget()->GetRepresentation());
	resliceViewer->GetResliceCursorWidget()->ManageWindowLevelOff();
	cursorRepresentation->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(id);

	string nomeDaTela = "mpr " + lexical_cast<string>(id);
	renderWindow->SetWindowName(nomeDaTela.c_str());
	renderWindow->Render();
}

vtkSmartPointer<vtkResliceImageViewer> MPRView::GetVtkResliceImageViewer(){
	cout << __FUNCTION__ << endl;
	return nullptr;
}

void MPRView::Atualizar(){
	cout << __FUNCTION__ << endl;
}


Sistema::Sistema(vtkSmartPointer<vtkImageData> img){
	imagem = img;
	mprs[0] = make_unique<MPRView>(imagem, 0);
	mprs[1] = make_unique<MPRView>(imagem, 1);
	mprs[2] = make_unique<MPRView>(imagem, 2);
	mprs[0]->Atualizar();
	void* v = mprs[0]->GetVtkResliceImageViewer();
}


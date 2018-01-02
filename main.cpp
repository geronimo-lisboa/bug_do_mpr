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

using namespace std;

vtkSmartPointer<vtkRenderWindow> CreateDummyScreen();

class IMPRView{
public:
	virtual vtkSmartPointer<vtkResliceImageViewer> GetVtkResliceImageViewer() = 0;
	virtual void Atualizar() = 0;
	virtual ~IMPRView(){};
};

class MPRView : public IMPRView{
public:
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
	vtkSmartPointer<vtkRenderWindow> dummyWindow = CreateDummyScreen();
	dummyWindow->GetInteractor()->Start();
	return EXIT_SUCCESS;
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
	mprs[0] = make_unique<MPRView>();
	mprs[1] = make_unique<MPRView>();
	mprs[2] = make_unique<MPRView>();
	mprs[0]->Atualizar();
	void* v = mprs[0]->GetVtkResliceImageViewer();
}

vtkSmartPointer<vtkRenderWindow> CreateDummyScreen(){
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindow->SetInteractor(renderWindowInteractor);
	return renderWindow;
}
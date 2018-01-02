#include <iostream>
#include <map>
#include <string>
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageImport.h>
#include "loadVolume.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

using namespace std;

vtkSmartPointer<vtkRenderWindow> CreateDummyScreen();

int main(int argc, char** argv){
	//Carrega a imagem.
	const std::vector<std::string> lst = GetList("C:\\meus dicoms\\mm.txt");//GetList("C:\\meus dicoms\\Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt"); //("C:\\meus dicoms\\mm.txt");//Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt");
	std::map<std::string, std::string> metadataDaImagem;
	itk::Image<short, 3>::Pointer imagemItk = LoadVolume(metadataDaImagem, lst);
	vtkSmartPointer<vtkImageImport> imagemVtk = CreateVTKImage(imagemItk);

	vtkSmartPointer<vtkRenderWindow> dummyWindow = CreateDummyScreen();
	dummyWindow->GetInteractor()->Start();
	return EXIT_SUCCESS;
}

vtkSmartPointer<vtkRenderWindow> CreateDummyScreen(){
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindow->SetInteractor(renderWindowInteractor);
	return renderWindow;
}
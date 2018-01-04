#include "IMPRView.h"
#include "myResliceCursorCallback.h"
#include "MPRView.h"
#include "Sistema.h"

#include "iostream"
#include "map"
#include "string"
#include "itkImage.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImageImport.h"
#include "loadVolume.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "memory"
#include "myResliceImageViewer.h"
#include "array"
#include "myResliceCursor.h"
#include "vtkWidgetRepresentation.h"
#include "myResliceCursorWidget.h"
#include "myResliceCursorLineRepresentation.h"
#include "vtkImageReslice.h"
#include "boost/lexical_cast.hpp"
#include "myResliceCursorActor.h"
#include "myResliceCursorPolyDataAlgorithm.h"
#include "vector"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageSlabReslice.h"
#include "boost/assert.hpp"
#include "itkResampleImageFilter.h"
#include "itkOrientImageFilter.h"
#include "myAfterReslicedImageGeneratedCallback.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkSmartPointer.h"
#include <boost\lexical_cast.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"


using namespace std;

using boost::lexical_cast;
using boost::bad_lexical_cast;

itk::Image<short, 3>::Pointer CreateLowRes(itk::Image<short, 3>::Pointer imagem, float fator = 3.0f);


int main(int argc, char** argv){
	//Carrega a imagem.
	if (argc == 1)
	{
		cout << "informe o path pro txt com a lista de fatias.";
		return EXIT_FAILURE;
	}
	const string txtFile = argv[1];
	const std::vector<std::string> lst = GetList(txtFile); //GetList("C:\\meus dicoms\\mm.txt");//GetList("C:\\meus dicoms\\Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt"); //("C:\\meus dicoms\\mm.txt");//Distorcao.txt1.2.840.113704.1.111.788.1492526943.13.41.00512512.txt");
	std::map<std::string, std::string> metadataDaImagem;
	itk::Image<short, 3>::Pointer versaoHiRes = LoadVolume(metadataDaImagem, lst);
	//Reorientador - garante que tudo esteja na orientação RIP
	itk::OrientImageFilter<itk::Image<short, 3>, itk::Image<short, 3>>::Pointer orienter = itk::OrientImageFilter<itk::Image<short, 3>, itk::Image<short, 3>>::New();
	orienter->UseImageDirectionOn();
	////Consultar https://itk.org/Doxygen/html/namespaceitk_1_1SpatialOrientation.html#a8240a59ae2e7cae9e3bad5a52ea3496eaa5d3197482c4335a63a1ad99d6a9edee
	////para a lista de orientações
	orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
	orienter->SetInput(versaoHiRes);
	orienter->Update();
	versaoHiRes = orienter->GetOutput();
	//diminuir o tamanho da imagem aqui...
	itk::Image<short, 3>::Pointer versaoLowRes = CreateLowRes(versaoHiRes);

	vtkSmartPointer<vtkImageImport> imagemVtkHiRes = CreateVTKImage(versaoHiRes);
	vtkSmartPointer<vtkImageImport> imagemVtkLowRes = CreateVTKImage(versaoLowRes);
	//Cria o sistema
	unique_ptr<Sistema> sistema = make_unique<Sistema>(imagemVtkHiRes->GetOutput(), imagemVtkLowRes->GetOutput());
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

itk::Image<short, 3>::Pointer CreateLowRes(itk::Image<short, 3>::Pointer imagem, float fator )
{
	itk::Image<short, 3>::SizeType inputSize = imagem->GetLargestPossibleRegion().GetSize();
	itk::Image<short, 3>::SizeType outputSize;
	outputSize[0] = inputSize[0] / fator;
	outputSize[1] = inputSize[1] / fator;
	outputSize[2] = inputSize[2] / fator;
	itk::Image<short, 3>::SpacingType outputSpacing;
	outputSpacing[0] = imagem->GetSpacing()[0] * fator;
	outputSpacing[1] = imagem->GetSpacing()[1] * fator;
	outputSpacing[2] = imagem->GetSpacing()[2] * fator;
	typedef itk::IdentityTransform<double, 3> TransformType;
	typedef itk::ResampleImageFilter<itk::Image<short, 3>, itk::Image<short, 3>> ResampleImageFilterType;
	ResampleImageFilterType::Pointer resample = ResampleImageFilterType::New();
	resample->SetOutputParametersFromImage(imagem);
	resample->SetInput(imagem);
	resample->SetSize(outputSize);
	resample->SetOutputSpacing(outputSpacing);
	resample->SetTransform(TransformType::New());
	resample->UpdateLargestPossibleRegion();
	itk::Image<short, 3>::Pointer output = resample->GetOutput();
	return output;
}



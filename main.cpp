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

using namespace std;

using boost::lexical_cast;
using boost::bad_lexical_cast;

itk::Image<short, 3>::Pointer CreateLowRes(itk::Image<short, 3>::Pointer imagem, float fator = 3.0f);

class IMPRView{
protected:
	int id;
public:
	int GetId(){ return id; }
	virtual vtkSmartPointer<myResliceImageViewer> GetmyResliceImageViewer() = 0;
	virtual void Atualizar() = 0;
	virtual ~IMPRView(){};
};

class myResliceCursorCallback : public vtkCommand{
private:
	bool isUsingLowRes;
	vector<shared_ptr<IMPRView>> MPRs;
	double ww, wl;
	myResliceCursorCallback();
public:
	static myResliceCursorCallback *New(){
		return new myResliceCursorCallback();
	}
	void AddMPR(shared_ptr<IMPRView> m){
		MPRs.push_back(m);
	}
	void Execute(vtkObject * caller, unsigned long event, void* calldata);
};

class MPRView : public IMPRView{
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
	void SetCallbacks(vtkSmartPointer<myResliceCursor> _sharedCursor, vtkSmartPointer<myResliceCursorCallback> rcbk);
};

class Sistema{
private:
	vtkSmartPointer<myResliceCursorCallback> resliceCursorCallback;
	array<shared_ptr<MPRView>, 3> mprs;
	vtkSmartPointer<vtkImageData> imagemHiRes, imagemLowRes;
public:
	Sistema(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes);
};

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

MPRView::MPRView(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes, int _id){
	id = _id;
	imagemHiRes = imgHiRes;
	imagemLowRes = imgLowRes;
	resliceViewer = vtkSmartPointer<myResliceImageViewer>::New();
	resliceViewer->SetThickMode(1);
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);
	resliceViewer->SetRenderWindow(renderWindow);
	resliceViewer->SetupInteractor(interactor);
	resliceViewer->SetResliceModeToOblique();
	resliceViewer->SetInputData(imagemHiRes, imagemLowRes);
	vtkWidgetRepresentation *r = resliceViewer->GetResliceCursorWidget()->GetRepresentation();
	myResliceCursorLineRepresentation *tl = myResliceCursorLineRepresentation::SafeDownCast(r);
	vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(tl->GetResliceHiRes());
	BOOST_ASSERT((thickSlabReslice != nullptr));//sanity check do cast

	thickSlabReslice->SetSlabModeToMax();//Seta pra mip

	thickSlabReslice->SetSlabNumberOfSlices(10);
	myResliceCursorLineRepresentation *cursorRepresentation = myResliceCursorLineRepresentation::SafeDownCast(
		resliceViewer->GetResliceCursorWidget()->GetRepresentation());
	resliceViewer->GetResliceCursorWidget()->ManageWindowLevelOff();
	cursorRepresentation->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(id);

	string nomeDaTela = "mpr " + lexical_cast<string>(id);
	renderWindow->SetWindowName(nomeDaTela.c_str());
	renderWindow->Render();

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

Sistema::Sistema(vtkSmartPointer<vtkImageData> imgHiRes, vtkSmartPointer<vtkImageData> imgLowRes){
	imagemHiRes = imgHiRes;
	imagemLowRes = imgLowRes;
	mprs[0] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 0);
	mprs[1] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 1);
	mprs[2] = make_shared<MPRView>(imagemHiRes, imagemLowRes, 2);
	resliceCursorCallback = vtkSmartPointer<myResliceCursorCallback>::New();
	//Liga os callbacks
	for (std::shared_ptr<MPRView> m : mprs){
		m->SetCallbacks(mprs[0]->GetmyResliceImageViewer()->GetResliceCursor(),resliceCursorCallback);
		resliceCursorCallback->AddMPR(m);
	}
	//Liga os controles de qualidade
	myQualityControllable *c0 = mprs[0]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb � do do tipo myQualityControllable
	myQualityControllable *c1 = mprs[1]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb � do do tipo myQualityControllable
	myQualityControllable *c2 = mprs[2]->GetmyResliceImageViewer()->GetResliceCursorWidget(); //ele tb � do do tipo myQualityControllable
	mprs[0]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c1, c2);
	mprs[1]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c2);
	mprs[2]->GetmyResliceImageViewer()->LinkWithOtherWidgets(c0, c1);
}

void myResliceCursorCallback::Execute(vtkObject * caller, unsigned long ev, void* calldata){
	AbortFlagOn();


	vtkInteractorStyleImage *i = vtkInteractorStyleImage::SafeDownCast(caller);
	if (i){//Se isso � verdadeiro � pq � opera��o de Window/Level
		int *p1 = i->GetInteractor()->GetEventPosition();
		int *p2 = i->GetInteractor()->GetLastEventPosition();
		int dp[2];
		dp[0] = p1[0] - p2[0];
		dp[1] = p1[1] - p2[1];
		ww = ww + dp[0];
		wl = wl + dp[1];
	}
	for (shared_ptr<IMPRView> m : MPRs){
		m->GetmyResliceImageViewer()->SetColorWindow(ww);
		m->GetmyResliceImageViewer()->SetColorLevel(wl);
		m->Atualizar();
	}
}

myResliceCursorCallback::myResliceCursorCallback()
	:isUsingLowRes(false){
	ww = 350;
	wl = 150;
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

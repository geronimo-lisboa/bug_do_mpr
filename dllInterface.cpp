#include "dllInterface.h"
//Windows
#include <Windows.h>
//Standard Template Library
#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <io.h>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <regex>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <assert.h>
#include "loadVolume.h"
#include <itkImage.h>
#include <itkIdentityTransform.h>
#include <itkResampleImageFilter.h>
#include <itkOrientImageFilter.h>
#include "itkCommand.h"
#include "Sistema.h"
using namespace std;

FNCallbackDeCarga funcaoDeExibicaoDoProgressoDaCarga = nullptr;
itk::Image<short, 3>::Pointer versaoHiRes = nullptr;
itk::Image<short, 3>::Pointer versaoLowRes = nullptr;
unique_ptr<Sistema> sistema = nullptr;
class ObserveLoadProgressCommand : public itk::Command
{
public:
	itkNewMacro(ObserveLoadProgressCommand);
	void Execute(itk::Object * caller, const itk::EventObject & event)
	{
		Execute((const itk::Object *)caller, event);
	}

	void Execute(const itk::Object * caller, const itk::EventObject & event)
	{
		if (!itk::ProgressEvent().CheckEvent(&event))
		{
			return;
		}
		const itk::ProcessObject * processObject =
			dynamic_cast< const itk::ProcessObject * >(caller);
		if (!processObject)
		{
			return;
		}
		funcaoDeExibicaoDoProgressoDaCarga(processObject->GetProgress());
	}
};
itk::Image<short, 3>::Pointer CreateVersaoLowRes(itk::Image<short, 3>::Pointer imagem, float fator);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
//	cout << __FUNCTION__<<" - " <<fdwReason << endl;
	return TRUE;
}

//1a função
void _stdcall DLL_SetCallbackDeCarga(FNCallbackDeCarga cbk){
	funcaoDeExibicaoDoProgressoDaCarga = cbk;
}
//2a função
void _stdcall DLL_LoadVolume(const char* pathToFileList) {
	ObserveLoadProgressCommand::Pointer prog = ObserveLoadProgressCommand::New();
	const string txtFile = pathToFileList;
	const std::vector<std::string> lst = GetList(txtFile);
	std::map<std::string, std::string> metadataDaImagem;
	versaoHiRes = LoadVolume(metadataDaImagem, lst,   prog);
	itk::OrientImageFilter<itk::Image<short, 3>, itk::Image<short, 3>>::Pointer orienter = itk::OrientImageFilter<itk::Image<short, 3>, itk::Image<short, 3>>::New();
	orienter->AddObserver(itk::ProgressEvent(), prog);
	orienter->UseImageDirectionOn();
	orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
	orienter->SetInput(versaoHiRes);
	orienter->Update();
	versaoHiRes = orienter->GetOutput();
	//diminuir o tamanho da imagem aqui...
	versaoLowRes = CreateVersaoLowRes(versaoHiRes, 3);
}

void _stdcall DLL_CreateRenderer(HWND handle, int qual){
	assert(versaoHiRes && versaoLowRes);
	if (!sistema){
		sistema = make_unique<Sistema>();
		vtkSmartPointer<vtkImageImport> imagemVtkHiRes = CreateVTKImage(versaoHiRes);
		vtkSmartPointer<vtkImageImport> imagemVtkLowRes = CreateVTKImage(versaoLowRes);
		sistema->SetImages(imagemVtkHiRes->GetOutput(), imagemVtkLowRes->GetOutput());
	}
	sistema->CreateView(handle, qual);
	if (qual == 2){
		sistema->LinkEverything();
	}
}

void _stdcall DLL_ResizeRenderer(int qual, int w, int h){
	assert(sistema);
	sistema->ResizeWindow(qual, w, h);
}

void _stdcall DLL_SetThickness(int t){
	assert(false && "não implementado");
}

int _stdcall DLL_MouseMove(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->MouseMove(nFlags, X, Y, qualPanel);
	}
	return 1;
}
int _stdcall DLL_LMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->LMouseDown(nFlags, X, Y, qualPanel);
	}
	return 1;
}
int _stdcall DLL_LMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->LMouseUp(nFlags, X, Y, qualPanel);
	}
	return 1;
}

int _stdcall DLL_MMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->MMouseDown(nFlags, X, Y, qualPanel);
	}
	return 1;
}

int _stdcall DLL_MMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->MMouseUp(nFlags, X, Y, qualPanel);
	}
	return 1;
}

int _stdcall DLL_RMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->RMouseDown(nFlags, X, Y, qualPanel);
	}
	return 1;
}

int _stdcall DLL_RMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema){
	if (sistema){
		sistema->RMouseUp(nFlags, X, Y, qualPanel);
	}
	return 1;
}



itk::Image<short, 3>::Pointer CreateVersaoLowRes(itk::Image<short, 3>::Pointer imagem, float fator)
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
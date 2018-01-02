#ifndef  __load_volume_h
#define __load_volume_h
#include <vector>
#include <string>
#include <itkImage.h>
#include <vtkImageImport.h>
//Pega a lista de nomes de um arquivinho preparado pra isso
const std::vector<std::string> GetList(std::string path);
//Carrega o volume baseado na lista de nomes
itk::Image<short, 3>::Pointer LoadVolume(std::map<std::string, std::string> &outputMetadata,
	const std::vector<std::string> filepaths);

vtkImageImport* CreateVTKImage(itk::Image<short, 3>::Pointer img);
#endif
#pragma once
#include <vtkSmartPointer.h>
#include "myResliceImageViewer.h"
////Interface compartilhada por todos os visualizadores de MPR
class IMPRView{
protected:
	int id;
public:
	int GetId(){ return id; }
	virtual vtkSmartPointer<myResliceImageViewer> GetmyResliceImageViewer() = 0;
	virtual void Atualizar() = 0;
	virtual ~IMPRView(){};
};
#pragma once
#include <vector>
#include <memory>
#include <vtkCommand.h>
#include "IMPRView.h"
using namespace std;
///Esse callback é compartilhado entre todos os MPRView e serve pra ligar as rotações. Quando uma rotação ocorrer
///esse callback é invocado e as telas são renderizadas, fazendo com que seja vista a alteração dos angulos;
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
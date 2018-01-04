#pragma once
#include <vector>
#include <memory>
#include <vtkCommand.h>
#include "IMPRView.h"
using namespace std;
///Esse callback � compartilhado entre todos os MPRView e serve pra ligar as rota��es. Quando uma rota��o ocorrer
///esse callback � invocado e as telas s�o renderizadas, fazendo com que seja vista a altera��o dos angulos;
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
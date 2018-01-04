#include "myResliceCursorCallback.h"
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>
void myResliceCursorCallback::Execute(vtkObject * caller, unsigned long ev, void* calldata){
	AbortFlagOn();	//Se isso não estiver aqui o window/level fica bugadão
	vtkInteractorStyleImage *i = vtkInteractorStyleImage::SafeDownCast(caller);//Se o caller é desse tipo foi um evento de window/level
	if (i){//Se isso é verdadeiro é pq é operação de Window/Level
		int *p1 = i->GetInteractor()->GetEventPosition();
		int *p2 = i->GetInteractor()->GetLastEventPosition();
		int dp[2];
		dp[0] = p1[0] - p2[0];
		dp[1] = p1[1] - p2[1];
		ww = ww + dp[0];
		wl = wl + dp[1];
	}
	//Aplica o janelamento e manda atualizar.
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
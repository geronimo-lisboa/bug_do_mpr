/*=========================================================================

Program:   Visualization Toolkit
Module:    myResliceCursorWidget.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "myResliceCursorWidget.h"
#include <iostream>
#include "myResliceCursorLineRepresentation.h"
#include "vtkCommand.h"
#include "myResliceCursor.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "myResliceCursorThickLineRepresentation.h"

vtkStandardNewMacro(myResliceCursorWidget);

//----------------------------------------------------------------------------
myResliceCursorWidget::myResliceCursorWidget()
{
	// Set the initial state
	this->WidgetState = myResliceCursorWidget::Start;

	this->ModifierActive = 0;

	// Okay, define the events for this widget
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkEvent::NoModifier, 0, 0, NULL,
		vtkWidgetEvent::Select,
		this, myResliceCursorWidget::SelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkEvent::ControlModifier, 0, 0, NULL,
		vtkWidgetEvent::Rotate,
		this, myResliceCursorWidget::RotateAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, myResliceCursorWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
		vtkWidgetEvent::Resize,
		this, myResliceCursorWidget::ResizeThicknessAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
		vtkWidgetEvent::EndResize,
		this, myResliceCursorWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, myResliceCursorWidget::MoveAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
		vtkEvent::NoModifier, 111, 1, "o",
		vtkWidgetEvent::Reset,
		this, myResliceCursorWidget::ResetResliceCursorAction);

	this->ManageWindowLevel = 1;
}

//----------------------------------------------------------------------------
myResliceCursorWidget::~myResliceCursorWidget()
{
}

//----------------------------------------------------------------------
void myResliceCursorWidget::SetEnabled(int enabling)
{
	this->Superclass::SetEnabled(enabling);
}

//----------------------------------------------------------------------
void myResliceCursorWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		this->WidgetRep = myResliceCursorLineRepresentation::New();
	}
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::SetCursor(int cState)
{
	switch (cState)
	{
	case myResliceCursorRepresentation::OnAxis1:
	case myResliceCursorRepresentation::OnAxis2:
		this->RequestCursorShape(VTK_CURSOR_HAND);
		break;
	case myResliceCursorRepresentation::OnCenter:
		if (vtkEvent::GetModifier(this->Interactor) != vtkEvent::ControlModifier)
		{
			this->RequestCursorShape(VTK_CURSOR_SIZEALL);
		}
		break;
	case myResliceCursorRepresentation::Outside:
	default:
		this->RequestCursorShape(VTK_CURSOR_DEFAULT);
	}
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::ResizeThicknessAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = reinterpret_cast<myResliceCursorWidget*>(w);
	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	rep->ComputeInteractionState(X, Y, self->ModifierActive);

	if (self->WidgetRep->GetInteractionState() == myResliceCursorRepresentation::Outside || rep->GetResliceCursor()->GetThickMode() == 0)
	{
		return;
	}
	rep->SetUseLowRes(true);
	for (myQualityControllable *q : self->qualityControllables){//reduz as dos outros
		q->UseLowQuality();
	}
	rep->SetManipulationMode(myResliceCursorRepresentation::ResizeThickness);

	self->GrabFocus(self->EventCallbackCommand);
	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);
	self->WidgetRep->StartWidgetInteraction(eventPos);

	// We are definitely selected
	self->WidgetState = myResliceCursorWidget::Active;
	self->SetCursor(self->WidgetRep->GetInteractionState());

	// Highlight as necessary
	self->WidgetRep->Highlight(1);

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	self->Render();

	self->InvokeAnEvent();

	// Show the thickness in "mm"
	rep->ActivateText(1);
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::EndResizeThicknessAction(vtkAbstractWidget *)
{
}

void myResliceCursorWidget::UseLowQuality(){
	myResliceCursorRepresentation *rep = reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);
	rep->SetUseLowRes(true);
}

void myResliceCursorWidget::UseHiQuality(){
	myResliceCursorRepresentation *rep = reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);
	rep->SetUseLowRes(false);
	this->Render();
}


void myResliceCursorWidget::AddQualityControlable(myQualityControllable* q){
	qualityControllables.push_back(q);
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::SelectAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = reinterpret_cast<myResliceCursorWidget*>(w);
	myResliceCursorRepresentation *rep = reinterpret_cast<myResliceCursorRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	self->ModifierActive = vtkEvent::GetModifier(self->Interactor);
	rep->ComputeInteractionState(X, Y, self->ModifierActive);
	int interactionState = rep->GetInteractionState();
	if (self->WidgetRep->GetInteractionState() == myResliceCursorRepresentation::Outside)
	{
		if (self->GetManageWindowLevel() && rep->GetShowReslicedImage())
		{
			self->StartWindowLevel();
		}
		else
		{
			rep->SetManipulationMode(myResliceCursorRepresentation::None);
			return;
		}
	}
	if (interactionState == 4)//CENTER
	{//Aqui é o pan
		rep->SetUseLowRes(true);
		for (myQualityControllable *q : self->qualityControllables){//reduz as dos outros
			q->UseLowQuality();
		}
		rep->SetManipulationMode(myResliceCursorRepresentation::PanAndRotate);
	}
	if (interactionState == 5 || interactionState == 6)//OS DOIS EIXOS
	{//Aqui é a rotação dos eixos
		rep->SetUseLowRes(true);//Reduz a minha qualidade
		for (myQualityControllable *q : self->qualityControllables){//reduz as dos outros
			q->UseLowQuality();
		}
		rep->ComputeInteractionState(X, Y, 2);
		rep->SetManipulationMode(myResliceCursorLineRepresentation::RotateBothAxes);
	}

	if (rep->GetManipulationMode() == myResliceCursorRepresentation::None)
	{
		return;
	}

	self->GrabFocus(self->EventCallbackCommand);
	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);
	self->WidgetRep->StartWidgetInteraction(eventPos);

	// We are definitely selected
	self->WidgetState = myResliceCursorWidget::Active;
	self->SetCursor(self->WidgetRep->GetInteractionState());

	// Highlight as necessary
	self->WidgetRep->Highlight(1);

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	self->Render();

	self->InvokeAnEvent();
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::RotateAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = reinterpret_cast<myResliceCursorWidget*>(w);//ROTATE ACTION INVOCADA PELO STACK DE EVENTOS
	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	self->ModifierActive = vtkEvent::GetModifier(self->Interactor);//AQUI É 2 C/ O Ctrl pressionado. E sem?
	rep->ComputeInteractionState(X, Y, self->ModifierActive);

	if (self->WidgetRep->GetInteractionState()
		== myResliceCursorRepresentation::Outside)
	{
		return;
	}

	rep->SetManipulationMode(myResliceCursorRepresentation::RotateBothAxes);

	self->GrabFocus(self->EventCallbackCommand);
	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);
	self->WidgetRep->StartWidgetInteraction(eventPos);

	// We are definitely selected
	self->WidgetState = myResliceCursorWidget::Active;
	self->SetCursor(self->WidgetRep->GetInteractionState());

	// Highlight as necessary
	self->WidgetRep->Highlight(1);

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	self->Render();

	self->InvokeAnEvent();
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::MoveAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = reinterpret_cast<myResliceCursorWidget*>(w);
	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(self->WidgetRep);

	// compute some info we need for all cases
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// Set the cursor appropriately
	if (self->WidgetState == myResliceCursorWidget::Start)
	{
		self->ModifierActive = vtkEvent::GetModifier(self->Interactor);
		int state = self->WidgetRep->GetInteractionState();

		rep->ComputeInteractionState(X, Y, self->ModifierActive);

		self->SetCursor(self->WidgetRep->GetInteractionState());

		if (state != self->WidgetRep->GetInteractionState())
		{
			self->Render();
		}
		return;
	}

	// Okay, adjust the representation

	double eventPosition[2];
	eventPosition[0] = static_cast<double>(X);
	eventPosition[1] = static_cast<double>(Y);
	self->WidgetRep->WidgetInteraction(eventPosition);

	// Got this event, we are finished
	self->EventCallbackCommand->SetAbortFlag(1);
	self->InvokeEvent(vtkCommand::InteractionEvent, NULL);
	self->Render();

	self->InvokeAnEvent();
}

//-------------------------------------------------------------------------
//Quando o reslice ou o pan acabam eles passam por aqui. Então é aqui que eu volto pra alta qualidade.
//A mudança de tamanho da fatia tb passa por aqui.
void myResliceCursorWidget::EndSelectAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = static_cast<myResliceCursorWidget*>(w);
	myResliceCursorRepresentation *rep = reinterpret_cast<myResliceCursorRepresentation*>(self->WidgetRep);
	rep->SetUseLowRes(false);//Aumenta minha qualidade
	for (myQualityControllable *q : self->qualityControllables){//Aumenta as dos outros
		q->UseHiQuality();
	}


	if (self->WidgetState != myResliceCursorWidget::Active)
	{
		return;
	}

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);

	self->WidgetRep->EndWidgetInteraction(eventPos);

	// return to initial state
	self->WidgetState = myResliceCursorWidget::Start;
	self->ModifierActive = 0;

	// Highlight as necessary
	self->WidgetRep->Highlight(0);

	// Remove any text displays. We are no longer active.
	rep->ActivateText(0);

	// stop adjusting
	self->ReleaseFocus();
	self->EventCallbackCommand->SetAbortFlag(1);
	self->EndInteraction();
	self->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
	self->WidgetState = myResliceCursorWidget::Start;
	rep->SetManipulationMode(myResliceCursorRepresentation::None);

	self->Render();

	self->InvokeAnEvent();
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::ResetResliceCursorAction(vtkAbstractWidget *w)
{
	myResliceCursorWidget *self = reinterpret_cast<myResliceCursorWidget*>(w);
	self->ResetResliceCursor();

	// Render in response to changes
	self->Render();

	// Invoke a reslice cursor event
	self->InvokeEvent(myResliceCursorWidget::ResetCursorEvent, NULL);
}

//-------------------------------------------------------------------------
void myResliceCursorWidget::ResetResliceCursor()
{
	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);

	if (!rep->GetResliceCursor())
	{
		return; // nothing to reset
	}

	// Reset the reslice cursor
	rep->GetResliceCursor()->Reset();
	rep->InitializeReslicePlane();
}

//----------------------------------------------------------------------------
void myResliceCursorWidget::StartWindowLevel()
{
	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, make sure that the pick is in the current renderer
	if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
	{
		rep->SetManipulationMode(myResliceCursorRepresentation::None);
		return;
	}

	rep->SetManipulationMode(myResliceCursorRepresentation::WindowLevelling);

	rep->ActivateText(1);
	rep->ManageTextDisplay();
}

//----------------------------------------------------------------------------
void myResliceCursorWidget::InvokeAnEvent()
{
	// We invoke the appropriate event. In cases where the cursor is moved
	// around, or rotated, also have the reslice cursor invoke an event.

	myResliceCursorRepresentation *rep =
		reinterpret_cast<myResliceCursorRepresentation*>(this->WidgetRep);
	if (rep)
	{
		int mode = rep->GetManipulationMode();
		if (mode == myResliceCursorRepresentation::WindowLevelling)
		{
			this->InvokeEvent(WindowLevelEvent, NULL);
		}
		else if (mode == myResliceCursorRepresentation::PanAndRotate)
		{
			this->InvokeEvent(ResliceAxesChangedEvent, NULL);
			rep->GetResliceCursor()->InvokeEvent(ResliceAxesChangedEvent, NULL);
		}
		else if (mode == myResliceCursorRepresentation::RotateBothAxes)
		{
			this->InvokeEvent(ResliceAxesChangedEvent, NULL);
			rep->GetResliceCursor()->InvokeEvent(ResliceAxesChangedEvent, NULL);
		}
		else if (mode == myResliceCursorRepresentation::ResizeThickness)
		{
			this->InvokeEvent(ResliceThicknessChangedEvent, NULL);
			rep->GetResliceCursor()->InvokeEvent(ResliceAxesChangedEvent, NULL);
		}
	}
}

//----------------------------------------------------------------------------
void myResliceCursorWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	os << indent << "ManageWindowLevel: " << this->ManageWindowLevel << endl;
	// this->ModifierActive;
	// this->WidgetState;

}

void myResliceCursorWidget::AddAfterResliceListener(myAfterReslicedImageGeneratedCallback* l){
	myResliceCursorThickLineRepresentation * rep = myResliceCursorThickLineRepresentation::SafeDownCast(this->GetResliceCursorRepresentation());    //GetResliceCursor()->  GetResliceCursor()->AddResliceResultListener(l);
	rep->AddResliceResultListener(l);
}

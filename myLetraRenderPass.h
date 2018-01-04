#pragma once
#include <vtkRenderPass.h>
#include <vtkSmartPointer.h>
#include <vtkLightsPass.h>
#include <vtkDefaultPass.h>
#include <vtkRenderPassCollection.h>
#include <vtkSequencePass.h>
#include <vtkCameraPass.h>
#include <vtkRenderState.h>
#include <SDL2\SDL_ttf.h>
#undef main

#include <SDL2/SDL.h>
#undef main

class myLetraRenderPass : public vtkRenderPass{
private:
	TTF_Font* font;
	SDL_Surface* canvasSdlSurface;
	SDL_Renderer* canvasSdlRenderer;
	vtkSmartPointer<vtkCameraPass> cameraPass;
	vtkSmartPointer<vtkLightsPass> lightsPass;
	vtkSmartPointer<vtkDefaultPass> defaultPass;
	vtkSmartPointer<vtkRenderPassCollection> passes;
	vtkSmartPointer<vtkSequencePass> sequencePass;
	myLetraRenderPass();
	~myLetraRenderPass();
public:
	static myLetraRenderPass *New(){
		return new myLetraRenderPass();
	}
	void Render(const vtkRenderState* s);
};
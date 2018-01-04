#include "myLetraRenderPass.h"
#include <vtkOpenGLRenderer.h>
#include <vtkRenderWindow.h>
#include <SDL2\SDL_ttf.h>
#include <boost\exception\all.hpp>

myLetraRenderPass::myLetraRenderPass(){
	//Inicialização das coisas do SDL
	if (SDL_Init(SDL_INIT_VIDEO) == -1){
		BOOST_THROW_EXCEPTION(std::exception(SDL_GetError()));
	}
	if (TTF_Init() == -1){
		BOOST_THROW_EXCEPTION(std::exception(SDL_GetError()));
	}
	font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 25);
	if (!font){
		BOOST_THROW_EXCEPTION(std::exception(TTF_GetError()));
	}

	sequencePass = vtkSmartPointer<vtkSequencePass>::New();
	lightsPass = vtkSmartPointer<vtkLightsPass>::New();
	defaultPass = vtkSmartPointer<vtkDefaultPass>::New();
	passes = vtkSmartPointer<vtkRenderPassCollection>::New();
	cameraPass = vtkSmartPointer<vtkCameraPass>::New();
	passes->AddItem(lightsPass);
	passes->AddItem(defaultPass);
	sequencePass->SetPasses(passes);
	cameraPass->SetDelegatePass(sequencePass);
}

myLetraRenderPass::~myLetraRenderPass(){
	//Limpeza do SDL
	TTF_CloseFont(font);
	SDL_Quit();
	TTF_Quit();
}

void myLetraRenderPass::Render(const vtkRenderState* s){
	cameraPass->Render(s);//Renderização para a tela
	vtkOpenGLRenderer* glRen = vtkOpenGLRenderer::SafeDownCast(s->GetRenderer());
	vtkRenderWindow* wnd = glRen->GetRenderWindow();
	const int screenWidth = wnd->GetSize()[0];
	const int screenHeight = wnd->GetSize()[1];
	typedef unsigned char BYTE;
	BYTE *screenBuffer = wnd->GetRGBACharPixelData(0, 0, screenWidth - 1, screenHeight - 1, 0);
	this->canvasSdlSurface = SDL_CreateRGBSurfaceFrom(screenBuffer, screenWidth, screenHeight, 32, screenWidth * 4, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	this->canvasSdlRenderer = SDL_CreateSoftwareRenderer(canvasSdlSurface);
	//Desenha o retangulo
	SDL_SetRenderDrawColor(canvasSdlRenderer, 0, 0, 255, 0);
	SDL_Rect rect;
	rect.x = 10;
	rect.y = 10;
	rect.w = 20;
	rect.h = 30;
	SDL_RenderDrawRect(canvasSdlRenderer, &rect);
	//Desenha um texto qqer
	SDL_Color color = { 255, 0, 0 };
	SDL_Surface *surface = TTF_RenderText_Solid(font, "HELLO", color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(canvasSdlRenderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { 0, 0, texW, texH };
	SDL_RenderCopy(canvasSdlRenderer, texture, NULL, &dstrect);
	//Cópia do buffer do SDL pra imagem da janela da VTK, é aqui que eu aplico os desenhos feitos
	wnd->SetRGBACharPixelData(0, 0, screenWidth - 1, screenHeight - 1, screenBuffer, 0);
	//Limpa
	delete[] screenBuffer;
	SDL_DestroyRenderer(canvasSdlRenderer);
	SDL_FreeSurface(canvasSdlSurface);
}
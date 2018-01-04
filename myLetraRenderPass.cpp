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
	font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 12);
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
	rect.x = 1;
	rect.y = screenHeight -1;
	rect.w = 20;
	rect.h = 20;
	SDL_RenderDrawRect(canvasSdlRenderer, &rect);
	//Desenho das letras
	SDL_Color color = { 255, 0, 0 };
	int texW = 0;
	int texH = 0;
	SDL_Rect dstrect;
	//Desenha a letra da esquerda
	SDL_Surface *surfaceEsquerda = TTF_RenderText_Solid(font, "L", color);
	SDL_Texture * textureEsquerda = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceEsquerda);
	SDL_QueryTexture(textureEsquerda, NULL, NULL, &texW, &texH);
	dstrect = { 15, screenWidth/2, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureEsquerda, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra da direita
	SDL_Surface *surfaceDireita = TTF_RenderText_Solid(font, "R", color);
	SDL_Texture * textureDireita = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceDireita);
	SDL_QueryTexture(textureDireita, NULL, NULL, &texW, &texH);
	dstrect = { screenWidth-15, screenHeight / 2, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureDireita, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra de cima
	SDL_Surface *surfaceCima = TTF_RenderText_Solid(font, "C", color);
	SDL_Texture * textureCima = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceCima);
	SDL_QueryTexture(textureCima, NULL, NULL, &texW, &texH);
	dstrect = { screenWidth / 2, screenHeight - 15, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureCima, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra de baixo
	SDL_Surface *surfaceBaixo = TTF_RenderText_Solid(font, "B", color);
	SDL_Texture * textureBaixo = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceBaixo);
	SDL_QueryTexture(textureBaixo, NULL, NULL, &texW, &texH);
	dstrect = { screenWidth / 2, 15, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureBaixo, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);

	//Cópia do buffer do SDL pra imagem da janela da VTK, é aqui que eu aplico os desenhos feitos
	wnd->SetRGBACharPixelData(0, 0, screenWidth - 1, screenHeight - 1, screenBuffer, 0);
	//Limpa
	delete[] screenBuffer;
	SDL_DestroyRenderer(canvasSdlRenderer);
	SDL_FreeSurface(canvasSdlSurface);
}
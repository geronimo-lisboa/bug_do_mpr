#include "myLetraRenderPass.h"
#include <vtkOpenGLRenderer.h>
#include <vtkRenderWindow.h>
#include <SDL2\SDL_ttf.h>
#include <boost\exception\all.hpp>
#include <vtkCamera.h>
#include <vtkPlane.h>
#include <vtkGeneralTransform.h>
#include <string>
using namespace std;

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

void myLetraRenderPass::Calculate(vtkRenderer *ren){
	double *_cameraOrientation;
	//pega a orientação da câmera e cria uma transform.
	_cameraOrientation = ren->GetActiveCamera()->GetOrientationWXYZ();
	vtkSmartPointer<vtkGeneralTransform> _rotationTransform = vtkSmartPointer<vtkGeneralTransform>::New();
	_rotationTransform->RotateWXYZ(_cameraOrientation[0], _cameraOrientation[1], _cameraOrientation[2], _cameraOrientation[3]);
	_rotationTransform->Update();
	//define algunas constantes:
	const double origin[] = { 0, 0, 0 }; // A origem do sistema
	const double nLeft[] = { 1, 0, 0 };
	const double nRight[] = { -1, 0, 0 };
	const double nTop[] = { 0, 1, 0 };
	const double nBottom[] = { 0, -1, 0 };
	const double nAnterior[] = { 0, 0, 1 };
	const double nPosterior[] = { 0, 0, -1 };
	//Transforma as normais usando a transformação criada acima
	double _newLeft[3], _newRight[3], _newTop[3], _newBottom[3], _newAnterior[3], _newPosterior[3];
	_rotationTransform->TransformVectorAtPoint(origin, nLeft, _newLeft);
	_rotationTransform->TransformVectorAtPoint(origin, nRight, _newRight);
	_rotationTransform->TransformVectorAtPoint(origin, nTop, _newTop);
	_rotationTransform->TransformVectorAtPoint(origin, nBottom, _newBottom);
	_rotationTransform->TransformVectorAtPoint(origin, nAnterior, _newAnterior);
	_rotationTransform->TransformVectorAtPoint(origin, nPosterior, _newPosterior);
	const float tam = 0.5;
	{
		vtkSmartPointer<vtkPlane> planoEsquerdo = vtkSmartPointer<vtkPlane>::New();
		planoEsquerdo->SetOrigin(-tam, 0, 0);
		planoEsquerdo->SetNormal(1, 0, 0);
		double p;
		double x[3];
		int iL = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newLeft, p, x);
		int iR = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newRight, p, x);
		int iT = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newTop, p, x);
		int iB = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newBottom, p, x);
		int iA = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newAnterior, p, x);
		int iP = planoEsquerdo->IntersectWithLine((double*)origin, (double*)_newPosterior, p, x);
		//vê o que está acontecendo
		string letraLadoEsquerdo = "";
		if (iL) letraLadoEsquerdo = letraLadoEsquerdo + "L";
		if (iR) letraLadoEsquerdo = letraLadoEsquerdo + "R";
		if (iT) letraLadoEsquerdo = letraLadoEsquerdo + "H";
		if (iB) letraLadoEsquerdo = letraLadoEsquerdo + "F";
		if (iA) letraLadoEsquerdo = letraLadoEsquerdo + "A";
		if (iP) letraLadoEsquerdo = letraLadoEsquerdo + "P";

		letraEsquerda = letraLadoEsquerdo;
	}
	{
		vtkSmartPointer<vtkPlane> planoDireito = vtkSmartPointer<vtkPlane>::New();
		planoDireito->SetOrigin(tam, 0, 0);
		planoDireito->SetNormal(-1, 0, 0);
		double p;
		double x[3];
		int iL = planoDireito->IntersectWithLine((double*)origin, (double*)_newLeft, p, x);
		int iR = planoDireito->IntersectWithLine((double*)origin, (double*)_newRight, p, x);
		int iT = planoDireito->IntersectWithLine((double*)origin, (double*)_newTop, p, x);
		int iB = planoDireito->IntersectWithLine((double*)origin, (double*)_newBottom, p, x);
		int iA = planoDireito->IntersectWithLine((double*)origin, (double*)_newAnterior, p, x);
		int iP = planoDireito->IntersectWithLine((double*)origin, (double*)_newPosterior, p, x);
		//vê o que está acontecendo
		string letra = "";
		if (iL) letra = letra + "L";
		if (iR) letra = letra + "R";
		if (iT) letra = letra + "H";
		if (iB) letra = letra + "F";
		if (iA) letra = letra + "A";
		if (iP) letra = letra + "P";
		letraDireita = letra;
	}
	{
		vtkSmartPointer<vtkPlane> planoSuperior = vtkSmartPointer<vtkPlane>::New();
		planoSuperior->SetOrigin(0, tam, 0);
		planoSuperior->SetNormal(0, -1, 0);
		double p;
		double x[3];
		int iL = planoSuperior->IntersectWithLine((double*)origin, (double*)_newLeft, p, x);
		int iR = planoSuperior->IntersectWithLine((double*)origin, (double*)_newRight, p, x);
		int iT = planoSuperior->IntersectWithLine((double*)origin, (double*)_newTop, p, x);
		int iB = planoSuperior->IntersectWithLine((double*)origin, (double*)_newBottom, p, x);
		int iA = planoSuperior->IntersectWithLine((double*)origin, (double*)_newAnterior, p, x);
		int iP = planoSuperior->IntersectWithLine((double*)origin, (double*)_newPosterior, p, x);
		//vê o que está acontecendo
		string letra = "";
		if (iL) letra = letra + "L";
		if (iR) letra = letra + "R";
		if (iT) letra = letra + "H";
		if (iB) letra = letra + "F";
		if (iA) letra = letra + "A";
		if (iP) letra = letra + "P";
		letraCima = letra;
	}
	{
		vtkSmartPointer<vtkPlane> planoInferior = vtkSmartPointer<vtkPlane>::New();
		planoInferior->SetOrigin(0, -tam, 0);
		planoInferior->SetNormal(0, 1, 0);
		double p;
		double x[3];
		int iL = planoInferior->IntersectWithLine((double*)origin, (double*)_newLeft, p, x);
		int iR = planoInferior->IntersectWithLine((double*)origin, (double*)_newRight, p, x);
		int iT = planoInferior->IntersectWithLine((double*)origin, (double*)_newTop, p, x);
		int iB = planoInferior->IntersectWithLine((double*)origin, (double*)_newBottom, p, x);
		int iA = planoInferior->IntersectWithLine((double*)origin, (double*)_newAnterior, p, x);
		int iP = planoInferior->IntersectWithLine((double*)origin, (double*)_newPosterior, p, x);
		//vê o que está acontecendo
		string letra = "";
		if (iL) letra = letra + "L";
		if (iR) letra = letra + "R";
		if (iT) letra = letra + "H";
		if (iB) letra = letra + "F";
		if (iA) letra = letra + "A";
		if (iP) letra = letra + "P";
		letraBaixo = letra;
	}
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
	rect.y = screenHeight -21;
	rect.w = 20;
	rect.h = 20;
	SDL_RenderDrawRect(canvasSdlRenderer, &rect);
	//Desenho das letras
	SDL_Color color = { 255, 255, 0 };
	int texW = 0;
	int texH = 0;
	SDL_Rect dstrect;
	//Desenha a letra da esquerda
	SDL_Surface *surfaceEsquerda = TTF_RenderText_Solid(font, letraEsquerda.c_str(), color);
	SDL_Texture * textureEsquerda = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceEsquerda);
	SDL_QueryTexture(textureEsquerda, NULL, NULL, &texW, &texH);
	dstrect = { 15, screenHeight / 2, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureEsquerda, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra da direita
	SDL_Surface *surfaceDireita = TTF_RenderText_Solid(font, letraDireita.c_str(), color);
	SDL_Texture * textureDireita = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceDireita);
	SDL_QueryTexture(textureDireita, NULL, NULL, &texW, &texH);
	dstrect = { screenWidth-15, screenHeight / 2, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureDireita, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra de cima
	SDL_Surface *surfaceCima = TTF_RenderText_Solid(font, letraCima.c_str() , color);
	SDL_Texture * textureCima = SDL_CreateTextureFromSurface(canvasSdlRenderer, surfaceCima);
	SDL_QueryTexture(textureCima, NULL, NULL, &texW, &texH);
	dstrect = { screenWidth / 2, screenHeight - 15, texW, texH };
	SDL_RenderCopyEx(canvasSdlRenderer, textureCima, NULL, &dstrect, 0, NULL, SDL_FLIP_VERTICAL);
	//Desenha a letra de baixo
	SDL_Surface *surfaceBaixo = TTF_RenderText_Solid(font, letraBaixo.c_str(), color);
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
	SDL_FreeSurface(surfaceEsquerda);
	SDL_FreeSurface(surfaceCima);
	SDL_FreeSurface(surfaceBaixo);
	SDL_FreeSurface(surfaceDireita);
	SDL_DestroyTexture(textureBaixo);
	SDL_DestroyTexture(textureCima);
	SDL_DestroyTexture(textureDireita);
	SDL_DestroyTexture(textureEsquerda);
}
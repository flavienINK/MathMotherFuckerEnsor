#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "MathIO.hpp"
#include "draw.hpp"

#define NB_POINTS 7

static const uint32_t FRAME_RATE = 30;
static const uint32_t MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 1200, WINDOW_HEIGHT = 300;
static const size_t BYTES_PER_PIXEL = 32;

typedef struct
{
	char key[SDLK_LAST];
	int mousex,mousey;
	int mousexrel,mouseyrel;
	char mousebuttons[8];
    char quit;
} Input;


void UpdateEvents(Input* in)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			in->key[event.key.keysym.sym]=1;
			break;
		case SDL_KEYUP:
			in->key[event.key.keysym.sym]=0;
			break;
		case SDL_MOUSEMOTION:
			in->mousex=event.motion.x;
			in->mousey=event.motion.y;
			in->mousexrel=event.motion.xrel;
			in->mouseyrel=event.motion.yrel;
			break;
		case SDL_MOUSEBUTTONDOWN:
			in->mousebuttons[event.button.button]=1;
			break;
		case SDL_MOUSEBUTTONUP:
			in->mousebuttons[event.button.button]=0;
			break;
		case SDL_QUIT:
			in->quit = 1;
			break;
		default:
			break;
		}
	}
}

enum Color{
	RED = 0,
	BLUE = 2,
	GREEN = 1,
	PURPLE = 3
};

/* Computing the tensor */
const Eigen::VectorXd computeTensor(const Eigen::MatrixXd& list1, const Eigen::MatrixXd& list2, const Eigen::MatrixXd& list3){
	//Remplir la matrice A pour construire le tensor
	std::cout<<"//-> STARTING THE COMPUTING OF THE TENSOR"<<std::endl;
	uint32_t nbPoints = 200;
	nbPoints = list1.rows();
	if(nbPoints > list2.rows()){ nbPoints = list2.rows(); }
	if(nbPoints > list3.rows()){ nbPoints = list3.rows(); }
	std::cout<<"//-> Nb Points :"<<nbPoints<<std::endl;
	
	/* Calcul de la matrice A pour calculer le tenseur */
	Eigen::MatrixXd A = Eigen::MatrixXd::Zero(4*nbPoints, 27);
	std::cout<<"//-> Matrix A [rows="<<A.rows()<<" | cols="<<A.cols()<<"]"<<std::endl;
	
	
	for(uint32_t p=0;p<nbPoints;++p){
		for(uint32_t i=0;i<2;++i){
			for(uint32_t l=0;l<2;++l){
				for(uint32_t k=0;k<3;++k){
					A(4*p+2*i+l, 9*k+3*l+2) += list1(p, k) * list2(p, i)* list3(p, 2); 
					A(4*p+2*i+l, 9*k+3*l+i) += -list1(p, k) * list2(p, 2)* list3(p, 2); 
					A(4*p+2*i+l, 9*k+3*2+2) += -list1(p, k) * list2(p, i)* list3(p, l); 
					A(4*p+2*i+l, 9*k+3*2+i) += list1(p, k) * list2(p, 2)* list3(p, l); 
				}
			}
		}
	}
	
	/* Computing the SVD of A */
	Eigen::JacobiSVD<MatrixXd> mySVD(A, ComputeThinU | ComputeThinV);
	Eigen::MatrixXd V = mySVD.matrixV();
	std::cout<<"//-> Matrix V [rows="<<V.rows()<<" | cols="<<V.cols()<<"]"<<std::endl;
	
	/* Vector T : le tensor */
	Eigen::VectorXd T = V.col(26);
	std::cout<<"//-> Vector T [size="<<T.size()<<"]"<<std::endl;
	
	std::cout<<"//-> TENSOR IS HERE MY FRIEND !"<<std::endl<<std::endl;
	return T;
}

/* Do the transfert */
const Eigen::VectorXd doTransfert(const Eigen::VectorXd& T, const Eigen::VectorXd& p1, const Eigen::VectorXd& p2){
	std::cout<<"//-> STARTING THE TRANSFERT"<<std::endl;
	/* create the Aprime matrix */
	Eigen::MatrixXd Aprime = Eigen::MatrixXd::Zero(4, 2);
	Eigen::VectorXd res = Eigen::VectorXd::Zero(4);
	
	for(uint32_t i=0;i<2;++i){
		for(uint32_t l=0;l<2;++l){
			for(uint32_t k=0;k<3;++k){
				Aprime(2*i+l, l) += p1(k) * (p2(2)*T(9*k+3*2+i) - p2(i)*T(9*k+3*2+2));
				res(2*i+l) += -p1(k) * (p2(i)*T(9*k+3*l+2) - p2(2)*T(9*k+3*l+i));
			}
		}
	}
	
	/* Computing the SVD of Aprime */
	Eigen::JacobiSVD<MatrixXd> trSVD(Aprime, ComputeThinU | ComputeThinV);
	
	Eigen::VectorXd guessPoint = trSVD.solve(res);
	
	Eigen::VectorXd p3(3);
	p3(0) = floor(guessPoint(0));
	p3(1) = floor(guessPoint(1));
	p3(2) = 1;
	std::cout<<"//-> Point 3 [size="<<p3.size()<<"]"<<std::endl;
	std::cout<<p3<<std::endl;
	
	std::cout<<"//-> TRAAAAAAAAAAN-SFERT!!!"<<std::endl<<std::endl;
	return p3;
}

int main(int argc, char *argv[]){
	/*******************************************************
	* PROGRAM INITIALIZATION
	*******************************************************/
	
	//Verification
	if (argc <= 3){
		std::cerr<<"[!]-> Error IMAGES is missing. Close program."<<std::endl;
		return EXIT_FAILURE;
	}
		
	//SDL image
	if(-1 == IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)){
		std::cerr<<"[!]-> Error IMG_INIT. Close program."<<std::endl;
		return EXIT_FAILURE;
	}
	
	//Load the 3 images	
	std::string image_1 = "input/" + std::string(argv[1]);
	std::string image_2 = "input/" + std::string(argv[2]);
	std::string image_3 = "input/" + std::string(argv[3]);
	SDL_Surface* img1 = IMG_Load(image_1.c_str());
	SDL_Surface* img2 = IMG_Load(image_2.c_str());
	SDL_Surface* img3 = IMG_Load(image_3.c_str());
	if(!img1 || !img2 || !img3){
		std::cerr<<"[!]-> Error while loading the image. Close the program"<<std::endl;
		return EXIT_FAILURE;
	}
	
	//SDL 
	uint32_t w_width = img1->w + img2->w + img3->w;
	uint32_t w_height = img1->h;
	if(-1 == SDL_Init(SDL_INIT_VIDEO)){
		std::cerr<<"[!]-> Error SDL_Init. Close the program"<<std::endl;
	}
	SDL_Surface* screen = SDL_SetVideoMode(w_width, w_height, BYTES_PER_PIXEL, SDL_HWSURFACE);
	SDL_WM_SetCaption("Super Tensor !", NULL);
	
	/**************************************************
	 *  PRE-PROCESSING
	 * ****************************************************/
	 //Creation des couleurs
	 uint32_t colors[] = {
		0xffff0000,
		0xff0000ff,
		0xff00ff00,
		0xfff608e3
	 };
	 
	 //Tensor
	 Eigen::VectorXd tensor;
	 
	 //Création des listes
	 bool listCharged = false;
	 bool tensorComputed = false;
	 
	 Eigen::MatrixXd list1 = Eigen::MatrixXd::Zero(NB_POINTS,3);
	 Eigen::MatrixXd list2 = Eigen::MatrixXd::Zero(NB_POINTS,3);
	 Eigen::MatrixXd list3 = Eigen::MatrixXd::Zero(NB_POINTS,3);
	 
	 //IF lists are inclued
	 if (argc <= 7 && argc > 4){
		 //Chargement des listes
		 listCharged = true;		 
		 kn::loadMatrix(list1, "input/" + std::string(argv[4]));
		 kn::loadMatrix(list2, "input/" + std::string(argv[5]));
		 kn::loadMatrix(list3, "input/" + std::string(argv[6]));
		 
		 tensorComputed = true;
		 tensor = computeTensor(list1, list2, list3);
	}
	
	/* Transfert */	
	//Eigen::VectorXd p3 = doTransfert(T, list1.row(2), list2.row(2));	
	
	/**************************************
	 *  DISPLAY LOOP
	 * **************************************/
	 
	 int compteListe = 0;
	 int compteItemDansPoint = 0;
	 int comptePointDansListe = 0;
	 
	Input in;
	memset(&in,0,sizeof(in));
	while(!in.key[SDLK_ESCAPE] && !in.quit)
	{
		uint32_t start = 0;
		uint32_t end = 0;
		uint32_t ellapsedTime = 0;
		start = SDL_GetTicks();
		 
		/* DRAWING */
		//Bild the image on the screen
		SDL_Rect img1_offset;
		img1_offset.x = 0;
		img1_offset.y = 0;
		SDL_BlitSurface(img1, NULL, screen, &img1_offset);
		
		SDL_Rect img2_offset = img1_offset;
		img2_offset.x += img1->w;
		SDL_BlitSurface(img2, NULL, screen, &img2_offset);
		
		SDL_Rect img3_offset = img2_offset;
		img3_offset.x += img2->w;
		SDL_BlitSurface(img3, NULL, screen, &img3_offset);	
		
		//Draw the points
		// ATTENTION BUG ICI
		if (listCharged == true)
		{
			for(int i=0;i<list1.rows();++i){
				fill_circle(screen, img1_offset.x + list1(i, 0), list1(i, 1), 3, colors[RED]);
			}
		
			for(int i=0;i<list2.rows();++i){
				fill_circle(screen, img2_offset.x + list2(i, 0), list2(i, 1), 3, colors[GREEN]);
			}
		
			for(int i=0;i<list3.rows();++i){
				fill_circle(screen, img3_offset.x + list3(i, 0), list3(i, 1), 3, colors[BLUE]);
			}
		}
		else if (listCharged == false && comptePointDansListe >= 1) {
			//std::cout << "on est ici" << std::endl;
			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list1(i, 0), list1(i, 1), 3, colors[RED]);
			}

			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list2(i, 0), list2(i, 1), 3, colors[GREEN]);
			}
			
			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list3(i, 0), list3(i, 1), 3, colors[BLUE]);
			}
		} 
		
		SDL_Flip(screen);
		
		/* EVENT */
		
		UpdateEvents(&in);
		if (in.mousebuttons[SDL_BUTTON_LEFT])
		{
			// On clique une fois, donc on remet à 0 l'état du bouton
			in.mousebuttons[SDL_BUTTON_LEFT] = 0;
			
			// Si les listes sont à créer alors
			if (listCharged == false)
			{
				// On incrémente la liste pour commencer à la première list
				compteListe++;
			
				if (comptePointDansListe < NB_POINTS) 
				{
					if (compteListe == 1) 
					{
						list1(comptePointDansListe, compteItemDansPoint)   = in.mousex; // list1(0,0) = x, => list1(1,0) = x => ...
						list1(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list1(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
				
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
				
						std::cout << "on est dans la liste 1" << std::endl;
					}
			
					else if (compteListe == 2) 
					{
						list2(comptePointDansListe, compteItemDansPoint)   = in.mousex; // list1(0,0) = x, => list1(1,0) = x => ...
						list2(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list2(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
						
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
				
						std::cout << "on est dans la liste 2" << std::endl;
					}
					else
					{
						list3(comptePointDansListe, compteItemDansPoint)   = in.mousex; // list1(0,0) = x, => list1(1,0) = x => ...
						list3(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list3(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
						
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
		
						// On passe au point suivant
						comptePointDansListe++;
				
						// On retourne à la liste1
						compteListe = 0;
				
						std::cout << "on est dans la liste 3" << std::endl;
					}
				}
			}
		}	
		
		/* IDLE */
		if(!tensorComputed && comptePointDansListe>=NB_POINTS){
			tensorComputed = true;
			tensor = computeTensor(list1, list2, list3);
		}
		

		end = SDL_GetTicks();
		ellapsedTime = end - start;
		if(ellapsedTime < MIN_LOOP_TIME){
		SDL_Delay(MIN_LOOP_TIME - ellapsedTime);
		 }
	 }
	
	
	/*******************************************
	 * 	FREE RESOURCES
	 * *******************************************/
	SDL_FreeSurface(img1); 
	SDL_FreeSurface(img2); 
	SDL_FreeSurface(img3); 
	IMG_Quit();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}




#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "MathIO.hpp"
#include "draw.hpp"
#include "Tensor.hpp"

#define NB_POINTS_NEEDED 7
#define NB_POINTS 10

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
	 leydef::Tensor tensor;
	 
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
		tensor.compute(list1, list2, list3);
		 
		list1 = Eigen::MatrixXd::Zero(NB_POINTS,3);
		list2 = Eigen::MatrixXd::Zero(NB_POINTS,3);
		list3 = Eigen::MatrixXd::Zero(NB_POINTS,3);
	}	
	
	/**************************************
	 *  DISPLAY LOOP
	 * **************************************/
	 
	int compteListe = 1;
	int compteItemDansPoint = 0;
	int comptePointDansListe = 0;
	 
	Eigen::VectorXd firstPoint(3);
	Eigen::VectorXd secondPoint(3);
	Eigen::VectorXd thirdPoint(3);
	 
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
		if (comptePointDansListe >= 1) 
		{
			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list1(i, 0), list1(i, 1), 3, colors[RED]);
			}

			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list2(i, 0) + img1->w, list2(i, 1), 3, colors[GREEN]);
			}
			
			for(int i=0;i<comptePointDansListe;++i){
				fill_circle(screen, list3(i, 0) + img1->w + img2->w, list3(i, 1), 3, colors[BLUE]);
			}
		}
		
		SDL_Flip(screen);
		
		/* EVENT */
		UpdateEvents(&in);
		if (in.mousebuttons[SDL_BUTTON_LEFT])
		{
			// On clique une fois, donc on remet à 0 l'état du bouton
			in.mousebuttons[SDL_BUTTON_LEFT] = 0;
						
			if (comptePointDansListe < NB_POINTS) 
			{
				//std::cout << "X = " << in.mousex << std::endl;
				if (compteListe == 1) 
				{
					// On vérifie que le clic se situe bien dans l'image 1
					if (in.mousex <= img1->w)
					{
						//std::cout << "on est dans la liste 1" << std::endl;
						//std::cout << "Taille de l'image 1 : " << img1->w << std::endl;
						list1(comptePointDansListe, compteItemDansPoint)   = in.mousex; // list1(0,0) = x, => list1(1,0) = x => ...
						list1(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list1(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
			
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
						
						compteListe++;
						
						if (listCharged == true)
						{
							// On génère le premier point
							firstPoint(0) = in.mousex;
							firstPoint(1) = in.mousey;
							firstPoint(2) = 1;
						}
					}
				}
		
				else if (compteListe == 2) 
				{
					// On vérifie que le clic se situe bien dans l'image 2
					if (in.mousex <= img1->w + img2->w && in.mousex > img1->w)
					{
						
						//std::cout << "on est dans la liste 2" << std::endl;
						//std::cout << "Taille de l'image 2 : " << img2->w << std::endl;
						list2(comptePointDansListe, compteItemDansPoint)   = in.mousex - img1->w; // list1(0,0) = x, => list1(1,0) = x => ...
						list2(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list2(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
					
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
						
						compteListe++;
						
						// Si on suffisament de points pour le tenseur, alors on peut calculer le nouveau point
						// sur l'image 3 à l'aide des deux premiers points cliqués
						if (listCharged == true)
						{								

							// On génère le second point
							secondPoint(0) = in.mousex - img1->w;
							secondPoint(1) = in.mousey;
							secondPoint(2) = 1;
							
							//std::cout << "x1 = " << firstPoint(0) << " y1 = " << firstPoint(1) << std::endl;
							//std::cout << "x2 = " << secondPoint(0) << " y2 = " << secondPoint(1) << std::endl;
							
							thirdPoint = tensor.doTransfert(firstPoint, secondPoint);
							
							list3(comptePointDansListe, compteItemDansPoint)   = thirdPoint(0);
							list3(comptePointDansListe, compteItemDansPoint+1) = thirdPoint(1); // list1(0,1) = y, => list1(1,1) = y => ...
							list3(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
						
							// On passe au point suivant
							comptePointDansListe++;
			
							// On retourne à la liste1
							compteListe = 1;
						}
						
					}
					
				}
				else
				{
					// On vérifie que le clic se situe bien dans l'image 3
					if (in.mousex <= img1->w + img2->w + img3->w && in.mousex > img1->w + img2->w)
					{
						list3(comptePointDansListe, compteItemDansPoint)   = in.mousex - img1->w - img2->w; // list1(0,0) = x, => list1(1,0) = x => ...
						list3(comptePointDansListe, compteItemDansPoint+1) = in.mousey; // list1(0,1) = y, => list1(1,1) = y => ...
						list3(comptePointDansListe, compteItemDansPoint+2) = 1;         // list1(0,2) = 1, => list1(1,2) = 1 => ...
					
						// On remet le compteur des items à 0
						compteItemDansPoint = 0;
	
						// On passe au point suivant
						comptePointDansListe++;
			
						// On retourne à la liste1
						compteListe = 1;
					}
				}
			}
		}	
		
		/* IDLE */
		if(!tensorComputed && comptePointDansListe >= NB_POINTS_NEEDED)
		{
			tensorComputed = true;
			tensor.compute(list1, list2, list3);
			
			// On réinitialise les listes après avoir fait le tensor
			list1 = Eigen::MatrixXd::Zero(NB_POINTS,3);
			list2 = Eigen::MatrixXd::Zero(NB_POINTS,3);
			list3 = Eigen::MatrixXd::Zero(NB_POINTS,3);
			
			listCharged = true;
			comptePointDansListe = 0;
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




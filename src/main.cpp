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
#include "PointList.hpp"

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
	 
	 /* Lists of points used for building the Tensor */
	 leydef::PointList equiv1(colors[RED]);
	 leydef::PointList equiv2(colors[GREEN]);
	 leydef::PointList equiv3(colors[BLUE]);
	 
	 /* Lists of points generate by the user for transfering */
	 leydef::PointList running1(colors[PURPLE]);
	 leydef::PointList running2(colors[PURPLE]);
	 leydef::PointList running3(colors[PURPLE]);
	 
	 //IF lists are inclued
	 if (argc <= 7 && argc > 4){
		//Chargement des listes
		listCharged = true;
		 
		equiv1.load("input/" + std::string(argv[4]));
		equiv2.load("input/" + std::string(argv[5]));
		equiv3.load("input/" + std::string(argv[6]));
		 
		tensorComputed = true;
		tensor.compute(equiv1.getData(), equiv2.getData(), equiv3.getData());
	}	
	
	/**************************************
	 *  DISPLAY LOOP
	 * **************************************/
	 
	int compteListe = 1;
	 
	Eigen::VectorXd clickedPoint(3);
	Eigen::VectorXd guessedPoint(3);
	 
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
		
		//Drawing the lists
		equiv1.draw(screen, img1_offset.x);
		equiv2.draw(screen, img2_offset.x);
		equiv3.draw(screen, img3_offset.x);
		running1.draw(screen, img1_offset.x);
		running2.draw(screen, img2_offset.x);
		running3.draw(screen, img3_offset.x);
		
		SDL_Flip(screen);
		
		/* EVENT */
		UpdateEvents(&in);
		if (in.mousebuttons[SDL_BUTTON_LEFT])
		{
			// On clique une fois, donc on remet à 0 l'état du bouton
			in.mousebuttons[SDL_BUTTON_LEFT] = 0;
			
			/* Evaluer sur quelle image on doit cliquer */
			if (compteListe == 1)
			{
				// On vérifie que le clic se situe bien dans l'image 1
				if (in.mousex <= img1->w)
				{
					// On génère le premier point
					clickedPoint(0) = in.mousex;
					clickedPoint(1) = in.mousey;
					clickedPoint(2) = 1;
					
					if(listCharged == false){
						equiv1.addPoint(clickedPoint);
					}else{
						running1.addPoint(clickedPoint);
					}
					compteListe++;
				}
			}
	
			else if (compteListe == 2) 
			{
				// On vérifie que le clic se situe bien dans l'image 2
				if (in.mousex <= img1->w + img2->w && in.mousex > img1->w)
				{	
					// On génère le second point
					clickedPoint(0) = in.mousex - img1->w;
					clickedPoint(1) = in.mousey;
					clickedPoint(2) = 1;
					
					if (listCharged == false){
						equiv2.addPoint(clickedPoint);
						compteListe++;
					}else{
						running2.addPoint(clickedPoint);
						guessedPoint = tensor.doTransfert(running1.getLastPoint(), running2.getLastPoint());
						running3.addPoint(guessedPoint);
						compteListe = 1;
					}						
				}
			}
			else
			{
				// On vérifie que le clic se situe bien dans l'image 3
				if (in.mousex <= img1->w + img2->w + img3->w && in.mousex > img1->w + img2->w)
				{
					clickedPoint(0) = in.mousex - img1->w - img2->w;
					clickedPoint(1) = in.mousey;
					clickedPoint(2) = 1;
					equiv3.addPoint(clickedPoint);
		
					// On retourne à aux listes 1
					compteListe = 1;
				}
			}
		}	
		
		/* IDLE */
		if(!tensorComputed && equiv1.getSize() >= NB_POINTS_NEEDED && equiv2.getSize() >= NB_POINTS_NEEDED && equiv3.getSize() >= NB_POINTS_NEEDED)
		{
			tensorComputed = true;
			tensor.compute(equiv1.getData(), equiv2.getData(), equiv3.getData());
			
			listCharged = true;
		}
		

		end = SDL_GetTicks();
		ellapsedTime = end - start;
		if(ellapsedTime < MIN_LOOP_TIME){
		SDL_Delay(MIN_LOOP_TIME - ellapsedTime);
		 }
	 }
	
	/* Saving the new lists */
	running1.save("input/customlist1.list");
	running2.save("input/customlist2.list");
	running3.save("input/customlist3.list");
	
	
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




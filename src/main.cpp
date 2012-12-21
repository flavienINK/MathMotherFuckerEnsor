#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "MathIO.hpp"
#include "draw.hpp"

static const uint32_t FRAME_RATE = 30;
static const uint32_t MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 1200, WINDOW_HEIGHT = 300;
static const size_t BYTES_PER_PIXEL = 32;

/* INK */

enum Color{
	RED = 0,
	BLUE = 2,
	GREEN = 1
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
		0xff00ff00
	 };
	 
	 //Chargement des listes
	 Eigen::MatrixXd list1;
	 Eigen::MatrixXd list2;
	 Eigen::MatrixXd list3;
	 kn::loadMatrix(list1, "input/list1.list");
	 kn::loadMatrix(list2, "input/list2.list");
	 kn::loadMatrix(list3, "input/list3.list");
	 
	 //IF lists are inclued
	 if (argc <= 7 && argc > 4){
		 //Chargement des listes
		 kn::loadMatrix(list1, "input/" + std::string(argv[4]));
		 kn::loadMatrix(list2, "input/" + std::string(argv[5]));
		 kn::loadMatrix(list3, "input/" + std::string(argv[6]));
	}
	
	//Remplir la matrice A pour construire le tensor
	uint8_t nbPoints = 200;
	nbPoints = (uint8_t) list1.rows();
	if(nbPoints > list2.rows()){ nbPoints = (uint8_t) list2.rows(); }
	if(nbPoints > list3.rows()){ nbPoints = (uint8_t) list3.rows(); }
	std::cout<<"//-> "<<nbPoints<<std::endl;
	
	
	/**************************************
	 *  DISPLAY LOOP
	 * **************************************/
	 bool loop = true;
	 while(loop){
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
		for(int i=0;i<list1.rows();++i){
			fill_circle(screen, img1_offset.x + list1(i, 0), list1(i, 1), 3, colors[RED]);
		}
		
		for(int i=0;i<list2.rows();++i){
			fill_circle(screen, img2_offset.x + list2(i, 0), list2(i, 1), 3, colors[GREEN]);
		}
		
		for(int i=0;i<list3.rows();++i){
			fill_circle(screen, img3_offset.x + list3(i, 0), list3(i, 1), 3, colors[BLUE]);
		}
		
		SDL_Flip(screen);
		
		/* EVENT */
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			switch(e.type){
				//Gestion clavier
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym){
						case SDLK_ESCAPE:
							loop = false;
							break;

						default:
							break;
					}
					break;

				case SDL_QUIT:
					loop = false;
					break;

				default:
					break;
			}			 
		}

		/* IDLE */


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




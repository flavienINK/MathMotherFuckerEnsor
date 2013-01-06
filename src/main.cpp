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

//flag to know what image was clicked
enum ImgClicked{
	NOIMG,
	IMG1,
	IMG2,
	IMG3
};

void displayHelp(){
	std::cout<<std::endl;
	std::cout<<"############################ TENSOR TRIFOCAL HELP ###############################"<<std::endl;
	std::cout<<"Welcome in the help of the TENSOR TRIFOCAL program"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"-> usage : ./bin/trifocal [-h]"<<std::endl;
	std::cout<<"                          <image1_path> <image2_path> <image3_path>"<<std::endl;
	std::cout<<"                          [input_list1_path] [input_list2_path] [input_list3_path] "<<std::endl;
	std::cout<<"                          [-s/-w/-sw [output_filename]]"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"-> Note that the image paths are compulsory. The input lists of point path are optionnal."<<std::endl;
	std::cout<<"->If they aren't mentionned, you will have to do yourself the necessary 7 equivalences."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"-> Here is the function of the different options : "<<std::endl;
	std::cout<<"\t-h : displaying the help"<<std::endl;
	std::cout<<"\t-s : saving the lists generated by you during the transfert."<<std::endl;
	std::cout<<"\t     This lists of points will be save in files name [output_filename]-RunningList-(1-3).list in the output directory."<<std::endl;
	std::cout<<"\t-w : saving the lists generated by you to build the tensor."<<std::endl;
	std::cout<<"\t     This lists of points will be save in files name [output_filename]-Equiv-(1-3).list in the input directory."<<std::endl;
	std::cout<<"\t     Use this only if you don't already load point lists."<<std::endl;
	std::cout<<"\t-sw : saving the both lists."<<std::endl;
	std::cout<<"\t     Use this only if you don't already load point lists."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"-> Have a good transfert !"<<std::endl;
	std::cout<<"##################################################################################"<<std::endl;
	std::cout<<std::endl;
}

void displayError(char * arg){
	std::cerr<<"[!]-> Error : command " << std::string(arg) << " is unknow" << std::endl;
	std::cerr<<"[!]-> Did you mean -s to save the currents points ?" << std::endl;
	std::cerr<<"[!]-> See '-h' for more information on a specific command." << std::endl;
}

int main(int argc, char *argv[]){
	/*******************************************************
	* PROGRAM INITIALIZATION
	*******************************************************/
	
	//Check if the user is waiting for the help
	if(argc>=2){
		if(std::string(argv[1]) == "-h"){
			displayHelp();
			return EXIT_SUCCESS;
		}
	}
	
	//Verification
	if (argc <= 3){
		std::cerr<<"[!]-> Error IMAGES is missing. Close program."<<std::endl;
		displayHelp();
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
	 bool listRunningPointsSaved = false;
	 bool listOriginsPointsSaved = false;
	 int countArg = 0;
	 
	 /* Lists of points used for building the Tensor */
	 leydef::PointList equiv1(colors[RED]);
	 leydef::PointList equiv2(colors[GREEN]);
	 leydef::PointList equiv3(colors[BLUE]);
	 
	 /* Lists of points generate by the user for transfering */
	 leydef::PointList running1(colors[PURPLE]);
	 leydef::PointList running2(colors[PURPLE]);
	 leydef::PointList running3(colors[PURPLE]);
	 
	 //IF lists are inclued or save is waiting for	
 	if (argc <= 6 && argc > 4)  
	{
		if (argv[4] == std::string("-s"))
			listRunningPointsSaved = true;
			
		else if (argv[4] == std::string("-w"))
			listOriginsPointsSaved = true;
			
		else if (argv[4] == std::string("-sw"))
		{
			listRunningPointsSaved = true;
			listOriginsPointsSaved = true;
		}
		else
			displayError(argv[4]);
			
		countArg = 4;
	}
	else if (argc > 4)
	{
		//Chargement des listes
		listCharged = true;
		
		equiv1.load("input/" + std::string(argv[4]));
		equiv2.load("input/" + std::string(argv[5]));
		equiv3.load("input/" + std::string(argv[6]));
		 
		tensorComputed = true;
		tensor.compute(equiv1.getData(), equiv2.getData(), equiv3.getData());
		
		if(argc >=8){
			if(argv[7] == std::string("-s")){
				listRunningPointsSaved = true;
				countArg = 7;
			}else{
				displayError(argv[7]);
			}
		}
	}
	else
	{
	
	}
	
	/**************************************
	 *  DISPLAY LOOP
	 * **************************************/
	 
	int compteClic = 1;
	 
	Eigen::VectorXd clickedPoint(3);
	Eigen::VectorXd guessedPoint(3);
	
	//flag de la dernière image cliquée
	ImgClicked lastImgClic = NOIMG;
	
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
			
			//if we click for the tensor or for the transfert
			if(listCharged == false){
				//Click for tensor
				
				/* Evaluer sur quelle image on doit cliquer */
				if (compteClic == 1)
				{
					// On vérifie que le clic se situe bien dans l'image 1
					if (in.mousex < img1->w)
					{
						// On génère le premier point
						clickedPoint(0) = in.mousex;
						clickedPoint(1) = in.mousey;
						clickedPoint(2) = 1;
						
						equiv1.addPoint(clickedPoint);
						compteClic++;
					}
				}
				else if (compteClic == 2) 
				{
					// On vérifie que le clic se situe bien dans l'image 2
					if (in.mousex < img1->w + img2->w && in.mousex >= img1->w)
					{	
						// On génère le second point
						clickedPoint(0) = in.mousex - img1->w;
						clickedPoint(1) = in.mousey;
						clickedPoint(2) = 1;

						equiv2.addPoint(clickedPoint);
						compteClic++;					
					}
				}
				else
				{
					// On vérifie que le clic se situe bien dans l'image 3
					if (in.mousex >= img1->w + img2->w)
					{
						clickedPoint(0) = in.mousex - img1->w - img2->w;
						clickedPoint(1) = in.mousey;
						clickedPoint(2) = 1;
						equiv3.addPoint(clickedPoint);
			
						// On retourne aux listes 1
						compteClic = 1;
					}
				}
			}else{
				//click for transfert
				//On génère le point cliqué
				clickedPoint(0) = in.mousex;
				clickedPoint(1) = in.mousey;
				clickedPoint(2) = 1;
				
				//On vérifie dans quelle image il se trouve et on le met dans la bonne liste
				if(clickedPoint(0) < img1->w){
					//IMG1
					//On vérifie que dernière image cliquée n'est pas elle même.
					if(lastImgClic != IMG1){
						running1.addPoint(clickedPoint);
						
						if(compteClic >=2){
							//do transfert
							if(lastImgClic == IMG2){
								running3.addPoint(tensor.doTransfert(running1.getLastPoint(), running2.getLastPoint()));
							}else if(lastImgClic == IMG3){
								running2.addPoint(tensor.doTransfert(running1.getLastPoint(), running3.getLastPoint()));
							}
							lastImgClic = NOIMG;
							compteClic = 1;
						}else{
							lastImgClic = IMG1;
							compteClic++;
						}
					}
				}else if(clickedPoint(0) < img1->w + img2->w && clickedPoint(0) >= img1->w){
					//IMG2
					//On vérifie que dernière image cliquée n'est pas elle même.
					if(lastImgClic != IMG2){
						clickedPoint(0)-= img1->w;
						running2.addPoint(clickedPoint);
						
						if(compteClic >=2){
							//do transfert
							if(lastImgClic == IMG1){
								running3.addPoint(tensor.doTransfert(running1.getLastPoint(), running2.getLastPoint()));
							}else if(lastImgClic == IMG3){
								running1.addPoint(tensor.doTransfert(running2.getLastPoint(), running3.getLastPoint()));
							}
							compteClic = 1;
							lastImgClic = NOIMG;
						}else{
							lastImgClic = IMG2;
							compteClic++;
						}
					}
				}else if(clickedPoint(0) >= img1->w + img2->w){
					//IMG3
					//On vérifie que dernière image cliquée n'est pas elle même.
					if(lastImgClic != IMG3){
						clickedPoint(0)-= img1->w + img2->w;
						running3.addPoint(clickedPoint);
						
						if(compteClic >=2){
							//do transfert
							if(lastImgClic == IMG1){
								running2.addPoint(tensor.doTransfert(running1.getLastPoint(), running3.getLastPoint()));
							}else if(lastImgClic == IMG2){
								running1.addPoint(tensor.doTransfert(running2.getLastPoint(), running3.getLastPoint()));
							}
							lastImgClic = NOIMG;
							compteClic = 1;
						}else{
							lastImgClic = IMG3;
							compteClic++;
						}
					}
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
	if (listRunningPointsSaved == true)
	{
		if (argv[countArg+1] == NULL)
		{		
			running1.save("output/" + std::string("INK") + "-RunningList-1.list");
			running2.save("output/" + std::string("INK") + "-RunningList-2.list");
			running3.save("output/" + std::string("INK") + "-RunningList-3.list");
		}
		else 
		{
			running1.save("output/" + std::string(argv[countArg+1]) + "-RunningList-1.list");
			running2.save("output/" + std::string(argv[countArg+1]) + "-RunningList-2.list");
			running3.save("output/" + std::string(argv[countArg+1]) + "-RunningList-3.list");
		}
	}
	if (listOriginsPointsSaved == true)
	{
		if (argv[countArg+1] == NULL)
		{	
			equiv1.save("input/" + std::string("INK") + "-Equiv-1.list");
			equiv2.save("input/" + std::string("INK") + "-Equiv-2.list");
			equiv3.save("input/" + std::string("INK") + "-Equiv-3.list");
		}
		else
		{
			equiv1.save("input/" + std::string(argv[5]) + "-Equiv-1.list");
			equiv2.save("input/" + std::string(argv[5]) + "-Equiv-2.list");
			equiv3.save("input/" + std::string(argv[5]) + "-Equiv-3.list");
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




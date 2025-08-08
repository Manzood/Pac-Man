#include<stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include<stdbool.h>
#include<math.h>
#include"makingpacman.h"

#define WIDTH 570
#define HEIGHT 630
#define SPEED 3
#define SCALE 30
#define PACMANRIGHT 1
#define PACMANLEFT -1
#define DIRECTIONUP -1
#define DIRECTIONDOWN 1
#define DIRECTIONRIGHT 2
#define DIRECTIONLEFT -2

/*itenerary :
1)Making Pac-Man move about the grid, entirely as he pleases 
2)Making the grid image the background image of the entire game
3)Adding enemies
4)Enemy AI is a must, with different levels of difficulty
5)Adding pellets at every locatiom, alongside making them disappear when Pac-Man interacts with them
6)Cherries!
7)Multiple lives, level resets whem all the cherries are collected, score
8)Further improvements not mentioned here that are bound to cross my mind as coding progresses
*/

//global declaration of the player x and y coordinates and speeds
//along with the global declaration of other seemingly necessary variables, of course
//int xpos=WIDTH/2, ypos=HEIGHT/2, xspeed=10, yspeed=10;
int FPS = 80;
bool up=false, down=false, left=false, right=false;
char *pacmanimage = "midimageleft.png";

//So these two variables determine the logic behind which direction Pac-Man will turn
//In the original game, the direction you entered was saved so that the second you hit a dead end Pac-Man would change direction
//That added to the tactical element of the game, and made it feel very premium
//This is an attempt to recreate that
//The variable direction just stores the current direction Pac-Man is headed, while nextdirection stores the next input given
//If the next direction and current direction complement each other, direction changes immediately.
//Otherwise, direction changes only after Pac-Man encounters a dead end
int direction=0;
int nextdirection=0;
bool teleporting=false;

struct sprite
{
	int x;
	int y;
	int xspeed;
	int yspeed;
};

//zero is stop... one is go. Also, the zero in the 8th line (second line above halfway point) should be coloured orange
int grid[21][19]={
	{0,0,0,0,0,0,0,0,0, 0 ,0,0,0,0,0,0,0,0,0},
	{0,1,1,1,1,1,1,1,1, 0 ,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,0,0,1, 0 ,1,0,0,0,1,0,0,1,0},
	{0,1,1,1,1,1,1,1,1, 1 ,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,1,0,0, 0 ,0,0,1,0,1,0,0,1,0},
	{0,1,1,1,1,0,1,1,1, 0 ,1,1,1,0,1,1,1,1,0},
	{0,0,0,0,1,0,0,0,1, 0 ,1,0,0,0,1,0,0,0,0},
	{1,1,1,0,1,0,1,1,1, 1 ,1,1,1,0,1,0,1,1,1},
	{0,0,0,0,1,0,1,0,0, 0 ,0,0,1,0,1,0,0,0,0},
	{1,1,1,1,1,1,1,0,1, 1 ,1,0,1,1,1,1,1,1,1},
	
	{0,0,0,0,1,0,1,0,0, 0 ,0,0,1,0,1,0,0,0,0},

	{1,1,1,0,1,0,1,1,1, 1 ,1,1,1,0,1,0,1,1,1},
	{0,0,0,0,1,0,1,0,0, 0 ,0,0,1,0,1,0,0,0,0},
	{0,1,1,1,1,1,1,1,1, 0 ,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,0,0,1, 0 ,1,0,0,0,1,0,0,1,0},
	{0,1,1,0,1,1,1,1,1, 1 ,1,1,1,1,1,0,1,1,0},
	{0,0,1,0,1,0,1,0,0, 0 ,0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,0,1,1,1, 0 ,1,1,1,0,1,1,1,1,0},
	{0,1,0,0,0,0,0,0,1, 0 ,1,0,0,0,0,0,0,1,0},
	{0,1,1,1,1,1,1,1,1, 1 ,1,1,1,1,1,1,1,1,0},
	{0,0,0,0,0,0,0,0,0, 0 ,0,0,0,0,0,0,0,0,0}
};

//basic initialisation function for SDL
int init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING)!=0)
		printf("Error initialising SDL = %s\n", SDL_GetError());
	if (SCALE%SPEED!=0)
	{
		printf("Sorry, the speed is not a factor of the scale of each box in the grid, which means the game will not work.\n. Please make the speed a factor of the scale, and try again.\n");
		return -1;
	}
}

//Can be called to toggle fullscreen mode at any time
void togglefullscreen(SDL_Window* window)
{
	Uint32 FullScreen = SDL_WINDOW_FULLSCREEN;
	int status = SDL_GetWindowFlags(window);
	if (status = FullScreen)
		SDL_SetWindowFullscreen(window, 0);
	else
		SDL_SetWindowFullscreen(window, FullScreen);
}

void updatescreen(SDL_Renderer* renderer, SDL_Texture *backgroundtexture, SDL_Texture *PacMan, SDL_Rect *pacman)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderCopy(renderer,backgroundtexture,NULL,NULL);
	SDL_RenderCopy(renderer,PacMan,NULL,pacman);
}

//function to make drawing multiple rectangles in one page easier: mimics the functions to draw rectangles in pygame
void drawrectangle(SDL_Renderer* renderer, int x, int y, int w, int h)
{
	SDL_Rect rect;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;

	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &rect);
}

//function that controls the movement of pacman, and maybe the ghosts as well (it can be pretty generalized)
//my coup de grace, pretty much
struct sprite move (struct sprite s) 
{
	int gridx=s.x/30;
	int gridy=s.y/30;
	
	//the code that stores the next click of the user, and thereby allows them to change direction preemptively
	if (direction+nextdirection==0)
	{
		direction=nextdirection;
	}
	else
	{
		if (nextdirection==DIRECTIONUP||nextdirection==DIRECTIONDOWN)
		{
			if (nextdirection==DIRECTIONUP)
			{
				if (s.x%30==0)
				{
					if(grid[gridy-1][gridx]==1)
						direction=nextdirection;
				}
			}
			else
			{
				if (s.x%30==0)
					if (grid[gridy+1][gridx]==1)
						direction=nextdirection;
			}
		}
		else if (nextdirection==DIRECTIONLEFT||nextdirection==DIRECTIONRIGHT)
		{
			if (nextdirection==DIRECTIONLEFT)
			{
				if (s.y%30==0)
				{
					if (grid[gridy][gridx-1]==1)
						direction=nextdirection;
				}
			}
			else
			{
				if (s.y%30==0)
				{
					if (grid[gridy][gridx+1]==1)
						direction=nextdirection;
				}
			}
		}
	}

	//the actual movement happens here
	if (direction==DIRECTIONUP)
	{
		if (grid[gridy][gridx]==1 && s.y%30>=s.yspeed)
		{
			s.y-=s.yspeed;
		}
		else if (grid[gridy][gridx]==1 && s.y%30>0)
		{
			s.y-=s.y%30;
		}
		else
		{
			if (grid[gridy-1][gridx]==0)
				direction=nextdirection;
			else
				s.y-=s.yspeed;
		}
	}
	if (direction==DIRECTIONLEFT)
	{
		if (teleporting==false)
		{
			if (grid[gridy][gridx]==1 && s.x%30>=s.xspeed)
			{
				s.x-=s.xspeed;
			}
			else if (grid[gridy][gridx]==1 && s.x%30>0)
			{
				s.x-=s.x%30;
			}
			else
			{
				if (gridx==0)
				{
					teleporting=true;
				}
				else if (grid[gridy][gridx-1]==0)
					direction=nextdirection;
				else
					s.x-=s.xspeed;
			}
		}
		else
		{
			if(s.x==-30)
			{
				s.x=WIDTH;
				teleporting=false;
			}
			else
				s.x-=s.xspeed;
		}
	}
	if (direction==DIRECTIONRIGHT)
	{
		if (teleporting==false) 
		{
			if (grid[gridy][gridx+1]==1 && s.x%30+s.xspeed<=30)
			{
				s.x+=s.xspeed;
			}
			else if (grid[gridy][gridx+1]==1 && s.x%30+s.xspeed>30)
			{
				s.x+=30-s.x%30;
			}
			else
			{
				if (gridx==18||gridx==19)
					teleporting=true;
				else if (grid[gridy][gridx+1]==0)
					direction=nextdirection;
				else
				 	s.x+=s.xspeed;
			}
		}
		else
		{
			if (s.x==WIDTH+30)
			{
				s.x=-30;
				teleporting=false;
			}
			else
				s.x+=s.xspeed;
		}
	}
	if (direction==DIRECTIONDOWN)
	{
		if (grid[gridy+1][gridx]==1 && s.y%30+s.yspeed<=30)
		{
			s.y+=s.yspeed;
		}
		else if (grid[gridy+1][gridx]==1 && s.y%30+s.yspeed>30)
		{
			s.y+=30-s.y%30;
		}
		else
		{
			if (grid[gridy+1][gridx]==0)
				direction=nextdirection;
			else
				s.y+=s.yspeed;
		}
	}
	return s;
}

//function that starts player movement when the key is pressed
void startplayer(Uint32 code)
{
	if (code == SDLK_UP)
	{
		up=true;
		if (direction==0)
			direction=DIRECTIONUP;
		else
			nextdirection=DIRECTIONUP;
	}
	if (code == SDLK_DOWN)
	{
		down=true;
		if (direction==0)
			direction=DIRECTIONDOWN;
		else
			nextdirection=DIRECTIONDOWN;
	}
	if (code == SDLK_LEFT)
	{
		left=true;
		if (direction==0)
			direction=DIRECTIONLEFT;
		else
			nextdirection=DIRECTIONLEFT;
	}
	if (code == SDLK_RIGHT)
	{
		right=true;
		if (direction==0)
			direction=DIRECTIONRIGHT;
		else
			nextdirection=DIRECTIONRIGHT;
	}
}

//function that stops player movement upon the arrow keys being lifted up
void stopplayer(Uint32 code)
{
	if (code == SDLK_UP)
		up=false;
	if (code == SDLK_DOWN)
		down=false;
	if (code == SDLK_LEFT)
		left=false;
	if (code == SDLK_RIGHT)
		right=false;
}

int main()
{
	//function that initialises everything in the SDL library
	if (init()==-1)
		return -1;

	//Creation of a window and setting up of the renderer with a default colour
	SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);
	SDL_Surface *background=IMG_Load("pacman_grid.png");
	SDL_Texture *backgroundtexture = SDL_CreateTextureFromSurface(renderer, background);
	SDL_FreeSurface(background);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetWindowFullscreen(window, 0);
	SDL_RenderPresent(renderer);
	SDL_Surface *PacMan=IMG_Load("midimageleft.png");
	SDL_Texture *PacManTexture = SDL_CreateTextureFromSurface(renderer,PacMan);

	SDL_Rect pacman;
	SDL_QueryTexture(PacMan,NULL,NULL,&pacman.w,&pacman.h);
	pacman.w=SCALE;
	pacman.h=SCALE;
	pacman.x=WIDTH/2-SCALE/2;
	pacman.y=HEIGHT/2+SCALE/2;

	SDL_Rect bg;
	SDL_QueryTexture(backgroundtexture, NULL, NULL, &bg.w, &bg.h);
	bg.x=0;
	bg.y=0;

	struct sprite player;
	player.x=WIDTH/2-SCALE/2;
	player.y=HEIGHT/2+SCALE/2;
	player.xspeed=SPEED;
	player.yspeed=SPEED;

	//Checks for errors in initialisation and creation of the window, and prints the error
	if (window == NULL)
		printf("Window could not be created : %s\n", SDL_GetError());

	if (background==NULL)
		printf("Background could not be initialized properly: %s\n", SDL_GetError());

	if (PacMan==NULL)
		printf("You picked the wrong image you dolt\n");

	//creates a window event object for mouse movements, button clicks, etc
	SDL_Event event;

	bool playing=1;
	Uint32 code;
	
	while(playing)
	{
		if (SDL_PollEvent(&event))
		{
			//checking if the user decides to click on the quit button
			if (SDL_QUIT == event.type)
				playing=0;
			else if (event.type == SDL_KEYDOWN)
			{
				code=event.key.keysym.sym;
				startplayer(code);
			}
			else if (event.type==SDL_KEYUP)
			{
				code=event.key.keysym.sym;
				stopplayer(code);
			}
		}

		//Moves the player
		player=move(player);
		pacman.x=player.x;
		pacman.y=player.y;
		SDL_RenderClear(renderer);
		updatescreen(renderer,backgroundtexture,PacManTexture,&pacman);
		//drawrectangle(renderer, player.x, player.y, SCALE, SCALE);
		//drawcircle(renderer,10,10,10);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
	}

	SDL_DestroyWindow(window);
	SDL_Quit;

	return 0;
}

//creation of a surface
	/*SDL_Surface* surface;
	surface = surface = IMG_Load("white_block.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
			
	//to get control over the position of the image
	SDL_Rect dest;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	dest.x = WIDTH/2;
	dest.y = HEIGHT/2;
	*/
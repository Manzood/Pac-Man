#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_timer.h>
#include<stdbool.h>

#define WIDTH 570
#define HEIGHT 630
#define SPEED 3
#define SCALE 30

//global declaration of the player x and y coordinates and speeds
//along with the global declaration of other seemingly necessary variables, of course
//int xpos=WIDTH/2, ypos=HEIGHT/2, xspeed=10, yspeed=10;
int FPS = 60;
bool up=false, down=false, left=false, right=false;

struct sprite
{
	int x;
	int y;
	int xspeed;
	int yspeed;
};

//zero is stop... one is go. Also, the zero in the 8th line (second line above halfway point) should be coloured orange
int grid[17][19]={
	{1,1,1,1,1,1,1,1, 0 ,1,1,1,1,1,1,1,1},
	{1,0,0,1,0,0,0,1, 0 ,1,0,0,1,0,0,0,1},
	{1,1,1,1,1,1,1,1, 1 ,1,1,1,1,1,1,1,1},
	{1,0,0,1,0,1,0,0, 0 ,1,0,0,1,0,1,0,0},
	{1,1,1,1,0,1,1,1, 0 ,1,1,1,1,0,1,1,1},
	{0,0,0,1,0,0,0,1, 0 ,0,0,0,1,0,0,0,1},
	{1,1,0,1,0,1,1,1, 1 ,1,1,0,1,0,1,1,1},
	{0,0,0,1,0,1,0,0, 0 ,0,0,0,1,0,1,0,0},
	{1,1,1,1,1,1,0,1, 1 ,1,1,1,1,1,1,0,1},
	
	{0,0,0,1,0,1,0,0, 0 ,0,0,0,1,0,1,0,0},

	{1,1,0,1,0,1,1,1, 1 ,1,1,0,1,0,1,1,1},
	{0,0,0,1,0,1,0,0, 0 ,0,0,0,1,0,1,0,0},
	{1,1,1,1,1,1,1,1, 0 ,1,1,1,1,1,1,1,1},
	{1,0,0,1,0,0,0,1, 0 ,1,0,0,1,0,0,0,1},
	{1,1,0,1,1,1,1,1, 1 ,1,1,0,1,1,1,1,1},
	{0,1,0,1,0,1,0,0, 0 ,0,1,0,1,0,1,0,0},
	{1,1,1,1,0,1,1,1, 0 ,1,1,1,1,0,1,1,1},
	{1,0,0,0,0,0,0,1, 0 ,1,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1, 1 ,1,1,1,1,1,1,1,1}
};

//basic initialisation function for SDL
void init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING)!=0)
		printf("Error initialising SDL = %s\n", SDL_GetError());
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

void updatescreen(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderPresent(renderer);
}

//function to make drawing multiple rectangles in one page easier: mimics the functions to draw rectangles in pygame
void drawrectangle(SDL_Renderer* renderer, int x, int y, int w, int h)
{
	SDL_Rect rect;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &rect);
}

//function that calculates the movement of the player
struct sprite move(struct sprite s)
{
	if (up)
	{
		if (s.y-s.yspeed>0)
			s.y-=s.yspeed;
		else
			s.y=0;
	}
	if (down)
	{
		if (s.y+s.yspeed<HEIGHT-30)
			s.y+=s.yspeed;
		else
			s.y=HEIGHT-30;
	}
	if (right)
	{
		if (s.x+s.xspeed<WIDTH-30)
			s.x+=s.xspeed;
		else
			s.x=WIDTH-30;
	}
	if (left)
	{
		if (s.x-s.xspeed>0)
			s.x-=s.xspeed;
		else
			s.x=0;
	}

	return s;
}

//function that starts player movement when the key is pressed
void startplayer(Uint32 code)
{
	if (code == SDLK_UP)
		up=true;
	if (code == SDLK_DOWN)
		down=true;
	if (code == SDLK_LEFT)
		left=true;
	if (code == SDLK_RIGHT)
		right=true;
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

void drawend(int pos, int angle)
{

}

void drawgrid(int grid[17][19])
{
	for (int i=0;i<17;i++)
	{
		for (int j=0;j<19;j++)
		{

		}
	}
}

int main()
{
	//function that initialises everything in the SDL library
	init();

	//Creation of a window and setting up of the renderer with a default colour
	SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetWindowFullscreen(window, 0);
	SDL_RenderPresent(renderer);

	struct sprite player;
	player.x=WIDTH/2;
	player.y=HEIGHT/2;
	player.xspeed=SPEED;
	player.yspeed=SPEED;

	//Checks for errors in initialisation and creation of the window, and prints the error
	if (window == NULL)
		printf("Window could not be created : %s\n", SDL_GetError());

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

		player=move(player);
		SDL_RenderClear(renderer); 
		drawrectangle(renderer, player.x, player.y, 30, 30);
		updatescreen(renderer);
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
#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_timer.h>
#include<SDL2/SDL_ttf.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>

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
int FPS = 90;
bool up=false, down=false, left=false, right=false;
char *pacmanimage = "PacManOpen-right.png";
int imagetoggle=0; //0=closed,1=mid-range open mouth and 2=fully open mouth
int previoustoggle=0;
int score=0;
int pelletcount=0;
SDL_Color White = {255, 255, 255};

//So these three variables determine the logic behind which direction Pac-Man will turn
//In the original game, the direction you entered was saved so that the second you hit a dead end Pac-Man would change direction
//That added to the tactical element of the game, and made it feel very premium
//This is an attempt to recreate that
//The variable direction just stores the current direction Pac-Man is headed, while nextdirection stores the next input given
//If the next direction and current direction complement each other, direction changes immediately.
//Otherwise, direction changes only after Pac-Man encounters a dead end
//the third variable, "s.teleporting", checks if pac man is, in fact, s.teleporting, and therefore prompts the behaviour in that situation

int direction=0;
int nextdirection=0;
int enemydirection=DIRECTIONLEFT;
int nextenemydirection=0;
int level=1;
int highscore=0;

struct sprite
{
	int x;
	int y;
	int xspeed;
	int yspeed;
	bool teleporting;
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

int pellets[42][38];

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

void updatescreen(SDL_Renderer* renderer, SDL_Texture *backgroundtexture, SDL_Texture *PacMan, SDL_Texture *BlinkyTexture, SDL_Rect *pacman,SDL_Rect *bg, SDL_Rect *blinkyrect)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderCopy(renderer,backgroundtexture,NULL,bg);
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

struct sprite initialiseblinky()
{
	struct sprite blinky;
	blinky.x=WIDTH/2-15;
	blinky.y=HEIGHT/2-75;
	blinky.xspeed=SPEED-1;
	blinky.yspeed=SPEED-1;
	blinky.teleporting=false;
	return blinky;
}

struct sprite initialisepinky () 
{
	struct sprite pinky;
	//pinky.x=
}

//function that controls the movement of pacman, and maybe the ghosts as well (it can be pretty generalized)
//my coup de grace, pretty much
struct sprite move (struct sprite s) 
{
	int gridx=s.x/30;
	int gridy=(s.y-30)/30;
	//printf("%d %d\n",gridy,gridx);
	
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
				if (s.x%30==0 && !(gridy==9&&(gridx<4||gridx>14)))
				{
					if(grid[gridy-1][gridx]==1)
						direction=nextdirection;
				}
			}
			else
			{
				if (s.x%30==0 && !(gridy==9&&(gridx<4||gridx>14)))
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
		if (s.teleporting==false)
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
					s.teleporting=true;
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
				s.teleporting=false;
			}
			else
				s.x-=s.xspeed;
		}
	}
	if (direction==DIRECTIONRIGHT)
	{
		if (s.teleporting==false) 
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
					s.teleporting=true;
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
				s.teleporting=false;
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

int choosedirectionforenemy()
{
	int randomnumbergenerated=rand()%3;
	while (randomnumbergenerated==0)
	{
		randomnumbergenerated=(rand()%2)+1;
	}
	int multiplier=rand()%2;
	if (multiplier)
		return randomnumbergenerated;
	else
		return randomnumbergenerated*(-1);
}

struct sprite moveenemy (struct sprite enemy)
{
	int gridx=enemy.x/30;
	int gridy=(enemy.y-30)/30;

	if (enemydirection==nextenemydirection || nextenemydirection==0)
	{
		nextenemydirection=choosedirectionforenemy();
	}

	//following is untested as of yet

	if (nextenemydirection==DIRECTIONUP||nextenemydirection==DIRECTIONDOWN)
	{
		if (nextenemydirection==DIRECTIONUP)
		{
			if (enemy.x%30==0 && !(gridy==9&&(gridx<4||gridx>14)))
			{
				if(grid[gridy-1][gridx]==1)
					enemydirection=nextenemydirection;
			}
		}
		else
		{
			if (enemy.x%30==0 && !(gridy==9&&(gridx<4||gridx>14)))
				if (grid[gridy+1][gridx]==1)
					enemydirection=nextenemydirection;
		}
	}
	else if (nextenemydirection==DIRECTIONLEFT||nextenemydirection==DIRECTIONRIGHT)
	{
		if (nextenemydirection==DIRECTIONLEFT)
		{
			if (enemy.y%30==0)
			{
				if (grid[gridy][gridx-1]==1)
					enemydirection=nextenemydirection;
			}
		}
		else
		{
			if (enemy.y%30==0)
			{
				if (grid[gridy][gridx+1]==1)
					enemydirection=nextenemydirection;
			}
		}
	}


	//works beyond this point

	if (enemydirection==DIRECTIONUP)
	{
		if (grid[gridy][gridx]==1 && enemy.y%30>=enemy.yspeed)
		{
			enemy.y-=enemy.yspeed;
		}
		else if (grid[gridy][gridx]==1 && enemy.y%30>0)
		{
			enemy.y-=enemy.y%30;
		}
		else
		{
			if (grid[gridy-1][gridx]==0) {
				enemydirection=nextenemydirection;
			}
			else
				enemy.y-=enemy.yspeed;
		}
	}
	if (enemydirection==DIRECTIONLEFT)
	{
		if (enemy.teleporting==false)
		{
			if (grid[gridy][gridx]==1 && enemy.x%30>=enemy.xspeed)
			{
				enemy.x-=enemy.xspeed;
			}
			else if (grid[gridy][gridx]==1 && enemy.x%30>0)
			{
				enemy.x-=enemy.x%30;
			}
			else
			{
				if (gridx==0)
				{
					enemy.teleporting=true;
				}
				else if (grid[gridy][gridx-1]==0)
					enemydirection=nextenemydirection;
				else
					enemy.x-=enemy.xspeed;
			}
		}
		else
		{
			if(enemy.x==-30)
			{
				enemy.x=WIDTH;
				enemy.teleporting=false;
			}
			else
				enemy.x-=enemy.xspeed;
		}
	}
	if (enemydirection==DIRECTIONRIGHT)
	{
		if (enemy.teleporting==false) 
		{
			if (grid[gridy][gridx+1]==1 && enemy.x%30+enemy.xspeed<=30)
			{
				enemy.x+=enemy.xspeed;
			}
			else if (grid[gridy][gridx+1]==1 && enemy.x%30+enemy.xspeed>30)
			{
				enemy.x+=30-enemy.x%30;
			}
			else
			{
				if (gridx==18||gridx==19)
					enemy.teleporting=true;
				else if (grid[gridy][gridx+1]==0)
					enemydirection=nextenemydirection;
				else
				 	enemy.x+=enemy.xspeed;
			}
		}
		else
		{
			if (enemy.x==WIDTH+30)
			{
				enemy.x=-30;
				enemy.teleporting=false;
			}
			else
				enemy.x+=enemy.xspeed;
		}
	}
	if (enemydirection==DIRECTIONDOWN)
	{
		if (grid[gridy+1][gridx]==1 && enemy.y%30+enemy.yspeed<=30)
		{
			enemy.y+=enemy.yspeed;
		}
		else if (grid[gridy+1][gridx]==1 && enemy.y%30+enemy.yspeed>30)
		{
			enemy.y+=30-enemy.y%30;
		}
		else
		{
			if (grid[gridy+1][gridx]==0)
				enemydirection=nextenemydirection;
			else
				enemy.y+=enemy.yspeed;
		}
	}

	return enemy;
}

int checkcollisionwithenemy (struct sprite enemy, struct sprite pacman)
{
	int distancex=enemy.x-pacman.x;
	distancex=distancex*distancex;
	int distancey=enemy.y-pacman.y;
	distancey=distancey*distancey;
	if (distancex+distancey<=900)
		return 1;
	else
		return 0;
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

const char* changeimage(Uint32 *starttime)
{
	unsigned int currenttime=SDL_GetTicks();
	if (currenttime-*starttime>60)
	{
		*starttime=currenttime;
		if (imagetoggle==0||imagetoggle==2)
		{
			previoustoggle=imagetoggle;
			imagetoggle=1;
			if (direction==DIRECTIONRIGHT)
				return "PacManMid-right.png";
			else if (direction==DIRECTIONLEFT)
				return "PacManMid-left.png";
			else if (direction==DIRECTIONUP)
				return "PacManMid-up.png";
			else if (direction==DIRECTIONDOWN)
				return "PacManMid-down.png";
		}
		else 
		{
			if (previoustoggle==0)
			{
				previoustoggle=imagetoggle;
				imagetoggle=2;
				if (direction==DIRECTIONRIGHT)
					return "PacManOpen-right.png";
				else if (direction==DIRECTIONLEFT)
					return "PacManOpen-left.png";
				else if (direction==DIRECTIONUP)
					return "PacManOpen-up.png";
				else if (direction==DIRECTIONDOWN)
					return "PacManOpen-down.png";
			}
			else
			{
				previoustoggle=imagetoggle;
				imagetoggle=0;
				return "PacManClosed.png";
			}
		}
	}
	return pacmanimage;
}

void calculateinitialpellets ()
{
	for (int i=0;i<21;i++)
	{
		for (int j=0;j<19;j++)
		{
			if (grid[i][j]==0||(i>=6&&i<=14)&&(j>=5&&j<=13)||((i>=7&&i<13)&&(j<4||j>14)))
			{
				pellets[2*i][2*j]=0;
				pellets[2*i+1][2*j]=0;
				pellets[2*i][2*j+1]=0;
				pellets[2*i+1][2*j+1]=0;
			}
			else
			{
				if (grid[i-1][j]==0||grid[i][j-1]==0||grid[i-1][j-1]==0)
					pellets[2*i][2*j]=0;
				else
				{
					pelletcount++;
					pellets[2*i][2*j]=1;
				}
				if (grid[i][j-1]==0)
					pellets[2*i+1][2*j]==0;
				else
				{
					pelletcount++;
					pellets[2*i+1][2*j]=1;
				}
				if (grid[i-1][j]==0)
					pellets[2*i][2*j+1]=0;
				else
				{
					pelletcount++;
					pellets[2*i][2*j+1]=1;
				}
				pelletcount++;
				pellets[2*i+1][2*j+1]=1;
			}
		}
	}
	pellets[30][17]=0;
	pellets[30][21]=0;
	pellets[27][28]=0;
	pellets[19][28]=0;
	pellets[31][18]=0;
	pellets[31][19]=0;
	pellets[31][20]=0;
	pellets[19][8]=0;
	pelletcount-=8;
	//printf("%d\n",pellets[3][34]);
}

void printpelletgrid ()
{
	for (int i=0;i<42;i++)
	{
		for (int j=0;j<38;j++)
			printf("%d ",pellets[i][j]);
		printf("\n");
	}
}

int animationdirection=1;
int largepelletsize=8;

void drawpellets(SDL_Renderer *renderer, Uint32 *animationstarttime)
{
	unsigned int currenttime=SDL_GetTicks();
	for (int i=0;i<42;i++)
	{
		for (int j=0;j<38;j++)
		{
			if (pellets[i][j]==1)
			{
				//printf("%d %d\n",20*i,20*j);
				SDL_Rect rect;
				if ((i==5&&j==3)||(i==5&&j==35)||(i==37&&j==3)||(i==37&&j==35))
				{
					if (currenttime-*animationstarttime>=120)
					{
						if (largepelletsize==9||largepelletsize==7)
						{
							if (animationdirection==1)
								animationdirection=-1;
							else
								animationdirection=1;
						}
						largepelletsize+=animationdirection;
						*animationstarttime=currenttime;
					}
					rect.x=15*j-2;
					rect.y=15*i-2+30;
					rect.w=largepelletsize;
					rect.h=largepelletsize;
				}
				else
				{
					rect.x=15*j;
					rect.y=15*i+30;
					rect.w=4;
					rect.h=4;
				}

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(renderer, &rect);
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

void checkcollisionwithpellets (struct sprite s) 
{
	int pacmanx=s.x/15;
	int pacmany=(s.y-30)/15;
	pacmanx++;
	pacmany++;
	int scoretobeadded=10;
	if (pacmany==5)
	{
		if (pacmanx==3||pacmanx==35)
			scoretobeadded=50;
	}
	else if (pacmany==37)
	{
		if (pacmanx==3||pacmanx==35)
			scoretobeadded=50;
	}
	//printf("%d %d %d\n",pacmanx,pacmany, pellets[pacmany][pacmanx]);
	if (direction==DIRECTIONDOWN)
	{
		if (pellets[pacmany][pacmanx]==1)
		{
			//printf("Made it here\n");
			pellets[pacmany][pacmanx]=0;
			score+=scoretobeadded;
			pelletcount--;
		}
	}
	else if (direction==DIRECTIONUP)
	{
		if (pellets[pacmany][pacmanx]==1)
		{
			pellets[pacmany][pacmanx]=0;
			score+=scoretobeadded;
			pelletcount--;
		}
	}
	else if (direction==DIRECTIONRIGHT)
	{
		if (pellets[pacmany][pacmanx]==1)
		{
			pellets[pacmany][pacmanx]=0;
			score+=scoretobeadded;
			pelletcount--;
		}
	}
	else if (direction==DIRECTIONLEFT)
	{
		if (pellets[pacmany][pacmanx]==1)
		{
			pellets[pacmany][pacmanx]=0;
			score+=scoretobeadded;
			pelletcount--;
		}
	}
}

void displayscoreboard(TTF_Font* Sans,SDL_Renderer *renderer)
{
	//Makes a surface and texture to display said score
	char scorestr[5];
	sprintf(scorestr,"%d",score);
	SDL_Surface* messagesurface = TTF_RenderText_Solid(Sans,scorestr,White);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer,messagesurface);

	SDL_Rect messagerect;
	messagerect.x=78;
	messagerect.y=3;
	messagerect.h=24;
	if (score==0)
		messagerect.w=15;
	else if (score<10)
		messagerect.w=20;
	else if (score<100)
		messagerect.w=30;
	else if (score<1000)
		messagerect.w=40;
	else if (score<10000)
		messagerect.w=50;
	else
		messagerect.w=60;

	SDL_RenderCopy(renderer,Message,NULL,&messagerect);

	SDL_FreeSurface(messagesurface);
}






int main()
{
	//function that initialises everything in the SDL library
	if (init()==-1)
		return -1;
	if (TTF_Init()<0)
		return -1;
	//printf("Made it here\n");

	//initialises the random function seed with the system time
	srand(time(0));

	//initialises the window and the renderer
	SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT+30, SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);
	
	//initialises the image for the background
	SDL_Surface *background=IMG_Load("pacman_grid.png");
	SDL_Texture *backgroundtexture = SDL_CreateTextureFromSurface(renderer, background);
	SDL_FreeSurface(background);
	
	//this makes the window stop being fullscreen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetWindowFullscreen(window, 0);
	SDL_RenderPresent(renderer);
	
	//Loads up Pac Man, and sets his initial position and size
	SDL_Surface *PacMan=IMG_Load(pacmanimage);
	SDL_Texture *PacManTexture = SDL_CreateTextureFromSurface(renderer,PacMan);
	SDL_Rect pacman;
	SDL_QueryTexture(PacMan,NULL,NULL,&pacman.w,&pacman.h);
	pacman.w=SCALE;
	pacman.h=SCALE;
	//this rectangle could have had its x and y coordinates set over here, but they will be assigned to the x and y coordinates of the structure player.

	//loads up blinky, and sets up his initial position and size
	SDL_Surface *BlinkySurface=IMG_Load("blinky.png");
	SDL_Texture *BlinkyTexture = SDL_CreateTextureFromSurface(renderer,BlinkySurface);
	SDL_Rect blinkyrect;
	SDL_QueryTexture(BlinkySurface,NULL,NULL,&blinkyrect.w,&blinkyrect.h);
	blinkyrect.w=SCALE;
	blinkyrect.h=SCALE;

	//initialises the background, and sets the whole rectangle for the background image
	SDL_Rect bg;
	SDL_QueryTexture(backgroundtexture, NULL, NULL, &bg.w, &bg.h);
	bg.w=WIDTH;
	bg.h=HEIGHT;
	bg.x=0;
	bg.y=30;

	//initialises the player completely
	struct sprite player;
	player.x=WIDTH/2-SCALE/2;
	player.y=HEIGHT/2+SCALE/2+150;
	player.xspeed=SPEED;
	player.yspeed=SPEED;
	player.teleporting=false;

	struct sprite blinky;
	blinky=initialiseblinky();
	//printf("%d %d\n",blinky.x,blinky.y);

	//initialises the enemies completely


	//Loads up the font that will be used to display the score
	TTF_Font* font = TTF_OpenFont("sans.ttf", 24);
	SDL_Surface* messagesurface = TTF_RenderText_Solid(font,"Score:",White);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer,messagesurface);

	SDL_Rect messagerect;
	messagerect.x=8;
	messagerect.y=3;
	messagerect.h=24;
	messagerect.w=70;

	SDL_RenderCopy(renderer,Message,NULL,&messagerect);
	SDL_FreeSurface(messagesurface);

	//Checks for errors in initialisation and creation of the window, the background, and the image for PacMan itself and prints the error
	if (window == NULL)
		printf("Window could not be created : %s\n", SDL_GetError());

	if (background==NULL)
		printf("Background could not be initialized properly: %s\n", SDL_GetError());

	if (PacMan==NULL)
		printf("The image for Pac Man did not initialise properly\n");

	//creates a window event object for mouse movements, button clicks, etc
	SDL_Event event;

	//Some important variables that need to be initialised here;
	bool playing=1;
	Uint32 code;
	int temp=0;
	int temp2=0;
	Uint32 *starttime=&temp;
	Uint32 *pelletstarttime=&temp2;

	calculateinitialpellets();
	//printpelletgrid();
	
	// int previousticks=0;
	// int ticks;

	// Ask SDL for the time in milliseconds

	while(playing)
	{
		/*ticks=SDL_GetTicks();
		if (ticks-previousticks>=15) 
		{
			previousticks=ticks;
*/
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

		if (pelletcount==0) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			calculateinitialpellets();
			SDL_RenderCopy(renderer,backgroundtexture,NULL,&bg);
			SDL_RenderPresent(renderer);
			level++;
			player.x=WIDTH/2-SCALE/2;
			player.y=HEIGHT/2+SCALE/2+150;
			blinky=initialiseblinky();
			SDL_Delay(1500);
		}

		if (checkcollisionwithenemy(blinky,player))
		{
			SDL_Delay(1500);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			calculateinitialpellets();
			SDL_RenderCopy(renderer,backgroundtexture,NULL,&bg);
			SDL_RenderPresent(renderer);
			level++;
			player.x=WIDTH/2-SCALE/2;
			player.y=HEIGHT/2+SCALE/2+150;
			blinky=initialiseblinky();
		}

		//the following is responsible for the animation of Pac Man
		pacmanimage=changeimage(starttime);

		SDL_Surface *PacMan=IMG_Load(pacmanimage);
		SDL_Texture *PacManTexture = SDL_CreateTextureFromSurface(renderer,PacMan);
		SDL_Rect pacman;
		SDL_QueryTexture(PacMan,NULL,NULL,&pacman.w,&pacman.h);

		pacman.w=SCALE;
		pacman.h=SCALE;

		//Moves the player
		player=move(player);
		pacman.x=player.x;
		pacman.y=player.y;

		blinky=moveenemy(blinky);
		blinkyrect.x=blinky.x;
		blinkyrect.y=blinky.y;
		//printf("blinky.x=%d\t blinky.y=%d\n",blinky.x,blinky.y);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		
		//All actual displaying and rendering happens in this final bit, after all calculations have been completed
		updatescreen(renderer,backgroundtexture,PacManTexture,BlinkyTexture,&pacman,&bg,&blinkyrect);
		checkcollisionwithpellets(player);
		//printpelletgrid();
		drawpellets(renderer,pelletstarttime);
		SDL_RenderCopy(renderer,BlinkyTexture,NULL,&blinkyrect);
		//printf("%d\n",score);

		if (font==NULL) 
		{
			printf("Failed to load font\n");
			printf("Error is: %s\n",TTF_GetError());
		}
		else
		{
			SDL_RenderCopy(renderer,Message,NULL,&messagerect);
			displayscoreboard(font,renderer);
		}
		//drawrectangle(renderer, blinky.x, blinky.y, SCALE, SCALE);
		SDL_RenderPresent(renderer);

		//So apparently SDL_Delay is very inefficient and slow, so I should instead be making my own function for smoother movement
		SDL_Delay(1000 / FPS);
	}

	SDL_FreeSurface(messagesurface);
	SDL_DestroyWindow(window);
	SDL_Quit;

	return 0;
}
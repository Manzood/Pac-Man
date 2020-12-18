#OBJS specif.ies which files to compile as part of the project
OBJS = Pac-Man-v2.c

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = PacMan

#This is the target that compiles our executable
all : $(OBJS)
	gcc $(OBJS) -w -static-libgcc -lSDL2 -lSDL_image -lSDL2_ttf -o $(OBJ_NAME)

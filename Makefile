#OBJS specif.ies which files to compile as part of the project
OBJS = Game_v1.c

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Game_v1

#This is the target that compiles our executable
all : $(OBJS)
	gcc $(OBJS) -w -lSDL2 -lSDL2_image -o $(OBJ_NAME)
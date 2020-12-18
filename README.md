#Pac-Man
Can be run using the executable file provided.
However, this executable file had been built on a linux Ubuntu distribution, so compatibility with your system will vary.
Compilation in C can be done if the SDL2 library is installed, which can bewith further ease using the Makefile provided.
The remaining assets, used or unused, are also provided in this repository.

There are various tutorials for installing SDL2 on your computer, which will be required to run the code in this project
The main website for SDL2 is linked here: 
 https://www.libsdl.org/download-2.0.php
On linux, a simple look at this tutorial could help with installation:      https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php

Also, it is imperative that the code be downloaded alongside all the other images and other assets provided, without which it is needless to say that the final build will not run properly.
The names of the images and files should also not be changed; this will result in errors.

The images used in this project were found from various sources on the internet, and edited to suit the project.

SDL_ttf is a library that I had insttalled on linux using the command: "sudo apt-get install libsdl2-ttf.dev"
That's all that was required on linux

For Windows users, they seem to have a much better instructions on the main SDL-ttf website:
  https://www.libsdl.org/projects/SDL_ttf/
Also, a tutorial on SDL_ttf can be found here: 
  http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/

Command to install the SDL2_image library:
    ```sudo apt-get install libsdl-image1.2-dev```

Update:
    SDL2 seems to have been updated since this project was made, so it may not work with an install of the latest binaries.
    In the future, I might port it and make sure everything works, but at the moment, the project will not commpile

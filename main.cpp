#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define WIND_WIDTH 640 
#define WIND_HEIGHT 320 

void init();
void initGL();
void close();
void input();
void render();

SDL_Window *window = NULL;
SDL_GLContext glContext = NULL;
GLubyte glBuffer[WIND_HEIGHT][WIND_WIDTH][3];
int quit = 0;

int main() {
   init();

   do {
      render();
      input();
   } while (!quit);

   close();
   return 0;
}

void init() {
   srand(time(NULL));
   SDL_Init(SDL_INIT_VIDEO);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
   window = SDL_CreateWindow("CHIP-8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
         WIND_WIDTH, WIND_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
   initGL();
}

void initGL() {
   glContext = SDL_GL_CreateContext(window);
   SDL_GL_SetSwapInterval(0);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glShadeModel(GL_FLAT);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void close() {
   SDL_DestroyWindow(window);
   window = NULL;
   SDL_Quit();
}

void input() {
   SDL_Event e;
   while (SDL_PollEvent(&e) != 0){
      if (e.type == SDL_QUIT){
         quit = 1;
      }
   }
}

void render() {
   int col, row;
   for (row = 0; row < WIND_HEIGHT; row++){
      for (col = 0; col < WIND_WIDTH; col++){
         glBuffer[row][col][0] = rand();
         glBuffer[row][col][1] = rand(); 
         glBuffer[row][col][2] = rand(); 
      }
   }

   /* refresh window buffer */
   glClear(GL_COLOR_BUFFER_BIT);
   glDrawPixels(WIND_WIDTH, WIND_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, glBuffer);
   SDL_GL_SwapWindow(window);
}

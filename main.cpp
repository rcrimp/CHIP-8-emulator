#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "chip8.h"

#define WIND_WIDTH 640 
#define WIND_HEIGHT 320 

void init();
void initGL();
void close();
void input();
void render();

SDL_Window *window = NULL;
SDL_GLContext glContext = NULL;
GLubyte glBuffer[WIND_HEIGHT][WIND_WIDTH];
bool quit = false;

int main() {
   init();

   chip8_init();
   if(!chip8_load_rom("roms/Pong [Paul Vervalin, 1990].ch8"))
      goto done;

   do {
      chip8_cycle();
      if (chip8_refresh_screen())
         render();
      input();
   } while (!quit);

done:
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

const SDL_Keycode key_map[16] = {
   SDLK_x, SDLK_1, SDLK_2, SDLK_3, 
   SDLK_q, SDLK_w, SDLK_e, SDLK_a, 
   SDLK_s, SDLK_d, SDLK_z, SDLK_c, 
   SDLK_4, SDLK_r, SDLK_f, SDLK_v 
};

void input() {
   SDL_Event e;
   while (SDL_PollEvent(&e) != 0){
      if (e.type == SDL_QUIT){
         quit = true;
      } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
         key_state_t ks = (e.type == SDL_KEYDOWN) ? KEY_DOWN : KEY_UP; 
         for (int i = 0; i < 16; i++){
            if (e.key.keysym.sym == key_map[i])
               chip8_input(i, ks);
         }
      }
   }
}

void render() {
   int col, row, i = 0, scale = 10;
   for (row = 0; row < WIND_HEIGHT; row++){
      for (col = 0; col < WIND_WIDTH; col++){
         glBuffer[WIND_HEIGHT - 1 - row][col] = chip8_screen[i] * 255;
         if (col % scale == 0 && col != 0)
            i++;
      }
      i++;
      if (row % scale != 0 || row == 0)
         i -= 64;
   }

   /* refresh window buffer */
   glClear(GL_COLOR_BUFFER_BIT);
   glDrawPixels(WIND_WIDTH, WIND_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, glBuffer);
   SDL_GL_SwapWindow(window);
}

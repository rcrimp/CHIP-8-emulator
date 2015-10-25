#ifndef chip8_h_
#define chip8_h_

#include <stdint.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef enum key_state {KEY_UP, KEY_DOWN} key_state_t;

extern uint8_t chip8_screen[SCREEN_WIDTH * SCREEN_HEIGHT];
extern key_state chip8_key[16];

extern void chip8_init();
extern bool chip8_load_rom(const char* filename);
extern void chip8_cycle();
extern void chip8_input(uint8_t key_code, key_state_t state);
extern bool chip8_refresh_screen();

#endif

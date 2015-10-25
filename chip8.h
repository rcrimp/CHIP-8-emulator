#ifndef chip8_h_
#define chip8_h_

#include <stdint.h>

typedef enum {KEY_UP, KEY_DOWN} key_state;

extern void chip8_init();
extern bool chip8_load_rom(const char*);
extern void chip8_cycle();
extern void chip8_key(uint8_t, key_state);

#endif

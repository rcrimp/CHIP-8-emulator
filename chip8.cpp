#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint8_t SCREEN_WIDTH = 64;
const uint8_t SCREEN_HEIGHT = 32;

const uint8_t font_data[80] = {
   0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
   0x20, 0x60, 0x20, 0x20, 0x70, // 1
   0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
   0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
   0x90, 0x90, 0xF0, 0x10, 0x10, // 4
   0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
   0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
   0xF0, 0x10, 0x20, 0x40, 0x40, // 7
   0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
   0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
   0xF0, 0x90, 0xF0, 0x90, 0x90, // A
   0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
   0xF0, 0x80, 0x80, 0x80, 0xF0, // C
   0xE0, 0x90, 0x90, 0x90, 0xE0, // D
   0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
   0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/* emulator state */
uint8_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t memory[4096]; /* 4KB of memory */
uint8_t reg[16]; /* 16 8bit registers */
uint8_t key[16]; /* 16 input keys */
uint16_t stack[16]; /* stack */
uint8_t dt, st; /* delay timer, sound timer */
uint16_t op, pc, sp, ind; /* 16 bit registers */

inline void clear_screen() {
   memset(screen, 0, sizeof screen);
}

void chip8_init() {
   pc = 0x200;
   op = 0;
   sp = 0;
   dt = 0;
   st = 0;
   ind = 0;

   memset(memory, 0, sizeof memory);
   memset(reg, 0, sizeof reg);
   memset(stack, 0, sizeof stack);
   memset(key, 0, sizeof key);
   memcpy(memory, font_data, sizeof font_data);
   clear_screen();
}

bool chip8_load_rom(const char* filename) {
   FILE *f = fopen(filename, "rb");
   if (f) {
      size_t n = fread(memory + 0x200, 1, 0xFFF - 0x200, f);
      fclose(f);
      fprintf(stderr, "read file %s\nfile size: %lu bytes\n", filename, n);
      return true;
   }
   fprintf(stderr, "unable to open file %s\n", filename);
   return false;
}

void print_byte(uint8_t b) {
   for (int i = 7; i >= 0; i--)
      fprintf(stderr, "%c", ((b>>i) & 1) ? '1' : '0');
   fprintf(stderr, "\n");
}

void chip8_cycle() {
   op = (memory[pc] << 8) + memory[pc + 1];   
   pc += 2;
   fprintf(stderr, "unknown instruction 0x%X\n", op);

   // update timers
   if (dt > 0)
      dt--;

   if (st > 0){
      fprintf(stderr, "beep\n");
      st--;
   }
}

void chip8_key(uint8_t key_code, key_state state) {
   key[key_code] = state;
}

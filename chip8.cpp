#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

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

/* hardware */
uint8_t chip8_screen[SCREEN_WIDTH * SCREEN_HEIGHT];
key_state chip8_key[16]; /* 16 input keys */

/* CPU state */
uint8_t memory[4096]; /* 4KB of memory */
uint8_t reg[16]; /* 16 8bit registers */
uint16_t stack[16]; /* stack */
uint8_t dt, st; /* delay timer, sound timer */
uint16_t op, pc, sp, ind; /* 16 bit registers */

inline void clear_screen() {
   memset(chip8_screen, 0, sizeof chip8_screen);
}

void chip8_init() {
   srand(time(NULL));

   /* reset CPU state */
   pc = 0x200;
   op = 0;
   sp = 0;
   dt = 0;
   st = 0;
   ind = 0;
   memset(memory, 0, sizeof memory);
   memset(reg, 0, sizeof reg);
   memset(stack, 0, sizeof stack);
   memcpy(memory, font_data, sizeof font_data);

   /* reset hardware */
   memset(chip8_key, 0, sizeof chip8_key);
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
   uint8_t vx, vy;
   bool unknown_opcode = false;
   op = (memory[pc] << 8) + memory[pc + 1];   

   switch (op & 0xF000) {
      case 0x0000:
         switch (op){
            /* 0x00E0 : clear screen */
            case 0x00E0:
               clear_screen();
               pc += 2;
               break;
               /* 0x00EE : return from subroutine */
            case 0x00EE:
               pc = stack[--sp];
               break;
            default:
               unknown_opcode = true;
               pc += 2;
         }
         break;
         /* 0x1NNN : jump to address NNN */
      case 0x1000:
         pc = op & 0x0FFF;
         break;
         /* 0x2NNN : jump to subroutine at NNN */
      case 0x2000:
         stack[sp++] = pc + 2;
         pc = op & 0x0FFF;
         break;
         /* 0x3XNN : skip next instruction if VX == NN */
      case 0x3000:
         if (reg[(op & 0x0F00) >> 8] == (op & 0x00FF))
            pc += 4;
         else
            pc += 2; 
         break;
         /* 0x4XNN : skip next instruction if VX != NN */
      case 0x4000:
         if (reg[(op & 0x0F00) >> 8] != (op & 0x00FF))
            pc += 4;
         else
            pc += 2;
         break;
         /* 0x5XY0 : skip next instruction if VX == VY */
      case 0x5000:
         if (reg[(op & 0x0F00) >> 8] == reg[(op & 0x00F0) >> 4])
            pc += 4;
         else
            pc += 2;
         break;
         /* 0x6XNN : sets VX to NN */
      case 0x6000:
         reg[(op & 0x0F00) >> 8] = (op & 0x00FF);
         pc += 2;
         break;
         /* 0x7XNN : adds NN to VX */
      case 0x7000:
         reg[(op & 0x0F00) >> 8] += (op & 0x00FF); 
         pc += 2;
         break;
         /* arithmetic instructions */  
      case 0x8000:
         switch (op & 0x000F) {
            /* 0x8XY0 : sets VX to value of VY */
            case 0x0000:
               reg[(op & 0x0F00) >> 8] = reg[(op & 0x00F0) >> 4]; 
               pc += 2;
               break;
               /* 0x8XY1 : sets VX to VX or VY */
            case 0x0001:
               reg[(op & 0x0F00) >> 8] |= reg[(op & 0x00F0) >> 4];
               pc += 2;
               break;
               /* 0x8XY2 : sets VX to VX and VY */
            case 0x0002:
               reg[(op & 0x0F00) >> 8] &= reg[(op & 0x00F0) >> 4];
               pc += 2;
               break;
               /* 0x8XY3 : sets VX to VX xor VY */
            case 0x0003:
               reg[(op & 0x0F00) >> 8] ^= reg[(op & 0x00F0) >> 4];
               pc += 2;
               break;
               /* 0x8XY4 : adds VY to VX. VF is set to 1 when carry, 0 when not */
            case 0x0004:
               vx = reg[(op & 0x0F00) >> 8];
               vy = reg[(op & 0x00F0) >> 4];
               reg[(op & 0x0F00) >> 8] = vx + vy;  
               reg[0xF] = (vx > reg[(op & 0x0F00) >> 8]) ? 1 : 0; 
               pc += 2;
               break;
               /* 0x8XY5 : subtract VY from VX. VF is set to 1 when borrow, 0 when not */
            case 0x0005:
               vx = reg[(op & 0x0F00) >> 8];
               vy = reg[(op & 0x00F0) >> 4];
               reg[(op & 0x0F00) >> 8] = vx - vy;  
               reg[0xF] = (vx < reg[(op & 0x0F00) >> 8]) ? 1 : 0; 
               pc += 2;
               break;
               /* 0x8XY6 : shift VX right 1 bit. VF set to least significant bit of VX */
            case 0x0006:
               vx = reg[(op & 0x0F00) >> 8];
               reg[0xF] = vx & 1;
               reg[(op & 0x0F00) >> 8] = vx >> 1;
               pc += 2;
               break;
               /* 0x8XY7: sets VX to VY minus VX. VF is set to 0 when borrow, 1 when not */
            case 0x0007:
               vx = reg[(op & 0x0F00) >> 8];
               vy = reg[(op & 0x00F0) >> 4];
               reg[(op & 0x0F00) >> 8] = vy - vx;  
               reg[0xF] = (vy < reg[(op & 0x0F00) >> 8]) ? 1 : 0; 
               pc += 2;
               break;
               /* 0x8XYE : shift VX left 1 bit. VF set to most significant bit of VX */
            case 0x000E:
               vx = reg[(op & 0x0F00) >> 8];
               reg[0xF] = vx & 0x80;
               reg[(op & 0x0F00) >> 8] = vx << 1;
               pc += 2;
               break;
            default:
               unknown_opcode = true;
               pc += 2;
         }
         break;
         /* 0x9XY0 : skips next instruction if VX doesn't equal VY */
      case 0x9000:
         if (reg[(op & 0x0F00) >> 8] != reg[(op & 0x00F0) >> 4])
            pc += 4;
         else
            pc += 2;
         break;
         /* 0xANNN : sets index to the address NNN */
      case 0xA000:
         ind = op & 0x0FFF;
         pc += 2;
         break;
         /* 0xBNNN : jumps to address NNN plus register 0 */
      case 0xB0000:
         pc = (op & 0x0FFF) + reg[0];
         break;
         /* 0xCXNN : sets VX to bitwise-and of random number and NN */
      case 0xC000:
         reg[(op & 0x0F00) >> 8] = rand() & (op & 0x0FF); 
         pc += 2;
         break;
         /* DXYN : draw sprite at X, Y with height of N */
      case 0xD000:
         {
            uint8_t vx = reg[(op & 0x0F00) >> 8];
            uint8_t vy = reg[(op & 0x00F0) >> 4]; 
            uint8_t n = op & 0x000F;
            for (uint8_t y = 0; y < n; y++){
               uint8_t row = memory[ind + y];
               for (uint8_t x = 0; x < 8; x++){
                  chip8_screen[(vy + y) * SCREEN_WIDTH + (vx + x)] = (row >> x) & 1; 
               }
            }
         }
         pc += 2;
         break;
         /* input instructions */
      case 0xE000:
         switch (op & 0x00FF) {
            /* 0xEX9E : skips next instruction if key stored in VX is pressed */
            case 0x009E:
               if (chip8_key[(op & 0x0F00) >> 8])
                  pc += 4;
               else
                  pc += 2;
               break;
               /* 0xEXA1 : skips next instruction if key stored in VX isn't pressed */
            case 0x00A1:
               if (!chip8_key[(op & 0x0F00) >> 8])
                  pc += 4;
               else
                  pc += 2;
               break;

            default:
               unknown_opcode = true;
               pc += 2;
         }  
         break;
      case 0xF000:
         switch (op & 0x00FF){
            /* FX07 : sets VX to value of the delay timer */
            case 0x0007:
               reg[(op & 0x0F00) >> 8] = dt;
               pc += 2;
               break;
               /* FX0A : A key press is awaited, and then stored in VX */
               /* FX15 : sets the delay timer to VX */
            case 0x0015:
               dt = reg[(op & 0x0F00) >> 8];
               pc += 2;
               break;
               /* FX18 : sets the sound timer to VX */
            case 0x0018:
               st = reg[(op & 0x0F00) >> 8];
               pc += 2;
               break;
               /* FX1E : adds VX to index */
            case 0x001E:
               ind += reg[(op & 0x0F00) >> 8];
               pc += 2;
               break;
               /* FX29 : sets index to adress of font character VX */
            case 0x0029:
               ind = 5 * reg[(op & 0x0F00) >> 8]; 
               pc += 2;
               break;
               /* FX33 : stores BCD of VX into index, index+1, index+2.*/
            case 0x0033:
               vx = reg[(op & 0x0F00) >> 8];
               memory[ind] = vx / 100;
               memory[ind + 1] = (vx / 10) % 10;
               memory[ind + 2] = vx % 10;
               pc += 2;
               break;
               /* FX55 : stores v0 to VX in memory, stating at index */
            case 0x0055:
               vx = reg[(op & 0x0F00) >> 8];
               for (uint8_t i = 0; i <= vx; i++)
                  memory[ind + i] = reg[i];
               pc += 2;
               break;
               /* FX65 : fills v0 to VX with values from memory, starting at index */
            case 0x0065:
               vx = reg[(op & 0x0F00) >> 8];
               for (uint8_t i = 0; i <= vx; i++)
                  reg[i] = memory[ind + i];
               pc += 2;
               break;
            default:
               unknown_opcode = true;
               pc += 2;
         }
         break;
      default:
         unknown_opcode = true;
         pc += 2;
   }

   if (unknown_opcode){
      fprintf(stderr, "unknown instruction 0x%X\n", op);
   }

   /* update delay timer */
   if (dt > 0)
      dt--;

   /* update sound timer */
   if (st > 0){
      fprintf(stderr, "beep\n");
      st--;
   }
}

void chip8_change_key(uint8_t key_code, key_state_t state) {
   chip8_key[key_code] = state;
}

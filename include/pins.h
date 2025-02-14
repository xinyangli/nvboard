#ifndef _VFPGA_CONSTRS_H
#define _VFPGA_CONSTRS_H

#include <stdint.h>
#include <assert.h>

// RST is defined but not used. NVBoard contains some internal states in *.cpp
// file. Only resetting the RTL design in *.v file may make the RTL design and
// NVBoard inconsistent. The easy way to reset the whole system is to exit
// NVBoard and re-run. The reset work for RTL design is done by the verilator
// wrapper. See `example/csrc/main.cpp` for more details.
#define BTN_INPUT BTNC, BTNU, BTND, BTNL, BTNR, RST

#define SW_INPUT SW0, SW1, SW2, SW3, \
                 SW4, SW5, SW6, SW7, \
                 SW8, SW9, SW10, SW11, \
                 SW12, SW13, SW14, SW15

#define NAIVE_LEDS_OUTPUT LD0, LD1, LD2, LD3, \
                          LD4, LD5, LD6, LD7, \
                          LD8, LD9, LD10, LD11, \
                          LD12, LD13, LD14, LD15

#define RGB_LEDS_OUTPUT R16, G16, B16, R17, G17, B17


#define SEG7_SEGS_OUTPUT SEG0A, SEG0B, SEG0C, SEG0D, SEG0E, SEG0F, SEG0G, DEC0P, \
                         SEG1A, SEG1B, SEG1C, SEG1D, SEG1E, SEG1F, SEG1G, DEC1P, \
                         SEG2A, SEG2B, SEG2C, SEG2D, SEG2E, SEG2F, SEG2G, DEC2P, \
                         SEG3A, SEG3B, SEG3C, SEG3D, SEG3E, SEG3F, SEG3G, DEC3P, \
                         SEG4A, SEG4B, SEG4C, SEG4D, SEG4E, SEG4F, SEG4G, DEC4P, \
                         SEG5A, SEG5B, SEG5C, SEG5D, SEG5E, SEG5F, SEG5G, DEC5P, \
                         SEG6A, SEG6B, SEG6C, SEG6D, SEG6E, SEG6F, SEG6G, DEC6P, \
                         SEG7A, SEG7B, SEG7C, SEG7D, SEG7E, SEG7F, SEG7G, DEC7P

//#define UART_OUTPUT

#define VGA_OUTPUT VGA_VSYNC, VGA_HSYNC, VGA_BLANK_N,  \
                   VGA_R0, VGA_R1, VGA_R2, VGA_R3, VGA_R4, VGA_R5, VGA_R6, VGA_R7, \
                   VGA_G0, VGA_G1, VGA_G2, VGA_G3, VGA_G4, VGA_G5, VGA_G6, VGA_G7, \
                   VGA_B0, VGA_B1, VGA_B2, VGA_B3, VGA_B4, VGA_B5, VGA_B6, VGA_B7

#define KEYBOARD_INPUT PS2_CLK, PS2_DAT

enum {
  BTN_INPUT,
  SW_INPUT,
  KEYBOARD_INPUT,
  NR_INPUT_PINS,
  NAIVE_LEDS_OUTPUT,
  RGB_LEDS_OUTPUT,
  SEG7_ENBS_OUTPUT,
  SEG7_SEGS_OUTPUT,
  VGA_OUTPUT,
  NR_PINS,
  NR_OUTPUT_PINS = NR_PINS - NR_INPUT_PINS
};

typedef struct PinNode {
  void *ptr;
  uint8_t data;
  uint8_t vector_len;
  uint8_t bit_offset;
} PinNode;
extern PinNode pin_array[];

static inline bool is_input_pin(int pin) {
  return (pin < NR_INPUT_PINS);
}

static inline uint8_t pin_peek(int pin) {
  PinNode *p = &pin_array[pin];
  if (p->vector_len == 1) {
    return *(uint8_t *)p->ptr & 1;
  } else {
    uint64_t v = *(uint64_t *)p->ptr;
    return (v >> p->bit_offset) & 1;
  }
}

static inline uint8_t pin_peek8(int pin) {
  PinNode *p = &pin_array[pin];
  assert(p->vector_len == 8);
  return *(uint8_t *)p->ptr;
}

static inline void pin_poke(int pin, uint64_t v) {
  PinNode *p = &pin_array[pin];
  if (p->vector_len == 1) {
    *(uint8_t *)p->ptr = v & 1;
  } else {
    uint64_t x = *(uint64_t *)p->ptr;
    uint64_t mask = 1 << p->bit_offset;
    *(uint64_t *)p->ptr = (x & ~mask) | ((v & 1) << p->bit_offset);
  }
}

#endif

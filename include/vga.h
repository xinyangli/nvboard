#ifndef _VFPGA_VGA_H
#define _VFPGA_VGA_H

#include <component.h>
#include <SDL.h>

#define VGA_DEFAULT_WIDTH  640
#define VGA_DEFAULT_HEIGHT 480

enum { //VGA_MOD_ID
  VGA_MODE_640_480, NR_VGA_MODE
};

struct VGA_MODE{
  int h_frontporch, h_active, h_backporch, h_total;
  int v_frontporch, v_active, v_backporch, v_total;
};

class VGA : public Component{
private:
  int vga_screen_width, vga_screen_height;
  uint32_t *pixels;
  int vga_clk_cnt;
  uint32_t *p_pixel;
  uint32_t *p_pixel_end;
  bool is_r_len8, is_g_len8, is_b_len8;
  bool is_all_len8;
  bool is_pixels_same;
public:
  VGA(SDL_Renderer *rend, int cnt, int init_val, int ct);
  ~VGA();

  virtual void update_gui();
  virtual void update_state();
};

#endif

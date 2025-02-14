#include <vga.h>
#include <nvboard.h>
#include <macro.h>
#include <assert.h>

VGA* vga = NULL;

VGA_MODE vga_mod_accepted[NR_VGA_MODE] = {
  [VGA_MODE_640_480] = {
    .h_frontporch = 96,
    .h_active = 144,
    .h_backporch = 784,
    .h_total = 800,
    .v_frontporch = 2,
    .v_active = 35,
    .v_backporch = 515,
    .v_total = 525,
  },
};

static int vga_clk_cycle = 0;

VGA::VGA(SDL_Renderer *rend, int cnt, int init_val, int ct):
    Component(rend, cnt, init_val, ct),
    vga_screen_width(VGA_DEFAULT_WIDTH), vga_screen_height(VGA_DEFAULT_HEIGHT),
    vga_clk_cnt(1) {
  SDL_Texture *temp_texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING, vga_screen_width, vga_screen_height);
  set_texture(temp_texture, 0);
  pixels = new uint32_t[vga_screen_width * vga_screen_height];
  memset(pixels, 0, vga_screen_width * vga_screen_height * sizeof(uint32_t));

  is_r_len8 = pin_array[VGA_R0].vector_len == 8;
  is_g_len8 = pin_array[VGA_G0].vector_len == 8;
  is_b_len8 = pin_array[VGA_B0].vector_len == 8;
  is_all_len8 = is_r_len8 && is_g_len8 && is_b_len8;
  p_pixel = pixels;
  p_pixel_end = pixels + vga_screen_width * vga_screen_height;
}

VGA::~VGA() {
  SDL_DestroyTexture(get_texture(0));
  delete []pixels;
}

void VGA::update_gui() {
#ifdef DEBUG
  static int frames = 0;
  frames ++;
  printf("%d frames\n", frames);
#endif
  SDL_Texture *temp_texture = get_texture(0);
  SDL_Renderer *temp_renderer = get_renderer();
  SDL_Rect *temp_rect = get_rect(0);
  SDL_UpdateTexture(temp_texture, NULL, pixels, vga_screen_width * sizeof(uint32_t));
  //SDL_RenderClear(temp_renderer);
  SDL_RenderCopy(temp_renderer, temp_texture, NULL, temp_rect);
  set_redraw();
}

void VGA::update_state() {
  if (vga_clk_cnt > 1) {
    if (vga_clk_cnt < vga_clk_cycle) {
      vga_clk_cnt ++;
      return;
    }
    vga_clk_cnt = 1;
  }

  int r = 0, g = 0, b = 0;
  if (is_all_len8) {
    r = pin_peek8(VGA_R0);
    g = pin_peek8(VGA_G0);
    b = pin_peek8(VGA_B0);
  } else {
#define concat3(a, b, c) concat(concat(a, b), c)
#define MAP2(c, f, x)  c(f, x)
#define GET_COLOR_BIT(color, n) (pin_peek(concat3(VGA_, color, n)) << n)
#define BITS(f, color) f(color, 0) f(color, 1) f(color, 2) f(color, 3) \
                       f(color, 4) f(color, 5) f(color, 6) f(color, 7)
#define GET_COLOR_BIT_REDUCE(color, n) GET_COLOR_BIT(color, n) |
#define GET_COLOR(color) MAP2(BITS, GET_COLOR_BIT_REDUCE, color) 0
    r = is_r_len8 ? pin_peek8(VGA_R0) : GET_COLOR(R);
    g = is_g_len8 ? pin_peek8(VGA_G0) : GET_COLOR(G);
    b = is_b_len8 ? pin_peek8(VGA_B0) : GET_COLOR(B);
  }
  uint32_t color = (r << 16) | (g << 8) | b;
  if (*p_pixel != color) {
    *p_pixel = color;
    is_pixels_same = false;
  }
  p_pixel ++;
  if (p_pixel == p_pixel_end) {
    p_pixel = pixels;
    if (!is_pixels_same) {
      update_gui();
      is_pixels_same = true;
    }
  }
}

void vga_set_clk_cycle(int cycle) {
  vga_clk_cycle = cycle;
}

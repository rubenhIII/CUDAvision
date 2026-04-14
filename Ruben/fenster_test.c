// main.c
#include "lib/tools/fenster.h"
#define W 320
#define H 240

int main() {
  uint32_t buf[W * H];
  struct fenster f = { .title = "hello", .width = W, .height = H, .buf = buf };
  fenster_open(&f);
  while (fenster_loop(&f) == 0) {
    for (int i = 0; i < W; i++) {
      for (int j = 0; j < H; j++) {
        fenster_pixel(&f, i, j) = rand();
      }
    }
  }
  fenster_close(&f);
  return 0;
}
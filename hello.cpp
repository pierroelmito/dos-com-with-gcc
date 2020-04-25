
asm(".code16gcc\n"
    "call _dosmain\n"
    "mov $0x4C,%ah\n"
    "int $0x21\n");

#include "dos.h"

namespace {

#include "mode13.h"
#include "pipomath.h"

template <int W, int H, class T>
void computeFx(unsigned char *const screen, const T &f) {
  for (int y = H, o = 0; y; --y) {
    for (int x = W; x; --x) {
      screen[o++] = f(x, y);
    }
  }
}

template <class T, int W, int H, typename F> T *initBuffer(const F &f) {
  T *buffer = (T *)allocBuffer(W * H * sizeof(T));
  for (int y = 0, o = 0; y < H; ++y) {
    for (int x = 0; x < W; ++x) {
      buffer[o++] = f(x, y);
    }
  }
  return buffer;
}

} // namespace

extern "C" int dosmain(void) {
  const unsigned short ds = getDs();

  allocBuffer(1 << 16);

  unsigned char *const screen = ((unsigned char *)0xa0000) - (16 * ds);
  unsigned char *const buffer = allocBuffer(320 * 200);

  setMode(0x13);

  fill(screen, 0, 320 * 200);

  unsigned char *const lutSin = initBuffer<unsigned char, 256, 1>(
      [](int x, int) { return fakesini<int>(x, 256); });

  unsigned char *const map = initBuffer<unsigned char, 256, 256>(
      [=](int x, int y) { return lutSin[x] + lutSin[y]; });

  setPalette([=](int i, int &r, int &g, int &b) {
    r = 63 - lutSin[i] / 2;
    b = lutSin[i] / 4;
    g = i / 4;
  });

  auto *const target = buffer;
  for (unsigned int j = 0, jx = 0, jy = 0; j < 1000;
       ++j, jx += 7 * 64, jy += 9 * 64) {
    const int c0x = 160 - 128 + 2 * lutSin[(jx / 256) & 255];
    const int c0y = 100;
    const int c1x = 160;
    const int c1y = 100 - 64 + lutSin[(jy / 256) & 255];
    computeFx<320, 200>(target, [=](int x, int y) -> unsigned char {
      const int d0 = (lengthSq(c0x - x, c0y - y) >> 7) & 0xff;
      const int d1 = (lengthSq(c1x - x, c1y - y) >> 7) & 0xff;
      const unsigned char r = map[(d0 << 8) | d1];
      return r;
    });
    if (target != screen)
      blit(screen, target, 320 * 200);
  }

  fill(screen, 0, 320 * 200);

  setMode(0x3);

  print("Hello, World!\n$");

  return 0;
}

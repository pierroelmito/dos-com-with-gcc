#pragma once

void setMode(unsigned short mode) { asm volatile("int $0x10\n" : : "a"(mode)); }

template <class T> void setPalette(const T &f) {
  asm volatile(R"(
               mov $0x3c8, %%dx
               xor %%al, %%al
               out %%al, %%dx
               )"
               :
               :
               : "dx", "ax");
  for (int i = 0; i < 256; ++i) {
    int r, g, b;
    f(i, r, g, b);
    asm volatile(R"(
                 mov $0x3c9, %%dx
                 out %%al, %%dx
                 mov %%bl, %%al
                 out %%al, %%dx
                 mov %%bh, %%al
                 out %%al, %%dx
                 )"
                 :
                 : "a"(r), "b"(g | (b << 8))
                 : "dx");
  }
}

void fill(unsigned char *const dst, unsigned char c, int const count) {
  unsigned int *const idst = (unsigned int *)dst;
  const int icount = count / 4;
  const unsigned int tmp = c;
  const unsigned int v = (c << 24) | (c << 16) | (c << 8) | (c << 0);
  for (int i = 0; i < icount; ++i)
    idst[i] = v;
}

void blit(unsigned char *const dst, const unsigned char *const src,
          const int count) {
  unsigned int *const idst = (unsigned int *)dst;
  unsigned int *const isrc = (unsigned int *)src;
  const int icount = count / 4;
  for (int i = 0; i < icount; ++i)
    idst[i] = isrc[i];
}

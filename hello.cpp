
asm (
	".code16gcc\n"
	"call _dosmain\n"
	"mov $0x4C,%ah\n"
	"int $0x21\n"
);

#include "dos.h"

namespace {

#include "mode13.h"
#include "pipomath.h"

template <int W, int H, class T>
void computeFx(unsigned char* const screen, const T& f)
{
	for (int y = H, o = 0; y; --y) {
		for (int x = W; x; --x) {
			screen[o++] = f(x, y);
		}
	}
}

template <int W, int H, class T>
void raytraceFxf(unsigned char* screen, const vec3<float>& pos, const vec3<float>& dir, const vec3<float>& up, const T& f)
{
	typedef vec3<float> vec;

	const vec basech = unitizef(cross3(dir, up));
	const vec basecv = unitizef(cross3(dir, basech));

	const vec ch = basech / float(H);
	const vec cv = basecv / float(H);

	vec scrpos = pos + dir - (0.5f * W) * ch - (0.5f * H) * cv;
	for (int y = 0, o = 0; y < H; ++y) {
		vec linepos = scrpos;
		scrpos += cv;
		for (int x = 0; x < W; ++x) {
			screen[o++] = f(pos, unitizef(linepos));
			linepos += ch;
		}
	}
}

template <int W, int H, int B, class U, class T>
void raytraceFxi(unsigned char* screen, const vec3<int>& pos, const vec3<int>& dir, const vec3<int>& up, const U& u, const T& f)
{
	typedef vec3<int> vec;

	const vec basech = u(cross3(dir, up) / B);
	const vec basecv = u(cross3(dir, basech) / B);

	const vec ch = basech / H;
	const vec cv = basecv / H;

	vec scrpos = pos + dir - (W / 2) * ch - (H / 2) * cv;
	for (int y = 0, o = 0; y < H; ++y) {
		vec linepos = scrpos;
		scrpos += cv;
		for (int x = 0; x < W; ++x) {
			screen[o++] = f(pos, u(linepos));
			linepos += ch;
		}
	}
}

template <class T, int W, int H, typename F>
T* initBuffer(const F& f)
{
	T* buffer = (T*)allocBuffer(W * H * sizeof(T));
	for (int y = 0, o = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			buffer[o++] = f(x, y);
		}
	}
	return buffer;
}

}

extern "C" int dosmain(void)
{
	const unsigned short ds = getDs();

	allocBuffer(1 << 16);

	unsigned char* const screen = ((unsigned char*)0xa0000) - (16 * ds);
	unsigned char* const buffer = allocBuffer(320 * 200);

	setMode(0x13);

	fill(screen, 0, 320 * 200);

	unsigned char* const lutSin = initBuffer<unsigned char, 256, 1>([] (int x, int) {
		return fakesini<int>(x, 256);
	});

	unsigned char* const map = initBuffer<unsigned char, 256, 256>([=] (int x, int y) {
		return lutSin[x] + lutSin[y];
	});

#if 0
	static const int lutLength2Max = 8192;
	int* const lutLength2 = initBuffer<int, lutLength2Max, 1>([] (int x, int) {
		return int(65535.999f * Q_rsqrt(float(x) / float(lutLength2Max - 1)));
	});

	setPalette([] (int i, int& r, int& g, int& b) {
		if (i < 128) {
			r = b = i / 2;
			g = 0;
		} else {
			r = b = 0;
			g = (i - 128) / 2;
		}
	});

	auto fakeAtan = [] (int x, int y) {
		if ((x > 0) ^ (y > 0))
			return abs(x) > abs(y) ? 1 : 0;
		else
			return abs(x) > abs(y) ? 0 : 1;
	};

	for (unsigned int j = 0; j < 1000; ++j) {
#if 1
		const vec3<float> pos{ 0, 0, 0 };
		const vec3<float> dir = unitizef({ 0.5, 0, 1 });
		const vec3<float> up{ 0, 1, 0 };
		raytraceFxf<320, 200>(buffer, pos, dir, up, [=] (const vec3<float>&, const vec3<float>& d) -> unsigned char {
			const int coeff = 1024;
			const int ix = d.x * (float(coeff) - 0.0001);
			const int iy = d.y * (float(coeff) - 0.0001);
			const int lenSq = (lutLength2Max * (ix * ix + iy * iy)) / (coeff * coeff + coeff * coeff);
			const int idist = lutLength2[lenSq] >> 8;
			const int u = ((idist * 2 + j * 200) & 128) ? 1 : 0;
			const int v = fakeAtan(ix, iy);
			return ((u ^ v) * 128) + (255 * 127) / int(255 + idist);
		});
#else
		const int base = 1024;
		const int baseSq = base * base;
		auto unitizei = [&] (const vec3<int>& t) -> vec3<int> {
			//return t;
			const vec3<int> v = t / 2;
			const int lsq = (lutLength2Max * lengthsq(v)) / (3 * baseSq);
			return (lutLength2[lsq] * v) / 65536;
		};
		const vec3<int> pos{ 0, 0, 0 };
		const vec3<int> dir = unitizei({ 0, base / 2, base / 2 });
		const vec3<int> up{ 0, base, 0 };
		raytraceFxi<320, 200, base>(buffer, pos, dir, up, unitizei, [=] (const vec3<int>&, const vec3<int>& d) -> unsigned char {
			const int lenSq = (lutLength2Max * (d.x * d.x + d.y * d.y)) / (2 * baseSq);
			const int idist = ((d.z * lutLength2[lenSq]) / base) >> 8;
			const int u = ((idist * 2 + j * 200) & 128) ? 1 : 0;
			const int v = fakeAtan(d.x, d.y);
			//return ((u ^ v) * 128) + (255 * 127) / int(255 + idist);
			return ((u ^ v) * 128) + 127;
		});
#endif
		blit(screen, buffer, 320 * 200);
	}
#endif

	setPalette([=] (int i, int& r, int& g, int& b) {
		r = 63 - lutSin[i] / 2;
		b = lutSin[i] / 4;
		g = i / 4;
	});

	auto* const target = buffer;
	for (unsigned int j = 0, jx = 0, jy = 0; j < 1000; ++j, jx += 7 * 64, jy += 9 * 64) {
		const int c0x = 160 - 128 + 2 * lutSin[(jx / 256) & 255];
		const int c0y = 100;
		const int c1x = 160;
		const int c1y = 100 - 64 + lutSin[(jy / 256) & 255];
		computeFx<320, 200>(target, [=] (int x, int y) -> unsigned char {
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


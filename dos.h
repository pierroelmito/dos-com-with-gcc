#pragma once

extern unsigned char heap;
static unsigned char *hbreak = &heap;

static inline unsigned char *allocBuffer(unsigned int size)
{
	unsigned char *ptr = hbreak;
	hbreak += size;
	return ptr;
}

static inline void print(const char *string)
{
	asm volatile (
		"mov $0x09, %%ah\n"
		"int $0x21\n"
		:
		: "d"(string)
		: "ah"
	);
}

static inline unsigned short getDs()
{
	unsigned short o;
	asm volatile ("mov %%ds, %%bx\n" : "=bx"(o));
	return o;
}


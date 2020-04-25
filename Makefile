
CFLAGS = \
	-Os \
	-s \
	-nostdlib \
	-std=c++17 \
	-m32 \
	-march=i586 \
	-ffreestanding \
	-fno-pie \
	-fleading-underscore \
	-fomit-frame-pointer \
	-fno-stack-protector \
	-fno-unwind-tables \
	-fmerge-all-constants \
	-fno-unroll-loops \
	-falign-functions=1 \
	-falign-jumps=1 \
	-falign-loops=1 \
	-fno-asynchronous-unwind-tables \
	-fno-math-errno \
	-mpreferred-stack-boundary=2 \
	-fno-ident

CC = g++

LFLAGS = \
	-Wl,--nmagic,--script=com.ld

hello.com : hello.bin
	objcopy -S -O binary $< $@
	ls -l $@

hello.bin : hello.cpp *.h com.ld
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $<

clean:
	rm -f hello.com hello.bin


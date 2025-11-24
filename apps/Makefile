CC=i386-elf-gcc
LD=i386-elf-ld

LDFLAGS=


helloc.elf: helloc.c
	$(CC) -o helloc.o $^ -c
	$(LD) -o $@ helloc.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -frv *.o *.elf

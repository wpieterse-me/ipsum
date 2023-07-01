COMPILER = gcc
FILESYSTEM_FILES = main.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o main `pkg-config fuse --cflags --libs` `pkg-config physfs --cflags --libs`

clean:
	rm main

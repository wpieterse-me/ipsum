COMPILER = gcc
FILESYSTEM_FILES = main.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -O3 -o main `pkg-config libgit2 --cflags --libs`

clean:
	rm main

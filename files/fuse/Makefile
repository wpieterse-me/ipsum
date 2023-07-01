COMPILER = gcc
FILESYSTEM_FILES = ssfs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o ssfs `pkg-config fuse --cflags --libs` `pkg-config physfs --cflags --libs`

clean:
	rm ssfs

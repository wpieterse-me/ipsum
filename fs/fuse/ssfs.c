/**
 * Simple & Stupid Filesystem.
 *
 * Mohammed Q. Hussain - http://www.maastaar.net
 *
 * This is an example of using FUSE to build a simple filesystem. It is a part
 * of a tutorial in MQH Blog with the title "Writing a Simple Filesystem Using
 * FUSE in C":
 * http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
 *
 * License: GNU GPL
 */

#define FUSE_USE_VERSION 30

#include <stdbool.h>
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <physfs.h>

static int do_getattr(const char* path, struct stat* st) {
  // printf("[getattr] Called\n");
  // printf("\tAttributes of %s requested\n", path);

  st->st_uid   = getuid();
  st->st_gid   = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  PHYSFS_Stat stat;
  if(!PHYSFS_stat(path, &stat)) {
    return -1;
  }

  if(stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
    st->st_mode  = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if(stat.filetype == PHYSFS_FILETYPE_REGULAR) {
    st->st_mode  = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size  = stat.filesize;
  } else {
    return -1;
  }

  return 0;
}

static int do_readdir(const char*            path,
                      void*                  buffer,
                      fuse_fill_dir_t        filler,
                      off_t                  offset,
                      struct fuse_file_info* fi) {
  // printf("--> Getting The List of Files of %s\n", path);

  filler(buffer, ".", NULL, 0);
  filler(buffer, "..", NULL, 0);

  char** rc = PHYSFS_enumerateFiles(path);
  if(rc == NULL) {
    return -1;
  }

  for(char** i = rc; *i != NULL; i++) {
    filler(buffer, *i, NULL, 0);
  }

  PHYSFS_freeList(rc);
  return 0;
}

static int do_read(const char*            path,
                   char*                  out_buffer,
                   size_t                 size,
                   off_t                  offset,
                   struct fuse_file_info* fi) {
  PHYSFS_file* file = PHYSFS_openRead(path);
  if(file == NULL) {
    printf("READ - 1\n");
    return -1;
  }

  if(!PHYSFS_seek(file, offset)) {
    printf("READ - 2\n");
    return -1;
  }

  PHYSFS_sint64 bytesRead = PHYSFS_readBytes(file, out_buffer, size);
  PHYSFS_close(file);

  // printf("--> READ %s, %lu, %lu, %llu\n", path, offset, size, bytesRead);

  return bytesRead;
}

static struct fuse_operations operations = {
  .getattr = do_getattr,
  .readdir = do_readdir,
  .read    = do_read,
};

int main(int argc, char* argv[]) {
  if(!PHYSFS_init(argv[0])) {
    printf("FAILED - 1\n");
    return 1;
  }

  if(!PHYSFS_mount("/home/wpieterse/games/quake/id1/PAK0.PAK", NULL, 1)) {
    printf("FAILED - 2\n");
    return 1;
  }

  if(!PHYSFS_mount("/home/wpieterse/games/quake/id1/PAK1.PAK", NULL, 1)) {
    printf("FAILED - 2\n");
    return 1;
  }

  return fuse_main(argc, argv, &operations, NULL);
}
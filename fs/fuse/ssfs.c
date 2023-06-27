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

void read_file(const char* filename, uint8_t** buffer, size_t* buffer_size) {
  FILE* file = fopen(filename, "rb");

  fseek(file, 0, SEEK_END);
  *buffer_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buffer = (uint8_t*)malloc(*buffer_size);
  fread(*buffer, *buffer_size, 1, file);

  fclose(file);
}

typedef struct {
    uint32_t identifier;
    uint32_t entries_offset;
    uint32_t entries_size;
} pack_header_t;

typedef struct {
    char     name[56];
    uint32_t offset;
    uint32_t size;
} pack_entry_t;

typedef struct pack_tree_s {
    char                name[64];
    bool                is_folder;
    struct pack_tree_s* items[1024];
    size_t              used_items;
    pack_entry_t*       entry;
} pack_tree_t;

pack_tree_t* build_pack_tree(uint32_t entry_count, pack_entry_t* entries) {
  pack_tree_t* result = (pack_tree_t*)malloc(sizeof(pack_tree_t));

  memset(result, 0, sizeof(pack_tree_t));

  result->name[0]   = '/';
  result->is_folder = true;
  for(uint32_t index = 0; index < entry_count; index++) {
    pack_entry_t* entry      = &entries[index];
    char*         entry_name = entry->name;

    if(strchr(entry_name, '/') == 0) {
      pack_tree_t* tree_entry = (pack_tree_t*)malloc(sizeof(pack_tree_t));

      memset(tree_entry, 0, sizeof(pack_tree_t));

      strncpy(tree_entry->name, entry_name, 56);
      tree_entry->entry = entry;

      result->items[result->used_items] = tree_entry;
      result->used_items++;
    } else {
      pack_tree_t* insert_folder = result;

      char  temporary[64] = { 0 };
      char* token         = strtok(entry_name, "/");
      while(token != NULL) {
        strcpy(temporary, token);

        token = strtok(NULL, "/");
        if(token != NULL) {
          bool found = false;

          for(size_t index = 0; index < insert_folder->used_items; index++) {
            pack_tree_t* item = insert_folder->items[index];
            if(!strcmp(temporary, item->name)) {
              insert_folder = item;
              found         = true;

              break;
            }
          }

          if(found == false) {
            pack_tree_t* new_node = (pack_tree_t*)malloc(sizeof(pack_tree_t));
            memset(new_node, 0, sizeof(pack_tree_t));

            new_node->is_folder = true;
            strcpy(new_node->name, temporary);

            insert_folder->items[insert_folder->used_items] = new_node;
            insert_folder->used_items++;

            insert_folder = new_node;
          }
        }
      }

      pack_tree_t* file_node = (pack_tree_t*)malloc(sizeof(pack_tree_t));
      strcpy(file_node->name, temporary);
      file_node->entry = entry;

      insert_folder->items[insert_folder->used_items] = file_node;
      insert_folder->used_items++;
    }
  }

  return result;
}

void print_indent(int32_t indent) {
  for(int32_t i = 0; i < indent; i++) {
    printf(" ");
  }
}

void print_tree(pack_tree_t* tree, int32_t indent) {
  print_indent(indent);
  printf("%s\n", tree->name);

  for(size_t index = 0; index < tree->used_items; index++) {
    pack_tree_t* item = tree->items[index];
    if(item->is_folder) {
      print_tree(item, indent + 2);
    } else {
      print_indent(indent + 2);
      printf("%s\n", item->name);
    }
  }
}

uint8_t*       buffer      = NULL;
size_t         buffer_size = 0;
pack_header_t* header      = NULL;
uint32_t       entry_count = 0;
pack_entry_t*  entries     = NULL;
pack_tree_t*   tree        = NULL;

static int do_getattr(const char* path, struct stat* st) {
  printf("[getattr] Called\n");
  printf("\tAttributes of %s requested\n", path);

  // GNU's definitions of the attributes
  // (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
  // 		st_uid: 	The user ID of the file’s owner.
  //		st_gid: 	The group ID of the file.
  //		st_atime: 	This is the last access time for the file.
  //		st_mtime: 	This is the time of the last modification to the
  // contents of the file. 		st_mode: 	Specifies the mode of
  // the file. This includes file type information (see Testing File Type) and
  // the file
  // permission bits (see Permission Bits). 		st_nlink: 	The
  // number of hard links to the file. This count keeps track of how many
  // directories have entries for this file. If the count is ever decremented to
  // zero, then the file itself is discarded as soon as no process still holds
  // it open. Symbolic links are not counted in the total. 		st_size:
  // This specifies the size of a regular file in bytes. For files that are
  // really devices this field isn’t usually meaningful. For symbolic links this
  // specifies the length of the file name the link refers to.

  st->st_uid = getuid(); // The owner of the file/directory is the user who
                         // mounted the filesystem
  st->st_gid = getgid(); // The group of the file/directory is the same as the
                         // group of the user who mounted the filesystem
  st->st_atime =
      time(NULL); // The last "a"ccess of the file/directory is right now
  st->st_mtime =
      time(NULL); // The last "m"odification of the file/directory is right now

  if(strcmp(path, "/") == 0) {
    st->st_mode  = S_IFDIR | 0755;
    st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is
                      // here: http://unix.stackexchange.com/a/101536
  } else {
    const char* subpath = path + 1;

    for(size_t index = 0; index < tree->used_items; index++) {
      pack_tree_t* item = tree->items[index];

      if(!strcmp(subpath, item->name)) {
        if(item->is_folder) {
          st->st_mode  = S_IFDIR | 0755;
          st->st_nlink = 2;
        } else {
          st->st_mode  = S_IFREG | 0644;
          st->st_nlink = 1;
          st->st_size  = item->entry->size;
        }
      }
    }
  }

  return 0;
}

static int do_readdir(const char*            path,
                      void*                  buffer,
                      fuse_fill_dir_t        filler,
                      off_t                  offset,
                      struct fuse_file_info* fi) {
  printf("--> Getting The List of Files of %s\n", path);

  filler(buffer, ".", NULL, 0);  // Current Directory
  filler(buffer, "..", NULL, 0); // Parent Directory

  // If the user is trying to show the files/directories of the root
  // directory show the following
  if(strcmp(path, "/") == 0) {
    for(size_t index = 0; index < tree->used_items; index++) {
      pack_tree_t* item = tree->items[index];

      filler(buffer, item->name, NULL, 0);
    }
  }

  return 0;
}

static int do_read(const char*            path,
                   char*                  out_buffer,
                   size_t                 size,
                   off_t                  offset,
                   struct fuse_file_info* fi) {
  printf("--> Trying to read %s, %lu, %lu\n", path, offset, size);

  char  file54Text[]  = "Hello World From File54!";
  char  file349Text[] = "Hello World From File349!";
  char* selectedText  = NULL;

  // ... //

  const char* subpath = path + 1;
  for(size_t index = 0; index < tree->used_items; index++) {
    pack_tree_t* item = tree->items[index];

    if(!strcmp(item->name, subpath)) {
      if(item->is_folder == false) {
        selectedText = (char*)(buffer + item->entry->offset);
        memcpy(out_buffer, (buffer + item->entry->offset) + offset, size);
        return item->entry->size - offset;
      }
    }
  }

  return -1;
}

static struct fuse_operations operations = {
  .getattr = do_getattr,
  .readdir = do_readdir,
  .read    = do_read,
};

int main(int argc, char* argv[]) {
  read_file("/home/wpieterse/games/quake/id1/PAK0.PAK", &buffer, &buffer_size);

  header      = (pack_header_t*)buffer;
  entry_count = header->entries_size / sizeof(pack_entry_t);
  entries     = (pack_entry_t*)(buffer + header->entries_offset);
  tree        = build_pack_tree(entry_count, entries);

  return fuse_main(argc, argv, &operations, NULL);
}
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

int32_t main(int32_t argument_count, char** arguments) {
  uint8_t* buffer      = NULL;
  size_t   buffer_size = 0;

  read_file("/home/wpieterse/games/quake/id1/PAK0.PAK", &buffer, &buffer_size);

  pack_header_t* header      = (pack_header_t*)buffer;
  uint32_t       entry_count = header->entries_size / sizeof(pack_entry_t);
  pack_entry_t*  entries     = (pack_entry_t*)(buffer + header->entries_offset);
  pack_tree_t*   tree        = build_pack_tree(entry_count, entries);

  print_tree(tree, 0);

  free(tree);
  free(buffer);

  return 0;
}

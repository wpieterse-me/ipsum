#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <liburing.h>

#include "util.h"

typedef struct _FILE_INDEX_NODE FILE_INDEX_NODE;

struct _FILE_INDEX_NODE {

};

typedef struct _FILE_SYSTEM FILE_SYSTEM;

struct _FILE_SYSTEM { };

typedef struct _FILE_SYSTEM_FACTORY FILE_SYSTEM_FACTORY;

typedef void (*FILE_SYSTEM_FACTORY_CREATE)(FILE_SYSTEM_FACTORY* This,
                                           FILE_SYSTEM**        Result);

typedef void (*FILE_SYSTEM_FACTORY_DESTROY)(FILE_SYSTEM_FACTORY* This,
                                            FILE_SYSTEM*         FileSystem);

struct _FILE_SYSTEM_FACTORY {
    FILE_SYSTEM_FACTORY_CREATE  Create;
    FILE_SYSTEM_FACTORY_DESTROY Destroy;
};

#define FILE_SYSTEM_WAD_SIGNATURE SIGNATURE_32('F', 'S', 'W', 'D')

#define FILE_SYSTEM_WAD_FROM_THIS(Record)                                      \
  CONTAINER_OF(Record, FILE_SYSTEM_WAD, FileSystem, FILE_SYSTEM_WAD_SIGNATURE)

typedef struct _FILE_SYSTEM_WAD {
    uint32_t Signature;

    FILE_SYSTEM FileSystem;
} FILE_SYSTEM_WAD;

#define FILE_SYSTEM_FACTORY_WAD_SIGNATURE SIGNATURE_32('F', 'S', 'F', 'W')

#define FILE_SYSTEM_FACTORY_WAD_FROM_THIS(Record)                              \
  CONTAINER_OF(Record,                                                         \
               FILE_SYSTEM_FACTORY_WAD,                                        \
               FileSystemFactory,                                              \
               FILE_SYSTEM_FACTORY_WAD_SIGNATURE)

typedef struct _FILE_SYSTEM_FACTORY_WAD {
    uint32_t Signature;

    FILE_SYSTEM_FACTORY FileSystemFactory;
} FILE_SYSTEM_FACTORY_WAD;

static void Create(FILE_SYSTEM_FACTORY* This, FILE_SYSTEM** Result) {
  FILE_SYSTEM_FACTORY_WAD* Private = NULL;

  Private = FILE_SYSTEM_FACTORY_WAD_FROM_THIS(This);
}

static void Destroy(FILE_SYSTEM_FACTORY* This, FILE_SYSTEM* FileSystem) {
  FILE_SYSTEM_FACTORY_WAD* PrivateFactory = NULL;
  FILE_SYSTEM_WAD*         Private        = NULL;

  PrivateFactory = FILE_SYSTEM_FACTORY_WAD_FROM_THIS(This);
  Private        = FILE_SYSTEM_WAD_FROM_THIS(FileSystem);
}

FILE_SYSTEM_FACTORY_WAD FileSystemWADTemplate = {
  FILE_SYSTEM_FACTORY_WAD_SIGNATURE,
  {
    Create, Destroy,
    },
};

FILE_SYSTEM_FACTORY* GetFactory(void) {
  return &FileSystemWADTemplate.FileSystemFactory;
}

int32_t main(int32_t argument_count, char** arguments) {
  return 0;
}

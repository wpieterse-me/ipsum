#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

typedef struct _EFI_CPU_ARCHITECTURE_PROTOCOL EFI_CPU_ARCHITECTURE_PROTOCOL;

typedef int64_t (*EFI_CPU_ENABLE_INTERRUPT)(
    EFI_CPU_ARCHITECTURE_PROTOCOL* This);
typedef int64_t (*EFI_CPU_DISABLE_INTERRUPT)(
    EFI_CPU_ARCHITECTURE_PROTOCOL* This);

struct _EFI_CPU_ARCHITECTURE_PROTOCOL {
    EFI_CPU_ENABLE_INTERRUPT  EnableInterrupt;
    EFI_CPU_DISABLE_INTERRUPT DisableInterrupt;
};

#define CPU_ARCHITECTURE_PROTOCOL_PRIVATE_SIGNATURE                            \
  SIGNATURE_32('c', 'a', 'p', 'd')

struct _EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE {
    int32_t Signature;

    int32_t Value;

    EFI_CPU_ARCHITECTURE_PROTOCOL Cpu;
};

typedef struct _EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE
    EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE;

#define CPU_ARCHITECTURE_PROTOCOL_PRIVATE_FROM_THIS(Record)                    \
  CONTAINER_OF(Record,                                                         \
               EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE,                          \
               Cpu,                                                            \
               CPU_ARCHITECTURE_PROTOCOL_PRIVATE_SIGNATURE)

int64_t EmuEnableInterrupt(EFI_CPU_ARCHITECTURE_PROTOCOL* This) {
  EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE* Private = NULL;

  Private = CPU_ARCHITECTURE_PROTOCOL_PRIVATE_FROM_THIS(This);

  printf("Enabling interrupts - %d\n", Private->Value);

  return 0;
}

int64_t EmuDisableInterrupt(EFI_CPU_ARCHITECTURE_PROTOCOL* This) {
  EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE* Private = NULL;

  Private = CPU_ARCHITECTURE_PROTOCOL_PRIVATE_FROM_THIS(This);

  printf("Disabling interrupts - %d\n", Private->Value);

  return 0;
}

EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE Template = {
  CPU_ARCHITECTURE_PROTOCOL_PRIVATE_SIGNATURE,
  64,
  {
    EmuEnableInterrupt, EmuDisableInterrupt,
    },
};

void CreateInstance(EFI_CPU_ARCHITECTURE_PROTOCOL** Instance) {
  EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE* Private = NULL;

  Private = (EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE*)malloc(
      sizeof(EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE));

  memcpy(Private, &Template, sizeof(EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE));

  *Instance = &Private->Cpu;
}

void DestroyInstance(EFI_CPU_ARCHITECTURE_PROTOCOL* This) {
  EFI_CPU_ARCHITECTURE_PROTOCOL_PRIVATE* Private = NULL;

  Private = CPU_ARCHITECTURE_PROTOCOL_PRIVATE_FROM_THIS(This);

  free(Private);
}

int32_t main(int32_t argument_count, char** arguments) {
  EFI_CPU_ARCHITECTURE_PROTOCOL* Cpu = NULL;

  CreateInstance(&Cpu);

  Cpu->EnableInterrupt(Cpu);
  Cpu->DisableInterrupt(Cpu);

  DestroyInstance(Cpu);

  printf("DONE!!!\n");
  return 0;
}

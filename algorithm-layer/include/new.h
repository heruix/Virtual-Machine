#ifndef NEW_H
#define NEW_H

#include "virtualMachine.h"

void *New(const void *_class, ...);
void Delete(void *_class);
void SetProgramLoader(void *_vm, void *_loader);
void SetVirtualAccelerator(void *vm, void *_accelerator);
void CreateVirtualMachine(void *vm, VMParameter *_params);
void LoadProgramImage(void *_vm, char *_name);
void RunVirtualMachine(void *vm);

#endif

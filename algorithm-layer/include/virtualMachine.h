#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include "accelerator.h"
#include <stdlib.h>
#include <stdarg.h>
#include <linux/kvm.h>

typedef struct {
    int vcpuNumber;
    __u64 ramSize;
    VCPUThread *thread;
} VMParameter;

typedef struct {
    size_t size;
    void *(*ctor)(void *_self, va_list *_params);
    void *(*dtor)(void *_self);
    void (*setProgramLoader)(void *_self, void *_loader);
    void (*setVirtualAccelerator)(void *_self, void *_accelerator);
    void (*createVirtualMachine)(void *_self, VMParameter *_params);
    void (*loadProgramImage)(void *_self, char *_name);
    void (*run)(void *_self);
} VirtualMachine;

#endif

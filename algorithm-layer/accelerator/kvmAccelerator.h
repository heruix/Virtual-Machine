#ifndef KVMACCELERATOR_H
#define KVMACCELERATOR_H

#include "accelerator.h"

typedef struct {
    const void *_;
    KVMState *kvm;
} _KVMAccelerator;

#endif

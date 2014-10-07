#ifndef KVMACCELERATOR_H
#define KVMACCELERATOR_H

#include "accelerator.h"
#include <pthread.h>

struct kvm_userspace_memory_region;

typedef struct {
    int id:
    int fd;
    pthread_t vcpuThread;
    struct kvm_run *kvmRun;
    int kvmRunMmapSize;
    struct kvm_regs *regs;
    struct kvm_regs *sregs;
    VCPUThread runThread;
} VCPUState;

typedef struct {
    int version;
    int devFd;
    int vmFd;
    __u64 ramSize;
    __u64 ramStart;
    struct kvm_userspace_memory_region *mem;
    VCPUState *vcpus;
    int vcpuNumber;
} KVMState;

typedef struct {
    const void *_;
    KVMState *kvm;
} _KVMAccelerator;

extern const void *KVMAccelerator;

#endif

int *a = calloc(5, sizeof(int*));

#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>

typedef struct {
    int id;
    int fd;
    pthread_t vcpuThread;
    struct kvm_run *kvmRun;
    int kvmRunMmapSize;
    struct kvm_regs regs;
    struct kvm_sregs sregs;
    VCPUThread *runThread;
} VCPUState;

typedef struct {
    int version;
    int devFd;
    int vmFd;
    __u64 ramSize;
    __u64 ramStart;
    struct kvm_userspace_memory_region *mem;
    VCPUState **vcpus;
    int vcpuNumber;
} KVMState;

typedef struct {
    KVMState *kvm;
    int vcpuID;
} VCPUThreadParameter;

extern const void *KVMAccelerator;
extern const void *BinaryLoader;
extern const void *X86VirtualMachine;

#endif

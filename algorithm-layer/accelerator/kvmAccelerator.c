#include "new.h"
#include "types.h"
#include "accelerator.h"
#include "kvmAccelerator.h"
#include <linux/kvm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>

const char *KVM_DEVICE = "/dev/kvm";

static void kvmAcceleratorInit(void *_self) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;

    kvm->devFd = open(KVM_DEVICE, O_RDWR);

    if (kvm->devFd < 0) {
        perror("Open kvm device failed");
        exit(1);
    } else {
        kvm->version = ioctl(kvm->devFd, KVM_GET_API_VERSION, 0);
    }
}

static void *kvmAcceleratorCtor(void *_self, va_list *_params) {
    _KVMAccelerator *self = _self;
    int vcpuNumber = va_arg(*_params, int);

    self->kvm = (KVMState*)calloc(1, sizeof(KVMState));
    self->kvm->vcpus = (VCPUState**)calloc(5, sizeof(VCPUState*));

    kvmAcceleratorInit(_self);
    
    return self;
}

static void cleanVM(void *_kvm) {
    KVMState *kvm = _kvm;

    close(kvm->vmFd);
    munmap((void*)kvm->ramStart, kvm->ramSize);
}

static void cleanVCPU(void *_kvm) {
    KVMState *kvm = _kvm;
    VCPUState **vcpu = kvm->vcpus;

    int i = 0;
    for (; i < kvm->vcpuNumber; ++i) {
        munmap(vcpu[i]->kvmRun, vcpu[i]->kvmRunMmapSize);
        close(vcpu[i]->fd);
    }
}

static void freeKVMStatus(void *_kvm) {
    KVMState *kvm = _kvm;

    close(kvm->devFd);

    int i = 0;
    for (; i < kvm->vcpuNumber; ++i) {
        free(kvm->vcpus[i]);
    }

    free(kvm->vcpus);
    free(kvm);
}

static void *kvmAcceleratorDtor(void *_self) {
    _KVMAccelerator *self = _self;

    cleanVM(self->kvm);
    cleanVCPU(self->kvm);
    freeKVMStatus(self->kvm);

    return self;
}

static void kvmAcceleratorCreateVM(void *_self, int _ramSize) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;

    kvm->vmFd = ioctl(kvm->devFd, KVM_CREATE_VM, 0);

    if (kvm->vmFd < 0) {
        perror("Create Virtual Machine failed");
        exit(1);
    } else {
        kvm->ramSize = _ramSize;
        kvm->ramStart = (__u64)mmap(NULL, kvm->ramSize,
                                       PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);

        if ((void*)kvm->ramStart == MAP_FAILED) {
            perror("Alloc Virtual Machine's ram failed");
            exit(1);
        } else {
            kvm->mem = calloc(1, sizeof(struct kvm_userspace_memory_region));
            kvm->mem->slot = 0;
            kvm->mem->guest_phys_addr = 0;
            kvm->mem->memory_size = kvm->ramSize;
            kvm->mem->userspace_addr = kvm->ramStart;

            int ret = ioctl(kvm->vmFd, KVM_SET_USER_MEMORY_REGION, kvm->mem);
            if (ret < 0) {
                perror("Set user memory region failed");
                exit(1);
            }
        }
    }
}

static __u64 kvmAcceleratorGetVMRamBaseAddr(void *_self) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;
    
    return kvm->ramStart;
}

static void kvmAcceleratorInitVCPU(void *_self, int _vcpuID, VCPUThread _thread) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;
    VCPUState *vcpu = calloc(1, sizeof(VCPUState));
    
    vcpu->id = _vcpuID;
    vcpu->fd = ioctl(kvm->vmFd, KVM_CREATE_VCPU, vcpu->id);

    if (vcpu->id < 0) {
        perror("Create VCPU failed");
        exit(1);
    } else {
        vcpu->kvmRunMmapSize = ioctl(kvm->devFd, KVM_GET_VCPU_MMAP_SIZE, 0);
        if (vcpu->kvmRunMmapSize < 0) {
            perror("Get vcpu mmap size failed");
            exit(1);
        } else {
            fprintf(stdout, "Get vcpu mmap size: %d\n", vcpu->kvmRunMmapSize);
            vcpu->kvmRun = mmap(NULL, vcpu->kvmRunMmapSize,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 vcpu->fd, 0);
            if (vcpu->kvmRun == MAP_FAILED) {
                perror("Mmap kvm run failed");
                exit(1);
            } else {
                vcpu->runThread = _thread;
                self->kvm->vcpus[self->kvm->vcpuNumber] = vcpu;
                ++self->kvm->vcpuNumber;
            }
        }
    }
}

static void kvmAcceleratorRunVM(void *_self) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;
    VCPUState **vcpus = kvm->vcpus;

    VCPUThreadParameter *vcpuThreadParameter =
        (VCPUThreadParameter*)calloc(kvm->vcpuNumber, sizeof(VCPUThreadParameter*));

    int i = 0;
    for (; i < kvm->vcpuNumber; ++i) {
        vcpuThreadParameter[i].kvm = kvm;
        vcpuThreadParameter[i].vcpuID = i;
        if (pthread_create(&(vcpus[i]->vcpuThread),
                             (const pthread_attr_t *)NULL, vcpus[i]->runThread, &vcpuThreadParameter[i]) != 0) {
            perror("Create kvm thread failed");
        } 
    }

    for (i = 0; i < kvm->vcpuNumber; ++i) {
        pthread_join(vcpus[i]->vcpuThread, NULL);
    }

    free(vcpuThreadParameter);
}

static const Accelerator _kvmAccelerator = {
    sizeof(_KVMAccelerator),
    kvmAcceleratorCtor,
    kvmAcceleratorDtor,
    kvmAcceleratorInit,
    kvmAcceleratorCreateVM,
    kvmAcceleratorGetVMRamBaseAddr,
    kvmAcceleratorInitVCPU,
    kvmAcceleratorRunVM
};

const void *KVMAccelerator = &_kvmAccelerator;

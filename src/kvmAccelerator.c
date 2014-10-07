#include "accelerator.h"
#include "kvmAccelerator.h"
#include <linux/kvm.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

const char *KVM_DEVICE = "/dev/kvm";

static void *kvmAcceleratorCtor(void *_self, va_list *_params) {
    _KVMAccelerator *self = _self;

    self->kvm = calloc(1, sizeof(KVMState));
    self->kvm->vcpus = calloc(va_arg(*_params, int), sizeof(VCPUState*));

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
    struct vcpu *vcpu = kvm->vcpus;

    munmap(vcpu->kvm_run, vcpu->kvm_run_mmap_size);
    close(vcpu->vcpu_fd);
}

static void freeKVMStatus(void *_kvm) {
    KVMState *kvm = _kvm;

    close(kvm->devFd);
    
    

}

static void *kvmAcceleratorDtor(void *_self) {
    _KVMAccelerator *self = _self;

    cleanVM(self->kvm);
    cleanVCPU(self->kvm);
    freeKVMStatus(self->kvm);

    return self;
}

static void kvmAcceleratorInit(void *_self) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = calloc(1, sizeof(KVMState));

    kvm->devFd = open(KVM_DEVICE, O_RDWR);

    if (kvm->devFd < 0) {
        perror("Open kvm device failed: ");
        exit(1);
    } else {
        kvm->version = ioctl(kvm->devFd, KVM_GET_API_VERSION, 0);
    }
}

static void kvmAcceleratorCreateVM(void *_self, int _ramSize) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;

    kvm->vmFd = ioctl(kvm->devFd, KVM_CREATE_VM, 0);

    if (kvm->vmFd) {
        perror("Create Virtual Machine failed: ");
        exit(1);
    } else {
        kvm->ramSize = _ramSize;
        kvm->ramStart = (__u64)mmap(NULL, kvm->ramSize,
                                       PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);

        if ((void*)kvm->ramStart == MAP_FAILED) {
            perror("Alloc Virtual Machine's ram failed: ");
            exit(1);
        } else {
            kvm->mem = calloc(1, sizeof(kvm_userspace_memory_region));
            kvm->mem->slot = 0;
            kvm->mem->guest_phys_addr = 0;
            kvm->mem->memory_size = kvm->ramSize;
            kvm->mem->userspace_addr = kvm->ramStart;

            int ret = ioctl(kvm->vmFd, KVM_SET_USER_MEMORY_REGION, kvm->mem);
            if (ret < 0) {
                perror("Set user memory region failed: ");
                exit(1);
            }
        }
    }
}

static void kvmAcceleratorInitVCPU(void *_self, int _vcpuID, VCPUThread _thread) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;
    VCPUState *vcpu = calloc(1, sizeof(VCPUState));
    
    vcpu->id = _vcpuID;
    vcpu->fd = ioctl(kvm->vmFd, KVM_CREATE_VCPU, vcpu->id);

    if (vcpu->id < 0) {
        perror("Create VCPU failed: ");
        exit(1);
    } else {
        vcpu->kvmRunMmapSize = ioctl(kvm->devFd, KVM_GET_VCPU_MMAP_SIZE, 0);
        if (vcpu->kvmRunMmapSize < 0) {
            perror("Get vcpu mmap size failed: ");
            exit(1);
        } else {
            fprintf(stdout, "Get vcpu mmap size: %d\n", vcpu->kvmRunMmapSize);
            vcpu->kvmRun = mmap(NULL, vcpu->kvmRunMmapSize,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 vcpu->fd, 0);
            if (vcpu->kvmRun == MAP_FAILED) {
                perror("Mmap kvm run failed: ");
                exit(1);
            } else {
                vcpu->runThread = _thread;
                self->kvm->vcpus++ = vcpus;
                ++self->kvm->vcpuNumber;
            }
        }
    }
}

static kvmAcceleratorRunVM(void *_self) {
    _KVMAccelerator *self = _self;
    KVMState *kvm = self->kvm;
    VCPUState *vcpus = kvm->vcpus;
                    
    int i = 0;
    for (; i < kvm->vcpuNumber; ++i) {
        if (pthread_create(&(vcpus[i].vcpuThread,
                             (const pthread_attr_t *)NULL, vcpus[i].runThread, kvm) != 0) {
            perror("Create kvm thread failed: ");
        }         
    }

    for (i = 0; i < kvm->vcpuThread; ++i) {
        pthread_join(&vcpus[i].vcpuThread, NULL);
    }
}

static const _KVMAccelerator _kvmAccelerator = {
    sizeof(_KVMAccelerator),
    kvmAcceleratorCtor,
    kvmAcceleratorDtor,
    kvmAcceleratorInit,
    kvmAcceleratorCreateVM,
    kvmAcceleratorInitVCPU,
    kvmAcceleratorRunVM
};

const void *KVMAccelerator = &_kvmAccelerator;

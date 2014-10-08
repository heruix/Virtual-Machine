#include "new.h"
#include "virtualMachine.h"
#include "types.h"

#include <stdio.h>

#define BINARY_FILE "test.bin"
#define CODE_START 0x1000

void resetVCPU(VCPUState *vcpu) {
	if (ioctl(vcpu->fd, KVM_GET_SREGS, &(vcpu->sregs)) < 0) {
		perror("can not get sregs\n");
		exit(1);
	}

	vcpu->sregs.cs.selector = CODE_START;
	vcpu->sregs.cs.base = CODE_START * 16;
	vcpu->sregs.ss.selector = CODE_START;
	vcpu->sregs.ss.base = CODE_START * 16;
	vcpu->sregs.ds.selector = CODE_START;
	vcpu->sregs.ds.base = CODE_START *16;
	vcpu->sregs.es.selector = CODE_START;
	vcpu->sregs.es.base = CODE_START * 16;
	vcpu->sregs.fs.selector = CODE_START;
	vcpu->sregs.fs.base = CODE_START * 16;
	vcpu->sregs.gs.selector = CODE_START;

	if (ioctl(vcpu->fd, KVM_SET_SREGS, &vcpu->sregs) < 0) {
		perror("can not set sregs");
		exit(1);
	}

	vcpu->regs.rflags = 0x0000000000000002ULL;
	vcpu->regs.rip = 0;
	vcpu->regs.rsp = 0xffffffff;
	vcpu->regs.rbp= 0;

	if (ioctl(vcpu->fd, KVM_SET_REGS, &(vcpu->regs)) < 0) {
		perror("KVM SET REGS\n");
		exit(1);
	}
}

void *vcpuThread(void *data) {
    VCPUThreadParameter *vcpuThreadParameter = (VCPUThreadParameter*)data;
    KVMState *kvm = vcpuThreadParameter->kvm;
    int vcpuID = vcpuThreadParameter->vcpuID;
    VCPUState *vcpu = kvm->vcpus[vcpuID];

	int ret = 0;
	resetVCPU(vcpu);

	while (1) {
		fprintf(stdout, "KVM start run\n");
        fprintf(stdout, "VCPUID: %d\n", vcpuID);
        
		ret = ioctl(vcpu->fd, KVM_RUN, 0);
	
		if (ret < 0) {
			fprintf(stderr, "KVM_RUN failed\n");
			exit(1);
		}

		switch (vcpu->kvmRun->exit_reason) {
		case KVM_EXIT_UNKNOWN:
			fprintf(stderr, "KVM_EXIT_UNKNOWN\n");
			break;
		case KVM_EXIT_DEBUG:
			fprintf(stderr, "KVM_EXIT_DEBUG\n");
			break;
		case KVM_EXIT_IO:
			fprintf(stderr, "KVM_EXIT_IO\n");
			fprintf(stderr, "out port: %d, data: %d\n", 
				vcpu->kvmRun->io.port,  
				*(int *)((char *)(vcpu->kvmRun) + vcpu->kvmRun->io.data_offset)
				);
			sleep(1);
			break;
		case KVM_EXIT_MMIO:
			fprintf(stderr, "KVM_EXIT_MMIO\n");
			break;
		case KVM_EXIT_INTR:
			fprintf(stderr, "KVM_EXIT_INTR\n");
			break;
		case KVM_EXIT_SHUTDOWN:
			fprintf(stderr, "KVM_EXIT_SHUTDOWN\n");
			goto exit_kvm;
			break;
		default:
			fprintf(stderr, "KVM PANIC\n");
			goto exit_kvm;
		}
	}

exit_kvm:
    return 0;
}

int main(int argc, char *argv[]) {
    void *loader = New(BinaryLoader);
    void *accelerator = New(KVMAccelerator);

    void *vm = New(X86VirtualMachine);

    SetProgramLoader(vm, loader);
    SetVirtualAccelerator(vm, accelerator);

    VMParameter vmParam;
    vmParam.vcpuNumber = 3;
    vmParam.ramSize = 512000000;
    vmParam.thread = vcpuThread;
    
    CreateVirtualMachine(vm, &vmParam);
    LoadProgramImage(vm, BINARY_FILE);
    RunVirtualMachine(vm);
    
    return 0;
}

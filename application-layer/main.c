#include "new.h"
#include "vcpuThread.h"
#include "virtualMachine.h"
#include "types.h"

#define BINARY_FILE "image.bin"

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

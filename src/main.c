#include "new.h"
#include "binaryLoader.h"
#include "kvm.h"
#include "x86VirtualMachine.h"

int main(int argc, char *argv[]) {
    void *loader = New(BinaryLoader);
    void *accelerator = New(KVM);

    void *vm = New(X86VirtualMachine);

    SetProgramLoader(vm, loader);
    SetVirtualAccelerator(vm, accelerator);

    RunVirtualMachine(vm);
    
    return 0;
}

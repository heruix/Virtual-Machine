#include "virtualMachine.h"
#include "x86VirtualMachine.h"
#include "loader.h"
#include "accelerator.h"
#include "new.h"
#include <assert.h>

static void *x86VirtuaLMachineCtor(void *_self, va_list *_params) {
    _X86VirtualMachine *self = _self;

    self->loader = NULL;
    self->accelerator = NULL;

    return self;
}

static void *x86VirtuaLMachineDtor(void *_self) {
    _X86VirtualMachine *self = _self;

    Delete(self->loader);
    Delete(self->accelerator);
    
    self->loader = NULL;
    self->accelerator = NULL;

    return self;
}

static void x86VirtuaLMachineSetProgramLoader(void *_self, void *_loader) {
    _X86VirtualMachine *self = _self;

    self->loader = _loader;
}

static void x86VirtualMachineSetVirtualAccelerator(void *_self, void *_accelerator) {
    _X86VirtualMachine *self = _self;

    self->accelerator = _accelerator;
}

static void x86VirtuaLMachineCreateVirtualMachine(void *_self, VMParameter *_params) {
    _X86VirtualMachine *self = _self; 
    Accelerator **accelerator = self->accelerator;

    assert(self->accelerator && *accelerator && (*accelerator)->createVM &&
           (*accelerator)->initVCPU);

    (*accelerator)->createVM(self->accelerator, _params->ramSize);

    int i = 0;
    for (; i < _params->vcpuNumber; ++i) {
        (*accelerator)->initVCPU(self->accelerator, i, _params->thread);
    }
}

static void x86VirtuaLMachineLoadProgramImage(void *_self, char *_name) {
    _X86VirtualMachine *self = _self; 
    Loader **loader = self->loader;
    Accelerator **accelerator = self->accelerator;
    
    assert(self->loader && *loader && (*loader)->loadProgramImage);
    assert(self->accelerator && *accelerator && (*accelerator)->getVMRamBaseaddr);
    
    (*loader)->loadProgramImage(self->loader,
                                _name,
                                (*accelerator)->getVMRamBaseaddr(self->accelerator));
}

static void x86VirtuaLMachineRun(void *_self) {
    _X86VirtualMachine *self = _self; 
    Accelerator **accelerator = self->accelerator;

    assert(self->accelerator && *accelerator && (*accelerator)->runVM);

    (*accelerator)->runVM(self->accelerator);
}

static const VirtualMachine _x86VirtualMachine = {
    sizeof(_X86VirtualMachine),
    x86VirtuaLMachineCtor,
    x86VirtuaLMachineDtor,
    x86VirtuaLMachineSetProgramLoader,
    x86VirtualMachineSetVirtualAccelerator,
    x86VirtuaLMachineCreateVirtualMachine,
    x86VirtuaLMachineLoadProgramImage,
    x86VirtuaLMachineRun
};

const void *X86VirtualMachine = &_x86VirtualMachine;

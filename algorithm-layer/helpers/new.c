#include "new.h"
#include "abstractClass.h"
#include <assert.h>

void *New(const void *_class, ...) {
    const AbstractClass *class = _class;

    void *p = calloc(1, class->size);

    assert(p);
    *(const AbstractClass **)p = class;

    if (class->ctor) {
        va_list params;

        va_start(params, _class);
        p = class->ctor(p, &params);
        va_end(params);
    }

    return p;
}

void Delete(void *_class) {
    const AbstractClass **class = _class;

    if (_class && *class && (*class)->dtor) {
        _class = (*class)->dtor(_class);
    }

    free(_class);
}

void SetProgramLoader(void *_vm, void *_loader) {
    VirtualMachine **vm = _vm;

    if (_vm && *vm && (*vm)->setProgramLoader) {
        (*vm)->setProgramLoader(_vm, _loader);
    }
}

void SetVirtualAccelerator(void *_vm, void *_accelerator) {
    VirtualMachine **vm = _vm;

    if (_vm && *vm && (*vm)->setVirtualAccelerator) {
        (*vm)->setVirtualAccelerator(_vm, _accelerator);
    }
}

void CreateVirtualMachine(void *_vm, VMParameter *_params) {
    VirtualMachine **vm = _vm;

    if (_vm && *vm && (*vm)->createVirtualMachine) {
        (*vm)->createVirtualMachine(_vm, _params);
    }
}

void LoadProgramImage(void *_vm, char *_name) {
    VirtualMachine **vm = _vm;

    if (_vm && *vm && (*vm)->loadProgramImage) {
        (*vm)->loadProgramImage(_vm, _name);
    }    
}

void RunVirtualMachine(void *_vm) {
    VirtualMachine **vm = _vm;

    if (_vm && *vm && (*vm)->run) {
        (*vm)->run(_vm);
    }
}

#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include <stdarg.h>
#include <linux/kvm.h>

typedef struct {
    size_t size;
    void *(*ctor)(void *_self, va_list *_params);
    void *(*dtor)(void *_self);
    void (*loadProgramImage)(void *_self, char *_name, __u64 _ramBaseaddr);
} Loader;

#endif

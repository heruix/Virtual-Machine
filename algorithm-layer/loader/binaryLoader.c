#include "loader.h"
#include "binaryLoader.h"
#include <fcntl.h>
#include <stdlib.h>

static void *binaryLoaderCtor(void *_self, va_list *_params) {
    _BinaryLoader *self = _self;

    return self;
}

static void *binaryLoaderDtor(void *_self) {
    _BinaryLoader *self = _self;

    return self;
}

static void binaryLoaderLoadProgramImage(void *_self, char *_name, __u64 _ramBaseaddr) {
    _BinaryLoader *self = _self;
    int fd = open(_name, O_RDONLY);

    if (fd < 0) {
        perror("Open program image faile");
        exit(1);
    } else {
        struct stat imageState;  
        stat(_name, &imageState);
        
        char *p = (char*)_ramBaseaddr;
        int ret = read(fd, p, imageState.st_size);
        if (ret <= 0) {
            perror("Load program image failed");
            exit(1);
        } else {
            fprintf(stdout, "Load program image successfully, image size: %d\n", ret);
        }
    }
}

static const Loader _binaryLoader = {
    sizeof(_BinaryLoader),
    binaryLoaderCtor,
    binaryLoaderDtor,
    binaryLoaderLoadProgramImage
};

const void *BinaryLoader = &_binaryLoader;

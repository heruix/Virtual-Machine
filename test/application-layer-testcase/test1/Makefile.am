ACLOCAL_AMFLAGS = -I m4

LIBS =

bin_PROGRAMS = image.bin
image_bin_SOURCES = test1.S
image_bin_CPPFLAGS = -m32
image_bin_LDFLAGS = -N -e _start -Ttext 0x10000 -nostartfiles -nodefaultlibs -Xlinker --oformat -Xlinker binary -Xlinker -m -Xlinker elf_i386

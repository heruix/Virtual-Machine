ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = vmm
vmm_SOURCES = main.c vcpuThread.c
vmm_CFLAGS = -I$(top_srcdir)/algorithm-layer/include 
vmm_LDADD = $(top_srcdir)/algorithm-layer/accelerator/libKvmAccelerator.la \
                       $(top_srcdir)/algorithm-layer/helpers/libHelpers.la \
					   $(top_srcdir)/algorithm-layer/loader/libBinaryLoader.la \
					   $(top_srcdir)/algorithm-layer/virtualMachine/libVirtualMachine.la

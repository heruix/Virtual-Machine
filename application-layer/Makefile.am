ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = VirtualMachine
VirtualMachine_SOURCES = main.c
VirtualMachine_CPPFLAGS = -I$(top_srcdir)/algorithm-layer/include 
VirtualMachine_LDADD = $(top_srcdir)/algorithm-layer/accelerator/libKvmAccelerator.la \
                       $(top_srcdir)/algorithm-layer/helpers/libHelpers.la \
					   $(top_srcdir)/algorithm-layer/loader/libBinaryLoader.la \
					   $(top_srcdir)/algorithm-layer/virtualMachine/libVirtualMachine.la

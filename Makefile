ccflags-y := -fno-pie -Wno-error=date-time -Wdate-time
obj-m := EnsiCaen_ldd.o
EnsiCaen_ldd-objs := tuxit.o

all:
	make -C /lib/modules/4.15.0-66-generic/build  M=`pwd` modules
clean:
	make -C /lib/modules/4.15.0-66-generic/build  M=`pwd` clean

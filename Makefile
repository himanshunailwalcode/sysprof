ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m += sysprof.o
sysprof-objs := module/sysprof.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

KVER   ?= $(shell uname -r)
KDIR   ?= /lib/modules/$(KVER)/build/
CC     ?= gcc
obj-m   = rockusb.o
CFLAGS_rockusb.o := -Idrivers/usb/storage

all: rockusb.ko

rockusb.ko: rockusb.c
	make -C $(KDIR) M=$(CURDIR) modules

clean:
	-make -C $(KDIR) M=$(CURDIR) clean
	-rm -f *.o modules.order

install: | rockusb.ko
	make -C $(KDIR) M=$(CURDIR) modules_install INSTALL_MOD_PATH=$(DESTDIR)

.PHONY: all install clean

CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
CC = gcc -Wall

TARGET = logic_bomb

obj-m      := $(TARGET).o

all: default

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	cp -v $(TARGET).ko $(DEST)
	/sbin/depmod
	/sbin/insmod $(TARGET).ko
	/sbin/lsmod | grep $(TARGET)

uninstall:
	/sbin/rmmod $(TARGET)
	rm -v $(DEST)/$(TARGET).ko
	/sbin/depmod

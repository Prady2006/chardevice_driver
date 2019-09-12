obj-m := usbdriver.o
obj-m := usbdriver_read_write.o
obj-. := modified.o
all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 

obj-m := abcde.o
obj-m += modified.o
obj-m += 2modified.o
all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 

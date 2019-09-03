#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#define DEVICE_NAME "Pradytronix"

dev_t dev = 0 ;
static struct class *dev_class ;

static int __init startmodule(void){
    
    if( (alloc_chrdev_region(&dev , 0 , 1 ,DEVICE_NAME )) < 0 ){
        printk(KERN_INFO" cannot allocate major number to device \n" ) ;
        return -1 ;
    }
    printk(KERN_INFO " Major number : %d and Minor number : %d \n",MAJOR(dev) , MINOR(dev)) ;
    printk(KERN_INFO "Kernel Module Installed Successfully \n") ; 
    /* creating struct class */
    if( (dev_class = class_create(THIS_MODULE,"PRADY_CLASS")) == NULL){
        printk(KERN_INFO "Cannot create class \n");
        goto r_class ;
    }
    // creating device 
    if((device_create(dev_class,NULL,dev,NULL,"PRADY_DEVICE"))== NULL){
        printk(KERN_INFO"Cannot create device\n");
        goto r_device ;
    }
    return 0 ;
r_class:
    class_destroy(dev_class) ;
r_device:
    unregister_chrdev_region(dev,1);
    return -1 ; 
}

static void __exit closemodule(void){
    device_destroy(dev_class,dev);
    class_destroy(dev_class);    
    printk(KERN_INFO"trying to unregister") ;
    unregister_chrdev_region(dev,1) ;
    printk(KERN_INFO"kernel module removed successfully \n " ) ;

}
module_init(startmodule) ;
module_exit(closemodule) ;
MODULE_LICENSE("GPL") ;
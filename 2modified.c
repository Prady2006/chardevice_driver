#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm-generic/io.h>

static dev_t first ;
static char mem[PAGE_SIZE]  ;
static struct class *cl ;
static struct device *dev_ret ;
static struct cdev c_dev ;
// static char c ;

static int filerelease (struct inode *n, struct file *f_ops){

    printk(KERN_INFO"driver close\n") ;
    return 0;
}
static int fileopen (struct inode *n, struct file *f_ops){

    printk(KERN_INFO"driver open\n") ;
    return 0;
}

static ssize_t fileread (struct file *f_ops, char __user *buff, size_t len, loff_t *offset){

    printk(KERN_INFO"driver read\n") ;
    int read_bytes ;
    int bytes_to_read ;
    if(*offset == 0)
        return -EFAULT ;
    int max_read  = PAGE_SIZE - *offset;
    
    if(max_read > len )
        bytes_to_read = len ;
    else 
        bytes_to_read = max_read ;
    
    read_bytes = bytes_to_read - copy_to_user(buff , mem + *offset , bytes_to_read );
    *offset += read_bytes ;
    return read_bytes ;
}
static ssize_t filewrite (struct file *f_ops, const char __user *buff, size_t len, loff_t *offset){

    printk(KERN_INFO"driver write\n") ;
    int copied_bytes ;
    int bytes_to_copy ;
    int max_bytes  = PAGE_SIZE - *offset;
    if(*offset == 0)
        return -EFAULT ;
    if(max_bytes > len )
        bytes_to_copy = len ;
    else 
        bytes_to_copy = max_bytes ;
    copied_bytes = bytes_to_copy - copy_from_user(buff + *offset , mem , bytes_to_copy ) ;
    *offset += copied_bytes ;
    return copied_bytes ;
}

static struct file_operations f_ops ={
    .owner = THIS_MODULE , 
    .open = fileopen ,
    .release = filerelease ,
    .read = fileread ,
    .write = filewrite ,
};

static int __init vramfuncinit(void){
    int ret ;

    if( ( ret = alloc_chrdev_region(&first,0,1,"vram") ) < 0 ){
        printk(KERN_INFO"Cannot assign an inode number \n" ) ;
        return ret ;
    }

   printk(KERN_INFO"Module inserted \n");

   printk(KERN_INFO"MAJOR : %d and MINOR : %d " ,MAJOR(first) ,MINOR(first) ) ;


   if( IS_ERR(cl = class_create(THIS_MODULE,"P_DEVICE") ) ) {
      unregister_chrdev_region(first,1) ;
      return PTR_ERR(cl) ;
}

   if(IS_ERR(dev_ret = device_create(cl,NULL,MKDEV(MAJOR(first),MINOR(first)),NULL,"vram_driver") ) ){

      class_destroy(cl) ;
      unregister_chrdev_region(first,1) ;
      return PTR_ERR(dev_ret) ;

}
   cdev_init(&c_dev,&f_ops);
   printk(KERN_INFO"cdev is initialized\n"); 
   
   if( ( ret = cdev_add(&c_dev,first,1) ) < 0 ){
      device_destroy(cl,first) ;
      class_destroy(cl) ;
      unregister_chrdev_region(first,1) ;
}
   printk(KERN_INFO"cdev is added to list\n") ;
   
   return 0 ;

}


static void __exit vramfuncexit(void){

        cdev_del(&c_dev) ;
        device_destroy(cl,first) ;
        class_destroy(cl) ;

        unregister_chrdev_region(first,1) ;
//        unregister_chrdev_region(first,1) ;

        printk(KERN_INFO"device unregistered \n" ) ;

}

module_init(vramfuncinit);
module_exit(vramfuncexit);
MODULE_LICENSE("GPL");

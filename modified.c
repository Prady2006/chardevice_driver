#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
#define BUFFER_SIZE 1024
char *device_buf ;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
 
static struct file_operations fops =
{
.owner          = THIS_MODULE,
.read           = etx_read,
.write          = etx_write,
.open           = etx_open,
.release        = etx_release,
};
 
static int etx_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Opened\n");
        return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Released\n");
        return 0;
}
 
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Driver Reading \n");
        if(*off > BUFFER_SIZE){
                printk(KERN_INFO"OFF > BUFFER_SIZE");
                return 0 ;
        }
        if(*off + len > BUFFER_SIZE){
                len = BUFFER_SIZE - *off ;
        }
        printk(KERN_INFO"Reading initiated \n");
        if(copy_to_user(buf ,device_buf + *off, len ) != 0)
                return -EFAULT ;
        *off += len ;
        return len;
}
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        int bytes_to_copy ;
        printk(KERN_INFO "Driver Writing\n");
        if(BUFFER_SIZE - *off - 1 <= len){
                bytes_to_copy = BUFFER_SIZE - *off - 1 ;
                printk(KERN_INFO"buff - *off -1 <= len");
        }else{
                bytes_to_copy = len ;
                printk(KERN_INFO"buff - *off - 1 > len " );
        }
        copy_from_user(device_buf + *off ,buf, bytes_to_copy);
        device_buf[bytes_to_copy + *off] = '\0' ;
        *off += bytes_to_copy ;
        return bytes_to_copy;
}
 
 
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "Device Driver Inserted\n");
    return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Removed\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");

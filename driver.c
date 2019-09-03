#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

#define DEVICE_NAME "driver"
#define BUFF_LEN 80 
#define SUCCESS 0 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LEX_MARTIN");

static int majornumber ;
static char message[256] = {0} ;
static char *msg ;
static int Device_open = 0 ; 

static int device_open(struct inode *,struct file *);
static int device_release(struct inode *,struct file *);
static ssize_t device_read(struct file * , char *,size_t ,loff_t *);
static ssize_t device_write(struct file *,const char *,size_t,loff_t *) ;

static struct file_operations fops = {
	.open = device_open ,
	.read = device_read ,
	.write = device_read ,
	.release = device_release ,
};


static int __init start(void){
	printk(KERN_INFO"Initializing the driver\n");
	majornumber = register_chrdev(0,DEVICE_NAME,0666);
	if(majornumber < 0 ){
		printk(KERN_INFO"failed to register\n");
		return majornumber ;
	}
	printk(KERN_INFO "driver registered\n") ;
	return SUCCESS ;
}

static void __exit end(void){
	int ret = unregister_chrdev(majornumber,DEVICE_NAME);
	if(ret < 0){
		printk(KERN_INFO"error in unregistering device\n");

	}
}
static int device_open(struct inode *inode ,struct file *file){
	static int count = 0 ;
	if(Device_open)
		return -EBUSY ;
	Device_open++ ;
	sprintf(msg,"I already told you %d times hello world\n",count++);
	msg = message ;
	try_module_get(THIS_MODULE); 
	return SUCCESS ;
}
static int device_release(struct inode *inode , struct file *file ){
	Device_open-- ;
	module_put(THIS_MODULE);
	return 0 ;
}
static ssize_t device_read(struct file *filp , char *buffer , size_t length,loff_t *offset){
	int bytes_read = 0 ; 
	if(msg == 0 )
		return 0 ; 
	
	while(msg && )
}
/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name:Pramod Khatri
* Student ID:920831584
* GitHub ID:pramodkhatri10
* Project: Device Driver
*
* File: mydevice.c
*
* Description: implements device driver open, read, write, release and icotl function as well as starting and ending the driver
*
**************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                
#include<linux/uaccess.h>            

 
 
#define WRITEVALUE _IOW('a','a',int32_t*)
#define READVALUE _IOR('a','b',int32_t*)
 
static int isOpen = 0;
int num_bytes = 0; 
int32_t value = 0;
dev_t dev = 0;

static char message [1024];

static struct class *mydevice_class;
static struct cdev mydevice_cdev;


//definition of device driver functions
static int      mydevice_open(struct inode *inode, struct file *filep);
static ssize_t  mydevice_read(struct file *filep, char __user *outb, size_t nbytes,loff_t * offset);
static ssize_t  mydevice_write(struct file *filep, const char *inpb, size_t nbytes, loff_t * offset);
static long     mydevice_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);
static int      mydevice_release(struct inode *inode, struct file *filep);
static int      __init mydevice_start(void);
static void     __exit mydevice_end(void);


//file operation structure
static struct file_operations fops =
{

        .open           = mydevice_open,
        .read           = mydevice_read,
        .write          = mydevice_write,
        .unlocked_ioctl = mydevice_ioctl,
        .release        = mydevice_release,
};


//function for opening the device file
static int mydevice_open(struct inode *inode, struct file *filep)
{
        if (isOpen == 1) { //if file is open value is 1 else 0
	  printk(KERN_INFO "Error: device already opened \n");	
	  return -EBUSY; //return negative 
	}
	isOpen = 1; //file open
        printk(KERN_INFO "\tDevice Driver File Opened\n");
        return 0;
}



//function for reading from device driver
static ssize_t mydevice_read(struct file *filep, char __user *outb, size_t nbytes, loff_t *offset)
{
        int bytesread = 0;

	if (*offset == NULL)
	return -EINVAL;
	if (*offset >= num_bytes)
	return 0;

	while ((bytesread < nbytes) && (*offset < num_bytes)) {
		__user(message [*offset], outb[bytesread]);
	*offset++;
	bytesread++;
	}
        printk(KERN_INFO "\tRead Device file\n");
	return bytesread;
        return 0;
}


//function for writing to device driver
static ssize_t mydevice_write(struct file *filep, const char __user *inpb, size_t nbytes, loff_t *offset)
{
        int byteswritten = 0;
	if (*offset == NULL)
		return -EINVAL;
	if (*offset >= num_bytes)
		return -EINVAL;
	
	while ((byteswritten < nbytes) && (*offset < num_bytes)) {
			__user(message [*offset], inpb[byteswritten]);
		*offset++;
		byteswritten++;
		}
                printk(KERN_INFO "\tWrite to Device Driver\n");
		return byteswritten;	
        return 0;
}



//function for IOCTL operation in device file
static long mydevice_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WRITEVALUE:
                        copy_from_user(&value ,(int32_t*) arg, sizeof(value));
                        printk(KERN_INFO "IO Value : %d\n", value);
                        break;
                case READVALUE:
                        copy_to_user((int32_t*) arg, &value, sizeof(value));
                        break;
        }
        return 0;
}

//function to release the device driver
static int mydevice_release(struct inode *inode, struct file *filep)
{
        if (isOpen == 0 ) { //if device not opened
	  printk(KERN_INFO "ERROR: device was not opened \n");
	  return -EBUSY;
	}

        printk(KERN_INFO "\tDevice Driver File Closed\n");
        isOpen = 0; //keeping track of device file
        return 0;
}
 
//function to startialize the device driver
//allocates and validates major number
//creates device structure and adds to system
//creates struct class and device 
static int __init mydevice_start(void)
{
        if((alloc_chrdev_region(&dev, 0, 1, "mydevice_Dev")) <0){
                printk(KERN_INFO "ERROR: major number allocation failed! n");
                return -EINVAL;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        cdev_init(&mydevice_cdev,&fops);
 
        if((cdev_add(&mydevice_cdev,dev,1)) < 0){
            printk(KERN_INFO "ERROR: could not add to driver.\n");
            goto r_class;
        }
 
        if((mydevice_class = class_create(THIS_MODULE,"mydevice_class")) == NULL){
            printk(KERN_INFO "ERROR: could not create struct class\n");
            goto r_class;
        }
 
        if((device_create(mydevice_class,NULL,dev,NULL,"mydevice_device")) == NULL){
            printk(KERN_INFO "ERROR: could not create Device Driver\n");
            goto r_device;
        }
        printk(KERN_INFO "\tDevice Driver Installation Succeed.\n");
        return 0;
 
r_device:
        class_destroy(mydevice_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -EINVAL;
}

//function to end the device driver
static void __exit mydevice_end(void)
{
        device_destroy(mydevice_class,dev);
        class_destroy(mydevice_class);
        cdev_del(&mydevice_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "\tDevice Driver Removed Success.\n");
}
 
module_init(mydevice_start);
module_exit(mydevice_end);
 

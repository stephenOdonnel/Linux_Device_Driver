/*
 * Linux Device Driver
 * 
 * ENSICAEN training template
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 * Author : Stephen
 * Ensicaen 3A SATE
 * Date 2019 - 2020
 */

#include <linux/module.h>	
#include <linux/cdev.h>  
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/semaphore.h>


#define BUFFER_SIZE 64


struct cdev char_dev;
static int param_verbose=0;
dev_t dev_num;
int err;
int read_activation;
static char buffer[BUFFER_SIZE];
static struct semaphore sem;


 	
 	
module_param_named(verbose,param_verbose,int,S_IRUGO);
MODULE_PARM_DESC(verbose, "0: silent, 1: verbose");
 


int cdev_open(struct inode *pInode, struct file *pFile) {
	
	printk(KERN_DEBUG "open()\n");
	return 0;
}

long cdev_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{

	int activ = 0;
	int err = 0;

	printk(KERN_INFO "mytuxit:%s\n",__func__);

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch (_IOC_NR(cmd))
	{
	        case 1:
		{
			printk(KERN_INFO "mytuxit:%s: ENSICAEN_READ_BLOCKING\n",__func__);
			if (copy_from_user(&activ,(const char __user *)arg,sizeof(BUFFER_SIZE)))
			{
				printk(KERN_INFO "mytuxit:%s: ENSICAEN_READ_BLOCKING: error on copy_from_user\n",__func__);
				return -EACCES;
			}
			printk(KERN_INFO "mytuxit:%s: ENSICAEN_READ_BLOCKING: activation=%d\n",__func__,(int)activ);
			if (activ== 0){
				read_activation = 0;
			}else{
				read_activation = 1;
				break;
			}
		}

		default:
		{
			/* unrecognized ioctl */	
			printk(KERN_INFO "mytuxit:%s:unknown ioctl number: 0x%x/0x%x\n",__func__,cmd,_IOC_NR(cmd));
			return -ENOIOCTLCMD;
		}
	}
	return 0;
}

 

ssize_t cdev_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
	
	printk(KERN_DEBUG "read: wait for someone to write\n");

	down(&sem);
	
	printk(KERN_DEBUG "read: demande lecture de %d octets\n", size);
	
	int lus = 0;
	if (size <= BUFFER_SIZE - (int)*offset){
		lus = size;
	}else{  
		lus = BUFFER_SIZE - (int)*offset;
	}
	if(lus){
		copy_to_user(user_buffer, buffer + *offset, lus);
	}

	*offset += lus;
	printk(KERN_DEBUG "read: %d octets reellement lus\n", lus);
	
	return lus;
}

ssize_t cdev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
	
	
	printk(KERN_DEBUG "write()\n");
	size_t real;
	real= min((size_t)BUFFER_SIZE, size);
	if (real){
		if (copy_from_user(buffer, user_buffer, real)){
			return -EFAULT;
		}
	}
	size =real;
	
	printk(KERN_DEBUG "write:  writing of %d / %d char > %s\n", real, size, buffer);
	printk(KERN_DEBUG "write: enabling reading function\n");
	
	up(&sem);
	return real;
	

}

//int cdev_release/close(struct inode *pInode, struct file *pFile) {;}
 
 static struct file_operations cdev_fops = {
   owner:    THIS_MODULE,
   open:     cdev_open,
   write:	 cdev_write,
   //release:  cdev_release,
   //close:    cdev_close,
   read:     cdev_read,
  
   unlocked_ioctl: cdev_ioctl
 
 };

static int init(void)
{
	if(param_verbose==1){

		err = alloc_chrdev_region(&dev_num, 0, 1, "EnsiCaen_ldd");
		sema_init(&sem,0);
		cdev_init(&char_dev, &cdev_fops);
		char_dev.owner = THIS_MODULE;
		err = cdev_add(&char_dev, dev_num, 1);
		if (err){
		 unregister_chrdev_region(dev_num, 1);
		}   
		 if (err){ 
			return err;
		  }

		printk(KERN_INFO "mytuxit init: %s %s\n",__DATE__, __TIME__);	

		printk(KERN_INFO "mytuxit init: (%d,%d)\n",MAJOR(dev_num), MINOR(dev_num));

		return 0;    // Non-zero return means that the module couldn't be loaded.
	}

}

static void bye(void)
{
	if(param_verbose==1){
		printk(KERN_INFO "mytuxit exit\n");
		
	}
	cdev_del(&char_dev); // enleve l'allocation dynamique créé avec cdev add
	unregister_chrdev_region(dev_num, 1); //prevents module ghost character
	//unregister_chrdev_region(MKDEV(243,0),1); // to unregister a gost with the major 243
	
}

module_init(init);
module_exit(bye);

MODULE_LICENSE("GPL");	
MODULE_AUTHOR("www.ensicaen.com");


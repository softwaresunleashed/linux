

#include <linux/module.h>
#include <linux/kernel.h>

// Char Device Driver Support
#include <linux/fs.h>		// register_chrdev()
#include <linux/semaphore.h>

#include "scull.h"

static loff_t scull_llseek(struct file *file, loff_t offset, int origin)
{

	return 0; //dummy return to rectify compilation issue
}

static Scull_Dev * scull_follow(Scull_Dev * dev, int item)
{
	return dev->data;
}

static ssize_t scull_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	Scull_Dev *dev = filp->private_data; /* the first list item */
	Scull_Dev *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the list item */
	int item, s_pos, q_pos, rest;
	ssize_t ret = 0;

	
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if (*f_pos >= dev->size)
		goto out;

	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	/* find list item, qset index, and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;

	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position (defined elsewhere) */
	dptr = scull_follow(dev, item);

	if (!dptr->data)
		goto out; /* don’t fill holes */
	
	if (!dptr->data[s_pos])
		goto out;
	
	/* read only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_to_user(buf, dptr->data[s_pos]+q_pos, count)) {
		ret = -EFAULT;
			goto out;
	}
	*f_pos += count;
	ret = count;

	out:
	up(&dev->sem);
	return ret;
}

ssize_t scull_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	Scull_Dev *dev = filp->private_data;
	Scull_Dev *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t ret = -ENOMEM; /* value used in "goto out" statements */
	
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* find list item, qset index and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position */
	dptr = scull_follow(dev, item);
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);

		if (!dptr->data)
			goto out;

		memset(dptr->data, 0, qset * sizeof(char *));
	}
	
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);

		if (!dptr->data[s_pos])
			goto out;
	}
	
	/* write only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
		ret = -EFAULT;
		goto out;
	}
	
	*f_pos += count;
	ret = count;

	/* update the size */
	if (dev->size < *f_pos)
		dev-> size = *f_pos;

	out:
	up(&dev->sem);
	return ret;

}


static int scull_trim(Scull_Dev * dev)
{
	Scull_Dev * next, *dptr;

	int qset = dev->qset;	/* "dev" is not null */
	int i;

	for(dptr = dev; dptr; dptr = next)	/* all the list items */
	{
		if(dptr->data)
		{
			for(i = 0; i < qset ; i++)
			{
				if(dptr->data[i])
				{
					kfree(dptr->data[i]);
				}
			}
			kfree(dptr->data);
			dptr->data = NULL:
		}
		next = dptr->next;
		if(dptr != dev)
			kfree(dptr);	// free all , but the first
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->next = NULL;
	return 0;
}


static int scull_open(struct inode *inode, struct file * filp)
{
	Scull_Dev *dev; 	/* Device Information */

	int dev_num = NUM(inode->i_rdev); 
	int dev_type = TYPE(inode->i_rdev);

	/* 
		If private data is not valid, we are not using "devfs" 
		so use the dev_type(from minor nr), to select a new f_op
	*/
	if ( !filp->private_data && dev_type)
	{
		if(type > SCULL_MAX_TYPE)
			return -EINVAL;

		filp->f_op = scull_fop_array[type];

		// Need to call "open" method explicitly of new "f_ops" .
		return filp->f_op->open(inode, filp); 	// Dispatch to specific open
	}

	/* type0, check the device number (unless private data is valid) */
	// Private_data contains the device type and device number (in this driver case)
	dev = (Scull_Dev *) filp->private_data;
	if(!dev)
	{
		if(num >= scull_nr_devs)
			return -ENODEV;

		dev = &scull_devices[num];

		filp->private_data = dev;		
	}

	MOD_INC_USE_COUNT;		// Increment Module Usage count.

	if((filp->f_flags & O_ACCMODE) == O_WRONLY)
	{
		if(down_interruptible(&dev->sem))		// down_interruptible() -- obtain semaphore if available else sleep
		{
			MOD_DEC_USE_COUNT;	// Decrement Module Usage count.
			return -ERESTARTSYS;
		}
		scull_trim(dev);	/* ignore errors */

		/* Release the semaphore and wake up any process that is waiting for this semaphore */
		up(&dev->sem);			
	}
	return 0; 			/* success */
}

static int scull_release(struct inode *, struct file *filp)
{
	MOD_DEC_USE_COUNT;	// Decrement Module Usage count.
	
	return 0;
}




static int register_scull_drv(void)
{
	int scull_dyn_assigned = -1;
	
	scull_dyn_assigned = register_chrdev(scull_major, SCULL_DRV_NAME, &scull_fops );
	if(scull_dyn_assigned < 0)
	{
		printk(KERN_WARNING "scull : can't get major %d\n", scull_dyn_assigned);
		return scull_dyn_assigned;
	}
	
	if(scull_major == 0)
	{
		scull_major = scull_dyn_assigned;
		printk(KERN_INFO "scull : Scull Driver Installed. Major : %d", scull_dyn_assigned);
		return scull_dyn_assigned;	
	}
	


}

static void deregister_scull_drv(void)
{
	int scull_dyn_assigned = scull_major;
	int result = -1;

	result = unregister_chrdev(scull_major, SCULL_DRV_NAME);
	if(-EINVAL == result)
	{
		printk(KERN_INFO "scull : Problem UnInstalling SCULL Driver.");
	}
	else
	{
		printk(KERN_INFO "scull : Scull Driver Un-Installed.");
	}	


	// TODO : Delete all the relevant nodes / device files created during the module initialization here
	// Sudhanshu : 16 Jan 2013
	// delete_special_files();



	
	return;
	


}

/* Initialize quantum size, quantum set , and semaphores for individual SCULL devices; */
static void scull_init()
{
	int i;
	
	for(i = 0 ; i < scull_nr_devs; i++)
	{
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset	= scull_qset;
		sema_init(&scull_devices[i].sem , 1);
	}
}


static int __init scull_driver_init(void)
{
	int scull_dyn_assigned = -1;

	//SET_MODULE_OWNER(&scull_fops); // More portable...works on all versions including v2.4

	// Initialize device semaphores, quantum and quantum sets
	scull_init();
	
	scull_dyn_assigned = register_scull_drv();
	if(scull_dyn_assigned >=0)
	{	
		printk(KERN_INFO "scull : Scull Driver Installed.");
		return 0;	
	}
	else
	{
		printk(KERN_INFO "scull : Error loading Scull Driver.");
		return -1;
	}
	
}

static void __exit scull_driver_exit(void)
{

	deregister_scull_drv();

	printk(KERN_INFO "scull : Scull Driver Exit.");

	return;
}



module_init(scull_driver_init);
module_exit(scull_driver_exit);




MODULE_AUTHOR("Sudhanshu Gupta");
MODULE_DESCRIPTION("Driver for SCULL Char Device Driver");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("");





// Headers inclusion for Kernel Module Programming
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>		// struct file_operations





#include "../_utils/bcm_gpio.h"
#include "motor_control.h"



	
static void setup_motorcontroller_gpios(void)
{

#if 0
// Power GPIOs -- GPIOs used to supply power to Keypad circuit
// To be configured as OUTPUT
	// ROW (POWER) 
	select_alt_gpio_func(ROW_POW, GPIO_PIN_OUTPUT);
	clear_gpio(ROW_POW);

	// COL (POWER) 
	select_alt_gpio_func(COL_POW, GPIO_PIN_OUTPUT);
	clear_gpio(COL_POW);


// ROWs
	// ROW1 (OUTPUT) <==> GPIO_22 <==> Port1_Pin15 <==> INPUT
	select_alt_gpio_func(ROW1, GPIO_PIN_INPUT);
	set_gpio(ROW1);

	// ROW2 (OUTPUT) <==> GPIO_23 <==> Port1_Pin16 <==> INPUT
	select_alt_gpio_func(ROW2, GPIO_PIN_INPUT);
	set_gpio(ROW2);
	
	// ROW3 (OUTPUT) <==> GPIO_24 <==> Port1_Pin18 <==> INPUT
	select_alt_gpio_func(ROW3, GPIO_PIN_INPUT);
	set_gpio(ROW3);

	// ROW4 (OUTPUT) <==> GPIO_25 <==> Port1_Pin22 <==> INPUT
	select_alt_gpio_func(ROW4, GPIO_PIN_INPUT);
	set_gpio(ROW4);


// COLUMNs
	// COL1 (INPUT) <==>  GPIO_4  <==> Port1_Pin07 <==> INPUT
	select_alt_gpio_func(COL1, GPIO_PIN_INPUT);
	set_gpio(COL1);

	// COL2 (INPUT) <==> GPIO_27 <==> Port1_Pin13 <==> INPUT
	select_alt_gpio_func(COL2, GPIO_PIN_INPUT);
	set_gpio(COL1);
	
	// COL3 (INPUT) <==> GPIO_17 <==> Port1_Pin11 <==> INPUT
	select_alt_gpio_func(COL3, GPIO_PIN_INPUT);
	set_gpio(COL3);
#endif
	return;
}


static void clean_motorcontroller_gpios(void)
{

#if 0
	clear_gpio(ROW1);
	clear_gpio(ROW2);
	clear_gpio(ROW3);
	clear_gpio(ROW4);

	clear_gpio(COL1);
	clear_gpio(COL2);
	clear_gpio(COL3);
#endif
	return;
}



static int register_motorcontroller_drv(void)
{
	int motorcontroller_dyn_assigned = -1;
	
	motorcontroller_dyn_assigned = register_chrdev(motorcontroller_major, MOTORCONTROLLER_DRV_NAME, &motorcontroller_fops);
	if(motorcontroller_dyn_assigned < 0)
	{
		printk(KERN_WARNING "motorcontroller : can't get major %d\n", motorcontroller_dyn_assigned);
		return motorcontroller_dyn_assigned;
	}
	
	if(motorcontroller_major == 0)
	{
		motorcontroller_major = motorcontroller_dyn_assigned;
		printk(KERN_INFO "motorcontroller : motorcontroller Driver Installed. Major : %d", motorcontroller_dyn_assigned);
		return motorcontroller_dyn_assigned;	
	}



	
	// TODO: Make node "/dev/motorcontroller"
	//	dev_t devno = MKDEV(cfake_major, minor);
	//   mknod(struct inode * dir,struct dentry * dentry, S_IFCHR,dev_t dev)
	// create_special_files();



	return motorcontroller_dyn_assigned;
}



static void deregister_motorcontroller_drv(void)
{
	int motorcontroller_dyn_assigned = motorcontroller_major;
	
	unregister_chrdev(motorcontroller_dyn_assigned, MOTORCONTROLLER_DRV_NAME);
	

	// TODO : Delete all the relevant nodes / device files created during the module initialization here
	// Sudhanshu : 16 Jan 2013
	// delete_special_files();

	return;
}

static ssize_t motorcontroller_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{


#if 0
	byte key_pressed = -1;
	char * key_str = NULL;
	
	printk(KERN_INFO "motorcontroller : Entered motorcontroller_read function");


	key_pressed = get_key_pressed();

		
	key_str = key_to_string_map(key_pressed);
	if(key_str)
		printk(KERN_INFO "KeyPress : %s", key_str);

#endif	

	return 0;

}


static ssize_t motorcontroller_write(struct file *file, const char __user *buf, size_t, loff_t *ppos)
{


	return 0;
}


static int motorcontroller_open(struct inode *inode, struct file *file)
{

	if (motorcontroller_open_cnt)
		return -EBUSY;	/* open only once at a time */

	if (file->f_mode & FMODE_WRITE)	/* device is read-only */
		return -EPERM;

	motorcontroller_buflen = 0;	/* flush the buffer on opening */

	//Increment Module usage count.
	motorcontroller_open_cnt++;
	
	return 0;
}

static int motorcontroller_release(struct inode *inode, struct file *file)
{
	// Decrement Module usage count.
	motorcontroller_open_cnt--;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/////////////   MOTORCONTROLLER CODE -- ENDS   //////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




static int __init motorcontroller_init(void)
{
	int motorcontroller_dyn_assigned = 0;
	
	printk(KERN_INFO "motorcontroller : Module Enter");

	// Perform all the GPIO peripheral-to-virtual memory mapping here.	
	setup_gpio_io();


	// Initialize the motorcontroller pins.
	setup_motorcontroller_gpios();
	
	// All set to Register the Keypad driver.
	motorcontroller_dyn_assigned = register_motorcontroller_drv();

	return 0;
}

static void __exit motorcontroller_exit(void)
{

	if(!motorcontroller_open_cnt)
	{
		printk(KERN_INFO "motorcontroller : Module in use. Could not un-load driver.");
		return;
	}
	
	// Nobody is using the driver, OK to un-load.
	deregister_motorcontroller_drv();

	// Call the freeing functions here.
	clean_motorcontroller_gpios();


	// Perform all cleanup activities here.
	cleanup_gpio_io();

	printk(KERN_INFO "motorcontroller : Module Exit");

	return;
}



module_init(motorcontroller_init);
module_exit(motorcontroller_exit);




MODULE_AUTHOR("Sudhanshu Gupta");
MODULE_DESCRIPTION("Driver for Motor Controller L298N.");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("");



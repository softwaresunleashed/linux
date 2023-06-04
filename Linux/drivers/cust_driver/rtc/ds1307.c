


// Headers inclusion for Kernel Module Programming
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>		// struct file_operations

#include "../_utils/basetypes.h"
#include "ds1307.h"




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////   RTC CODE -- BEGINS  /////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


I2C_BUS_HDL * rtc_ds1307_i2c_hdl = NULL;

	
static void setup_rtc_ds1307(void)
{

	// Initialize I2C bus resources
	rtc_ds1307_i2c_hdl = bcm_i2c_initialize();
	if(!rtc_ds1307_i2c_hdl)
		return;

	//  Configure individual pins (SDA, SCL)
	bcm_i2c_prepare_pin(I2C_SDA, GPIO_PIN_ALT_FUNC_0);
	bcm_i2c_prepare_pin(I2C_SCL, GPIO_PIN_ALT_FUNC_0);

	return;
}


static void clean_rtc_ds1307(void)
{
	// Cleanup I2C bus resources
	bcm_i2c_cleanup(rtc_ds1307_i2c_hdl);

	return;
}



static int register_rtc_ds1307_drv(void)
{
	int rtc_ds1307_dyn_assigned = -1;
	
	rtc_ds1307_dyn_assigned = register_chrdev(rtc_ds1307_major_nr, RTC_DRV_NAME, &rtc_fops);
	if(rtc_ds1307_dyn_assigned < 0)
	{
		printk(KERN_WARNING "rtc_ds1307 : Can't get major %d\n", rtc_ds1307_dyn_assigned);
		return rtc_ds1307_dyn_assigned;
	}
	
	if(rtc_ds1307_major_nr == 0)
	{
		rtc_ds1307_major_nr = rtc_ds1307_dyn_assigned;
		printk(KERN_INFO "rtc_ds1307 : RTC-DS1307 Driver Installed. Major : %d", rtc_ds1307_dyn_assigned);
		return rtc_ds1307_dyn_assigned;	
	}



	
	// TODO: Make node "/dev/rtc"
	//	dev_t devno = MKDEV(cfake_major, minor);
	//   mknod(struct inode * dir,struct dentry * dentry, S_IFCHR,dev_t dev)
	// create_special_files();



	return rtc_ds1307_dyn_assigned;
}



static void deregister_rtc_ds1307_drv(void)
{
	int rtc_ds1307_dyn_assigned = rtc_ds1307_major_nr;
	
	unregister_chrdev(rtc_ds1307_dyn_assigned, RTC_DRV_NAME);
	

	// TODO : Delete all the relevant nodes / device files created during the module initialization here
	// Sudhanshu : 16 Jan 2013
	// delete_special_files();

	return;
}

static byte set_rtc(RTC_TIME *tm)
{
	tm = tm;

	// Add I2C routines here to write to DS1307 I2C Device

	
	return 0;

}



static byte get_rtc(RTC_TIME *tm)
{
	int i = 0;
	char * tmp_cpy = (char *)tm;

	// Add I2C routines here to read from DS1307 I2C Device


	// Start the Clock
	bcm_i2c_start_clock(rtc_ds1307_i2c_hdl, REGULAR_MODE_100kHz);

	// Write Slave Address
	bcm_i2c_write_slave_address(rtc_ds1307_i2c_hdl, RTC_DS1307_SLAVE_ADDRESS, READ_BIT);

	// Set DLEN = 8 
	bcm_i2c_set_data_length(rtc_ds1307_i2c_hdl , 8);

	// Configure FIFO for READ
	bcm_i2c_set_transfer_mode(rtc_ds1307_i2c_hdl, READ_BIT);

	// Put a "Start" bit on SDA line
	bcm_i2c_start(rtc_ds1307_i2c_hdl);


	// Wait for Acknoledge (from Slave device)
	bcm_i2c_wait_for_ack(rtc_ds1307_i2c_hdl);

	// Wait for DONE = 1
	


	 
	// Data arrives from Slave device.
	for(i = 0 ; i < 7; i++)
	{
		// Read from FIFO
		tmp_cpy[i] = bcm_i2c_read_one_byte_from_fifo(rtc_ds1307_i2c_hdl);
		
		// Put a ACK bit on SDA line after reading every byte from the slave
		// Wait for Acknoledge (from Slave device)
	    //bcm_i2c_wait_for_ack(rtc_ds1307_i2c_hdl);
	}
	
	
	return 0;

}

static ssize_t rtc_ds1307_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int data_read = 0;
	struct rtc_time cur_tm;

	data_read = get_rtc(&cur_tm);

	
	// Type current date & time to sys log
	printk(KERN_WARNING "rtc_ds1307 : Date = %d-%d-%d \n", cur_tm.date, cur_tm.month, cur_tm.year);
	printk(KERN_WARNING "rtc_ds1307 : Time = %d:%d:%d \n", cur_tm.hours, cur_tm.minutes, cur_tm.seconds);

	return data_read;

}

static ssize_t rtc_ds1307_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{

	return 0;
}

static int rtc_ds1307_open(struct inode *inode, struct file *file)
{
	if (rtc_ds1307_open_cnt)
		return -EBUSY;	/* open only once at a time */

	if (file->f_mode & FMODE_WRITE)	/* device is read-only */
		return -EPERM;


	//Increment Module usage count.
	rtc_ds1307_open_cnt++;

	return 0;
}

static int rtc_ds1307_release(struct inode *inode, struct file *file)
{
	// Decrement Module usage count.
	rtc_ds1307_open_cnt--;

	return 0;
}



static int __init rtc_ds1307_init(void)
{
	int rtc_ds1307_dyn_assigned = 0;
	
	printk(KERN_INFO "rtc_ds1307 : Module Enter");

	// Initialize the RTC pins.
	setup_rtc_ds1307();
	
	// All set to Register the Keypad driver.
	rtc_ds1307_dyn_assigned = register_rtc_ds1307_drv();

	return 0;
}

static void __exit rtc_ds1307_exit(void)
{

	if(!rtc_ds1307_open_cnt)
	{
		printk(KERN_INFO "rtc_ds1307 : Module in use. Could not un-load driver.");
		return;
	}
	
	// Nobody is using the driver, OK to un-load.
	deregister_rtc_ds1307_drv();

	// Call the freeing functions here.
	clean_rtc_ds1307();

	printk(KERN_INFO "rtc_ds1307 : Module Exit");

	return;
}



module_init(rtc_ds1307_init);
module_exit(rtc_ds1307_exit);




MODULE_AUTHOR("Sudhanshu Gupta");
MODULE_DESCRIPTION("Driver for support for RTC sub-subsystem DS1307.");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("");



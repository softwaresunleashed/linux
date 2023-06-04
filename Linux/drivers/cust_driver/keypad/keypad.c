
/*

ROWs are connected to Output Port
COLs are connected to Input Port



Read dev --> /dev/keypad

*/




// Headers inclusion for Kernel Module Programming
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>		// struct file_operations





#include "../_utils/bcm_gpio.h"
#include "keypad.h"



	
static void setup_keypad_gpios(void)
{

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

	return;
}


static void clean_keypad_gpios(void)
{
	clear_gpio(ROW1);
	clear_gpio(ROW2);
	clear_gpio(ROW3);
	clear_gpio(ROW4);

	clear_gpio(COL1);
	clear_gpio(COL2);
	clear_gpio(COL3);

	return;
}



static int register_keypad_drv(void)
{
	int keypad_dyn_assigned = -1;
	
	keypad_dyn_assigned = register_chrdev(keypad_major, KEYPAD_DRV_NAME, &keypad_fops);
	if(keypad_dyn_assigned < 0)
	{
		printk(KERN_WARNING "keypad : can't get major %d\n", keypad_dyn_assigned);
		return keypad_dyn_assigned;
	}
	
	if(keypad_major == 0)
	{
		keypad_major = keypad_dyn_assigned;
		printk(KERN_INFO "keypad : keypad Driver Installed. Major : %d", keypad_dyn_assigned);
		return keypad_dyn_assigned;	
	}



	
	// TODO: Make node "/dev/keypad"
	//	dev_t devno = MKDEV(cfake_major, minor);
	//   mknod(struct inode * dir,struct dentry * dentry, S_IFCHR,dev_t dev)
	// create_special_files();



	return keypad_dyn_assigned;
}



static void deregister_keypad_drv(void)
{
	int keypad_dyn_assigned = keypad_major;
	
	unregister_chrdev(keypad_dyn_assigned, KEYPAD_DRV_NAME);
	

	// TODO : Delete all the relevant nodes / device files created during the module initialization here
	// Sudhanshu : 16 Jan 2013
	// delete_special_files();

	return;
}


static char * key_to_string_map(byte key_press)
{
	char * key_str = NULL;

	switch((KEYS)key_press)
	{

		case KEY_1:
			key_str = "KEY_1";
			break;

		case KEY_2:
			key_str = "KEY_2";
			break;

		case KEY_3:
			key_str = "KEY_3";
			break;

		case KEY_4:
			key_str = "KEY_4";
			break;

		case KEY_5:
			key_str = "KEY_5";
			break;

		case KEY_6:
			key_str = "KEY_6";
			break;

		case KEY_7:
			key_str = "KEY_7";
			break;

		case KEY_8:
			key_str = "KEY_8";
			break;

		case KEY_9:
			key_str = "KEY_9";
			break;

		case KEY_S:
			key_str = "KEY_STAR";
			break;

		case KEY_0:
			key_str = "KEY_0";
			break;

		case KEY_H:
			key_str = "KEY_HASH";
			break;
		

		default:
			key_str = NULL;
			printk(KERN_INFO "No Keycode Found. Code Read = %d", key_press);
			break;
	};

	
	return key_str;

}

static byte scan_rows(void)
{
	byte row_scan_code = 0;
	bool R1=0, R2=0, R3=0, R4=0;

// Setup the voltage levels before scanning. 
// Make the pins HIGH that needs to be scanned, and the others LOW
	clear_gpio(ROW_POW);
	set_gpio(COL_POW);


//Wait for voltage to settle (1 sec)
	Delay(1);


// Get Individual Row status
	R1 = get_gpio_pin_status(ROW1);
	R2 = get_gpio_pin_status(ROW2);
	R3 = get_gpio_pin_status(ROW3);
	R4 = get_gpio_pin_status(ROW4);

// Calculate the ROW status logic.
	row_scan_code = (R4 * 8) + (R3 * 4) + (R2 * 2) + (R1 * 1);

	printk(KERN_INFO "Row Status = 0x%x", row_scan_code);
	return row_scan_code;
}

static byte scan_cols(void)
{
	byte col_scan_code = 0;
	bool C1=0, C2=0, C3=0; 

// Setup the voltage levels before scanning. 
// Make the pins HIGH that needs to be scanned, and the others LOW
	set_gpio(ROW_POW);
	clear_gpio(COL_POW);


//Wait for voltage to settle (1 sec)
	Delay(1);


// Get Individual Col status
	C1 = get_gpio_pin_status(COL1);
	C2 = get_gpio_pin_status(COL2);
	C3 = get_gpio_pin_status(COL3);

// Calculate the COL status logic.
	col_scan_code = (C3 * 64) + (C2 * 32) + (C1 * 16);

	printk(KERN_INFO "Col Status = 0x%x", col_scan_code);		
	return col_scan_code;
}


//	key_pressed  bit masking ==>
// --------------------------------------------------------
// | xx xx xx xx  | xx xx xx xx | xx  C3    C2    C1 | R4   R3   R2   R1 |
// --------------------------------------------------------
static byte get_key_pressed(void)
{
	byte key_pressed = 0;
	byte row_mask = 0;
	byte col_mask = 0;

	
	// Read present ROW status
	row_mask = scan_rows();
	
	// Read present COL status
	col_mask = scan_cols();


	// Create complete scan code
	key_pressed = row_mask + col_mask;
	
	// PULL Down voltage level to save current.
	clear_gpio(ROW_POW);
	clear_gpio(COL_POW);

	return key_pressed;

}

static ssize_t keypad_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	byte key_pressed = -1;
	char * key_str = NULL;
	
	printk(KERN_INFO "keypad : Entered keypad_read function");


	key_pressed = get_key_pressed();

		
	key_str = key_to_string_map(key_pressed);
	if(key_str)
		printk(KERN_INFO "KeyPress : %s", key_str);

	

	return 0;

}

static int keypad_open(struct inode *inode, struct file *file)
{

	if (keypad_open_cnt)
		return -EBUSY;	/* open only once at a time */

	if (file->f_mode & FMODE_WRITE)	/* device is read-only */
		return -EPERM;

	keypad_buflen = 0;	/* flush the buffer on opening */

	//Increment Module usage count.
	keypad_open_cnt++;
	
	return 0;
}

static int keypad_release(struct inode *inode, struct file *file)
{
	// Decrement Module usage count.
	keypad_open_cnt--;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////   KEYPAD CODE -- ENDS   //////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




static int __init keypad_init(void)
{
	int keypad_dyn_assigned = 0;
	
	printk(KERN_INFO "keypad : Module Enter");

	// Perform all the GPIO peripheral-to-virtual memory mapping here.	
	setup_gpio_io();


	// Initialize the keypad pins.
	setup_keypad_gpios();
	
	// All set to Register the Keypad driver.
	keypad_dyn_assigned = register_keypad_drv();

	return 0;
}

static void __exit keypad_exit(void)
{

	if(!keypad_open_cnt)
	{
		printk(KERN_INFO "keypad : Module in use. Could not un-load driver.");
		return;
	}
	
	// Nobody is using the driver, OK to un-load.
	deregister_keypad_drv();

	// Call the freeing functions here.
	clean_keypad_gpios();


	// Perform all cleanup activities here.
	cleanup_gpio_io();

	printk(KERN_INFO "keypad : Module Exit");

	return;
}



module_init(keypad_init);
module_exit(keypad_exit);




MODULE_AUTHOR("Sudhanshu Gupta");
MODULE_DESCRIPTION("Driver for 4x3 Matrix Keypad.");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("");



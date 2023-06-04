#ifndef __motor_control_h_
#define __motor_control_h_




#define MOTORCONTROLLER_MAJOR	0
#define MOTORCONTROLLER_DRV_NAME	"motorcontroller"



// Globals
int motorcontroller_major = MOTORCONTROLLER_MAJOR;	// Make this global, needs to be used by Kernel to invoke appropriate functions.






/* Defines */


#if 0
// ROW,COL to GPIO mapping (Configurable)
#define ROW1	GPIO_22
#define ROW2	GPIO_23
#define ROW3	GPIO_24
#define ROW4	GPIO_25

#define COL1	GPIO_4
#define COL2	GPIO_27		//#21 on RPI board (P1_13)
#define COL3	GPIO_17

#define ROW_POW		GPIO_7	// P1_26
#define COL_POW		GPIO_8	// P1_24
#endif 



/* global variables */
static int motorcontroller_open_cnt;	/* #times opened */
static int motorcontroller_buflen;








// Function declarations
static ssize_t motorcontroller_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t motorcontroller_write(struct file *file, const char __user *buf, size_t, loff_t *ppos);

static int motorcontroller_open(struct inode *inode, struct file *file);
static int motorcontroller_release(struct inode *inode, struct file *file);



// Data Structures


// File Operations struct  for Keypad
static struct file_operations motorcontroller_fops = 
{
	//.owner	 = THIS_MODULE,
	.read    = motorcontroller_read,		/* read */
	.write	 = motorcontroller_write,		/* write */
	.open    = motorcontroller_open,		/* open */
	.release = motorcontroller_release,	/* close */
	//.llseek  = default_llseek,
};



#endif /* __motor_control_h_ */

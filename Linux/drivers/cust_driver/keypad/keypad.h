


#define KEYPAD_MAJOR	0
#define KEYPAD_DRV_NAME	"keypad"



// Globals
int keypad_major = KEYPAD_MAJOR;	// Make this global, needs to be used by Kernel to invoke appropriate functions.






/* Defines */

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

/* global variables */
static int keypad_open_cnt;	/* #times opened */
static int keypad_buflen;


typedef enum 
{
	ROW_1 = 0,		// ROW index begins from ZERO
	ROW_2,
	ROW_3,
	ROW_4,

	COL_1 = 0,		// COL index begins from ZERO
	COL_2,
	COL_3
}eRC;

typedef enum 
{
// ROW_1
	KEY_1	=	0b00010001,
	KEY_2	=	0b00100001,
	KEY_3	=	0b01000001,
// ROW_2
	KEY_4	=	0b00010010,
	KEY_5	=	0b00100010, 
	KEY_6	=	0b01000010,
// ROW_3
	KEY_7	=	0b00010100,
	KEY_8	=	0b00100100,
	KEY_9	=	0b01000100,
// ROW_4	
	KEY_S	=	0b00011000,
	KEY_0	=	0b00101000,
	KEY_H	=	0b01001000,
// End of Key Table
	KEY_MAX_KEYS	=	0xFF
	
}KEYS;





// Function declarations
static ssize_t keypad_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static int keypad_open(struct inode *inode, struct file *file);
static int keypad_release(struct inode *inode, struct file *file);



// Data Structures


// File Operations struct  for Keypad
static struct file_operations keypad_fops = 
{
	//.owner	 = THIS_MODULE,
	.read    = keypad_read,		/* read */
	.open    = keypad_open,		/* open */
	.release = keypad_release,	/* close */
	//.llseek  = default_llseek,
};




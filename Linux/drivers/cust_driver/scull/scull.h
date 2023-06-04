
#ifndef __scull_h_
#define __scull_h_



// Macros
#define	SCULL_MAJOR	0	// implies dynamic major number requested
#define SCULL_DRV_NAME 	"scull"

#define TYPE(dev)	(MINOR(dev) >> 0x4)	// High Nibble - Device Type (personality) e.g. scull0 , scullpipe0
#define NUM(dev)	(MINOR(dev) &  0xf)	// Lower Nibble - Device Number. e.g. scull0, scull1



// Globals
int scull_major = SCULL_MAJOR;	// Make this global, needs to be used by Kernel to invoke appropriate functions.


int scull_mod_count = 0;
#define MOD_INC_USE_COUNT		scull_mod_count++;
#define MOD_DEC_USE_COUNT		scull_mod_count--;



// Local Statics

// File Operations for Device_Type-0
static struct file_operations scull_fops = 
{
	owner:		THIS_MODULE,
	open:		scull_open,
	llseek:		scull_llseek,
	read:		scull_read,
	write:		scull_write,
	//ioctl:		scull_ioctl,
	release:	scull_release,
};

// Array of File Operations structures for all the device types
static struct file_operations scull_fop_array[] = 
{
	&scull_fops,			/* for device type 0 */
	//&scull_priv_fops,		/* for device type 1 */
	//&scull_pipe_fops,		/* for device type 2 */
	//&scull_sngl_fops,		/* for device type 3 */
	//&scull_user_fops,		/* for device type 4 */
	//&scull_wusr_fops,		/* for device type 5 */
};

#define SCULL_MAX_TYPE		(sizeof(scull_fop_array) / sizeof(scull_fop_array[0]))

static typedef struct Scull_Dev
{
	void **data;
	struct Scull_Dev * next;	// Next item (device) in the list
	int quantum;				// Current quantum size
	int qset;					// Current array size
	unsigned long size;
	//devfs_handle_t handle;		// only used if devfs is there
	unsigned int access_key;	// used by sculluid and scullpriv
	struct semaphore sem;		// mutual exclusion sema
} Scull_Dev;


#endif 		//__scull_h_

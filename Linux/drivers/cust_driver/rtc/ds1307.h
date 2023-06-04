#ifndef __ds1307_h_
#define __ds1307_h_

#include "../_utils/bcm_i2c.h"

// Configurable stuff goes here
#define I2C_SDA		SDA1		// Use BSC1 bus for this project
#define I2C_SCL		SCL1






// Defines
#define RTC_DS1307_MAJOR				0
#define RTC_DRV_NAME					"rtc_ds1307"
#define RTC_DS1307_SLAVE_ADDRESS			0b11010000


// Globals
int rtc_ds1307_major_nr = RTC_DS1307_MAJOR;	// Make this global, needs to be used by Kernel to invoke appropriate functions.



// RTC DS1307 Register Addresses
#define REG_RTC_DS1307_SECONDS		0x00
#define REG_RTC_DS1307_MINUTES		0x01
#define REG_RTC_DS1307_HOURS			0x02
#define REG_RTC_DS1307_DAY			0x03
#define REG_RTC_DS1307_DATE			0x04
#define REG_RTC_DS1307_MONTH			0x05
#define REG_RTC_DS1307_YEAR			0x06
#define REG_RTC_DS1307_CONTROL		0x07
#define REG_NVRAM			0x08






// Enumerations

// BIT Field masking for CONTROL Register (on DS1307 Device)
typedef enum
{
// ===========================
// RS1	RS0			SQW Freq.
// ===========================
//  0	0			1 Hz
//  0	1			4.096 kHz
//  1	0			8.192 kHz
//  1	1			32.768 kHz
// ===========================
	BIT0_RS0,					// RATE Select 0			
	BIT1_RS1,					// RATE Select 1
	BIT2_RESEVERED,
	BIT3_RESEVERED,
	BIT4_SQWE,					// SQUARE Wave Enable
	BIT5_RESERVED,
	BIT6_RESERVED,
	BIT7_OUT					// OUTPUT Control
}RTC_DS1307_CONTROL_REG;







// Global (Local) variables
static int rtc_ds1307_open_cnt = 0;	/* #times opened */






// Function declarations

static ssize_t rtc_ds1307_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t rtc_ds1307_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static int rtc_ds1307_open(struct inode *inode, struct file *file);
static int rtc_ds1307_release(struct inode *inode, struct file *file);


typedef struct rtc_time
{
	byte seconds;
	byte minutes;
	byte hours;
	byte day;
	byte date;
	byte month;
	byte year;
}RTC_TIME;


// File Operations for Keypad
static struct file_operations rtc_fops = 
{
	.read    = rtc_ds1307_read,		/* read from DS1307 */
	.write	 = rtc_ds1307_write,		/* write to DS1307*/
	.open    = rtc_ds1307_open,		/* open */
	.release = rtc_ds1307_release		/* close */
};






#endif /* __ds1307_h_ */

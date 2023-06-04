#ifndef __bcm_gpio_h_
#define __bcm_gpio_h_




// Include Utility Header Files
#include "basetypes.h"

// Enable / Disable below flag to build code for KERNEL or USERSPACE
#define __KERNEL_COMPILE__

#ifdef __KERNEL_COMPILE__
#define __CODE_FOR_KERNEL__
#else
#define __CODE_FOR_USERSPACE__
#endif 





/* 
GPIO Peripheral Base address 
======================
The file <linux-rpi-3.32.27\arch\arm\mach-bcm\platform.h>	   
	-- contains the base addresses for all the peripherals (Physical addresses needs to be mapped to virtual addresses using mmap() or ioremap() )
*/
#define PERIPHERAL_BLOCK_BASE_ADDRESS_PHY		( 0x7E000000 )
#define PERIPHERAL_BLOCK_BASE_ADDRESS_PHY_END	( 0x7E2000B0 )

// Define Physical (BUS) Addresses here (Refer to addresses in Datasheet)
#define CLOCK_MANAGER_BASE_ADDRESS				( 0x7E101000 )
#define GPIO_FUN_PER_BASE_ADDRESS				( 0x7E200000 )


/*
Calculation of Virtual address 
------------------------
VA = gpio_virtual_add (provided by mmap) + (physical_address_of_controller_block_needed - physical_base_address_of_all_peripherals[0x7E000000]) + offset_of_register_in_a_controller_block
*/ 
#define GPIO_ACCESS_BLOCK(addr, register_offset)	( (int)gpio + (addr - PERIPHERAL_BLOCK_BASE_ADDRESS_PHY) + register_offset ) 	
 
#define GPIO_PERIPHERAL_BLOCK_SIZE	( PERIPHERAL_BLOCK_BASE_ADDRESS_PHY_END - PERIPHERAL_BLOCK_BASE_ADDRESS_PHY)
/*********************************/


typedef enum 
{
	GPIO_0,
	GPIO_1,
	GPIO_2,
	GPIO_3,
	GPIO_4,
	GPIO_5,
	GPIO_6,
	GPIO_7,
	GPIO_8,
	GPIO_9,
	GPIO_10,
	GPIO_11,
	GPIO_12,
	GPIO_13,
	GPIO_14,
	GPIO_15,
	GPIO_16,
	GPIO_17,
	GPIO_18,
	GPIO_19,
	GPIO_20,
	GPIO_21,
	GPIO_22,
	GPIO_23,
	GPIO_24,
	GPIO_25,
	GPIO_26,
	GPIO_27,
	GPIO_28,
	GPIO_29,
	GPIO_30,
	GPIO_31,
	GPIO_32,
	GPIO_33,
	GPIO_34,
	GPIO_35,
	GPIO_36,
	GPIO_37,
	GPIO_38,
	GPIO_39,
	GPIO_40,
	GPIO_41,
	GPIO_42,
	GPIO_43,
	GPIO_44,
	GPIO_45,
	GPIO_46,
	GPIO_47,
	GPIO_48,
	GPIO_49,
	GPIO_50,
	GPIO_51,
	GPIO_52,
	GPIO_53,

	// Terminating entry
	GPIO_MAX_COUNT

}GPIO_SYM_NAME;

/* General Defines */
#define BCM_NR_GPIOS	GPIO_MAX_COUNT



typedef enum 
{
	GPIO_PIN_INPUT 		= 0x00,
	GPIO_PIN_OUTPUT		= 0x01,
	GPIO_PIN_ALT_FUNC_0	= 0x04,
	GPIO_PIN_ALT_FUNC_1	= 0x05,
	GPIO_PIN_ALT_FUNC_2	= 0x06,
	GPIO_PIN_ALT_FUNC_3	= 0x07,
	GPIO_PIN_ALT_FUNC_4	= 0x03,
	GPIO_PIN_ALT_FUNC_5	= 0x02
}ALT_FUNCTION;


/* GPIO Pin Function Select */
#define GPFSEL0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0000)
#define GPFSEL1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0004)
#define GPFSEL2				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0008)
#define GPFSEL3				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x000C)
#define GPFSEL4				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0010)	
#define GPFSEL5				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0014)

/* GPIO Pin Output - Set */
#define GPSET0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x001C)
#define GPSET1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0020)

/* GPIO Pin Output - Clear */
#define GPCLR0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0028)
#define GPCLR1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x002C)

/* GPIO Pin Level  */
#define GPLEV0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0034)
#define GPLEV1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0038)

/* GPIO Pin Event Detect Status  */
#define GPEDS0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0040)
#define GPEDS1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0044)

/* GPIO Pin Rising Edge Detect Enable  */
#define GPREN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x004C)
#define GPREN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0050)

/* GPIO Pin Falling Edge Detect Enable  */
#define GPFEN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0058)
#define GPFEN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x005C)

/* GPIO Pin High Detect Enable  */
#define GPHEN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0064)
#define GPHEN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0068)

/* GPIO Pin Low Detect Enable  */
#define GPLEN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0070)
#define GPLEN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0074)

/* GPIO Pin Async. Rising Edge Detect */
#define GPAREN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x007C)
#define GPAREN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0080)

/* GPIO Pin Async. Falling Edge Detect */
#define GPAFEN0				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0088)
#define GPAFEN1				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x008C)

/* GPIO Pin Pull-Up/Down Enable */
#define GPPUD				(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0094)


/* GPIO Pin Pull-Up/Down Enable Clock */
#define GPPUDCLK0			(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x0098)
#define GPPUDCLK1			(volatile unsigned int *) GPIO_ACCESS_BLOCK(GPIO_FUN_PER_BASE_ADDRESS, 0x009C)

/* Clock Manager General Purpose Clocks Control */
#define CM_GP0CTL			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x0070)		// GPIO4 (alt-0) , GPIO20 (alt -5) , GPIO32 (alt-0), GPIO34 (alt-0) , GPIO44 (alt -0)
#define CM_GP1CTL			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x0078)		// GPIO5  (alt-0), GPIO21 (alt -5) , GPIO42 (alt -0) 
#define CM_GP2CTL			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x0080)		// GPIO6 (alt-0) , GPIO43 (alt -0)

/* Clock Manager General Purpose Clock Divisors */
#define CM_GP0DIV			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x0074)		// GPIO4 (alt-0) , GPIO20 (alt -5) , GPIO32 (alt-0), GPIO34 (alt-0) , GPIO44 (alt -0)
#define CM_GP1DIV			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x007C)		// GPIO5  (alt-0), GPIO21 (alt -5) , GPIO42 (alt -0) 
#define CM_GP2DIV			(volatile unsigned int *) GPIO_ACCESS_BLOCK(CLOCK_MANAGER_BASE_ADDRESS, 0x0084)		// GPIO6 (alt-0) , GPIO43 (alt -0)






typedef enum
{
	MASH_STAGE_0,		// Integer Division
	MASH_STAGE_1,		// 1-stage MASH (equivalent to non-MASH dividers)
	MASH_STAGE_2,		// 2-stage MASH filter
	MASH_STAGE_3		// 3-stage MASH filter
}MASH_STAGE_CONTROL;


typedef enum
{
	GPIO_CLK_SRC_GND,
	GPIO_CLK_SRC_OSCILLATOR,
	GPIO_CLK_SRC_TESTDEBUG0,
	GPIO_CLK_SRC_TESTDEBUG1,
	GPIO_CLK_SRC_PLLA,
	GPIO_CLK_SRC_PLLC,
	GPIO_CLK_SRC_PLLD,
	GPIO_CLK_SRC_HDMI_AUX,

	GPIO_CLK_SRC_MAX				// 8-15 GND
}GPIO_CLK_SRC;


#define CLOCK_MGR_PASSWD		0x5A		// As given in the datasheet.

/* Clock Manager General Purpose Clocks Control (CM_GP0CTL, GP1CTL & GP2CTL) */
typedef struct _GEN_PUR_CLK_CON_REG
{
	int PASSWD		: 8;		// 31-24 Bits 		- Clock manager Password "0x5a"
	int RESERVED1	: 13;		// 23-11 Bits 		- Unused (R)
	int MASH		: 2;		// 10-9 Bits	- MASH control. Takes value from enum MASH_STAGE_CONTROL (RW)
	int FLIP		: 1;		// 8 Bits		- Invert the Clock generator output (RW)
	int BUSY		: 1;		// 7 Bits		- Clock generator is running (R)
	int RESERVED0	: 1;		// 6 Bits 		- Unused (R)
	int KILL		: 1;		// 5 Bits 		- Kill the Clock generator (RW)
	int ENAB		: 1;		// 4 Bits 		- Enable Clock generator (RW)
	int SRC			: 4;		// 3:0  Bits	- CLOCK Source (RW)
}GEN_PUR_CLK_CON_REG;



/* Clock Manager General Purpose Clocks Divisors (CM_GP0DIV, GP1DIV & GP2DIV) */
typedef struct _GEN_PUR_CLK_DIV_REG
{
	int PASSWD		: 8;		// 31-24 Bits 		- Clock manager Password "0x5a" (W)
	int DIVI		: 12;		// 23-12 Bits 		- Integer Part of Divisor (RW)
	int DIVF		: 12;		// 11:0  Bits		- Fractional Part of Divisor (RW)
}GEN_PUR_CLK_DIV_REG;



/* Function Declarations */
int  setup_gpio_io(void);
void cleanup_gpio_io(void);

void select_alt_gpio_func(GPIO_SYM_NAME gn , ALT_FUNCTION af);

void set_gpio(GPIO_SYM_NAME gn);
void clear_gpio(GPIO_SYM_NAME gn);


bool get_gpio_pin_status(GPIO_SYM_NAME gn);

/* Global Declarations */









#endif /* __bcm_gpio_h_ */


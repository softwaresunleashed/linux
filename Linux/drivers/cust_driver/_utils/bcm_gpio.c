
// Headers inclusion for Kernel Module Programming
#include <linux/module.h>
#include <linux/kernel.h>

// Utility Header Files
#include "bcm_gpio.h"



// Headers inclusion for I/O or Memory Map
#ifdef __CODE_FOR_USERSPACE__				
	#include <linux/fcntl.h>
	#include <linux/mman.h>
#else
	#include <asm/io.h>		// ioremap();
#endif 

/* Global Variable Declarations */
char * gpio_map;
volatile unsigned int * gpio;		// I/O Access

/* Static Declarations */
static int gpio_initialized = 0;




void select_alt_gpio_func(GPIO_SYM_NAME gn , ALT_FUNCTION af)
{
	int sel_reg = (int)(gn / 10);
	int sel_gpio_num = (int)(gn % 10);
	int mask = 0;
	int temp = 0x7;


	if(!gpio_initialized)
	{
		setup_gpio_io();
	} 
	


	if (gn >= BCM_NR_GPIOS)
	{
		//TODO: Assert here.
		return;
	}


	temp = ~(temp << (sel_gpio_num*3));
	
	mask = (af << (sel_gpio_num*3));
	

	switch(sel_reg)
	{
		case 0:			
			*GPFSEL0 = *GPFSEL0 & temp;	
			*GPFSEL0 = *GPFSEL0 | mask;
			break;
		case 1:
			*GPFSEL1 = *GPFSEL1 & temp;	
			*GPFSEL1 = *GPFSEL1 | mask;
			break;
		case 2:
			*GPFSEL2 = *GPFSEL2 & temp;	
			*GPFSEL2 = *GPFSEL2 | mask;			
			break;
		case 3:	
			*GPFSEL3 = *GPFSEL3 & temp;	
			*GPFSEL3 = *GPFSEL3 | mask;
			break;
		case 4:
			*GPFSEL4 = *GPFSEL4 & temp;	
			*GPFSEL4 = *GPFSEL4 | mask;
			break;
		case 5:
			*GPFSEL5 = *GPFSEL5 & temp;	
			*GPFSEL5 = *GPFSEL5 | mask;	
			break;
	};
}
EXPORT_SYMBOL(select_alt_gpio_func);



void set_gpio(GPIO_SYM_NAME gn)
{
	int gpio_bank = gn / 32;	// to select appropriate GPIO register
	int gpio_pin_bit = gn - (gpio_bank * 32);

	// Select alternate function - OUTPUT for GPIO pin 23
	select_alt_gpio_func(gn, GPIO_PIN_OUTPUT);

	// Set Individual pin to HIGH (Logic HIGH)
	if(gpio_bank==0) 
		*GPSET0 = (1 << gpio_pin_bit);						//- Joan's suggestion
	else
		*GPSET1 = (1 << gpio_pin_bit);
	
	
	return;
}
EXPORT_SYMBOL(set_gpio);





void clear_gpio(GPIO_SYM_NAME gn)
{
	int gpio_bank = gn / 32;	// to select appropriate GPIO register
	int gpio_pin_bit = gn - (gpio_bank * 32);

	// Set Individual pin to LOW (Logic ZERO)
	if(gpio_bank==0) 
		*GPCLR0 = (1 << gpio_pin_bit);
	else
		*GPCLR1 = (1 << gpio_pin_bit);
	
	return;
}
EXPORT_SYMBOL(clear_gpio);






bool get_gpio_pin_status(GPIO_SYM_NAME gn)
{
	int pin_level; 
	int gpio_bank = gn / 32;	// to select appropriate GPIO register
	int gpio_pin_bit = gn - (gpio_bank * 32);

	if (gn >= BCM_NR_GPIOS)
	{
		//TODO: Assert here.
		return 0;
	}

	// Read Pin Status as per the GPIO Bank used. 	
	if(gpio_bank==0) 
	{
		pin_level = *GPLEV0;
	}
	else
	{
		pin_level = *GPLEV1;
	}
			
	return (((pin_level >> gpio_pin_bit) & 0x1) ? TRUE : FALSE);
}
EXPORT_SYMBOL(get_gpio_pin_status);



#ifdef __CODE_FOR_USERSPACE__			// To be used in case being called from an Application context
int setup_gpio_io(void)
{
	int mem_fd;
	

	
	if(gpio_initialized)
		return SUCCESS;

	/* Open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		printk(KERN_INFO "can\'t open /dev/mem \n");
		return FAIL;
	} 
	
	/* mmap GPIO */
	gpio_map = (char *) mmap(
					NULL, 			//Any address in our memory
					GPIO_PERIPHERAL_BLOCK_SIZE,	// Map length 
					PROT_READ | PROT_WRITE,	// Enable Reading + Writting to mapped memory
					MAP_SHARED,		// Shared with other processes
					mem_fd,			// File to map
					PERIPHERAL_BLOCK_BASE_ADDRESS_PHY	// Physical Address of complete GPIO peripheral block
				);
	close(mem_fd);
	
	if(gpio_map < 0)
	{
		printk(KERN_INFO "mmap error %d\n", (int)gpio_map);
		return FAIL;	
	}


	gpio = (volatile unsigned int *)gpio_map;

	gpio_initialized = TRUE;
	
	return SUCCESS;
}

#else		/* __CODE_FOR_USERSPACE__ */

int setup_gpio_io(void)
{


	
	if(gpio_initialized)
		return SUCCESS;
	
	/* mmap GPIO */
	gpio_map = ioremap(PERIPHERAL_BLOCK_BASE_ADDRESS_PHY, GPIO_PERIPHERAL_BLOCK_SIZE);	// Size of GPIO per block
	
	
	if(gpio_map <= 0)
	{
		printk(KERN_INFO "ioremap error %d\n", (int)gpio_map);
		return FAIL;	
	}


	gpio = (volatile unsigned int *)gpio_map;
	gpio_initialized = TRUE;

	return SUCCESS;
}
EXPORT_SYMBOL(setup_gpio_io);
#endif		/* __CODE_FOR_KERNEL__ */


void cleanup_gpio_io(void)
{
	// Un-Map the Memory Mapped GPIO registers
	iounmap(gpio_map);

	return;
}
EXPORT_SYMBOL(cleanup_gpio_io);





ERR_CODE set_clock_source(GPIO_SYM_NAME gn , long freq)
{
	volatile GEN_PUR_CLK_CON_REG * clk_control = NULL;
	volatile GEN_PUR_CLK_DIV_REG * clk_divisor = NULL;


// function implementation steps : todo

// 1. first check if valid gpio pin number has been provided
	switch(gn)
	{
		case GPIO_4:
		case GPIO_20:
		case GPIO_32:
		case GPIO_34:
		case GPIO_44:	
			clk_control = (volatile GEN_PUR_CLK_CON_REG *)CM_GP0CTL;
			clk_divisor = (volatile GEN_PUR_CLK_DIV_REG *)CM_GP0DIV;
			break;			
			
		case GPIO_5:
		case GPIO_21:
		case GPIO_42:	
			clk_control = (volatile GEN_PUR_CLK_CON_REG *)CM_GP1CTL;
			clk_divisor = (volatile GEN_PUR_CLK_DIV_REG *)CM_GP1DIV;
			break;				
			
		case GPIO_6:
		case GPIO_43:	
			clk_control = (volatile GEN_PUR_CLK_CON_REG *)CM_GP2CTL;
			clk_divisor = (volatile GEN_PUR_CLK_DIV_REG *)CM_GP2DIV;
			break;

		default:
			LOG_MSG_DEBUG("BCM_GPIO : Invalid Parameter detected");
			return ERR_CODE_GPIO_PARAM_MISMATCH;
			break;

	};


// 2. set the clck control structure for the corresponding GPIO pin - GEN_PUR_CLK_CON_REG


// 3. set teh clck divisor structure for the corresponding GPIO pin - GEN_PUR_CLK_DIV_REG

// Ensure that BUSY bit is NOT set while changing the structure values. this is needed to avoid lockups




}
EXPORT_SYMBOL(set_clock_source);








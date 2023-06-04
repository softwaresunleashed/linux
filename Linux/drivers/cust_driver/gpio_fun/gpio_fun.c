
// Headers inclusion for Kernel Module Programming
#include <linux/module.h>
#include <linux/kernel.h>


// Include Utility Header Files
#include "../_utils/bcm_gpio.h"
#include "gpio_fun.h"


//JTAG ARM Signals & GPIO mappings
//==========================
// ARM_TRST signal <==> GPIO_22 <==> Port1_Pin15 <==> Alt. Func. 4
// ARM_RTCK signal <==> GPIO_23 <==> Port1_Pin16 <==> Alt. Func. 4
// ARM_TDO  signal <==> GPIO_24 <==> Port1_Pin18 <==> Alt. Func. 4
// ARM_TCK  signal <==> GPIO_25 <==> Port1_Pin22 <==> Alt. Func. 4
// ARM_TDI  signal <==>  GPIO_4  <==> Port1_Pin07 <==> Alt. Func. 5
// ARM_TMS  signal <==> GPIO_27 <==> Port1_Pin13 <==> Alt. Func. 4

static void set_jtag_gpios(void)
{
/*
	TODO : 
			** Create a CMM script for Trace32, with direct memory access of GPIO pins 
			to select proper alternate functions.
			
			** This(creation of cmm script) is very much needed if debugging of target is required
			from the very beginning of bootup.

			** C-code based debugging is helpful only after Linux_OS loads the module during bootup. 
			This (C-code based)  may not allow for bootcode debugging. 
*/

	// ARM_TRST signal <==> GPIO_22 <==> Port1_Pin15 <==> Alt. Func. 4
	select_alt_gpio_func(GPIO_22, GPIO_PIN_ALT_FUNC_4);

	// ARM_RTCK signal <==> GPIO_23 <==> Port1_Pin16 <==> Alt. Func. 4
	select_alt_gpio_func(GPIO_23, GPIO_PIN_ALT_FUNC_4);

	// ARM_TDO  signal <==> GPIO_24 <==> Port1_Pin18 <==> Alt. Func. 4
	select_alt_gpio_func(GPIO_24, GPIO_PIN_ALT_FUNC_4);

	// ARM_TCK  signal <==> GPIO_25 <==> Port1_Pin22 <==> Alt. Func. 4
	select_alt_gpio_func(GPIO_25, GPIO_PIN_ALT_FUNC_4);
	
	// ARM_TDI  signal <==>  GPIO_4  <==> Port1_Pin07 <==> Alt. Func. 5
	select_alt_gpio_func(GPIO_4, GPIO_PIN_ALT_FUNC_5);

	// ARM_TMS  signal <==> GPIO_27 <==> Port1_Pin13 <==> Alt. Func. 4
	select_alt_gpio_func(GPIO_27, GPIO_PIN_ALT_FUNC_4);


	return;
}


static void clean_jtag_gpios(void)
{


	return;
}


static int __init gpio_fun_init(void)
{
	printk(KERN_INFO "GPIO_FUN: Module Enter");

	// Perform all the initialization here.	
	setup_gpio_io();


	// Call the actual function here.
	set_jtag_gpios();
	

	return 0;
}

static void __exit gpio_fun_exit(void)
{
	printk(KERN_INFO "GPIO_FUN : Module Exit");

	// Call the freeing functions here.
	clean_jtag_gpios();

	// Perform all cleanup activities here.
	cleanup_gpio_io();

	return;
}



module_init(gpio_fun_init);
module_exit(gpio_fun_exit);




MODULE_AUTHOR("Sudhanshu Gupta");
MODULE_DESCRIPTION("Driver for Fun with GPIO.");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("");



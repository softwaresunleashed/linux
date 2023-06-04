

#include <asm/io.h>		// ioremap() , iounmap()
#include <linux/slab.h>		// kmalloc() , kfree()

#include <linux/export.h>	// EXPORT_SYMBOL()


#include "bcm_i2c.h"

void bcm_i2c_set_transfer_mode(I2C_BUS_HDL * i2c_hdl, byte mode)
{
	i2c_regs->control_reg.READ_ = ((mode)?1:0);

	return;
}
EXPORT_SYMBOL(bcm_i2c_set_transfer_mode);

void bcm_i2c_set_data_length(I2C_BUS_HDL * i2c_hdl , int data_len)
{
	i2c_regs->datalength_reg.DLEN = data_len;

	return;
}
EXPORT_SYMBOL(bcm_i2c_set_data_length);


byte bcm_i2c_read_one_byte_from_fifo(I2C_BUS_HDL * i2c_hdl)
{
	return i2c_regs->datafifo_reg.DATA;
}
EXPORT_SYMBOL(bcm_i2c_read_one_byte_from_fifo);


void bcm_i2c_start_clock(I2C_BUS_HDL * i2c_hdl, I2C_OPERATING_MODE op_mode)
{

	

	switch(op_mode)
	{

		case REGULAR_MODE_100kHz:
			i2c_regs->clockdivider_reg.CDIV = CDIV_RESET_DEFAULT_100kHz;
			break; 

		/* TODO: Modify the CDIV value to get 400kHz frequency */
		case FAST_MODE_400kHz:
			i2c_regs->clockdivider_reg.CDIV = CDIV_RESET_DEFAULT_100kHz;
			break;

		// Default frequency is 100kHz	
		default:
			i2c_regs->clockdivider_reg.CDIV = CDIV_RESET_DEFAULT_100kHz;
			break;

	};


}
EXPORT_SYMBOL(bcm_i2c_start_clock);


void bcm_i2c_put_ack(I2C_BUS_HDL * i2c_hdl)
{
	// simulate Ack bit on SDA
	// may be by pulling SDA line low ?????????
	// Add code here;
	

	return;
}
EXPORT_SYMBOL(bcm_i2c_put_ack);




void bcm_i2c_wait_for_ack(I2C_BUS_HDL * i2c_hdl)
{

	// Check for the Ack bit ..loop till ACK is recieved.
	while(!i2c_regs->status_reg.DONE);

	// Acknowledge the completion of transfer
	printk(KERN_WARNING "bcm_i2c : Transfer DONE");
	i2c_regs->status_reg.DONE = 1;

	return;
}
EXPORT_SYMBOL(bcm_i2c_wait_for_ack);





void bcm_i2c_write_slave_address(I2C_BUS_HDL * i2c_hdl, byte slave_address, byte read_write_bit)
{
	// Append Read / Write Bit in slave address before writing it onto the I2C bus
	i2c_regs->slaveaddress_reg.ADDR = slave_address | read_write_bit;

	return;
}
EXPORT_SYMBOL(bcm_i2c_write_slave_address);



void bcm_i2c_start(I2C_BUS_HDL * i2c_hdl)
{

	// Enable I2C Peripheral
	i2c_regs->control_reg.I2CEN = 1;

	// Generate a Start Bit
	i2c_regs->control_reg.ST = 1;

	return;
}
EXPORT_SYMBOL(bcm_i2c_start);


void bcm_i2c_stop(I2C_BUS_HDL * i2c_hdl)
{

	// Disable I2C Peripheral
	i2c_regs->control_reg.I2CEN = 0;

	return;
}
EXPORT_SYMBOL(bcm_i2c_stop);



I2C_BUS_HDL * bcm_i2c_initialize(void)
{

	static int i2c_initialized = 0;
	
		
		if(i2c_initialized)
			return i2c_hdl;


	// Initialize the GPIO subsystem if not already done.
	if(!setup_gpio_io())
	{
		// Allocate a handle to i2c bus
		i2c_hdl = (I2C_BUS_HDL *) kmalloc(sizeof(I2C_BUS_HDL), GFP_KERNEL);
		if(!i2c_hdl)
		{
			memset(i2c_hdl, 0x00, sizeof(I2C_BUS_HDL));
		}
		else
			return 0x00;	// return NULL on error
	}

	// Map Physical Peripheral Address to Virtual Memory Address Space.
	i2c_hdl->_i2c_base = ioremap(I2C_BSC1, I2C_PERIPHERAL_BLOCK_SIZE);	// 0x20 is the size of I2C per block
	if((i2c_hdl->_i2c_base) <= 0)
	{
		printk(KERN_INFO "ioremap error : %d\n", (int)i2c_hdl->_i2c_base);
		return 0x00;	
	}

	// Initialize all I2C register pointers
	if(i2c_hdl)
	{
		i2c_regs = (I2C_PERIPHERALS *)i2c_hdl->_i2c_base;
	}
	
	
	i2c_initialized = TRUE;

	return i2c_hdl;
}
EXPORT_SYMBOL(bcm_i2c_initialize);

void bcm_i2c_cleanup(I2C_BUS_HDL * i2c_hdl)
{

	// 1. Remove the Virtual - to - Physical memory mapping
	iounmap(i2c_hdl->_i2c_base);
	
	// 2. Free i2c_hdl 
	if(i2c_hdl)
	{
		kfree(i2c_hdl);
		i2c_hdl = NULL;
	}

	return;
}
EXPORT_SYMBOL(bcm_i2c_cleanup);




void bcm_i2c_prepare_pin(GPIO_SYM_NAME gn , ALT_FUNCTION af)
{

	// Set individual I2C pins (ie. GPIO pins) 
	select_alt_gpio_func(gn, af);

	return;
}
EXPORT_SYMBOL(bcm_i2c_prepare_pin);






void bcm_i2c_read_reg()
{



}
EXPORT_SYMBOL(bcm_i2c_read_reg);











































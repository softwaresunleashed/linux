#ifndef __bcm_i2c_h_
#define __bcm_i2c_h_

/*
BCS -- Broadcom Serial Controller / I2C Controller
*/


/* 
I2C Operations  (Notes)
==================

Master device to generate the clock. 

Master device to generate 9th bit (Acknowledge Clock). This is after the 8-bit data transfer
Slave device has to Pull SDA line LOW during this 9th Bit Clock.


Master must signal an END OF DATA transfer to the slave, by not generating an acknowledge bit on the last byte that has been clocked out of 	SLAVE.

Data transfers happen via MSB (Most Significant Bit) first. (Both ways Master-->Slave or Slave-->Master)

Direction Bit (R/_W_) : 
0 -- Write
1 -- Read


*/



#include "bcm_gpio.h"




// I2C Peripheral Bus 0,1 -- Base Physical Addresses
#define I2C_BSC0			( PERIPHERAL_BLOCK_BASE_ADDRESS + 0x00205000 )	// I2C0 Bus Base Address
#define I2C_BSC1			( PERIPHERAL_BLOCK_BASE_ADDRESS + 0x00804000 )	// I2C1 Bus Base Address
#define I2C_PERIPHERAL_BLOCK_SIZE	( 0x20 ) 	// 8 Registers * 32bit (each) => 32 bytes

// I2C Controller -- Peripheral Base Offsets
#define I2C_CONTROL_C				0x00
#define I2C_STATUS_S				0x04
#define I2C_DATALEN_DLEN			0x08
#define I2C_SLAVEADD_A				0x0C
#define I2C_DATAFIFO_FIFO			0x10
#define I2C_CLOCKDIV_DIV			0x14
#define I2C_DATADELAY_DEL			0x18
#define I2C_CLKSTRETCH_CLKT		0x1C

// I2C Controller -- READ / WRITE bit
#define WRITE_BIT		0
#define READ_BIT		1



// Symbolic Name <--> GPIO Pin mapping
#define SDA0	GPIO_0
#define SCL0	GPIO_1
#define SDA1	GPIO_2
#define SCL1	GPIO_3



// Clock Modes in I2C
typedef enum
{
	REGULAR_MODE_100kHz,		// I2C_CLK = 100kHz
	FAST_MODE_400kHz			// I2C_CLK = 400kHz
}I2C_OPERATING_MODE;


// Addressing Mode
typedef enum
{
	ADDRESSING_7_BIT,		// Addressing mode -- 7 Bit
	ADDRESSING_10_BIT		// Addressing mode -- 10 Bit
}I2C_ADDRESSING_MODE;



/* 
I2C Bus Handle 
============

*/ 
typedef struct _I2C_BUS_HDL
{
	volatile void * _i2c_base;
}I2C_BUS_HDL;


/* 
C Register 
========
Control Register

*/ 
typedef struct _I2C_CONTROL_REG
{
	int RESERVED1		: 16;		// 31:16 Bits 	- Reserved
	int I2CEN 		: 1;		// 15		- I2C Enable (RW)
	int RESERVED2		: 4;		// 14:11 Bits 	- Reserved
	int INTR 		: 1;		// 10		- INTR Interrupt on RX (RW)
	int INTT		: 1;		// 9		- INTT Interrupt on TX (RW)
	int INTD		: 1;		// 8		- INTD Interrupt on DONE (RW)
	int ST			: 1;		// 7		- ST Start Transfer (RW)
	int RESERVED3		: 1;		// 6 Bit	- Reserved
	int CLEAR		: 2;		// 5:4		- CLEAR FIFO Clear (RW)
	int RESERVED4		: 3;		// 3:1 Bits 	- Reserved
	int READ_		: 1;		// 0  Bits	- READ Read Transfer (RW)
}I2C_CONTROL_REG;


/* 
S Register 
========
Status Register

*/ 
typedef struct _I2C_STATUS_REG
{
	int RESERVED		: 22;		// 31:10 Bits 	- Reserved	
	int CLKT		: 1;		// 9		- CLKT Clock Stretch Timeout
	int ERR 		: 1;		// 8		- ERR ACK Error (RW)
	int RXF			: 1;		// 7		- RXF - FIFO Full (RO)
	int TXE			: 1;		// 6		- TXE - FIFO Empty (RO)
	int RXD			: 1;		// 5		- RXD FIFO contains Data (RO)	
	int TXD			: 1;		// 4		- TXD FIFO can accept Data (RO)
	int RXR			: 1;		// 3 		- RXR FIFO needs Reading (full) (RO)
	int TXW			: 1;		// 2		- TXW FIFO needs Writting(full)  (RO)
	int DONE		: 1;		// 1  		- DONE Transfer Done
	int TA			: 1;		// 0  		- TA Transfer Active (RO)
}I2C_STATUS_REG;




/* 
DLEN Register 
========
Data Length Register

*/ 
typedef struct _I2C_DATALENGTH_REG
{
	int RESERVED	: 16;		// 31:16 Bits 	- Reserved	
	int DLEN	: 16;		// 15:0  Bits 	- DLEN Data Length (RW)
}I2C_DATALENGTH_REG;



/* 
A Register 
========
Slave Address Register

*/ 
typedef struct _I2C_SLAVEADDRESS_REG
{
	int RESERVED	: 25;		// 31:7  Bits 	- Reserved	
	int ADDR	:  7;		// 6:0    Bits 	- Slave Address (RW)
}I2C_SLAVEADDRESS_REG;




/* 
FIFO Register 
===========
Data FIFO Register

*/ 
typedef struct _I2C_DATAFIFO_REG
{
	int RESERVED	: 24;		// 31:8  Bits 	- Reserved	
	int DATA	:  8;		// 7:0    Bits 	- FIFO DATA Register (RW). 
					// Write multiple 1byte of data to fill seprate 16byte FIFO RX and TX each
}I2C_DATAFIFO_REG;



/* 
DIV Register 
===========
Clock Divider Register

Clock for I2C Peripheral is calculated by the following formula
	=====================	
	|  SCL = core clock / CDIV |
	=====================
Where "core_clk" is nominally 150 MHz. 

If CDIV is set to 0, the divisor is 32768. 

The default value should result in a 100 kHz I2C clock frequency.

Reset Value of CDIV = 0x5DC    (Results in I2C SCL clock of 100kHz)
*/ 
#define CDIV_RESET_DEFAULT_100kHz	0x5DC
typedef struct _I2C_CLOCKDIVIDER_REG
{
	int RESERVED	: 16;		// 31:16  Bits 	- Reserved	
	int CDIV		: 16;		// 15:0    Bits 	- Clock Divider Register (RW). 
}I2C_CLOCKDIVIDER_REG;





/* 
DEL Register 
===========
Data Delay Register

*/ 
#define DEL_FEDL_RESET_DEFAULT		0x30
#define DEL_REDL_RESET_DEFAULT		0x30
typedef struct _I2C_DATADELAY_REG
{
	int FEDL	: 16;		// 31:16  Bits 	- FEDL - Falling Edge Delay (RW). 
	int REDL	: 16;		// 15:0    Bits 	- REDL - Rising Edge Delay (RW). 
}I2C_DATADELAY_REG;


/* 
CLKT Register 
===========
Clock Stretch Time Register

TOUT Clock Stretch Timeout Value 
	-- Number of "SCL clock cycles" to wait after the rising edge of SCL 
		before deciding that the slave is not responding.

When a timeout occurs, the I2CS.CLKT bit is 0x1.

Writing 0x0 to TOUT will result in the Clock Stretch Timeout being DISABLED.
*/ 
#define CLKT_TOUT_RESET_DEFAULT		0x40
typedef struct _I2C_CLOCKSTRETCH_REG
{
	int RESERVED	: 16;		// 31:16  Bits 	-  Reserved
	int TOUT		: 16;		// 15:0    Bits 	- TOUT Clock Stretch Timeout Value (RW). 
}I2C_CLOCKSTRETCH_REG;





typedef struct _I2C_PERIPHERALS
{
	I2C_CONTROL_REG control_reg;
	I2C_STATUS_REG status_reg;
	I2C_DATALENGTH_REG datalength_reg;
	I2C_SLAVEADDRESS_REG slaveaddress_reg;
	I2C_DATAFIFO_REG datafifo_reg;
	I2C_CLOCKDIVIDER_REG clockdivider_reg;
	I2C_DATADELAY_REG datadelay_reg;
	I2C_CLOCKSTRETCH_REG clockstretch_reg;
}I2C_PERIPHERALS;



// Globals (Local) Variables
static I2C_BUS_HDL 		* i2c_hdl = NULL;
volatile I2C_PERIPHERALS	* i2c_regs = NULL;


// Function Declarations

void bcm_i2c_set_transfer_mode(I2C_BUS_HDL * i2c_hdl, byte mode);
void bcm_i2c_set_data_length(I2C_BUS_HDL * i2c_hdl , int data_len);	
byte bcm_i2c_read_one_byte_from_fifo(I2C_BUS_HDL * i2c_hdl);
void bcm_i2c_start_clock(I2C_BUS_HDL * i2c_hdl, I2C_OPERATING_MODE op_mode);
void bcm_i2c_put_ack(I2C_BUS_HDL * i2c_hdl);
void bcm_i2c_wait_for_ack(I2C_BUS_HDL * i2c_hdl);
void bcm_i2c_write_slave_address(I2C_BUS_HDL * i2c_hdl, byte slave_address, byte read_write_bit);
void bcm_i2c_start(I2C_BUS_HDL * i2c_hdl);
void bcm_i2c_stop(I2C_BUS_HDL * i2c_hdl);
I2C_BUS_HDL * bcm_i2c_initialize(void);
void bcm_i2c_cleanup(I2C_BUS_HDL * i2c_hdl);
void bcm_i2c_prepare_pin(GPIO_SYM_NAME gn , ALT_FUNCTION af);
void bcm_i2c_read_reg(void);











#endif /* __bcm_i2c_h_ */

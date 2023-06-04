
# Author : Sudhanshu 23 Dec 2012
# Sub-component makefile, the deepest we can get in Makefile hierarchy
#

### The default functionality goes here
obj-y					+= utils.c

### Custom GPIO API support
obj-$(CONFIG_BCM_CUST_GPIO)		+= bcm_gpio.o

### Custom I2C API support
obj-$(CONFIG_BCM_CUST_I2C)		+= bcm_i2c.o



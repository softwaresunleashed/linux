#
# Makefile for cust_driver subcomponents.
# Here we include the individual sub-component folders.
#
# The files to be compiled are decided in the sub-component Makefiles.
#

# Comment/uncomment the following line to disable/enable debugging
DEBUG = y
# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
DEBFLAGS = -O -g -DENABLE_DEBUG_SUPPORT # "-O" is needed to expand inlines
else
DEBFLAGS = -O2
endif
# Add DEBUG FLAGS to CFLAGS to enable debugging (printf) support for CUST_DRIVERS
CFLAGS += $(DEBFLAGS)

# Utility Library API Support (BCM_CUST_I2C, BCM_CUST_GPIO , etc)
obj-$(CONFIG_CUST_DRIVER_UTILS)			+= _utils/

# E-View USB Webcam Driver
obj-$(CONFIG_WEBCAM_SUPPORT_ENABLE)		+= webcam/

# GPIO Fun Driver
obj-$(CONFIG_GPIO_FUN_ENABLE)			+= gpio_fun/

# SCULL Char Device Driver
obj-$(CONFIG_SCULL_CHAR_DRIVER_ENABLE)		+= scull/


# 4x3 Keypad Driver
obj-$(CONFIG_KEYPAD_ENABLE)			+= keypad/

# DS1307 RTC Driver
obj-$(CONFIG_RTC_DS1307_ENABLE)			+= rtc/

# L298N Motor Control Driver
obj-$(CONFIG_MOTORCONTROL_L298N_ENABLE)		+= motor_control/


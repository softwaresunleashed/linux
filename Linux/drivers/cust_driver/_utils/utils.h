#ifndef __utils_h_
#define __utils_h_



// Header files inclusion
#include <linux/kernel.h>			// Kernel Log macro -- KERN_DEBUG , KERN_INFO etc




// Debug Macros
/*
KERN_EMERG 		-- Used for emergency messages, usually those that precede a crash.
KERN_ALERT 		-- Situation requiring immediate action.
KERN_CRIT 		-- Critical conditions, often related to serious hardware or software failures.
KERN_ERR 		-- Used to report error conditions; device drivers will often use KERN_ERR to
					report hardware difficulties.
KERN_WARNING 	-- Warnings about problematic situations that do not, in themselves, create serious
					problems with the system.
KERN_NOTICE 	-- Situations that are normal, but still worthy of note. A number of security related
					conditions are reported at this level.
KERN_INFO 		-- Informational messages. Many drivers print information about the hardware
					they find at startup time at this level.
KERN_DEBUG 		-- Used for debugging messages.



DEFAULT_CONSOLE_LOGLEVEL -- The default value of LOG priority is defined in <kernel/printk.c>

*/
#define LOG_MSG_EMERG(DEBUG_MSG)		printk(KERN_EMERG   DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_ALERT(DEBUG_MSG)		printk(KERN_ALERT   DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_CRIT(DEBUG_MSG)		printk(KERN_CRIT    DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_ERR(DEBUG_MSG)		printk(KERN_ERR     DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_WARNING(DEBUG_MSG)	printk(KERN_WARNING DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_NOTICE(DEBUG_MSG)		printk(KERN_NOTICE  DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_INFO(DEBUG_MSG)		printk(KERN_INFO    DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)
#define LOG_MSG_DEBUG(DEBUG_MSG)		printk(KERN_DEBUG   DEBUG_MSG " [file: %s] [line: %d]", __FILE__, __LINE__)



#undef PDEBUG /* undef it, just in case */
#ifdef ENABLE_DEBUG_SUPPORT		/* Enabled / Disabled in makefile */
	#ifdef __KERNEL__
		/* This one if debugging is on, and kernel space */
		#define PDEBUG(fmt, args...) 	printk( KERN_DEBUG "DEBUG: " fmt, ## args)
	#else
		/* This one for user space */
		#define PDEBUG(fmt, args...) 	fprintf(stderr, fmt, ## args)
	#endif
#else
		#define PDEBUG(fmt, args...) 			/* not debugging: nothing */
#endif


// Function declarations
void Delay(int sec);


















#endif /* __utils_h_ */

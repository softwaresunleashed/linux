/*
* hello-1.c :  The simplest kernel module.
*/

#include <linux/module.h>     /* Needed by all modules */
#include <linux/kernel.h>     /* Needed for KERN_ALERT */
#include <linux/init.h>       /* Needed for MACROS */


MODULE_LICENSE("Dual BSD/GPL");


int __init hello_2_init(void)
{
     printk(KERN_EMERG "KERN_EMERG \n");
     printk(KERN_ALERT "KERN_ALERT\n");     
     printk(KERN_CRIT "KERN_CRIT \n");
     printk(KERN_ERR "KERN_ERR \n");     
     printk(KERN_WARNING "KERN_WARNING \n");
     printk(KERN_NOTICE "KERN_NOTICE \n");
     printk(KERN_INFO "KERN_INFO \n");
     printk(KERN_DEBUG "KERN_DEBUG\n");

  /*
      * A non 0 return means init_module failed; module can't be loaded.
      */
    return 0;
}

void __exit hello_2_exit(void)
{
     printk(KERN_ALERT "Goodbye world 1.\n");
}


module_init(hello_2_init);
module_exit(hello_2_exit);



/*
     print_string.c - Send output to the TTY we're running on, regardless if it's through X11, telnet, etc.
     We do this by printing the string to the tty associated with the current task.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>      /* For current */
#include <linux/tty.h>        /* For the tty declarations */
#include <linux/version.h>    /* For LINUX_VERSION_CODE */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Peter Jay Salzman");

static void print_string(char * str)
{
     struct tty_struct *my_tty;

     /* tty struct went into signal struct in 2.6.6 */
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,5))     
     /* The TTY for the current task */
     my_tty = current->tty;
#else
     /* The TTY for the current task, for 2.6.6+ kernels */
     my_tty = current->signal->tty;
#endif

     /* If my_tty is NULL, the current task has no tty you can print to (ie. if it's a daemon).
               If so, there's nothing we can do. */
     if(my_tty != NULL)
     {
          /* my_tty->driver is a struct which holds the tty's functions, one of which (write) is 
                      used to write strings to the tty. It can be used to take a string either from the 
                      user's or kernel's memory segment. 
                         param 1 : tty to write to, because the same function would be used for all tty's of a certain type.
                         param 2 : controls whether the function receives a string from kernel memory (false, 0) or from user memory (true, non zero). BTW this param has been removed in kernels >  2.6.9
                         param 3 : pointer to string.
                         param 4 : length of string.

                     As you will see, sometimes it's necessary to use preprocessor stuff to create code that works for different kernel versions. 
                  */
          (my_tty->driver)->ops->write(my_tty,             /* The TTY itself */
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,9))
                                   0,                  /* Don't take the string from user space */
#endif
                                   str,                /* String */
                                   strlen(str));       /* Length */

          /*
                    ttys were originally hardware devices, which (usually) strictly followed the ASCII standard. 
                    In ASCII, to move to a new line you need two characters, a carriage return, and a line feed.

                    On Unix, the ASCII line feed is used for both purposes - so we can't just use \n, 
                    because it wouldn't have a carriage return and the next line will start at the column 
                    right after the line feed.
                 */
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,9))
      (my_tty->driver)->ops->write(my_tty, 0, "\015\012", 2);
#else
       (my_tty->driver)->ops->write(my_tty, "\015\012", 2);      
#endif                 

     }
     
}


static int __init print_string_init(void)
{
     print_string("This module has been inserted. Hello world!");
     return 0;
}



static void __exit print_string_exit(void)
{
     print_string("This module has been removed. Farewell world!");
     return 0;
}


module_init(print_string_init);
module_exit(print_string_exit);






















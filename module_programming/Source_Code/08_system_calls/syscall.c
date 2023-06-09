
/*
     syscall.c
     System Call "stealing" sample
*/

/*
Standard Header files in Kernel Modules
*/
#include <linux/kernel.h>     /*we're doing kernel work*/
#include <linux/module.h>     /* specifically, a module */
#include <linux/moduleparam.h>     /* which will have params */
#include <linux/unistd.h>     /* The list of system calls */

/* For the current (process) structure, we need this to know who the current user is. */
#include <linux/sched.h>
#include <asm/uaccess.h>

/* The system call table (a table of functions). 
We just define this as external, and the kernel will fill it up for us when are are insmod'ed 

sys_call_table is no longer exported in 2.6.x kernels. 
If you really want to try this DANGEROUS module you will have to apply the supplied patch 
agains your current kernel and recompile it.
*/
extern void *sys_call_table[];


/* UID we want to spy on - will be filled from the command line */
static int uid;
module_param(uid, int, 0644);


/* A pointer to the original system call. The reason we keep this, rather than call the original 
function (sys_open), is because somebody else might have replacd the system call before us. 
Note that this is not 100% safe, because if another module replaced sys_open before us, then 
when we're inserted we'll call the function in that module - and it might be removed before we 
are. 

Another reason for this is that we can't get sys_open. It's a static variable, so it is not exported 
*/
asmlinkage int (*original_call)(const char *, int, int);


/*
The function we'll replace sys_open (the function called when you call the open system call) with.
To find the exact prototype, with the number and type of arguments, we find the original function
first (it's at fs/open.c).

In theory, this means that we're tied to the current version of the kernel. In practise, the system 
calls almost never change (it wold wreck havoc and require programs to be recompiled, since the 
system calls are the interface between the kernel and the processes).
*/
asmlinkage int our_sys_open(const char *filename, int flags, int mode)
{
     int i = 0;
     char ch;

     /* Check if this is the user we're spying on
          */

  //   if(uid == current->uid)
     {
          /* Report the file,  if relevant */
          printk("Opened file by %d: ", uid);
          do
          {
               get_user(ch, filename + i);
               i++;
               printk("%c", ch);
          }while(ch != 0);
          printk("\n");
     }

     /* Call the original sys_open - otherwise, we lose the ability to open files */
     return original_call(filename, flags, mode);
}



/* Initialize the module - replace the system call */
int init_module()
{
     /* Warning - too late for it now, but maybe for next time... */
     printk("I'm dangerous. I hope you did a sync before you insmod'ed me.\n");
     printk("My counterpart, cleanup_module(), is even more dangerous. \n");
     printk("If you value your file system, it will be \"sync; rmmod\" \n");
     printk("when you remove this module. \n");

     /*
               Keep a pointer to the original function in orignal_call, and then replace the sytem call 
               in the system call table with our_sys_open
          */
      original_call = sys_call_table[__NR_open];
     sys_call_table[__NR_open] = our_sys_open;

     /* To get the address of the function for system call 'foo', go to sys_call_table[__NR_foo] */

     printk("Spying on UID: %d \n", uid);

     return 0;
}


/* Cleanup - unregister the appropriate file from /proc */
void cleanup_module()
{
     /* Return the system call back to normal */
     if(sys_call_table[__NR_open] != our_sys_open)
     {
          printk("Somebody else also played with the ");
          printk("open system call \n");
          printk("The system may be left in an unstable state. \n");
     }
     sys_call_table[__NR_open] = original_call;
     
}



















/*
     procfs.c - create a "file" in /proc, which allows both input and output
*/

#include <linux/kernel.h>     /* We're doing kernel work */
#include <linux/module.h>     /* Specifically, a module */
#include <linux/proc_fs.h>    /* Necessary becuase we use proc fs */
#include <asm/uaccess.h>      /* for get_user()   &   put_user() */


/* Here we keep the last message received to prove that we can process our input. */

#define MESSAGE_LENGTH   80
static char Message[MESSAGE_LENGTH];
static struct proc_dir_entry * Our_Proc_File;
static struct proc_dir_entry proc_root;


#define PROC_ENTRY_FILENAME "rw_test3"

static size_t module_output(struct file *filp,    /* see include/linux/fs.h */
                              char * buffer,       /* buffer to fill with data */
                              size_t length,      /* length of the buffer */
                              loff_t * offset)
{
     static int finished = 0;
     int i;
     char message[MESSAGE_LENGTH + 30];

     printk(KERN_INFO "[Sudhanshu] module_output called...\n");

     /*
               We return 0 to indicate end of file, that we have no more information. 
               Otherwise, processes will continue to read from us in an endless loop.
     */
     if(finished)
     {
          finished = 0;
          return 0;
     }

     /*
               We use put_user to copy the string from the kernel's memory segment to the memory 
               segment of the process that called us.
               get_user() , BTW, is used for the reverse.
     */
     sprintf(message, "Last Input is: %s", Message);
     for(i=0; i < length && message[i]; i++)
          put_user(message[i], buffer+i);

     /*
               Notice, we assume here that the size of the message is below len, or it will be truncated.
               In real life situation, if the size of the message is less than len then we'd return len 
               and on the second call start filling the buffer with the len+1'th byte of the message.
     */
     finished = 1;

     return i;           /* Return the number of bytes 'read' */
}


static size_t
module_input(struct file *filp, const char * buff, size_t len, loff_t * off)
{
     int i;

     printk(KERN_INFO "[Sudhanshu] module_input called...\n");

     /*
               Put the input into Message, where module_output will later be able to use it.
          */
         for(i=0; i <  MESSAGE_LENGTH-1 && i < len; i++)
               get_user(Message[i], buff + i);

         Message[i] = '\0';        /* We want a standard, zero terminated string */
          return i;
}




/*
This function decides whether to allow an operation (return zero) or not allow it (return a 
non-zero which indicates why it is not allowed).

The operation can be one of the following values : 
0 - Execute (run the "file" - meaningless in our case.
2 - Write (input to the kernel module)
4 - Read (output from the kernel module)

This is the real function that checks file permissions. The permissions returned by 'ls -l' are for 
reference only, and can be overridden here.
*/
static int module_permission(struct inode * inode, int op, struct nameidata * foo)
{

   printk(KERN_INFO "[Sudhanshu] module_permission called...\n");

   /* Sudhanshu : 13 mar 2011 : Dirty Hack.... there was some problem in the if block and it returned PERMISSION DENIED all the time. */
   return 0;



   /* We allow everybody to read from our module, but only root may write to it. */
     if(op == 4 || (op == 2 /* && current->euid == 0 */))
          return 0;

   /* If it's anything else, access is denied */
   return -EACCES;
}


/* 
The file is opened - we dont really care about that, but it does mean we need to increment 
the module's reference count. 
*/
int module_open(struct inode * inode, struct file * file)
{
     printk(KERN_INFO "[Sudhanshu] module_open called...\n");

     try_module_get(THIS_MODULE);
     return 0;
}

/*
The file is closed - again, interesting only because of the reference count.
*/
int module_close(struct inode * inode, struct file * file)
{
     printk(KERN_INFO "[Sudhanshu] module_close called...\n");

     module_put(THIS_MODULE);
     return 0;           /* Success */
}



static struct file_operations File_ops_4_Our_Proc_File = {
                    .read     = module_output,
                    .write    = module_input,
                    .open     = module_open,
                    .release  = module_close,
};

/*
Inode operations for our proc file. 
We need it so we'll have some place to specify the file operations structure we want to use, and 
the function we use for permissions. It's also possible to specify functions to be called for 
anything else which could be done to an inode (although we dont' bother, we just put NULL).
*/
static struct inode_operations Inode_ops_4_Our_Proc_File = {
                    .permission    = module_permission,    /* check for permissions */
};


/* Module Initialization and cleanup */
int init_module()
{
     printk(KERN_INFO "[Sudhanshu] init_module called...\n");

     Our_Proc_File = create_proc_entry(PROC_ENTRY_FILENAME, 664, NULL);

     if(Our_Proc_File == NULL)
     {
          printk(KERN_INFO "Error: Could not initialize /proc/rw_test\n");
          return -ENOMEM;
     }

     //Our_Proc_File->owner = THIS_MODULE;
     Our_Proc_File->proc_iops = &Inode_ops_4_Our_Proc_File;
     Our_Proc_File->proc_fops = &File_ops_4_Our_Proc_File;
     Our_Proc_File->mode      =    S_IFREG | S_IRUGO | S_IWUSR;
     Our_Proc_File->uid       =    0;
     Our_Proc_File->gid       =    0;
     Our_Proc_File->size      =    80;       /* size of the proc file */

     return 0;
}


void cleanup_module()
{
     remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
     printk(KERN_INFO "[Sudhanshu] Removed PROC FS Entry.");

}


























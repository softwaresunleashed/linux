/*
     sleep.c -- create a /proc file, and if several processes try to open it at the same time, put all 
     but one to sleep.
*/

#include <linux/kernel.h>     /* We're doing kernel work */
#include <linux/module.h>     /* Specifically, a module */
#include <linux/proc_fs.h>    /* Necessary because we use proc fs */

#include <linux/sched.h>      /* For putting processes to sleep and waking them up */
#include <asm/uaccess.h>      /* for get_user() and put_user() */



/* The Module's file functions */

/* Here we keep the last message received, to prove that we can process our input */
#define MESSAGE_LENGTH   80
static char Message[MESSAGE_LENGTH];

static struct proc_dir_entry * Our_Proc_File;
static struct proc_dir_entry proc_root;

#define PROC_ENTRY_FILENAME   "sleep"

/*
     Since we use the file operations struct, we can't use the special proc output provisions --
     we have to use a standard read function, which is this function.
*/
static ssize_t module_output(struct file *file, /* see include/linux/fs.h */
                                 char * buf, /* The buffer to put data to (in the user segment )*/
                                 size_t len, /* The lenght of the buffer */
                                 loff_t * offset)
{
     static int finished = 0;
     int i;
     char message[MESSAGE_LENGTH + 30];

     /* Return 0 to signify end of file -- that we have nothing more to say at this point. */
     if(finished)
     {
          finished = 0 ;
          return 0;
     }

     // If you don't understand this by now, you're hopeless as a kernel programmer.
     sprintf(message, "Last input: %s\n", Message);
     for(i = 0; i < len && message[i]; i++)
          put_user(message[i], buf + i);

     finished = 1;
     return i;      /* Return the number of bytes "read" */
     
}


/* This function receives input from the user when the user writes to the /proc  file */
static ssize_t module_input(struct file *file, /* The file itself */
                              const char *buf,    /* The buffer with input */
                              size_t    length,   /* The buffer's length from user */
                              loff_t * offset)    /* offset to file -- ignore */
{
     int i;

     // Put the input into Message, where module_output will later be able to use it.
     for(i = 0; i < MESSAGE_LENGTH - 1 && i < length; i++)
          get_user(Message[i], buf + i);


     // We want a standard, zero terminated string
     Message[i] = '\0';

     // We need to return the number of input characters used.
     return i;     
}


/*
     Already_Open = 1 , if the file is currently open by somebody
*/
int Already_Open = 0;

/*
Queue of processes who want out file
*/
DECLARE_WAIT_QUEUE_HEAD(WaitQ);


/* This function is called when the /proc file is opened */
static int module_open(struct inode * inode, struct file * file)
{
   /*
          If the file's flag include O_NONBLOCK, it means the process doesn't want to wait for the file.
          In this case, if the file is already open, we should fail with -EAGAIN, meaing 
          "you'll have to try again", instead of blocking a process which would rather stay awake.
     */
     if((file->f_flags & O_NONBLOCK) && Already_Open)
          return -EAGAIN;

   /*
          This is the correct place for try_module_get(THIS_MODULE) because if a process is in the 
          loop, which is within the kernel module, the kernel module must not be removed.
     */
     try_module_get(THIS_MODULE);

  /*
         If the file is already open, wait until it isn't
   */
     while(Already_Open)
     {
        int i, is_sig = 0;

        /* This function puts the current process, including any system calls, such as us, to 
                    sleep. Execution whill be resumed right after the function call, either because 
                    somebody called wake_up(&WaitQ) (only module_close does that, when the file is 
                    closed) or when a signal, such as Ctrl-C, is sent to the process. 
              */
              wait_event_interruptible(WaitQ, !Already_Open);


        /*    If we woke up because we got a signal we're not blocking, return -EINT (fail the 
                     system call). This allows processes to be killed or stopped.
               */
              for(i = 0; i < _NSIG_WORDS && !is_sig; i++)
                    is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];

              if(is_sig)
              {
                  module_put(THIS_MODULE);
                  return -EINTR;
              }
     }

     /* If we got here , Already_Open */
     Already_Open = 1;
     return 0;      /* Allow the access */
}




/* Called when the /proc file is closed */
int module_close(struct inode * inode, struct file * file)
{
     /*
              Set Already_Open to zero, so one of the processes in the WaitQ will be able to set 
              Already_Open back to one and to open the file. ALl the other processes will be called 
              when Already_Open is back to one, so they'll go back to sleep.   
          */
     Already_Open = 0;

     /* Wake up all the processes in WaitQ, so if anybody is waiting for the file, they 
               can have it. 
          */
     wake_up(&WaitQ);

     module_put(THIS_MODULE);

     return 0;      /* Success */
}


/*
     This function decides whether to allow an operation (return ZERO) or not allow it 
          (return a non-zero which indicates why it is not allowed). 

     The operation can be one of the following values : 
     0 - Execute (run the "file" - meaningless in our case)
     2 - Write (input to the kernel module).
     4 - Read (output from the kernel module).

     This is the real function that checks file permissions. The permissions returned by ls -l are for 
     reference only,, and can be over ridden here.
*/
static int module_permission(struct inode * inode, int op, struct nameidata * nd)
{

     #if 0
               READ  from file : op = 42
               WRITE from file : op = 36
     #endif


     printk(KERN_INFO "module_permission : op = %d", op);

     
     /* We allow everybody to read from our module, but only root (uid 0) may write to it */
   //  if(op == 4 || (op == 2 /* && current->euid == 0 */))
          return 0;

     /* If it's anything else, access is denied */
     return -EACCES;
}


/* Structures to register as the /proc file, with pointers to all the relevant functions. */


/* File operations for our proc file. This is where we place pointers to all the functions called 
when somebody tried to do something to our file. 

     NULL means we don't want to deal with something. */
static struct file_operations File_Ops_4_Our_Proc_File = 
{    
     .read     =    module_output,      /* "read" from the file */
     .write    =    module_input,       /* "write" to the file */
     .open     =    module_open,        /* called when the /proc file is opened */
     .release  =    module_close,       /* called when it's closed */
};


/* Inode Operations for our proc file. 
We need it so we'll have somewhere to specify the file operations structure we want to use, 
and the function we use for permissions. It's also possible to specify functions to be called for 
anything else which could be done to an inode (although we don't bother, we just put NULL). */
static struct inode_operations Inode_Ops_4_Our_Proc_File = 
{
     .permission    =    module_permission,  /* Check for permissions */
};


/* Module initialization and cleanup */

/* -- Initialize the module - register the proc file -- */
int init_module()
{
     int rv = 0;
     Our_Proc_File = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);

//     Our_Proc_File->owner = THIS_MODULE;   //Commented by Sudhanshu 

     Our_Proc_File->proc_iops      =    &Inode_Ops_4_Our_Proc_File;
     Our_Proc_File->proc_fops      =    &File_Ops_4_Our_Proc_File;

     Our_Proc_File->mode = S_IFREG  |   S_IRUGO   |    S_IWUSR;

     Our_Proc_File->uid  =    0;
     Our_Proc_File->gid  =    0;
     Our_Proc_File->size =    80;

     if(Our_Proc_File == NULL)
     {
          rv = -ENOMEM;
          remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
          printk(KERN_INFO"Error: Could not initialize /proc/sleep\n");
     }

     return rv;
}


/* Cleanup - unregister our file from /proc. 
This could get dangerous if there are still processes waiting in WaitQ, because they are inside our 
open function, which will get unloaded. 
How to avoid such a situation will be explained in chapter 10.
*/
void cleanup_module()
{
     remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
}



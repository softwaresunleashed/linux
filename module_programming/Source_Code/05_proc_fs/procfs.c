/*
* procfs.c - create a "file" in /proc
*/

#include <linux/module.h>     /* Specifically, a module */
#include <linux/kernel.h>     /* We're doing kernel work */
#include <linux/proc_fs.h>    /* Necessary because we use the proc fs */




struct proc_dir_entry *Our_Proc_File;
struct proc_dir_entry proc_root;

/*
Put data into the proc fs file.
Arguments
=========
1. The buffer where the data is to be inserted, if
you decide to use it.
2. A pointer to a pointer to characters. This is
useful if you don't want to use the buffer
allocated by the kernel.
3. The current position in the file
4. The size of the buffer in the first argument.
5. Write a "1" here to indicate EOF.
6. A pointer to data (useful in case one common
read for multiple /proc/... entries)

Usage and Return Value
* ======================
* A return value of zero means you have no further
* information at this time (end of file). A negative
* return value is an error condition.
*
* For More Information
* ====================
* The way I discovered what to do with this function
* wasn't by reading documentation, but by reading the
* code which used it. I just looked to see what uses
* the get_info field of proc_dir_entry struct (I used a
* combination of find and grep, if you're interested),
* and I saw that it is used in <kernel source
* directory>/fs/proc/array.c.
*
* If something is unknown about the kernel, this is
* usually the way to go. In Linux we have the great
* advantage of having the kernel source code for
* free - use it.
*/

ssize_t
procfile_read(char *buffer, char **buffer_location, off_t offset, 
                                   int buffer_length, int *eof, void *data)
{
     
     int len = 0;             /* The number of bytes actually used */
     static int count = 1;

     printk(KERN_INFO "inside /proc/test : procfile_read\n");


  /*
     * We give all of our information in one go, so if the
     * user asks us if we have more information the
     * answer should always be no.
     *
     * This is important because the standard read
     * function from the library would continue to issue
     * the read system call until the kernel replies
     * that it has no more information, or until its
     * buffer is filled.
     */



     if (offset > 0) 
     {
          printk(KERN_INFO "offset %d : /proc/test : procfile_read, \
                                        wrote %d Bytes\n", (int)(offset), len);
          *eof = 1;
          return len;
     }

  /*
     * Fill the buffer and get its length
     */
     len = sprintf(buffer, "For the %d%s time, go away!\n", count,
                              (count % 100 > 10 && count % 100 < 14) ? "th" :
                              (count % 10 == 1) ? "st" :
                              (count % 10 == 2) ? "nd" :
                              (count % 10 == 3) ? "rd" : "th");
     count++;



  /*
     * Return the length
     */
   printk(KERN_INFO "leaving /proc/test : procfile_read, wrote %d Bytes\n", len);

   return len;
}


int init_module()
{
     int rv = 0;
     Our_Proc_File = create_proc_entry("test", 0644, NULL);

     if (Our_Proc_File == NULL)
     {
          rv = -ENOMEM;
          remove_proc_entry("test", &proc_root);
          printk(KERN_INFO "Error: Could not initialize /proc/test\n");
     }
     Our_Proc_File->read_proc = procfile_read;
     //Our_Proc_File->owner = THIS_MODULE;
     Our_Proc_File->mode = S_IFREG | S_IRUGO;
     Our_Proc_File->uid = 0;
     Our_Proc_File->gid = 0;
     Our_Proc_File->size = 37;
     printk(KERN_INFO "Created /proc/test:\n");
     return rv;
}


void cleanup_module()
{
     remove_proc_entry("test", &proc_root);       /* <--Sudhanshu : 10 Mar 2011 :  as expected some bug in this API ...no proper definition for this function available.*/
     printk(KERN_INFO "/proc/test removed\n");
}






























/*
     intrpt.c -- A keyboard interrupt handler.
*/

/* The necessary header files */
#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/sched.h> 
#include <linux/workqueue.h> /* We schedule tasks here */
#include <linux/interrupt.h> /* For irqreturn_t */
#include <asm/io.h>

#define MY_WORK_QUEUE_NAME    "Intrpt.c"

static struct workqueue_struct * my_workqueue;




/* This will get called by the kernel as soon as it's safe to do everything normally allowed by kernel modules */
static void got_char(void * scancode)
{
     printk("Scan Code %x %s.\n", (int)*((char *)scancode) & 0x7F, *((char *)scancode) & 0x80 ? "Released" : "Pressed");
}


/*
This function services keyboard interrupts. It reads the relevant information from the keyboard 
and then puts the non time critical part into the work queue.

This will be run when the kernel considers it safe.
*/
irqreturn_t irq_handler(int irq, void * dev_id, struct pt_regs *regs)
{
     /*
               This variables are static because they need to be accessible (through pointers) to the bottom half routine.
          */
     static int initialised = 0;
     static unsigned char scancode;
     static struct work_struct task;
     unsigned char status;

     /* Read Keyboard status */
     status = inb(0x64);      // Read from Command register (0x64) of keyboard i8042 chipset
     scancode = inb(0x60);    // Read from Data register (0x60) of keyboard i8042 chipset

     if(initialised == 0)
     {
          INIT_WORK(&task, got_char);
          initialised = 1;
     }
     else
     {
          PREPARE_WORK(&task, got_char);
     }
     queue_work(my_workqueue, &task);

     return IRQ_HANDLED;
     
}




/*
     Initialize the module -- register the IRQ Handler.
*/
int __init init_module()
{
     my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);

   /*
          Since the keyboard handler won't co-exist with another handler, such as us, we have to 
          disable it (free its IRQ) before we do anything.
          Since we don't know where it is, there's no way to reinstate it later - so the computer will 
          have to be rebooted when we're done.
     */
   free_irq(1, NULL);

   /* Request IRQ 1, the keyboard IRQ, to go to our irq_handler. 
          SA_SHIRQ means we're willing to have other handlers on this IRQ. 
          SA_INTERRUPT can be used to make the handler into a fast interrupt (system wide interrupts disabled)
     */
   return request_irq(1, /* The number of the keyboard IRQ on PCs (refer to /proc/interrupts for list of IRQ numbers &  their names */
                      irq_handler, /* our handler */
                         IRQF_SHARED, "test_keyboard_irq_handler",
                         (void *)(irq_handler));

      
}


/* Cleanup */

void __exit    cleanup_module()
{
     /* This is only here for completeness. It's totally irrelevant, since we don't have a way to 
               restore the normal keyboard interrupt so the computer is completely useless and has 
               to be rebooted 
         */
     free_irq(1, NULL);
}


/* Some work_queue related functions are just avaialble to GPS licences Modules */
MODULE_LICENSE("GPL");

















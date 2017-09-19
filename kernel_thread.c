#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h> // for spinlock
//#include <linux/spinlock_types.h>
//#include <asm/spinlock.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Neeraj Pal <neerajpal09@gmail.com>");
MODULE_DESCRIPTION("Working code for Kernel Threads");
static spinlock_t my_lock = __SPIN_LOCK_UNLOCKED(); //SPIN_LOCK_UNLOCKED;
 
//spinlock_t my_lock;
//spin_lock_init(&my_lock);
int var;
 
static struct task_struct *t1;
static struct task_struct *t2;
//static int reader();
 
int reader(void)
{
	int z;
	spin_lock(&my_lock);
	if (spin_is_locked(&my_lock))
                {
                        printk(KERN_INFO "Read Locked\n");
                }
                else
                {
                        printk(KERN_INFO "Read UnLocked\n");
                }
 
	 z = var;
	spin_unlock(&my_lock);
	if (spin_is_locked(&my_lock))
                {
                        printk(KERN_INFO "Read Locked\n");
                }
                else
                {
                        printk(KERN_INFO "Read UnLocked\n");
                }
 
	
	return z;
}
 
 
static int t2_f(void *unused)
{
 
        int sub=2,i=2;
        spin_lock(&my_lock);
        var = 1;
        if (spin_is_locked(&my_lock))
        {
                printk(KERN_INFO "Locked\n");
        }
        spin_unlock(&my_lock);
        if (!spin_is_locked(&my_lock))
        {
                printk(KERN_INFO "UnLocked\n");
        }
 
        while(reader())
        {
                sub = sub* i;
                i++;
                printk(KERN_ALERT "Thread 2 running...\n");
                printk(KERN_NOTICE "mul  is %d ",sub);
                ssleep(5);
        }
        printk(KERN_ALERT "Stopping thread 2...\n");
        do_exit(0);
        return 0;
}
 
 
 
 
 
static int t1_f(void *unused)
{
	
    int sum=0,i=1;
	spin_lock(&my_lock);
	var = 1;
    if (spin_is_locked(&my_lock))
	{
		printk(KERN_INFO "Locked\n");
	}
	spin_unlock(&my_lock);
	if (!spin_is_locked(&my_lock))
        {
                printk(KERN_INFO "UnLocked\n");
        }
	
	while(reader())
	{
		sum = sum + i;
		i++;
		printk(KERN_ALERT "Thread 1 running...\n");
		printk(KERN_NOTICE "sum  is %d ",sum);
		ssleep(5);
	}
	printk(KERN_ALERT "Stopping thread 1 ...\n");
	do_exit(0);
	return 0;
}
 
static int __init init_thread(void)
{
	printk(KERN_INFO "Thread creating ...\n");
	t1 = kthread_create(t1_f,NULL,"mythread1");
	t2 = kthread_create(t2_f,NULL,"mythread2");
	if(t1 &&  t2)
	{
		printk(KERN_INFO "Thread Created Sucessfully\n");
		wake_up_process(t1);
		wake_up_process(t2);
	}
	else
	{
		printk(KERN_ALERT "Thread Creation Failed\n");
	}
	return 0;
}
 
static void __exit cleanup_thread(void)
{
	printk(KERN_INFO "Cleaning up ...\n");
	int ret,ret1;
	spin_lock(&my_lock);
	if (spin_is_locked(&my_lock))
    {
         printk(KERN_INFO "Locked\n");
    }
 
	var = 0;
	spin_unlock(&my_lock);
	if (!spin_is_locked(&my_lock))
    {
        printk(KERN_INFO "UnLocked\n");
    }
 
	
	ret = kthread_stop(t1);
    ret1 = kthread_stop(t2);
	if(!ret && !ret1)
		printk(KERN_ALERT "Thread stopped");
}
module_init(init_thread)
module_exit(cleanup_thread)


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h> 	//kthreads 
#include <linux/delay.h> 	//ssleep 
#include <linux/signal.h> 	//signals  
#include <linux/semaphore.h> // semaphores

//Create two kernel threads (one give and one take threads), explicitly starting the take thread firstly.
static struct task_struct *kth1, *kth2; 

//semaphore declaration 
struct semaphore sem;
//mutex declaration
struct mutex lock1, lock2;

//The give thread prints the “lkm: give” message with a busy waiting of 1 second; And then schedule the take
//thread using a process synchronization technique.
int thread_give(){
	//lock
	mutex_lock(&lock2);
	while(!kthread_should_stop()){
		// printk("give thread!\n"); 
		down(&sem);
		printk("lkm: give\n");
		ssleep(1);
		up(&sem);
	}
	// printk(KERN_INFO "give thread stopping\n");
	//unlock 
	mutex_unlock(&lock2);
	do_exit(0); 
	return 0; 
}
//The take thread prints the “lkm: take” message with a busy waiting of 1 second; And then schedule the give
//thread using the process synchronization technique.
int thread_take(){
	// down(&sem);
	// printk("lkm: take\n");
	mutex_lock(&lock1);
	while(!kthread_should_stop()){
		// printk("take thread!\n");
		down(&sem);
		printk("lkm: take\n"); 
		ssleep(1);
		up(&sem);
	}
	// up(&sem);
	// printk(KERN_INFO "take thread stopping\n"); 
	mutex_unlock(&lock1);
	do_exit(0); 
	return 0; 
}

static int __init lkm_init(void){
	printk("kthread init called\n"); 
	//semaphore initialization
	sema_init(&sem, 1);
	//mutex initialization
	mutex_init(&lock1);
	mutex_init(&lock2);
#if 0
	kth1 = kthread_run(th_function, NULL, "kth1_name");
	if (kth1)
		printk(KERN_ERR "thread create success\n"); 
	else
		printk(KERN_ERR "can't create thread\n"); 
	
#else
	//Take thread goes first
	kth1 = kthread_run(thread_take, NULL, "kth1_take"); 
	if (kth1){
		printk(KERN_ERR "lkm: the take thread was generated firstly\n"); 
	}else{
		printk(KERN_ERR "can't create take thread\n"); 
	}
	//Give thread executed secondly
	kth2 = kthread_run(thread_give, NULL, "kth2_give"); 
	if (kth2){
		printk(KERN_ERR "lkm: the give thread was generated secondly\n"); 
	}else{
		printk(KERN_ERR "can't create give thread\n"); 
	}

#endif 

	return 0; 
}

static void __exit lkm_exit(void){
	printk("lkm: kthread exit called\n");
	kthread_stop(kth1); 
	kthread_stop(kth2);
}

module_init(lkm_init);
module_exit(lkm_exit); 

MODULE_LICENSE("GPL"); 

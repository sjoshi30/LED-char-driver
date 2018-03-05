/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in User Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 
*/

#define _GNU_SOURCE
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/msr.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>   /* required for semaphores */
#include <linux/sched.h> 
#include <linux/init.h>
#include <linux/mutex.h> 
#include <linux/time.h> 
#include<linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/ioctl.h>

#define CONFIG _IOR('I', 1, struct User_Input)
/* ----------------------------------------------- DRIVER gmem --------------------------------------------------
 
 Basic driver example to show skelton methods for several file operations.
 
 ----------------------------------------------------------------------------------------------------------------*/



#define DEVICE_NAME    "RGBLed"  // device name to be created and registered

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

struct Pin_Mapping
{
	int Pin;
	int Dir;
	int Mux1;
	int Mux1_Val;
	int Mux2;
	int Mux2_Val;
}GpioPin[14] = {
/*	Pin Dir Mux1 Val Mux2 Val */
	{11, 32,  0, 0,  0, 0}, //IO0
	{12, 28, 45, 0,  0, 0}, //IO1
	{13, 34, 77, 0,  0, 0}, //IO2
	{14, 16, 76, 0, 64, 0}, //IO3
	{ 6, 36,  0, 0,  0, 0}, //IO4
	{ 0, 18, 66, 0,  0, 0}, //IO5
	{ 1, 20, 68, 0,  0, 0}, //IO6
	{38,  0,  0, 0,  0, 0}, //IO7
	{40,  0,  0, 0,  0, 0}, //IO8
	{ 4, 22, 70, 0,  0, 0}, //IO9
	{10, 26, 74, 0,  0, 0}, //IO10
	{ 5, 24, 44, 0, 72, 0}, //IO11
	{15, 42,  0, 0,  0, 0}, //IO12
	{ 7, 30, 46, 0,  0, 0}  //IO13
};

static struct gpio request_gpio_arr[] = 
{
	{11, 0, "GPIO11"},
	{32, 0, "GPIO32"},
	{12, 0, "GPIO12"},
	{28, 0, "GPIO28"},
	{45, 0, "GPIO45"},
	{13, 0, "GPIO13"},
	{34, 0, "GPIO34"},
	{77, 0, "GPIO77"},
	{14, 0, "GPIO14"},
	{16, 0, "GPIO16"},
	{76, 0, "GPIO76"},
	{64, 0, "GPIO64"},
	{6, 0, "GPIO6"},
	{36, 0, "GPIO36"},
	{0, 0, "GPIO0"},
	{18, 0, "GPIO18"},
	{66, 0, "GPIO66"},
	{1, 0, "GPIO1"},
	{20, 0, "GPIO20"},
	{68, 0, "GPIO68"},
	{38, 0, "GPIO38"},
	{40, 0, "GPIO40"},
	{4, 0, "GPIO4"},
	{22, 0, "GPIO22"},
	{70, 0, "GPIO70"},
	{10, 0, "GPIO10"},
	{26, 0, "GPIO26"},
	{74, 0, "GPIO74"},
	{5, 0, "GPIO5"},
	{24, 0, "GPIO24"},
	{44, 0, "GPIO44"},
	{72, 0, "GPIO72"},
	{15, 0, "GPIO15"},
	{42, 0, "GPIO42"},
	{7, 0, "GPIO7"},
	{30, 0, "GPIO30"},
	{46, 0, "GPIO46"},	
};

struct User_Input{
	int Pin_Intensity;
	int Pin_Red;
	int Pin_Green;
	int Pin_Blue;
}Input;

int p_r, d_r, m1_r, m2_r;
int p_g, d_g, m1_g, m2_g;
int p_b, d_b, m1_b, m2_b;



/* per device structure */
struct led_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	char in_string[256];			/* buffer for the input string */
	int current_write_pointer;
	//struct User_Input i
} *led_devp;

static dev_t led_dev_number;      /* Allotted device number */
struct class *led_dev_class;          /* Tie with the device model */
static struct device *led_dev_device;

int mux_gpio_set(unsigned int gpio, unsigned int value);

/*
* Open led driver
*/
int led_driver_open(struct inode *inode, struct file *file)
{
	struct led_dev *led_devp;
//	printk("\nopening\n");
	/* Get the per-device structure that contains this cdev */
	led_devp = container_of(inode->i_cdev, struct led_dev, cdev);
	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = led_devp;
	printk("\n%s is openning \n", led_devp->name);
	return 0;
}

/*
 * Release led driver
 */
int led_driver_release(struct inode *inode, struct file *file)
{
	struct led_dev *led_devp = file->private_data;
	
	printk("\n%s is closing\n", led_devp->name);
	
	return 0;
}

/*
 * Write to led driver
 */
ssize_t led_driver_write(struct file *file, const char *buf,
            size_t count, loff_t *ppos)
 {
	int* k_value = (int*)kmalloc(sizeof(int),GFP_KERNEL);
	int* red  = (int*)kmalloc(sizeof(int),GFP_KERNEL);
	
	copy_from_user(k_value, (int *)buf, sizeof(int));
	printk(KERN_DEBUG"Ready to write k is %d\n",*k_value);
	printk(KERN_DEBUG"Completed write\n");
	/* 0 R G B*/
	*red = (0x04 & *k_value)>>2u;
	*blue = (0x01 & *k_value);
	*green = (0x02 & *k_value)>>1u;
	printk(KERN_DEBUG"Red avlue is %d\n",*red);
	gpio_set_value(p_r, (int)*red);
	printk(KERN_DEBUG"value set");
	gpio_set_value(p_b, blue);
	gpio_set_value(p_g, green);

	return 0;
}
char label1,label2,label3;
long led_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk(KERN_DEBUG"Ready to configure");
	struct User_Input* I;
	I = (struct User_Input*)kmalloc(sizeof(struct User_Input),GFP_KERNEL);

	switch(cmd)
	{
		/* TODO: Check for valid input values else return ERRNO */
		case CONFIG:
		printk(KERN_DEBUG"Ready to configure");
		//Retrive the inputs from user space
		copy_from_user(&I, (struct User_Input*) arg, sizeof(struct User_Input*));
		printk(KERN_DEBUG"Copy done");
		/*******************************/
		/* Set the Mux for Red LED */
		p_r = GpioPin[I->Pin_Red].Pin;
		m1_r = GpioPin[I->Pin_Red].Mux1;
		m2_r = GpioPin[I->Pin_Red].Mux2;
		d_r = GpioPin[I->Pin_Red].Dir;
		printk(KERN_DEBUG"p_r %d",p_r);
		printk(KERN_DEBUG"m1_r %d",m1_r);
		printk(KERN_DEBUG"d_r %d",d_r);
		printk(KERN_DEBUG"m2_r %d",m2_r);

		/*******************************/
		/* Select the Mux for Green LED */
		p_g = GpioPin[I->Pin_Green].Pin;
		m1_g = GpioPin[I->Pin_Green].Mux1;
		m2_g = GpioPin[I->Pin_Green].Mux2;
		d_g = GpioPin[I->Pin_Green].Dir;

		/*******************************/
		/* Select the Mux for BLUE LED */
		p_b = GpioPin[I->Pin_Blue].Pin;
		m1_b = GpioPin[I->Pin_Blue].Mux1;
		m2_b = GpioPin[I->Pin_Blue].Mux2;
		d_b = GpioPin[I->Pin_Blue].Dir;
	 	/*******************************/
		printk(KERN_DEBUG"Mux settings");
		//mux_gpio_set(d_r,0);
		printk(KERN_DEBUG"Mux gpio input 1\n");
		// gpio_request(d_r,"GPIO34");
		// gpio_direction_output(d_r,0);
		// //mux_gpio_set(m1_r,0);
		// printk(KERN_DEBUG"Mux gpio input 2\n");
		// gpio_request(m1_r,"GPIO77");
		// gpio_direction_output(m1_r,0);
		// //mux_gpio_set(m2_r,0);
		// printk(KERN_DEBUG"Mux works\n");

		// gpio_request(p_r,"GPIO13");
		// gpio_direction_output(p_r, 1);
		// printk(KERN_DEBUG"P_r done\n");
		gpio_request_array(request_gpio_arr,ARRAY_SIZE(request_gpio_arr));


	// 	/*********************************/
	// 	mux_gpio_set(d_g,0);
	// 	mux_gpio_set(m1_g,0);
	// 	mux_gpio_set(m2_g,0);

	// 	gpio_request(p_g,"GPIO");
	// 	gpio_direction_output(p_g, 1);

	// 	/*********************************/
	// 	mux_gpio_set(d_b,0);
	// 	mux_gpio_set(m1_b,0);
	// 	mux_gpio_set(m2_b,0);

	// 	gpio_request(p_b,"GPIO");
	// 	gpio_direction_output(p_b, 1);
		break;
		
		default:
		break;
	}

	return 0;
}


/* File operations structure. Defined in linux/fs.h */
static struct file_operations led_fops = {
    .owner		= THIS_MODULE,           /* Owner */
    .open		= led_driver_open,        /* Open method */
    .release	= led_driver_release,     /* Release method */
    .unlocked_ioctl		= led_driver_ioctl,        /* IOCTL method */
    .write		= led_driver_write,       /* Write method */
};

/*
 * Driver Initialization
 */
int __init led_driver_init(void)
{
	int ret;
	//int time_since_boot;

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&led_dev_number, 0, 1, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* Populate sysfs entries */
	led_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	/* Allocate memory for the per-device structure */
	led_devp = kmalloc(sizeof(struct led_dev), GFP_KERNEL);
	if (!led_devp) {
		printk("Bad Kmalloc\n"); return -ENOMEM;
	}

	/* Request I/O region */
	sprintf(led_devp->name, DEVICE_NAME);

	/* Connect the file operations with the cdev */
	cdev_init(&led_devp->cdev, &led_fops);
	led_devp->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&led_devp->cdev, (led_dev_number), 1);

	if (ret) {
		printk("Bad cdev\n");
		return ret;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	led_dev_device = device_create(led_dev_class, NULL, MKDEV(MAJOR(led_dev_number), 0), NULL, DEVICE_NAME);		

	printk("led driver initialized.\n");
	return 0;
}

/* Driver Exit */
void __exit led_driver_exit(void)
{
	/* Release the major number */
	unregister_chrdev_region((led_dev_number), 1);

	/* Destroy device */
	device_destroy (led_dev_class, MKDEV(MAJOR(led_dev_number), 0));
	cdev_del(&led_devp->cdev);
	kfree(led_devp);
	
	/* Destroy driver_class */
	class_destroy(led_dev_class);

	printk("led driver removed.\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL v2");


int mux_gpio_set(unsigned int gpio, unsigned int value)
{
	if(gpio>0)
 	{
 	printk(KERN_DEBUG"Mux gpio input\n");
 	gpio_request(gpio,"GPIO");
	gpio_direction_output(gpio,value);
	printk(KERN_DEBUG"Mux gpio set done\n");
	}
	return 0;
}


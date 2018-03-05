
/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in User Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <linux/input.h> /* required for mouse events*/
#include <math.h>
#include "led.h"


#define CYCLE_DURATION 20
#define SEQUENCE_DURATION 500
#define MOUSEFILE "/dev/input/event2" /* Change the event# as per the target machine */
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define LEFT_CLICK 0x110 	//BUTTON_LEFT
#define KEY_PRESS 0x1		//EV_VALUE

static int Intensity_Val;
 
int Red_Led;
int Green_Led;
int Blue_Led;
int Duty_Cycle;
int Exit_Flag =0;
int fd1;

int sequence_count=0;

/* Sequence
 * R  G  B 
 * 1  0  0
 * 0  1  0
 * 0  0  1
 * 1  1  0
 * 0  1  1
 * 1  0  1
 * 1  1  1 */


void Sequence(void);
// void* CheckMouseEvent(void*);
// void IOSetup(void);


/* Check for mouse event */
/* Description : Thread polls for mouse click event and signals to exit once mouse button is clicked */
// void* CheckMouseEvent(void *p)
// {
// 	int fd;
// 	struct input_event click;

	
// 	if((fd = open(MOUSEFILE, O_RDONLY)) == -1)
// 	{
// 		perror("opening device");
// 		exit(EXIT_FAILURE);
// 	}


// 	while(read(fd, &click, sizeof(struct input_event)))
// 	{
// 		if(click.type == MOUSE)  
// 		{
// 			if(((click.code == RIGHT_CLICK) && (click.value == KEY_PRESS)) || ((click.code == LEFT_CLICK) && (click.value == KEY_PRESS)))
// 			{/* Left or Right click event occured, set flag */
// 				Exit_Flag = 1;
// 				break;
// 			}
			
// 		}
// 	}
// return NULL;	
// }


/* Description : The below function executes the led lighting sequence 'R,G,B,RG,RB,GB,RGB' */
void Sequence()
{
	
	int a;
	int loop_i;
	int loop_count;
	int on_time, off_time;
	int* sequence_value=&a;
	/* Calculate the ON/OFF duration as per the Intensity Value passed by the user */
	on_time=((CYCLE_DURATION*0.01)*(Intensity_Val));
	off_time = (CYCLE_DURATION - on_time);
	loop_count = SEQUENCE_DURATION/CYCLE_DURATION;


	switch(sequence_count)
	{
		case 0:	
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Red LED Glows*/
				//a= 4;
				*sequence_value=4;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 1 Red LED ON\n");
				usleep(on_time*100000); 
				*sequence_value= 0;
				write(fd1,(char *)sequence_value,sizeof(int*));
				printf("Sequence 1 Red LED OFF\n");
				usleep(off_time*100000);
			}
			sequence_count++;
			break;
		case 1:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Green LED Glows*/
				*sequence_value = 0x02;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 2 Green LED ON\n");
				usleep(on_time*100000); 
				*sequence_value = 0x00;
				write(fd1,(char *)sequence_value,sizeof(int));
				usleep(off_time*100000);
			}
			sequence_count++;
			break;
		case 2:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Blue LED Glows*/
				*sequence_value = 0x01;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 3 Blue LED ON\n");
				usleep(on_time*1000); 
				*sequence_value = 0x00;
				write(fd1,(char *)sequence_value,sizeof(int));
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		// case 3:
		// 	for(loop_i =0; loop_i < loop_count; loop_i++)
		// 	{/*Red and Green LED Glows*/
		// 		sequence_value = 0x06;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		printf("Sequence 4 Red Green LED\n");
		// 		usleep(on_time*1000); 
		// 		sequence_value = 0x00;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		usleep(off_time*1000);
		// 	}
		// 	sequence_count++;
		// 	break;
		// case 4:
		// 	for(loop_i =0; loop_i < loop_count; loop_i++)
		// 	{/*Blue and Green LED Glows*/
		// 		sequence_value = 0x03;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		printf("Sequence 5 Blue Green LED\n");
		// 		usleep(on_time*1000); 
		// 		sequence_value = 0x00;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		usleep(off_time*1000);
		// 	}
		// 	sequence_count++;
		// 	break;
		// case 5:
		// 	for(loop_i =0; loop_i < loop_count; loop_i++)
		// 	{/*Blue and Red LED Glows*/
		// 		sequence_value = 0x05;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		printf("Sequence 6 Red Blue LED\n");
		// 		usleep(on_time*1000); 
		// 		sequence_value = 0x00;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		usleep(off_time*1000);
		// 	}
		// 	sequence_count++;
		// 	break;
		// case 6:
		// 	for(loop_i =0; loop_i < loop_count; loop_i++)
		// 	{/*Blue, Red and Green LED Glows*/
		// 		sequence_value = 0x07;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		printf("Sequence 7 Blue Red Green LED\n");
		// 		usleep(on_time*1000); 
		// 		sequence_value = 0x00;
		// 		write(fd1,(char *)sequence_value,sizeof(int));
		// 		usleep(off_time*1000);
		// 	}
		// 	sequence_count=0;
			break;
 		default:
	 			break;
	}
 }




int main(int argc, char** argv)
{
	int rerror;
	struct User_Input* I;
	I = (struct User_Input *)malloc(sizeof(struct User_Input));
	pthread_t thread_ID;    /* daemon thread to check for mouse event */
	pthread_attr_t thread_attr;
	int thread_priority=50;  /* Mouse detect */
	struct sched_param param;

	/* Extract the Pins and Duty cycle */
	sscanf(argv[1],"%i", &Intensity_Val);
	sscanf(argv[2],"%i", &Red_Led);
	sscanf(argv[3],"%i", &Green_Led);
	sscanf(argv[4],"%i", &Blue_Led);

	if((Intensity_Val>100)||(Intensity_Val<0))
	{
		printf("Incorrect Intensity Parameter, Allowed value is 0-100\n");
		exit(0);
	}
	
	if(((Red_Led<0)||(Red_Led>13))||((Green_Led<0)||(Green_Led>13))||((Blue_Led<0)||(Blue_Led>13)))
	{
		printf("Only GPIO Pins between 0 to 13 allowed\n");
		exit(0);
	}
	
	I->Pin_Intensity = Intensity_Val;
	I->Pin_Red = Red_Led;
	I->Pin_Green = Green_Led;
	I->Pin_Blue = Blue_Led;

	/* Initialise the GPIO Pins */
	fd1 = open("/dev/RGBLed",O_RDWR);
	ioctl(fd1,CONFIG,&I);

	// pthread_attr_init(&thread_attr);
	// pthread_attr_getschedparam(&thread_attr, &param);
	// param.sched_priority = thread_priority;  /* set thread priority */
	// pthread_attr_setschedparam(&thread_attr, &param);
	// pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
	// rerror = pthread_create(&thread_ID, &thread_attr, &CheckMouseEvent, NULL);
	// if(rerror != 0)
	// 	{
	// 	printf("Error while creating daemon thread \n");
	// 	}
	
	
		Sequence();
	
	// pthread_join(thread_ID,NULL);

	return 0;
}

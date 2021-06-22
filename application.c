/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name:Pramod Khatri
* Student ID:920831584
* GitHub ID:pramodkhatri10
* Project: Device Driver
*
* File: application.c
*
* Description: Application test file for testing ioctl command on device driver.
*
**************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/ioctl.h>

 
#define WRITEVALUE _IOW('a','a',int32_t*)
#define READVALUE _IOR('a','b',int32_t*)
 
int main()
{
        int fd;
        int32_t value;
        int32_t num;
        
        printf("\t--Device Driver Application Test --\n");

 
        printf("\n\t -- Opening Device Driver --\n");
        fd = open("/home/student/Desktop/mydevice", O_RDONLY);
        if(fd < 0) {
                printf("ERROR! File open failed!\n");
                return 0;
        }
 
        printf("\nEnter Number to Send to Device Driver: ");
        scanf("%d",&num);
        printf("\n\t-- Writing Entered Number to Device Driver --\n");
        printf("...\n");
        ioctl(fd, WRITEVALUE, (int32_t*) &num); 
 
        printf("\n\t -- Reading Number from Devie Driver --\n");
        printf("...\n");
        ioctl(fd, READVALUE, (int32_t*) &value);
        printf("\nNumber Written and Read from Device Driver is: %d\n", value);
 
        printf("\n\t-- Closing Device Driver Driver --\n");
	printf("success.\n");

        close(fd);
}

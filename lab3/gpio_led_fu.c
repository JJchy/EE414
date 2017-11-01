// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa 3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/
// http://blog.stackoverflow.com/2009/06/attribution-required/
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159)
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <unistd.h>		// close()
#include "userLEDmmap.h"

#define MIN 60000000

// Global variables
int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},\
                     {7, 1, 1, 0, 0, 0},\
                     {7, 1, 3, 1, 0, 0},\
                     {7, 1, 1, 3, 1, 1}};
pthread_t pthread;
int fd;

volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

void *turn_LED (void *);

int init_LED ()
{
    int thread_id;
    fd = open("/dev/mem", O_RDWR);
    printf("Mapping %X - %X (size: %X)\n", GPIO1_BASE,\
                             GPIO1_BASE + GPIO_SIZE, GPIO_SIZE);
    gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 
                        GPIO1_BASE);
    gpio_datain            = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr   = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if(gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    printf("GPIO mapped to %p\n", gpio_addr);
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

    
    pthread_mutex_init (&mutex_lock, NULL);
    thread_id = pthread_create (&pthread, NULL, turn_LED, NULL);
    if (thread_id < 0)
    {
        printf ("Can't make thread\n");
        munmap((void *)gpio_addr, GPIO_SIZE);
        close(fd);
        return -1;
    }
    else
    {
        printf ("This is a thread!\n");
        return 0;
    }
}

void *turn_LED (void *data) 
{
    int time;
    int beat[4] = {2, 3, 4, 6};

    while (keepgoing)
    {
        pthread_mutex_lock (&mutex_lock);
        if (run == True)
        {
            time = MIN / (tempo * 2);
            *gpio_setdataout_addr = TimeSig[status][location]<<USER_LED;
            printf ("%d", TimeSig[status][location]);
            fflush (stdout);
            usleep (time);
            *gpio_cleardataout_addr = TimeSig[status][location]<<USER_LED;
            usleep (time);
            if (++location >= beat[status])
                location = 0;
        }
        else usleep (100000);
        pthread_mutex_unlock (&mutex_lock);
    }
    pthread_exit (0);
}

void exit_LED ()
{
    keepgoing = 0;
    pthread_mutex_destroy (&mutex_lock);
    pthread_join (pthread, NULL);
    munmap((void *)gpio_addr, GPIO_SIZE);
    close(fd);
}

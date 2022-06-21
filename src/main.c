/** \file main.c
* \brief The system does a basic processing of an analog signal. It reads the input voltage
from an analog sensor and from the buttons. It digitally filters the signal and outputs the desired LUX.
*
*The sistem starts in Manual Mode and changes the strength of the LED with the duty-cycle. In automatic mode, it asks to input the current time and the time that
*the LED will be on at the required voltage.  
* 
*
* \author Goncalo Moniz, Joao Oliveira, Diogo Leao
* \date 21-6-2022
* \bug There are no bugs
*/

/**
@mainpage ASSIGNMENT 5
@author Goncalo Moniz, Joao Oliveira, Diogo Leao

The aim of this assignment is to learn how to implement a simple closed loop control application in
Zephyr, using the real-time model. The application mimics the typical structure of reactive real-time
applications, comprising a sensor, decision logic and actuation. 

The objective is to develop an application to control the light intensity of a given region. The
system comprises a light sensor, an illumination system and a Human-Machine Interface. The
system can operate in two modes: 
• Automatic: programmable via the terminal. Should allow setting On/Off periods and the
corresponding light intensity;
• Manual: interface via the DevKit buttons. Allows to turn the system On/Off (when in “Off”
the system operates in automatic mode), via two of the buttons(Button 1 for manual mode and Button 2 for auto mode) . The other two buttons allow
to set (increase/decrease(Button 4/Button 3) the desired intensity.


*/
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/adc.h>
#include <string.h>
#include <devicetree.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <timing/timing.h>
#include <stdio.h>
#include <sys/printk.h>
#include <hal/nrf_saadc.h>
#include "ADC.h"
#include "Filter.h"
#include "PWM.h"
#include "Placa.h"



//----------------THREAD CONFIG


/* Size of stack area used by each thread (can be thread specific, if necessary)*/
#define STACK_SIZE 1024
/* Thread scheduling priority */
#define thread_A_prio 1
#define thread_B_prio 1
#define thread_C_prio 1
#define thread_D_prio 1
/* Create thread stack space */
K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_B_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_C_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_D_stack, STACK_SIZE);
/* Create variables for thread data */
struct k_thread thread_A_data;
struct k_thread thread_B_data;
struct k_thread thread_C_data;
struct k_thread thread_D_data;
/* Create task IDs */
k_tid_t thread_A_tid;
k_tid_t thread_B_tid;
k_tid_t thread_C_tid;
k_tid_t thread_D_tid;
/* Thread code prototypes */
void thread_A_code(void *, void *, void *);
void thread_B_code(void *, void *, void *);
void thread_C_code(void *, void *, void *);
void thread_D_code(void *, void *, void *);





//------------------SEMS CONFIG + SHARED MEMORY


struct k_sem sem_ab;
struct k_sem sem_bc;
struct k_sem sem_cd;
/** Shared memory betwen ADC and Filter.
 */ 
uint16_t ab;
/** Shared memory betwen Fiter and PWM.
 */ 
uint16_t bc;
/** Shared memory betwen PWM and Buttons.
 */ 
uint16_t cd;

uint8_t c;
//----------------MAIN

/**brief Main function
* 
* Inicialization of Semaphores and Threads
* 
* \author Goncalo Moniz, Joao Oliveira, Diogo Leao
* \param[in,out] 
*void
*void
* \return void
* \date 21-6-2022
*/
void main(void)
{

    /* Create/Init Semaphores */
    k_sem_init(&sem_ab,0,1);
    k_sem_init(&sem_bc,0,1);
    k_sem_init(&sem_cd,0,1);
    /* Then create the task */
    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
    K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
    NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);

    thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
    K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
    NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);

    thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack,
    K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
    NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);

    thread_D_tid = k_thread_create(&thread_D_data, thread_D_stack,
    K_THREAD_STACK_SIZEOF(thread_D_stack), thread_D_code,
    NULL, NULL, NULL, thread_D_prio, 0, K_NO_WAIT);

     
}
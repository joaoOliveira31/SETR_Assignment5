#include "PWM.h"

void thread_C_code(void *argA , void *argB, void *argC)
{
    /* Local variables */
    int ret=0;      
    int duty=0;
    const struct device *gpio0_dev;         /* Pointer to GPIO device structure */
    const struct device *pwm0_dev;          /* Pointer to PWM device structure */
    unsigned int pwmPeriod_us = 1000;       /* PWM priod in us */

    int mode = 1;

/* Bind to GPIO 0 and PWM0 */
    gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));
    if (gpio0_dev == NULL) {
        printk("Error: Failed to bind to GPIO0\n\r");        
	return;
    }
    else {
        printk("Bind to GPIO0 successfull \n\r");        
    }
    
    pwm0_dev = device_get_binding(DT_LABEL(PWM0_NID));
    if (pwm0_dev == NULL) {
	printk("Error: Failed to bind to PWM0\n r");
	return;
    }
    else  {
        printk("Bind to PWM0 successful\n\r");            
    }

    printk("Thread C init (sporadic, waits on a semaphore by task A)\n");
    while(1) {
        k_sem_take(&sem_cd, K_FOREVER);
        //Escolher o modo, (Mode 1 => Manual), (Mode 2 => Automático)
        printk("Botão: %d\r\n",cd);
        if(cd == 1)
            mode = 1;
        if(cd == 2)
            mode = 2;

        if(mode == 1){
            printk("Modo Manual\r\n");
            if(cd == 3){
                duty = duty + 10;
            }
            if(cd == 4){
                duty = duty - 10;
            }

            if (duty > 100)
                duty = 100;
            if (duty < 0)
                duty = 0;
        }
        
        else{

            printk("Modo Automático\r\n");
            duty=(uint16_t)(1000*(bc)*((float)3/1023))/30;
 
        }   
        printk("PWM DC value set to %u %%\n\n\r",duty);
        ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,pwmPeriod_us,(unsigned int)((pwmPeriod_us*duty)/100), PWM_POLARITY_NORMAL);
        cd = 0;
  }
}
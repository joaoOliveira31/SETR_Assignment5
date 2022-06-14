#include "PWM.h"
#include "placa.h"

void thread_C_code(void *argA , void *argB, void *argC)
{
    
    /* Local variables */
    int ret=0;      
    int duty=100;
    int16_t Ti,Td, kp, saidaPI,ref;
    int16_t integ=0,diff=0;
    int16_t erro,erro2=0;
    ref=1500; 
    int tensaoMV;
    int tensaoambiente=0;
    int flag=0;
    int mode = 2;
    const struct device *gpio0_dev;         /* Pointer to GPIO device structure */
    const struct device *pwm0_dev;          /* Pointer to PWM device structure */
    unsigned int pwmPeriod_us = 1000;       /* PWM priod in us */
   
    
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

    while(1) 
    {
        k_sem_take(&sem_cd, K_FOREVER);        
                      
        printk("Botao: %d\r\n",cd);           //Escolher o modo, (Mode 1 => Manual), (Mode 2 => Automático)

        if(cd == 1) mode = 1;
        
        if(cd == 2) mode = 2;
        
        if(mode == 1)
        {
            printk("Modo Manual\r\n");

            if(cd == 3)duty = duty + 10;

            else if(cd == 4)duty = duty - 10;

            if (duty > 100) duty = 100;
            
            else if (duty < 0) duty = 0;
            
            tensaoMV=(uint16_t)(1000*bc*((float)3/1023));
            printk("Tensao no sensor => %4u mv\n\r",tensaoMV); 
            printk("Tensao ambiente => %4u mv\n\r",tensaoambiente);
        }     
        else
        {
            printk("***Modo Automatico (REF --> UP/DOWN)***\r\n\n"); 

            if(flag<10)
            {
                tensaoambiente=(uint16_t)(1000*bc*((float)3/1023));
                flag++;
            }
            else
            {          
                printk("Tensao ambiente => %4u mv\n\n\r",tensaoambiente);          
                tensaoMV=(uint16_t)(1000*bc*((float)3/1023));printk("Tensao no sensor => %4u mv\n\n\r",tensaoMV);   
                                               
                //PI CONTROLER
                 Ti=1.5;
                 Td=0.5;
                 kp=0.8;

                 if(cd == 3)  ref = ref + 300;
                          
                 else if(cd == 4)  ref = ref - 300;

                 if(ref >= 3300) ref = 3300;
             
                 else if(ref <= tensaoambiente+100) ref = tensaoambiente+100;
             
                 printk("(PID) ref = %d mv\r\n",ref);

                 erro2 = erro;
                 erro = ref - tensaoMV;
                    
                 printk("(PID) erro = %d mv\r\n",erro);

                 integ = integ + (erro2+erro) / 2; printk("(PID) integ = %d mv\r\n",integ);   
                 diff = (erro2-erro)/0.1; printk("(PID) diff = %d mv\r\n",diff);             
              
                 if(integ > 20000) integ = 20000;

                 else if(integ < -20000) integ = -20000;

                 saidaPI = kp * erro + (1 / Ti) * integ + Td*diff; printk("(PID) saidaPI = %d mv\r\n\n",saidaPI);                  
                          
                 duty = (uint16_t)100 - (saidaPI) / 30;
             
                 if (duty > 100)duty = 100;
             
                 else if (duty < 0) duty = 0;                     
           } 
       } 
       printk("PWM DC value set to %u %%\n\n\r",duty);
       ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,pwmPeriod_us,(unsigned int)((pwmPeriod_us*duty)/100), PWM_POLARITY_NORMAL);
       cd = 0;       
  }//fecha while
}//fecha thread

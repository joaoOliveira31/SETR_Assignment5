#include "PWM.h"

void thread_C_code(void *argA , void *argB, void *argC)
{
    
    /* Local variables */
    int ret=0;      
    int duty=100;
    const struct device *gpio0_dev;         /* Pointer to GPIO device structure */
    const struct device *pwm0_dev;          /* Pointer to PWM device structure */
    unsigned int pwmPeriod_us = 1000;       /* PWM priod in us */

    int mode = 2;
    
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

    uint16_t Ti, kp, integ=0 , saidaPI,ref,diff=0;
    uint16_t erro,erro2=0;
    uint16_t valorinserido = 1023;
    ref=1500; 
    int cnt1,soma1,array1[20];
    int tensaoMV;
    int tensaoambiente=(uint16_t)(1000*bc*((float)3/1023));
    while(1) 
    {
        k_sem_take(&sem_cd, K_FOREVER);        
        
        
        printk("Botao: %d\r\n",cd);           //Escolher o modo, (Mode 1 => Manual), (Mode 2 => Automático)

        if(cd == 1) mode = 1;
        
        if(cd == 2) mode = 2;
        

        if(mode == 1)
        {
            printk("Modo Manual\r\n");

            if(cd == 3)
            {
                duty = duty + 10;
            }

            else if(cd == 4)
            {
                duty = duty - 10;
            }

            else if (duty > 100)
            {
                duty = 100;
            }

            else if (duty < 0)
            {
                duty = 0;
            }
             tensaoMV=(uint16_t)(1000*bc*((float)3/1023));printk("Tensao no sensor => %4u mv\n\r",tensaoMV); 
        }
         
        else
        {
            printk("Modo Automatico    REF->UP/DOWN\r\n");            
            tensaoMV=(uint16_t)(1000*bc*((float)3/1023));printk("Tensao no sensor => %4u mv\n\r",tensaoMV);   
                           
            
            //PI CONTROLER
             Ti=1.5;
             int Td=0.5;
             kp=0.8;
             if(cd == 3)
             {
               ref = ref + 100;
             }
             else if(cd == 4)
             {
                ref = ref - 100;
             }
             if(ref>=2500)
             {
               ref = 2500;
             }
             if(ref<=900)
             {
                ref = 900;
             }
             printk("(PID) ref = %4u mv\r\n",ref);
             erro2=erro;
             erro = ref-tensaoMV;
          
             if (erro>3000) 
             {  erro = 65535-erro;
                printk("(PID) erro = -%4u mv\r\n",erro);
                integ = integ - (erro2+erro)/2; printk("(PID) integ = %4u mv\r\n",integ);  
                diff=diff + (erro2+erro)/2; printk("(PID) diff = %4u mv\r\n",diff);             
             }
             else
             {
                printk("(PID) erro = %4u mv\r\n",erro);
                integ = integ +  (erro2+erro)/2; printk("(PID) integ = %4u mv\r\n",integ);   
                diff=diff - (erro2+erro)/2; printk("(PID) diff = %4u mv\r\n",diff);             
             } 
                        
             saidaPI=kp*erro+(1/Ti)*integ+Td*diff;               
               
             if (saidaPI > 3000)
             {
                 saidaPI = 65535-saidaPI ;
                 printk("(PID) saidaPI = %4u mv\r\n",saidaPI);    
             }
             else printk("(PID) saidaPI = %4u mv\r\n",saidaPI);    
             duty=(uint16_t)100-(saidaPI)/30;
             
             if (duty > 100)
             {
                 duty = 100;
             }

             if (duty < 0)
             {
                 duty = 0;
             }
             
        }  
        printk("PWM DC value set to %u %%\n\n\r",duty);
        printk("Escreva o valor pretendido =>\r\n\n");
        ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,pwmPeriod_us,(unsigned int)((pwmPeriod_us*duty)/100), PWM_POLARITY_NORMAL);
        cd = 0;
        
  }
}

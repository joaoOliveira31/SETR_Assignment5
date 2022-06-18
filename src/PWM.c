#include "PWM.h"
#include "clk.h"
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <string.h>
#include <console/console.h>
#include <stdlib.h>
#include <stdio.h>

static const char prompt[] = "Character echo started ...\r\n";

int hour=0;
int day=1;
//int month=0;
int dayOn[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int arrLen = sizeof dayOn / sizeof dayOn[0];
int temp;
void clkPrint(){
 

  //k_msleep(100); 
  hour++;
  if(hour==24){
    hour=0;
    day= day+1;
  }
  if(day==31 && hour>=23 ){
    day=1;
    
  }
}
       
void thread_C_code(void *argA , void *argB, void *argC)
{
    
    /* Local variables */
    int ret=0;      
    int duty=100;
    int16_t Ti,Td, kp, saidaPI=0, ref=0,ref1=0,ref2=0,ref3=0,ref4=0;
    int16_t integ=0,diff=0;
    int16_t erro,erro2=0;
    ref=1500,ref1=1500, ref2=1500,ref3=1500,ref4=1500;
    //char str[4];
    //int val;
     
    //float ref1=1500;
    int tensaoMV=0;
    int tensaoambiente;
    int to_add;
    int total=0;
    int flag=0,flag1=0;
    int mode = 2;
    const struct device *gpio0_dev;         /* Pointer to GPIO device structure */
    const struct device *pwm0_dev;          /* Pointer to PWM device structure */
    unsigned int pwmPeriod_us = 1000;       /* PWM priod in us */
    uint8_t c;
    console_init();
    
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
    printk("dummy line");
    while(1) 
    {
     
        k_sem_take(&sem_cd, K_FOREVER);        
                 
        //printk("Botao: %d\r\n",cd);           //Escolher o modo, (Mode 1 => Manual), (Mode 2 => Automático)
           if(flag<15)
            {
                to_add=(uint16_t)(1000*bc*((float)3/1023));
                total=total+to_add;
                tensaoambiente=total/15;

                flag++;
            }
            else{
        if(cd == 1) mode = 1;
        
        if(cd == 2) mode = 2;
        
        puts("\x1B[2J");

        if(mode == 1)
        {   
            
            //puts("\x1B[2J");  
            printk("***Modo Manual***      | Tensao no sensor => %4u mv \n"
                   " Butao 2 => Automatic  | Tensao ambiente => %4u mv  \n"
                   " Butao 3 => +10 Dc     | PWM DC value set to %u     \n"
                   " Butao 4 => -10 DC     |                            \n",tensaoMV,tensaoambiente,duty);
            
                            
            if(cd == 3)  duty = duty + 10;

            else if(cd == 4)  duty = duty - 10;

            if (duty > 100) duty = 100;
            
            else if (duty < 0) duty = 0;
            
            tensaoMV=(uint16_t)(1000*bc*((float)3/1023));
            //printk("Tensao no sensor => %4u mv\n\r",tensaoMV); 
            //printk("Tensao ambiente => %4u mv\n\r",tensaoambiente);
            
            flag1=0;
            //flag=0;
            //duty=100;
            

        }     
        else
        {
         

                 if(flag1==0)
                 { 
                   hour=0;
                   day=0;

                   printk("Selecione os Dias em que o led deve estar Ligado . Enter quando terminar\n");
                   printk("-------------------------------------------------\n");
                   
                   for(int i=0;i< arrLen;i++){
                   
                   c = console_getchar();
                   ref1=(int16_t)(c-48)*10;
                                  
                   if(c-48 != -35){
                  
                   
                   printk("%d",c-48);
                   c = console_getchar();                  
                   ref2=(int16_t)(c-48);
                   printk("%d",c-48);
                   printk(", ");

                   dayOn[i]=ref1+ref2; 
                   
                    }
                   else{
                   printk("\n");
                    break;
                    }
                    }
                   printk("Inserir valor de referencia---> \n");
                   
                   c = console_getchar();
                   ref1=(int16_t)(c-48)*1000;
                   printk("%d",c-48);
                   c = console_getchar();
                   ref2=(int16_t)(c-48)*100;
                   printk("%d",c-48);
                   c = console_getchar();
                   ref3=(int16_t)(c-48)*10;
                   printk("%d",c-48);
                   c = console_getchar();
                   ref4=(int16_t)(c-48);
                   printk("%d",ref4);
                   
                   ref=ref1+ref2+ref3+ref4;
                   
                   flag1=1; 
                 }
            
             
            clkPrint();
            //puts("\x1B[2J");
            printk("**Modo Automatico*** |  (REF --> UP/DOWN)          | Butao 1 => Manual  \n"
                   "******************** |  Tensao ambiente => %4u mv | Butao 3 => Ref+    \n"
                   "        %d:%d          | Tensao no sensor => %4u mv  | Butao 4 => Ref-    \n"
                   "********************  |  (PID) ref = %d mv        | (PID) integ = %d mv\n"
                   " (PID) erro = %d mv  |  (PID) saidaPI = %d mv   | (PID) diff = %d mv \n",tensaoambiente,hour,day,tensaoMV,ref,integ,erro,saidaPI,diff);                  
            
                                                                                                      
                 //PID CONTROLER
                 Ti=1.5;
                 Td=0.5;
                 kp=1.85;

                 if(cd == 3)  ref = ref + 300;
                          
                 else if(cd == 4)  ref = ref - 300;

                 if(ref >= 3300) ref = 3300;

                 else if(ref <= tensaoambiente+100) ref = tensaoambiente+100;
                  
                                 
                 tensaoMV=(uint16_t)(1000*bc*((float)3/1023));
                 //printk("Tensao ambiente => %4u mv\n\n\r",tensaoambiente);
                 //printk("Tensao no sensor => %4u mv\n\n\r",tensaoMV);
                 //printk("(PID) ref = %d mv\r\n",ref);
                 
                 erro2 = erro;
                 erro = ref - tensaoMV;
                    
                 //printk("(PID) erro = %d mv\r\n",erro);

                  


                 integ = integ + erro2; //printk("(PID) integ = %d mv\r\n",integ);   
                 diff = (erro2-erro)/0.1;// printk("(PID) diff = %d mv\r\n",diff);             
              
                 if(integ > 2000) integ = 2000;

                 else if(diff < -2000) diff = -2000;

                 if(diff > 2000) diff = 2000;

                 else if(integ < -2000) integ = -2000;

                 saidaPI = kp * erro + (1 / Ti) * integ + Td*diff; //printk("(PID) saidaPI = %d mv\r\n\n",saidaPI);                
                          
                 for (int i = 0; i < arrLen; i++) {
                      if (dayOn[i] == day) {
                     
                      duty = (uint16_t)100 - (saidaPI) / 30;
             
                      if (duty > 100)  duty = 100;
             
                      else if (duty < 0) duty = 0; 
                       break;
                  }
                     duty=100; 
                }
                 
                  
                 
                       
            
       } 
       }
       //printk("PWM DC value set to %u %%\n\n\r",duty);
       ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,pwmPeriod_us,(unsigned int)((pwmPeriod_us*duty)/100), PWM_POLARITY_NORMAL);
       cd = 0;       
       
  }//fecha while
}//fecha thread

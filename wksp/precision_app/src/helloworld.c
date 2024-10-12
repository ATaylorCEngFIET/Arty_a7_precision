#include "master_include.h"



int setup_pwm();

int main()
{
    init_platform();
    
    
    xil_printf("Command Parsing Active\n\r");
    setup_pwm();
    init_uart0();
    while(1){
        
        
        cli_parse_command();
    }
    return 0;
}

int setup_pwm()
{
     
     Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_PERIOD_REG_OFFSET,0x1e8480);  //2000000
     for( int i = 0; i<6; i++){
        Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*i), 0x249f0);//0x249f0); 
     }
     Xil_Out32(XPAR_PWM_0_BASEADDR+PWM_AXI_CTRL_REG_OFFSET,0x01); //enable the PWM 
}




#include "../master_include.h"

XUartLite Uart;

char receive_buffer[CLI_BUFFER_SIZE];    /* Buffer for Receiving Data */
char receive_buffer_lst[CLI_BUFFER_SIZE];    /* Buffer for Receiving Data */
int mesg_ready;

int test_id = 0;
int test_stop = 0;
int test_oneshot = 0;
char command_delim[] = " ";

#define PWM_FREQUENCY 50                                // Frequency of PWM in Hz
#define CLOCK_FREQUENCY XPAR_CPU_CORE_CLOCK_FREQ_HZ     // Clock frequency in Hz (100 MHz)
#define MIN_PULSE_WIDTH_MS 0.5                          // Minimum pulse width in ms
#define MAX_PULSE_WIDTH_MS 2.5                          // Maximum pulse width in ms
#define ANGLE_MIN 0                                     // Minimum angle in degrees
#define ANGLE_MAX 180                                   // Maximum angle in degrees

int init_uart0()
{
    int Status;

    Status = XUartLite_Initialize(&Uart, XPAR_XUARTLITE_0_BASEADDR);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;

}

void xil_printf_float(float x){
    int integer, fraction, abs_frac;
    integer = x;
    fraction = (x - integer) * 100;
    abs_frac = abs(fraction);
    xil_printf("%d.%3d\n\r", integer, abs_frac);

}

int read_serial(char *buffer)
{
	static u8 read_val[3] = {0,0,0};
    unsigned int received_count = 0;
    static int fisrt = 1;
    static int index = 0;
    static int index_lst = 0;



    received_count =    XUartLite_Recv(&Uart, &read_val[received_count], 1);
    if (received_count > 0)
    {

        if (read_val[0] == '\r'){
            buffer[index] = '\0';

            // Save the entered command for when we use Arrow up
            index_lst = index;
            strcpy(receive_buffer_lst,receive_buffer);

            index = 0;
            fisrt = 0;
            //xil_printf("\n\r");

            return 1;
        }

        // Handles "arrow up" key press
        if (read_val[2] == 27 && read_val[1] == 91 && read_val[0] == 65 && fisrt == 0){
            // restore last entered command
        	strcpy(receive_buffer,receive_buffer_lst);
            index = index_lst;

            xil_printf("\b%s",receive_buffer);

            return 0;
        }

        // Handles backspace
        if (read_val[0] == 8)
        {
        	xil_printf("\b \b");
        	if (index > 0)
        	{
        		index--;
        	}

        }

        // Prevents buffer overflow
        if (index >= CLI_BUFFER_SIZE-1)
        {
            buffer[index] = '\0';
            index = 0;
            xil_printf("\n\r");
            return 1;
        }

        // Prevents not printable characters from being added to the buffer
        if (read_val[0] >= 32 && read_val[0] <= 126)
        {
            buffer[index++] = read_val[0];
            //xil_printf("%c", read_val[0]);
        }

        read_val[2] = read_val[1];
        read_val[1] = read_val[0];
    }
    return 0;
}

void cli_parse_command()
{
    // /static u32 block = 0;
    static u32 val = 0;

    mesg_ready = read_serial(receive_buffer);
    if (mesg_ready == 1)
    {
        //xil_printf("cmd> %s\n\r", receive_buffer);

        if (strcmp(receive_buffer, "") == 0)
		{
            val = 0; // NOP
		}
    
        else if (strcmp(receive_buffer, "read_accel_y") == 0)
        {


        }

        else if (strcmp(receive_buffer, "read_accel_z") == 0)
        {

        }
        else
        {
            char *ptr = strtok(receive_buffer, command_delim);

            if (strcmp(ptr, "r") == 0)
            {
                ptr = strtok(NULL, command_delim);
                u32 addr = char_to_int(strlen(ptr), ptr);
                val = Xil_In32( XPAR_XGPIO_0_BASEADDR + addr);
                xil_printf("Addr: 0x%x (%d)  Val: 0x%x (%d)\r\n",addr,addr, val, val);

            }
            else if (strcmp(ptr, "w") == 0)
            {
                ptr = strtok(NULL, command_delim);
                u32 addr = char_to_int(strlen(ptr), ptr);

                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);

                Xil_Out32(XPAR_XGPIO_0_BASEADDR + addr,val);

                val = Xil_In32( XPAR_XGPIO_0_BASEADDR + addr);
                xil_printf("Addr: 0x%x (%d)  Val: 0x%x (%d)\r\n",addr,addr, val, val);

            }

             else if (strcmp(ptr, "a") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET,pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET);
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else if (strcmp(ptr, "b") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*1),pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET  + (4*1));
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else if (strcmp(ptr, "c") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*2),pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET  + (4*2));
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else if (strcmp(ptr, "d") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*3),pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET  + (4*3));
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else if (strcmp(ptr, "e") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*4),pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET  + (4*4));
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else if (strcmp(ptr, "f") == 0)
            {
                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);
                unsigned int pulse_width = angle_to_pwm(val);
                Xil_Out32(XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET + (4*5),pulse_width);

                val = Xil_In32( XPAR_PWM_0_BASEADDR + PWM_AXI_DUTY_REG_OFFSET  + (4*5));
                xil_printf(" Val: 0x%x (%d)\r\n", val, val);
            }

            else
            {
                xil_printf("Command not found.\n\r");
            }

        }
    }

}

// Function to convert angle to PWM value
unsigned int angle_to_pwm(int angle) {
    // Clamp angle within valid range
    if (angle < ANGLE_MIN) angle = ANGLE_MIN;
    if (angle > ANGLE_MAX) angle = ANGLE_MAX;

    // Map angle to pulse width in ms
    double pulse_width_ms = MIN_PULSE_WIDTH_MS + ((double)(angle - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN)) * (MAX_PULSE_WIDTH_MS - MIN_PULSE_WIDTH_MS);

    // Convert pulse width in ms to counter value
    unsigned int pwm_period = CLOCK_FREQUENCY / PWM_FREQUENCY;
    unsigned int pulse_width_counts = (unsigned int)((pulse_width_ms / 1000.0) * CLOCK_FREQUENCY);

    return pulse_width_counts;
}

u8 string_to_u8(const char *str) {
    uint8_t value = 0;

    // Convert first char to upper 4 bits
    if (str[0] >= '0' && str[0] <= '9') {
        value |= (str[0] - '0') << 4;
    } else if (str[0] >= 'A' && str[0] <= 'F') {
        value |= (str[0] - 'A' + 10) << 4;
    } else if (str[0] >= 'a' && str[0] <= 'f') {
        value |= (str[0] - 'a' + 10) << 4;
    }

    // Convert second char to lower 4 bits
    if (str[1] >= '0' && str[1] <= '9') {
        value |= (str[1] - '0');
    } else if (str[1] >= 'A' && str[1] <= 'F') {
        value |= (str[1] - 'A' + 10);
    } else if (str[1] >= 'a' && str[1] <= 'f') {
        value |= (str[1] - 'a' + 10);
    }

    return value;
}


int char_to_int(int len, char* buffer)
{

    int value = 0, char_val, scalar = 1;

    for (int i = len - 1; i >= 0; i--) {
        char_val = (int)(buffer[i] - '0');
        value = value + (char_val * scalar);
        scalar = scalar * 10;
    }

    return value;
}


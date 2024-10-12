#ifndef _CLI_H_
#define _CLI_H_


#define CLI_BUFFER_SIZE 32
#define NUM_OF_BYTE 1

extern int test_id;
extern int test_stop;
extern int test_oneshot;

int read_serial(char* buffer);
int init_uart0();
void cli_parse_command();
int char_to_int(int len, char* buffer);
u8 string_to_u8(const char *str);
void  xil_printf_float(float x);
unsigned int angle_to_pwm(int angle);
#endif

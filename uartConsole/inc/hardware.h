#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "config.h"

void init_systick(void);
void init_status_led(void);
void init_console_uart(void);
void enable_uart_interrupt(void);

int get_console_uart_it_flag(uint16_t it);
uint8_t get_console_uart_data(void);
void clear_console_uart_idel_it_flag(void);

void delay_ms(uint32_t ms);

#endif // !__HARDWARE_H

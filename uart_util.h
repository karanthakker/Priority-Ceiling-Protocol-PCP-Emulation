#ifndef __UART_UTIL_H__
#define __UART_UTIL_H__

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Configure the UART and its pins.  
// This must be called before UARTprintf().
void
vConfigureUART(void);

#endif // __UART_UTIL_H__

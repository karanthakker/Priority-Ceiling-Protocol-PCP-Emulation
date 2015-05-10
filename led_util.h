#ifndef __LED_UTIL_H__
#define __LED_UTIL_H__

#include "FreeRTOS.h"

// The stack size for the LED toggle task.
#define LEDTASKSTACKSIZE        128         // Stack size in words

// LED workload
// Default LED toggle delay value. 
#define LED_BLINK_DELAY        1000
#define LED_RED                GPIO_PIN_1
#define LED_BLUE               GPIO_PIN_2
#define LED_GREEN              GPIO_PIN_3

// Counter workload
// max counter to achieve 1 second
#define MAX_COUNT               1333333
// Dummy counter (the task workload)
extern volatile unsigned portBASE_TYPE counter ;

// LED workload
// Initializing the LEDs
void vBlinkInit( void );
// Turning on and off the "led" colour
void vBlinkLed( unsigned portBASE_TYPE led );

// Counter workload
// Counting for 1 second
void vCont1Sec( void );
void vTimedCont1Sec( void );

#endif // __LED_UTIL_H__

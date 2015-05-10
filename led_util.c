#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "led_util.h"
#include "FreeRTOS.h"
#include "inc/tm4c123gh6pm.h"
#include "uart_util.h"
#include "driverlib/timer.h"

// Dummy counter (the task workload)
volatile unsigned portBASE_TYPE counter = 0;

// Following the blinky program as an example, initialize the LED
void
vBlinkInit(void)
{
    // Enable the GPIO port that is used for the on-board LED.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

    // insert a few cycles after enabling the peripheral.
    __nop();

    // Enable the GPIO pin for the LED (PF3).  
    // Set the direction as output, and
    // enable the GPIO pin for digital function.
    GPIO_PORTF_DIR_R = GPIO_PORTF_DIR_R | LED_GREEN | LED_RED | LED_BLUE;
    GPIO_PORTF_DEN_R = GPIO_PORTF_DEN_R | LED_GREEN | LED_RED | LED_BLUE;

}

// Blink the LED.
void 
vBlinkLed( unsigned portBASE_TYPE led )
{
  /* Check if the on board LED is on. */
  if( GPIO_PORTF_DATA_R & led )
  {
    // Turn off the LED.
    GPIO_PORTF_DATA_R &= ~(led);
  }
  else
  {
    // Turn on the LED.
    GPIO_PORTF_DATA_R |= led;
  }
}

void 
vCont1Sec(void)
{
  unsigned portBASE_TYPE i;
   
  for (i=0; i< (unsigned portBASE_TYPE) MAX_COUNT; i++)
  {
    counter++;
  }

}

void
vTimedCont1Sec( void )
{
    unsigned portBASE_TYPE max_count=1333333; // gives 16000019 ticks 
    unsigned portBASE_TYPE initial_value=0;
    unsigned portBASE_TYPE value=0;
    unsigned portBASE_TYPE i=0;
  
    // Set the clock
//    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
//                       SYSCTL_XTAL_16MHZ);
  
    // Enable the timer peripheral
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure the 32-bit periodic timer
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    initial_value = ROM_TimerValueGet(TIMER0_BASE, TIMER_A); 
  
    UARTprintf("\rSysCtlClock\t%u\n", ROM_SysCtlClockGet());
    UARTprintf("\rinitial_value\t%u\n", initial_value);
  
    // Enable the timer
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

    // Loop setup for experiment    
    for (i=0; i<max_count; i++)
    {
      counter++;
    }

    // Disable timer and retrieve timer value
    ROM_TimerDisable(TIMER0_BASE, TIMER_A);
    value = ROM_TimerValueGet(TIMER0_BASE, TIMER_A);  
    UARTprintf("\rvalue\t\t%u\n", value);
    
}

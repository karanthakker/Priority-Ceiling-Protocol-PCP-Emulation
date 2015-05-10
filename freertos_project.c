#include "project.h"
#include "priorities.h"
#include "led_red_task.h"
#include "led_green_task.h"
#include "led_blue_task.h"
#include "led_util.h"
#include "uart_util.h"

// Initialize FreeRTOS and start the initial set of tasks.
int
main(void)
{
 
    // Initialize the LED
    vBlinkInit();
    // Initialize the UART
    vConfigureUART();

#if ( TEST_NONE == 1 ) 
#endif
  
#if ( TEST_TEST2 == 1 )
    vtest2();  
#endif
  
#if ( TEST_PCP == 1 || TEST_VANILLA == 1 ) 
    vinitTasks();
#endif //( TEST_CURR == TEST_PCP || TEST_CURR == TEST_VANILLA) 
  
#if ( TEST_PCP == 1 )   
   vinitSemaphores();
   // Semaphores in "given" state by default.
#endif // TEST_CURR == TEST_PCP
    
#if ( TEST_VANILLA == 1 )
    vSemaphoreCreateBinary( xSemaphoreBinLed );

    if( xSemaphoreBinLed == NULL )
    {
        while(1)
        {
        } 
    }
    // Semaphore in "given" state by default.
#endif //    TEST_CURR == TEST_VANILLA
    
#if ( TEST_TEST1 == 1 )      
    vtest1();
#endif
    
#if ( TEST_PCP == 1 || TEST_VANILLA == 1 )      

    // suspend all tasks
    vSuspendAllTasks();
      
    // start the lowest priority task
    vStartLowestPriorityTask();
    
    // Start the scheduler.  This should not return.  
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop
    // forever.

    while(1)
    {
    }
#endif  
}

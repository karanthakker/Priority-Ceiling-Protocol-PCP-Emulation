
#include "led_green_task.h"
#include "led_util.h"
#include "project.h"
#include "priorities.h"
    
// This task toggles the LED 
static void
vLedGreenTask( void *pvParameters )
{  
    // Loop forever.
    while(1)
    {
        vMarkTag( TASKHANDLE_GREENLED, TAG_ENTER, uxTaskPriorityGet( taskHandles[TASKHANDLE_GREENLED] ), SEMHANDLE_MAX );
      
#if ( TEST_PCP1 == 1 )       
        vtaskBody( LED_GREEN, TASKHANDLE_GREENLED );
#endif    
      
#if ( TEST_PCP2 == 1 )       
        vtask1BodyDeadlock( LED_GREEN, TASKHANDLE_GREENLED );
#endif       

#if ( TEST_PCP3 == 1 )
        vtaskABody_PCPTEST3( LED_GREEN, TASKHANDLE_GREENLED );
#endif 
      
#if ( TEST_PCP4 == 1 )
        vtask1Body_PCPTEST4( LED_GREEN, TASKHANDLE_GREENLED );    
#endif       
      
#if ( TEST_PCP5 == 1 )
        vtask0Body_PCPTEST5( LED_GREEN, TASKHANDLE_GREENLED );      
#endif

#if ( TEST_PCP6 == 1 )      
        vtask2Body_PCPTEST6( LED_GREEN, TASKHANDLE_GREENLED );
#endif        
        
        vMarkTag( TASKHANDLE_GREENLED, TAG_EXIT, uxTaskPriorityGet( taskHandles[ TASKHANDLE_GREENLED ] ), SEMHANDLE_MAX );        
        printTags();
        taskStop( TASKHANDLE_GREENLED );
    }
}

// Initializes the LED tasks.
unsigned portBASE_TYPE
vLedGreenTaskInit( void )
{
    // Create the LED task.
    if(xTaskCreate(vLedGreenTask, (signed portCHAR *)"LED_GREEN", LEDTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_GREEN_TASK, &taskHandles[TASKHANDLE_GREENLED])
       != pdTRUE)
    {
        // Fail.
        return(1);
    }
    
    // Success.
    return(0);
}

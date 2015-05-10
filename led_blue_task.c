
#include "led_blue_task.h"
#include "led_util.h"
#include "project.h"
#include "priorities.h"

#if ( TEST_PCP2 == 0 ) 

// This task toggles the LED 
static void
vLedBlueTask(void *pvParameters)
{  
    // Loop forever.
    while(1)
    {
        vMarkTag( TASKHANDLE_BLUELED, TAG_ENTER, uxTaskPriorityGet( taskHandles[TASKHANDLE_BLUELED] ), SEMHANDLE_MAX );

#if ( TEST_PCP1 == 1 )         
        vtaskBody( LED_BLUE, TASKHANDLE_BLUELED );
#endif
      
#if ( TEST_PCP3 == 1 )
        vtaskBBody_PCPTEST3( TASKHANDLE_BLUELED );
#endif  

#if ( TEST_PCP4 == 1 )
        vtask2Body_PCPTEST4( TASKHANDLE_BLUELED );    
#endif   
      
#if ( TEST_PCP5 == 1 )
        vtask1Body_PCPTEST5( TASKHANDLE_BLUELED );      
#endif       

#if ( TEST_PCP6 == 1 )      
        vtask4Body_PCPTEST6( TASKHANDLE_BLUELED );
#endif

        vMarkTag( TASKHANDLE_BLUELED, TAG_EXIT, uxTaskPriorityGet( taskHandles[TASKHANDLE_BLUELED] ), SEMHANDLE_MAX );
        printTags();
        taskStop( TASKHANDLE_BLUELED );
    }
}

// Initializes the LED tasks.
unsigned portBASE_TYPE
vLedBlueTaskInit( void )
{
    if( xTaskCreate( vLedBlueTask, (signed portCHAR *)"LED_BLUE", LEDTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_BLUE_TASK, &taskHandles[TASKHANDLE_BLUELED] ) 
        != pdTRUE )
    {
        // Fail.
        return(1);
    }
        
    // Success.
    return(0);
}
#endif

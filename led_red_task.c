
#include "led_red_task.h"
#include "led_util.h"
#include "project.h"
#include "priorities.h"

// This task toggles the LED 
static void
vLedRedTask( void *pvParameters )
{  
    // Loop forever.
    while(1)
    {  
        vMarkTag( TASKHANDLE_REDLED, TAG_ENTER, uxTaskPriorityGet( taskHandles[TASKHANDLE_REDLED] ), SEMHANDLE_MAX );
      
#if ( TEST_PCP1 == 1 )                 
        vtaskBody( LED_RED, TASKHANDLE_REDLED );
#endif      
      
#if ( TEST_PCP2 == 1 )       
        vtask2BodyDeadlock( LED_RED,TASKHANDLE_REDLED, TASKHANDLE_GREENLED );
#endif 
      
#if ( TEST_PCP3 == 1 )
        vtaskCBody_PCPTEST3( TASKHANDLE_REDLED, TASKHANDLE_BLUELED, TASKHANDLE_GREENLED );
#endif    

#if ( TEST_PCP4 == 1 )
        vtask3Body_PCPTEST4( LED_RED,TASKHANDLE_REDLED, TASKHANDLE_BLUELED, TASKHANDLE_GREENLED );    
#endif   
      
#if ( TEST_PCP5 == 1 )
        vtask2Body_PCPTEST5( TASKHANDLE_REDLED, TASKHANDLE_BLUELED, TASKHANDLE_GREENLED );      
#endif  

#if ( TEST_PCP6 == 1 )
        vtask5Body_PCPTEST6( LED_RED,TASKHANDLE_REDLED, TASKHANDLE_BLUELED, TASKHANDLE_TEST1, TASKHANDLE_TEST2 ); 
#endif
        vMarkTag( TASKHANDLE_REDLED, TAG_EXIT, uxTaskPriorityGet( taskHandles[TASKHANDLE_REDLED] ), SEMHANDLE_MAX );
        printTags();
        taskStop(TASKHANDLE_REDLED);
    }
}

// Initializes the LED tasks.
unsigned portBASE_TYPE
vLedRedTaskInit( void )
{       
  
    if(xTaskCreate(vLedRedTask, (signed portCHAR *)"LED_RED", LEDTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_RED_TASK, &taskHandles[TASKHANDLE_REDLED]) != pdTRUE)
    {
        // Fail.
        return(1);
    }
        
    // Success.
    return(0);
}

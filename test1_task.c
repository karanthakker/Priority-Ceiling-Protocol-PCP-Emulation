
#include "test1_task.h"
#include "project.h"
#include "priorities.h"

#if ( TEST_PCP6 == 1 )

#define TASKSTACKSIZE        128 

static void
vTest1Task( void *pvParameters )
{  
    // Loop forever.
    while(1)
    {  
        vMarkTag( TASKHANDLE_TEST1, TAG_ENTER, uxTaskPriorityGet( taskHandles[TASKHANDLE_TEST1] ), SEMHANDLE_MAX );
      
        vtask3Body_PCPTEST6( TASKHANDLE_TEST1, TASKHANDLE_GREENLED ); 

        vMarkTag( TASKHANDLE_TEST1, TAG_EXIT, uxTaskPriorityGet( taskHandles[TASKHANDLE_TEST1] ), SEMHANDLE_MAX );
        printTags();
        taskStop(TASKHANDLE_TEST1);
    }
}

// Initializes the LED tasks.
unsigned portBASE_TYPE
vTest1TaskInit( void )
{       
  
    if(xTaskCreate(vTest1Task, (signed portCHAR *)"Test1", TASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_TEST1_TASK, &taskHandles[TASKHANDLE_TEST1]) != pdTRUE)
    {
        // Fail.
        return(1);
    }
        
    // Success.
    return(0);
}
#endif


#include "test2_task.h"
#include "project.h"
#include "priorities.h"

#if ( TEST_PCP6 == 1 )

#define TASKSTACKSIZE        128 


static void
vTest2Task( void *pvParameters )
{  
    // Loop forever.
    while(1)
    {  
        vMarkTag( TASKHANDLE_TEST2, TAG_ENTER, uxTaskPriorityGet( taskHandles[TASKHANDLE_TEST2] ), SEMHANDLE_MAX );
      
        vtask1Body_PCPTEST6( TASKHANDLE_TEST2 ); 

        vMarkTag( TASKHANDLE_TEST2, TAG_EXIT, uxTaskPriorityGet( taskHandles[TASKHANDLE_TEST2] ), SEMHANDLE_MAX );
        printTags();
        taskStop(TASKHANDLE_TEST2);
    }
}

// Initializes the LED tasks.
unsigned portBASE_TYPE
vTest2TaskInit( void )
{       
  
    if(xTaskCreate(vTest2Task, (signed portCHAR *)"Test2", TASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_TEST2_TASK, &taskHandles[TASKHANDLE_TEST2]) != pdTRUE)
    {
        // Fail.
        return(1);
    }
        
    // Success.
    return(0);
}
#endif

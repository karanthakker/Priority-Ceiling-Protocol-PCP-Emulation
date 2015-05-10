#include "project.h"
#include "priorities.h"
#include "led_red_task.h"
#include "led_green_task.h"
#include "led_blue_task.h"
#include "test1_task.h"
#include "test2_task.h"
#include "led_util.h"
#include "uart_util.h"

// Task Handles
xTaskHandle taskHandles[TASKHANDLE_MAX] = { NULL };

// Semaphores
xSemaphoreHandle xSemaphoreBinLed;
xSemaphoreHandle xSemaphores[SEMHANDLE_MAX];

// Task Tags
volatile tag_struct tags[MAXTAGS] ;
volatile unsigned portBASE_TYPE tag = 0;

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

// This hook is called by FreeRTOS when an stack overflow error is detected.
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

unsigned portBASE_TYPE taskStop ( unsigned portBASE_TYPE TASKHANDLE )
{
  if ( !taskHandles[TASKHANDLE] )
    return ( unsigned portBASE_TYPE ) 0;
  
  vTaskDelete( taskHandles [TASKHANDLE] );
  taskHandles[TASKHANDLE] = NULL;

  return ( unsigned portBASE_TYPE ) 1;
}

void 
vtest1 ( void )
{
    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], LED_BLINK_DELAY / portTICK_RATE_MS) == pdTRUE )
    {
        // We were able to obtain the semaphore and can now access the
        // shared resource.

        // Blink the LED.
        vBlinkLed(LED_BLUE);            
    
        vCont1Sec();

        // Blink the LED.
        vBlinkLed(LED_BLUE); 
        
        // We have finished accessing the shared resource.  Release the 
        // semaphore.
        xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
    }
}

void 
vtest2 ( void )
{
    vTimedCont1Sec();
}

void 
vinitTasks ( void )
{
    // Create LED task.
    if( vLedRedTaskInit() != (unsigned portBASE_TYPE) 0 )
    {
        while(1)
        {
        }
    }
 
    // Create LED task.
    if( vLedGreenTaskInit() != (unsigned portBASE_TYPE) 0 )
    {
        while(1)
        {
        }
    }

#if ( TEST_PCP2 == 0 && \
    ( TEST_PCP1 == 1 || TEST_PCP3 == 1 || TEST_PCP4 == 1 || TEST_PCP5 == 1 || TEST_PCP6 == 1 ) )  
    
    // Create LED task.
    if( vLedBlueTaskInit() != (unsigned portBASE_TYPE) 0 )
    {
        while(1)
        {
        }
    }
#endif

#if ( TEST_PCP6 == 1 )  
    
    if( vTest1TaskInit() != (unsigned portBASE_TYPE) 0 )
    {
        while(1)
        {
        }
    }
    
    if( vTest2TaskInit() != (unsigned portBASE_TYPE) 0 )
    {
        while(1)
        {
        }
    }
    
#endif
  
}

#if ( TEST_PCP == 1 ) 
void 
vinitSemaphores ( void )
{
    // This can be setup as a for loop when scaling (either as array of linked list)
    // ( leaving it as explicit for now for readability )
    vSemaphoreCreatePcp( xSemaphores[SEMHANDLE_LED], (unsigned portBASE_TYPE) PRIORITY_CEILING_LED );

    if( xSemaphores[SEMHANDLE_LED] == NULL )
    {
        while(1)
        {
        } 
    }
#if ( TEST_PCP1 == 0 && TEST_TEST1 == 0)
    vSemaphoreCreatePcp( xSemaphores[SEMHANDLE_COUNTER], (unsigned portBASE_TYPE) PRIORITY_CEILING_COUNTER );

    if( xSemaphores[SEMHANDLE_COUNTER] == NULL )
    {
        while(1)
        {
        } 
    }
#endif     
    
#if ( TEST_PCP3 == 1 || TEST_PCP5 == 1 )
    
    vSemaphoreCreatePcp( xSemaphores[SEMHANDLE_COUNTER2], (unsigned portBASE_TYPE) PRIORITY_CEILING_COUNTER2 );

    if( xSemaphores[SEMHANDLE_COUNTER2] == NULL )
    {
        while(1)
        {
        } 
    }
#endif    
}
#endif

void
vSuspendAllTasks ( void )
{
    unsigned portBASE_TYPE i;
  
    for ( i = 0; i < TASKHANDLE_MAX; i++)
    {
      if ( taskHandles[i] != NULL )
      {
        vTaskSuspend(taskHandles[i]);
      }
    }
}


/*
  This function returns one of the lowest priority tasks

  Example:

  unsigned portBASE_TYPE task = xGetLowestTask();
  if ( task != TASKHANDLE_MAX )
  {
    vTaskResume(taskHandles[task]);
  }
*/
unsigned portBASE_TYPE  
xGetLowestPriorityTask ( void )
{
  unsigned portBASE_TYPE i, temp;
  unsigned portBASE_TYPE min = configMAX_PRIORITIES;
  unsigned portBASE_TYPE task = TASKHANDLE_MAX;
  
  for ( i = 0; i < TASKHANDLE_MAX; i++ )
  {
    if ( taskHandles[i] != NULL ) 
    {     
      temp = uxTaskPriorityGet( taskHandles[i] );
      if ( min > temp )
      { 
        min = temp;
        task = i ;
      }
    }
  }
  
  return task;
}

unsigned portBASE_TYPE  
xGetNextHighestPriorityTask ( unsigned portBASE_TYPE currtask )
{
  unsigned portBASE_TYPE i, temp, currTaskPriority;
  unsigned portBASE_TYPE min = configMAX_PRIORITIES;
  unsigned portBASE_TYPE task = TASKHANDLE_MAX;
  
  currTaskPriority = uxTaskPriorityGet( taskHandles[currtask] );
  
  for ( i = 0; i < TASKHANDLE_MAX; i++ )
  {
    if ( taskHandles[i] != NULL ) 
    {     
      temp = uxTaskPriorityGet( taskHandles[i] );
      if ( min > temp && 
           temp >= currTaskPriority && 
           i != currtask)
      { 
        min = temp;
        task = i ;
      }
    }
  }
  
  return task;
}

void 
vStartNextHighestPriorityTask ( unsigned portBASE_TYPE currtask )
{
  unsigned portBASE_TYPE task = TASKHANDLE_MAX;
  
  task = xGetNextHighestPriorityTask(currtask);
  
  if ( task != TASKHANDLE_MAX )
  {
    vTaskResume(taskHandles[task]);
  }
  
}

// This function resumes one of the lowest priority tasks
void 
vStartLowestPriorityTask ( void )
{
  unsigned portBASE_TYPE task = TASKHANDLE_MAX;
  
  task = xGetLowestPriorityTask();
  
  if ( task != TASKHANDLE_MAX )
  {
    vTaskResume(taskHandles[task]);
  }
}



void
vMarkTag( taskHandle_e TASKHANDLE, tag_action_e ACTION, unsigned portBASE_TYPE PRIORITY, semHandle_e SEMAPHORE )
{
  tags[tag].task     = TASKHANDLE;
  tags[tag].action   = ACTION;
  tags[tag].priority = PRIORITY; 
  tags[tag].semaphore = SEMAPHORE;
  incrTag();
}

#if ( TEST_PCP1 == 1 )

void 
vtaskBody ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
        // Wait 1 sec   
        vCont1Sec();
      
        if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
        {
            // We were able to obtain the semaphore and can now access the
            // shared resource.
            
            // Turn on the LED.
            vBlinkLed(LED);   
  
            // Enqueue this task with "Take" tag
            vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

            // Wait 1 sec   
            vCont1Sec();
            vCont1Sec();

            // Turn off the LED.
            vBlinkLed(LED);
  
            // Resume next higher priority task if any
            vStartNextHighestPriorityTask ( TASKHANDLE );
  
            // Turn on the LED.
            vBlinkLed(LED); 
  
            // Wait 1 sec
            vCont1Sec();
            vCont1Sec();
            // Turn off the LED.
            vBlinkLed(LED);
            
            // Enqueue this task with "Give" tag    
            vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
            
            xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
        }
            
}
#endif // ( TEST_PCP1 == 1 )

#if ( TEST_PCP2 == 1 )
  
void 
vtask1BodyDeadlock ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
        // Wait 1 sec   
        vCont1Sec();
      
        if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
        {
          // Enqueue this task with "Take" tag
          vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
 
          vCont1Sec();
          ProtectedCounter++;
          vCont1Sec();
          
          if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
          {
             // Enqueue this task with "Take" tag
            vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
 
            // Turn on the LED.
            vBlinkLed(LED);     

            // Wait 1 sec   
            vCont1Sec();

            // Turn off the LED.
            vBlinkLed(LED);
            
            // Enqueue this task with "Give" tag    
            vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
            
            xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
          } // take SEMHANDLE_LED
          
          vCont1Sec();

          // Enqueue this task with "Give" tag    
          vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
 
          xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
        } // take SEMHANDLE_COUNTER
         
}

void 
vtask2BodyDeadlock ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE )
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);

    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE]); 

    // Turn on the LED.
    vBlinkLed(LED); 

    // Wait 1 sec
    vCont1Sec();
    vCont1Sec();
    // Turn off the LED.
    vBlinkLed(LED);
          
    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
    {
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      vCont1Sec();
      ProtectedCounter++;
      vCont1Sec();

      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
  
      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );
    } // take SEMHANDLE_COUNTER
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );  
  } // take SEMHANDLE_LED
   
}
#endif // ( TEST_PCP2 == 1 )

#if ( TEST_PCP3 == 1 )
void 
vtaskABody_PCPTEST3 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );  
  } // take SEMHANDLE_LED
   
  // Wait 1 sec   
  vCont1Sec();
}

void 
vtaskBBody_PCPTEST3 ( taskHandle_e TASKHANDLE )
{
        // Wait 1 sec   
        vCont1Sec();
      
        if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
        {
          // Enqueue this task with "Take" tag
          vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
 
          vCont1Sec();
          ProtectedCounter++;
          vCont1Sec();
          
          if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER2], portMAX_DELAY ) == pdTRUE )
          {
             // Enqueue this task with "Take" tag
            vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );
 
          vCont1Sec();
          ProtectedCounter2++;
          vCont1Sec();
            
            // Enqueue this task with "Give" tag    
            vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );
            
            xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER2] );
          } // take SEMHANDLE_COUNTER2
          
          vCont1Sec();

          // Enqueue this task with "Give" tag    
          vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
 
          xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
        } // take SEMHANDLE_COUNTER
        
        vCont1Sec();
}

void 
vtaskCBody_PCPTEST3 ( taskHandle_e TASKHANDLE, 
                      taskHandle_e HIGHERTASKHANDLE1, 
                      taskHandle_e HIGHERTASKHANDLE2 )
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER2], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );

    
    // Wait 1 sec   
    vCont1Sec();
    ProtectedCounter2++;
    vCont1Sec();

    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE1]); 

    // Wait 1 sec
    vCont1Sec();
    
    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE2]); 
    
    // Wait 1 sec
    vCont1Sec();
          
    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
    {
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      vCont1Sec();
      ProtectedCounter++;
      vCont1Sec();

      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
  
      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );
    } // take SEMHANDLE_COUNTER
    
    vCont1Sec();
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER2] );  
    
    vCont1Sec();
    
  } 
   
}

#endif // ( TEST_PCP3 == 1 )

#if ( TEST_PCP4 == 1 )
void 
vtask3Body_PCPTEST4 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE1,
                                                                           taskHandle_e HIGHERTASKHANDLE2)
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);

    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE1]); 
    
    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);
  
    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE2]); 
              
    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );  
  } // take SEMHANDLE_LED
   
}

void 
vtask2Body_PCPTEST4 ( taskHandle_e TASKHANDLE )
{
    // Wait 1 sec   
    vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
    {
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      vCont1Sec();
      ProtectedCounter++;
      vCont1Sec();

      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
    } // take SEMHANDLE_COUNTER
    
}

void 
vtask1Body_PCPTEST4 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
  
    // Wait 1 sec   
    vCont1Sec();
    ProtectedCounter++;
    vCont1Sec();
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
        
  }
  
  vCont1Sec();
  
  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {
    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);

    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
  }  
  
  vCont1Sec();
 
}

#endif // ( TEST_PCP4 == 1 )

#if ( TEST_PCP5 == 1 )
void 
vtask2Body_PCPTEST5 ( taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE1,
                                               taskHandle_e HIGHERTASKHANDLE2)
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER2], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );

    vCont1Sec();
    ProtectedCounter2++;
    vCont1Sec();

    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE1]); 
    
    // Wait 1 sec   
    vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
    {

      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
      
      // Wait 1 sec   
      vCont1Sec();
      ProtectedCounter++;
      vCont1Sec();
      
      // Resume higher priority task 
      vTaskResume(taskHandles[HIGHERTASKHANDLE2]); 
               
      // Wait 1 sec   
      vCont1Sec();
      
      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
                
      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
    }
    
    // Wait 1 sec   
    vCont1Sec();
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER2] );  
  } 
   
}

void 
vtask1Body_PCPTEST5 ( taskHandle_e TASKHANDLE )
{
    // Wait 1 sec   
    vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER2], portMAX_DELAY ) == pdTRUE )
    {
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );

      vCont1Sec();
      ProtectedCounter2++;
      vCont1Sec();

      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER2 );

      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER2] );  
    } // take SEMHANDLE_COUNTER
    
}

void 
vtask0Body_PCPTEST5 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
  // Wait 1 sec   
  vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {
    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();

    // Turn off the LED.
    vBlinkLed(LED);

    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
  } 
  
  vCont1Sec();
  
  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
  
    // Wait 1 sec   
    vCont1Sec();
    ProtectedCounter++;
    vCont1Sec();
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
        
  }
 
}

#endif // ( TEST_PCP5 == 1 )

#if ( TEST_PCP6 == 1 )

void 
vServer1 ( unsigned portBASE_TYPE LED )
{
    // Turn on the LED.
    vBlinkLed(LED); 
    // Wait 1 sec   
    vCont1Sec();
    // Turn off the LED.
    vBlinkLed(LED);
}

void 
vServer2 ( taskHandle_e TASKHANDLE )
{
  
  vCont1Sec();
  ProtectedCounter++;
  vCont1Sec();
  
  // Only called if "task 4", the blue LED task in our case
  if ( TASKHANDLE == TASKHANDLE_BLUELED )
  {
    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
    {   
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
      
      vServer1(LED_BLUE);
      
      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
                
      xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );  
    } 
    vCont1Sec();
  }
  
}

void 
vtask5Body_PCPTEST6 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE, 
                                               taskHandle_e HIGHERTASKHANDLE1,
                                               taskHandle_e HIGHERTASKHANDLE2,
                                               taskHandle_e HIGHERTASKHANDLE3)
{
  // Wait 1 sec   
  vCont1Sec();

  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    vServer1(LED);
    
    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE1]); 
    
    vServer1(LED);
    
    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE2]); 
    
    vServer1(LED);

    // Resume higher priority task 
    vTaskResume(taskHandles[HIGHERTASKHANDLE3]); 
    
    vServer1(LED);
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );  
  } 
  
  // Wait 1 sec   
  vCont1Sec();   
}

void 
vtask4Body_PCPTEST6 ( taskHandle_e TASKHANDLE )
{
    // Wait 1 sec   
    vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
    {
      // Enqueue this task with "Take" tag
      vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      vServer2( TASKHANDLE );

      // Enqueue this task with "Give" tag    
      vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );

      xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
    } // take SEMHANDLE_COUNTER
    
    vCont1Sec();
}

void 
vtask3Body_PCPTEST6 ( taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE)
{
  // Wait 1 sec   
  vCont1Sec();
  // Resume higher priority task 
  vTaskResume(taskHandles[HIGHERTASKHANDLE]);   
  vCont1Sec();
}

void 
vtask2Body_PCPTEST6 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE )
{
  // Wait 1 sec   
  vCont1Sec();

    if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_LED], portMAX_DELAY ) == pdTRUE )
  {
    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    vServer1(LED);
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_LED );

    xSemaphorePcpGive( xSemaphores[SEMHANDLE_LED] );
  } 
  
  vCont1Sec();
}

void 
vtask1Body_PCPTEST6 ( taskHandle_e TASKHANDLE )
{
 
  vCont1Sec();
  
  if ( xSemaphorePcpTake( xSemaphores[SEMHANDLE_COUNTER], portMAX_DELAY ) == pdTRUE )
  {

    // Enqueue this task with "Take" tag
    vMarkTag( TASKHANDLE, TAG_TAKE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
  
    vServer2( TASKHANDLE );
    
    // Enqueue this task with "Give" tag    
    vMarkTag( TASKHANDLE, TAG_GIVE, uxTaskPriorityGet( taskHandles[TASKHANDLE] ), SEMHANDLE_COUNTER );
              
    xSemaphorePcpGive( xSemaphores[SEMHANDLE_COUNTER] );  
        
  }
  
  vCont1Sec();

}

#endif // ( TEST_PCP6 == 1 )



void
incrTag ( void )
{
  if ( tag < MAXTAGS )
  {
    tag++;
  }
  else
  {
    while(1)
    {
    }
  }
  
}

void 
printTags ( void )
{
  unsigned portBASE_TYPE i;
  
  UARTprintf("\ntag = %u\n", tag);

  for ( i = 0; i < tag; i++ )
  { 
      UARTprintf("\ntag[%u].task =     \t%u", i, tags[i].task);     
      UARTprintf("\ntag[%u].priority = \t%u", i, tags[i].priority);
      UARTprintf("\ntag[%u].action =   \t%u", i, tags[i].action);
      UARTprintf("\ntag[%u].semaphore =\t%u\n", i, tags[i].semaphore);
  }
}



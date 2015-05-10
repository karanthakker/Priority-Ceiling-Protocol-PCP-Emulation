// project.h - Project specific structures and definitions

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tests.h"

// Task Handles
typedef enum TaskHandle
{
  TASKHANDLE_REDLED,
#if ( TEST_PCP2 == 0 )  
  TASKHANDLE_BLUELED,
#endif  
  TASKHANDLE_GREENLED,
#if ( TEST_PCP6 == 1 )
  TASKHANDLE_TEST1,
  TASKHANDLE_TEST2,
#endif
#if configGENERATE_RUN_TIME_STATS == 1
  TASKHANDLE_RUNTIMESTATS,
#endif
  TASKHANDLE_MAX
} taskHandle_e;

extern xTaskHandle taskHandles[TASKHANDLE_MAX];


// Semaphores
typedef enum SempahoreHandle
{
  SEMHANDLE_LED,
  SEMHANDLE_COUNTER,
#if ( TEST_PCP1 == 1 || TEST_PCP3 == 1 || TEST_PCP5 == 1 )
  SEMHANDLE_COUNTER2,
#endif
  SEMHANDLE_MAX
} semHandle_e;

extern xSemaphoreHandle xSemaphores[SEMHANDLE_MAX];

#if ( TEST_VANILLA == 1 )
extern xSemaphoreHandle xSemaphoreBinLed;
#endif

// A counter we want to protect using semaphore
static unsigned portBASE_TYPE ProtectedCounter = 0;
// A counter we want to protect using semaphore
static unsigned portBASE_TYPE ProtectedCounter2 = 0;

// Task Tags
typedef enum Action
{
  TAG_TAKE,
  TAG_GIVE,
  TAG_ENTER,
  TAG_EXIT,
  MAX_ACTION,
} tag_action_e;

typedef struct Tag
{
  taskHandle_e           task;
  unsigned portBASE_TYPE priority;
  tag_action_e           action;
  semHandle_e            semaphore;
} tag_struct;


// Array of tags for task testing (0 initially)
#define MAXTAGS 100
extern volatile tag_struct tags[MAXTAGS];
extern volatile unsigned portBASE_TYPE tag;

// Prototypes

// Task Tags
// Increment tag until MAXTAGS
void incrTag( void );
// print tag array to UART
void printTags ( void );
void vMarkTag( taskHandle_e TASKHANDLE, tag_action_e ACTION, unsigned portBASE_TYPE PRIORITY, semHandle_e SEMAPHORE );

// Tasks
// Task Utilitites
// Create tasks
void vinitTasks ( void );
// Stop and delete task from scheduler
unsigned portBASE_TYPE taskStop ( unsigned portBASE_TYPE TASKHANDLE );
void vSuspendAllTasks ( void );
void vStartLowestPriorityTask ( void );
void vStartNextHighestPriorityTask ( unsigned portBASE_TYPE currtask );

// Task Bodies
void vtaskBody ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtask1BodyDeadlock ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtask2BodyDeadlock ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE );
void vtaskABody_PCPTEST3 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtaskBBody_PCPTEST3 ( taskHandle_e TASKHANDLE );
void vtaskCBody_PCPTEST3 ( taskHandle_e TASKHANDLE, 
                           taskHandle_e HIGHERTASKHANDLE1, 
                           taskHandle_e HIGHERTASKHANDLE2 );
void vtask3Body_PCPTEST4 ( unsigned portBASE_TYPE LED, 
                           taskHandle_e TASKHANDLE, 
                           taskHandle_e HIGHERTASKHANDLE1,
                           taskHandle_e HIGHERTASKHANDLE2 );
void vtask2Body_PCPTEST4 ( taskHandle_e TASKHANDLE ) ; 
void vtask1Body_PCPTEST4 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtask2Body_PCPTEST5 ( taskHandle_e TASKHANDLE, 
                           taskHandle_e HIGHERTASKHANDLE1,
                           taskHandle_e HIGHERTASKHANDLE2);
void vtask1Body_PCPTEST5 ( taskHandle_e TASKHANDLE );
void vtask0Body_PCPTEST5 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtask5Body_PCPTEST6 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE, 
                                               taskHandle_e HIGHERTASKHANDLE1,
                                               taskHandle_e HIGHERTASKHANDLE2,
                                               taskHandle_e HIGHERTASKHANDLE3);
void vtask4Body_PCPTEST6 ( taskHandle_e TASKHANDLE ) ;
void vtask3Body_PCPTEST6 ( taskHandle_e TASKHANDLE, taskHandle_e HIGHERTASKHANDLE);
void vtask2Body_PCPTEST6 ( unsigned portBASE_TYPE LED, taskHandle_e TASKHANDLE );
void vtask1Body_PCPTEST6 ( taskHandle_e TASKHANDLE );

// Semaphores
void vinitSemaphores ( void );

// Smoke Tests
void vtest1 ( void );
void vtest2 ( void );

#endif // __PROJECT_H__

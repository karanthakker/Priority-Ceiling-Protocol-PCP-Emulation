// priorities.h - Priorities for the various FreeRTOS tasks.

#ifndef __PRIORITIES_H__
#define __PRIORITIES_H__

#include "tests.h"

// Since the IDLE TASK PRIORITY is set to 0,
// we will avoid setting any other task priority to 0.
// Hence all priority ceilings will also be > 0.

#define PRIORITY_LED_RED_TASK       2
#define PRIORITY_LED_BLUE_TASK      3
#if ( TEST_PCP6 == 1 )
  #define PRIORITY_LED_GREEN_TASK   5
#else
  #define PRIORITY_LED_GREEN_TASK   4
#endif
#define PRIORITY_TEST1_TASK         4
#define PRIORITY_TEST2_TASK         6

#if ( TEST_PCP6 == 1 )
  #define PRIORITY_CEILING_LED      5
#else
  #define PRIORITY_CEILING_LED      4
#endif

#if ( TEST_PCP6 == 1 )
  #define PRIORITY_CEILING_COUNTER  6
#endif
#if ( TEST_PCP2 == 1 || TEST_PCP4 == 1 || TEST_PCP5 == 1 )
  #define PRIORITY_CEILING_COUNTER  4    
#endif   
#if ( TEST_PCP3 == 1 )
  #define PRIORITY_CEILING_COUNTER  3
#endif

#if ( TEST_PCP3 == 1 || TEST_PCP5 == 1 )
  #define PRIORITY_CEILING_COUNTER2 3 
#endif


#endif // __PRIORITIES_H__

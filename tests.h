#ifndef __TESTS_H__
#define __TESTS_H__

// Tests
#define TEST_NONE    0
#define TEST_TEST1   0
#define TEST_TEST2   0
#define TEST_VANILLA 0
#define TEST_PCP     1

/* Basic semaphore functionality with an LED resource 
   shared among 3 tasks
*/
#define TEST_PCP1    0

/* 2 task deadlock scenario - only PCP can overcome
   ( 2 resources used )
*/
#define TEST_PCP2    1

/* 3 tasks : 2 of them similar to deadlock configuration 
             plus a third high priority task that uses
             a separate resource   
   ( 3 resources used )             
*/
#define TEST_PCP3    0

/* Chain blocking : 3 tasks and 2 resources (PCP slide 23)
*/
#define TEST_PCP4    0

/* Nesting example (max 1) (PCP slide 36)
   3 tasks and 3 resources
*/
#define TEST_PCP5    0

/* 5 tasks with 2 resources (PCP slide 44)
*/
#define TEST_PCP6    0

#endif

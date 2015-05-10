#ifndef INC_PCP_H
#define INC_PCP_H

#include "FreeRTOS.h"
#include "task.h"


/*
 * Definition of the queue used by the scheduler.
 * Items are queued by copy, not reference.
 */
typedef struct QueueDefinition
{
	signed char *pcHead;				/*< Points to the beginning of the queue storage area. */
	signed char *pcTail;				/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */

	signed char *pcWriteTo;				/*< Points to the free next place in the storage area. */
	signed char *pcReadFrom;			/*< Points to the last place that a queued item was read from. */

	xList xTasksWaitingToSend;				/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
	xList xTasksWaitingToReceive;			/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

	volatile unsigned portBASE_TYPE uxMessagesWaiting;/*< The number of items currently in the queue. */
	unsigned portBASE_TYPE uxLength;		/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
	unsigned portBASE_TYPE uxItemSize;		/*< The size of each items that the queue will hold. */

	signed portBASE_TYPE xRxLock;			/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
	signed portBASE_TYPE xTxLock;			/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
	
	#if ( configUSE_TRACE_FACILITY == 1 )
		unsigned char ucQueueNumber;
		unsigned char ucQueueType;
	#endif
  
  #if (configUSE_PCP == 1)
    unsigned portBASE_TYPE uxPriorityCeiling;
    unsigned portBASE_TYPE uxResId;
    xList xTasksWaiting;             /* Tasks blocked on this sempahore */
  #endif

} xQUEUE;
/*-----------------------------------------------------------*/

/*
 * xQueueHandle is a pointer to a xQUEUE structure.
 */
typedef xQUEUE * xQueueHandle;



// Since we are not using lists, we set the max number of resources to 10
// A list implementation would be able to use as many resurces as posible provided memory limitations

#define RESOURCES_MAX 10

typedef enum Status
{
  ACTIVE,     /* The resource is currently in use */
  PASSIVE,    /* The resource is being release when there are tasks blocked on it */
  FREE,       /* The resource is currently free (not in use and no tasks blocked waiting for it) */
} status_e;

typedef struct Resource
{
  status_e status;
  signed char *pcHoldingTask;
  unsigned portBASE_TYPE uxPriorityCeiling;
  unsigned portBASE_TYPE uxPriorityMaxBlocked;
  xQueueHandle xQueueRes;
} xResource;

extern xResource resources[RESOURCES_MAX];
// Latest added resource position
extern unsigned portBASE_TYPE uRes;

#endif // INC_PCP_H

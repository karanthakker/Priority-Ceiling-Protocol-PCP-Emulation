/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#if ( configUSE_CO_ROUTINES == 1 )
	#include "croutine.h"
#endif

#if (configUSE_PCP == 1)
  #include "pcp.h"
#endif

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*-----------------------------------------------------------
 * PUBLIC LIST API documented in list.h
 *----------------------------------------------------------*/

/* Constants used with the cRxLock and cTxLock structure members. */
#define queueUNLOCKED					      ( ( signed portBASE_TYPE ) -1 )
#define queueLOCKED_UNMODIFIED			( ( signed portBASE_TYPE ) 0 )

#define queueERRONEOUS_UNBLOCK			( -1 )

/* For internal use only. */
#define	queueSEND_TO_BACK				( 0 )
#define	queueSEND_TO_FRONT			( 1 )

/* Effectively make a union out of the xQUEUE structure. */
#define pxMutexHolder					pcTail
#define uxQueueType						pcHead
#define uxRecursiveCallCount	pcReadFrom
#define queueQUEUE_IS_MUTEX		NULL

#if (configUSE_PCP == 1)

/* PCP macros for xQUEUE */
#define pxPcpHolder					pcTail
#define uxQueueType					pcHead
#define queueQUEUE_IS_PCP		NULL

#endif

/* Semaphores do not actually store or copy data, so have an items size of
zero. */
#define queueSEMAPHORE_QUEUE_ITEM_LENGTH ( ( unsigned portBASE_TYPE ) 0 )
#define queueDONT_BLOCK					         ( ( portTickType ) 0U )
#define queueMUTEX_GIVE_BLOCK_TIME		   ( ( portTickType ) 0U )

/* These definitions *must* match those in queue.h. */
#define queueQUEUE_TYPE_BASE				        ( 0U )
#define queueQUEUE_TYPE_MUTEX 				      ( 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE	( 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE	  ( 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX		  ( 4U )
#define queueQUEUE_TYPE_PCP            		  ( 5U )


#if ( configUSE_PCP == 1 )

//typedef struct CurrPrioCeil
//{
// unsigned portBASE_TYPE uxValue;
// signed char *pcSource;
//} currentPriorityCeiling_t;

/* This is an index in the resources array that represents the current priority ceiling */
unsigned portBASE_TYPE uxCurrentPriorityCeiling;



// The array of resources to keep track of their status
xResource resources[RESOURCES_MAX];
// Latest added resource position
unsigned portBASE_TYPE uRes = 0;

#endif

/*
 * Prototypes for public functions are included here so we don't have to
 * include the API header file (as it defines xQueueHandle differently).  These
 * functions are documented in the API header file.
 */
xQueueHandle xQueueGenericCreate( unsigned portBASE_TYPE uxQueueLength, unsigned portBASE_TYPE uxItemSize, unsigned char ucQueueType ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueGenericSend( xQueueHandle xQueue, const void * const pvItemToQueue, portTickType xTicksToWait, portBASE_TYPE xCopyPosition ) PRIVILEGED_FUNCTION;
unsigned portBASE_TYPE uxQueueMessagesWaiting( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
void vQueueDelete( xQueueHandle xQueue ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueGenericSendFromISR( xQueueHandle pxQueue, const void * const pvItemToQueue, signed portBASE_TYPE *pxHigherPriorityTaskWoken, portBASE_TYPE xCopyPosition ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueGenericReceive( xQueueHandle pxQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueReceiveFromISR( xQueueHandle pxQueue, void * const pvBuffer, signed portBASE_TYPE *pxTaskWoken ) PRIVILEGED_FUNCTION;
xQueueHandle xQueueCreateMutex( unsigned char ucQueueType ) PRIVILEGED_FUNCTION;
xQueueHandle xQueueCreateCountingSemaphore( unsigned portBASE_TYPE uxCountValue, unsigned portBASE_TYPE uxInitialCount ) PRIVILEGED_FUNCTION;
portBASE_TYPE xQueueTakeMutexRecursive( xQueueHandle xMutex, portTickType xBlockTime ) PRIVILEGED_FUNCTION;
portBASE_TYPE xQueueGiveMutexRecursive( xQueueHandle xMutex ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueAltGenericSend( xQueueHandle pxQueue, const void * const pvItemToQueue, portTickType xTicksToWait, portBASE_TYPE xCopyPosition ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueAltGenericReceive( xQueueHandle pxQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueIsQueueEmptyFromISR( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
signed portBASE_TYPE xQueueIsQueueFullFromISR( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
unsigned portBASE_TYPE uxQueueMessagesWaitingFromISR( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
void vQueueWaitForMessageRestricted( xQueueHandle pxQueue, portTickType xTicksToWait ) PRIVILEGED_FUNCTION;
unsigned char ucQueueGetQueueNumber( xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
void vQueueSetQueueNumber( xQueueHandle pxQueue, unsigned char ucQueueNumber ) PRIVILEGED_FUNCTION;
unsigned char ucQueueGetQueueType( xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;
portBASE_TYPE xQueueGenericReset( xQueueHandle pxQueue, portBASE_TYPE xNewQueue ) PRIVILEGED_FUNCTION;
xTaskHandle xQueueGetMutexHolder( xQueueHandle xSemaphore ) PRIVILEGED_FUNCTION;

#if ( configUSE_PCP == 1 )

void                     vInitResources( void );
xQueueHandle             xQueuePcpCreate( unsigned char ucQueueType, unsigned portBASE_TYPE uxCeiling );
signed portBASE_TYPE     xQueuePcpReceive( xQueueHandle pxQueue, portTickType xTicksToWait );
signed portBASE_TYPE     xQueuePcpSend( xQueueHandle pxQueue );
unsigned portBASE_TYPE uGetCurrentPriorityCeiling( void );
void                     vUpdateCurrentPriorityCeiling( void );
unsigned portBASE_TYPE vGetActiveResMaxPriorityCeiling( void );

#endif

/*
 * Co-routine queue functions differ from task queue functions.  Co-routines are
 * an optional component.
 */
#if configUSE_CO_ROUTINES == 1
	signed portBASE_TYPE xQueueCRSendFromISR( xQueueHandle pxQueue, const void *pvItemToQueue, signed portBASE_TYPE xCoRoutinePreviouslyWoken ) PRIVILEGED_FUNCTION;
	signed portBASE_TYPE xQueueCRReceiveFromISR( xQueueHandle pxQueue, void *pvBuffer, signed portBASE_TYPE *pxTaskWoken ) PRIVILEGED_FUNCTION;
	signed portBASE_TYPE xQueueCRSend( xQueueHandle pxQueue, const void *pvItemToQueue, portTickType xTicksToWait ) PRIVILEGED_FUNCTION;
	signed portBASE_TYPE xQueueCRReceive( xQueueHandle pxQueue, void *pvBuffer, portTickType xTicksToWait ) PRIVILEGED_FUNCTION;
#endif

/*
 * The queue registry is just a means for kernel aware debuggers to locate
 * queue structures.  It has no other purpose so is an optional component.
 */
#if configQUEUE_REGISTRY_SIZE > 0

	/* The type stored within the queue registry array.  This allows a name
	to be assigned to each queue making kernel aware debugging a little
	more user friendly. */
	typedef struct QUEUE_REGISTRY_ITEM
	{
		signed char *pcQueueName;
		xQueueHandle xHandle;
	} xQueueRegistryItem;

	/* The queue registry is simply an array of xQueueRegistryItem structures.
	The pcQueueName member of a structure being NULL is indicative of the
	array position being vacant. */
	xQueueRegistryItem xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

	/* Removes a queue from the registry by simply setting the pcQueueName
	member to NULL. */
	static void vQueueUnregisterQueue( xQueueHandle xQueue ) PRIVILEGED_FUNCTION;
	void vQueueAddToRegistry( xQueueHandle xQueue, signed char *pcQueueName ) PRIVILEGED_FUNCTION;
#endif

/*
 * Unlocks a queue locked by a call to prvLockQueue.  Locking a queue does not
 * prevent an ISR from adding or removing items to the queue, but does prevent
 * an ISR from removing tasks from the queue event lists.  If an ISR finds a
 * queue is locked it will instead increment the appropriate queue lock count
 * to indicate that a task may require unblocking.  When the queue in unlocked
 * these lock counts are inspected, and the appropriate action taken.
 */
static void prvUnlockQueue( xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Uses a critical section to determine if there is any data in a queue.
 *
 * @return pdTRUE if the queue contains no items, otherwise pdFALSE.
 */
static signed portBASE_TYPE prvIsQueueEmpty( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Uses a critical section to determine if there is any space in a queue.
 *
 * @return pdTRUE if there is no space, otherwise pdFALSE;
 */
static signed portBASE_TYPE prvIsQueueFull( const xQueueHandle pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Copies an item into the queue, either at the front of the queue or the
 * back of the queue.
 */
static void prvCopyDataToQueue( xQUEUE *pxQueue, const void *pvItemToQueue, portBASE_TYPE xPosition ) PRIVILEGED_FUNCTION;

/*
 * Copies an item out of a queue.
 */
static void prvCopyDataFromQueue( xQUEUE * const pxQueue, const void *pvBuffer ) PRIVILEGED_FUNCTION;
/*-----------------------------------------------------------*/

/*
 * Macro to mark a queue as locked.  Locking a queue prevents an ISR from
 * accessing the queue event lists.
 */
#define prvLockQueue( pxQueue )								\
	taskENTER_CRITICAL();									\
	{														\
		if( ( pxQueue )->xRxLock == queueUNLOCKED )			\
		{													\
			( pxQueue )->xRxLock = queueLOCKED_UNMODIFIED;	\
		}													\
		if( ( pxQueue )->xTxLock == queueUNLOCKED )			\
		{													\
			( pxQueue )->xTxLock = queueLOCKED_UNMODIFIED;	\
		}													\
	}														\
	taskEXIT_CRITICAL()
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * PUBLIC QUEUE MANAGEMENT API documented in queue.h
 *----------------------------------------------------------*/

portBASE_TYPE xQueueGenericReset( xQueueHandle pxQueue, portBASE_TYPE xNewQueue )
{
portBASE_TYPE xReturn = pdPASS;

	configASSERT( pxQueue );

	/* If the queue being reset has already been used (has not just been
	created), then only reset the queue if its event lists are empty. */
	if( xNewQueue != pdTRUE )
	{
		if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
		{
			xReturn = pdFAIL;
		}

		if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
		{
			xReturn = pdFAIL;
		}
	}

	if( xReturn == pdPASS )
	{
		pxQueue->pcTail = pxQueue->pcHead + ( pxQueue->uxLength * pxQueue->uxItemSize );
		pxQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
		pxQueue->pcWriteTo = pxQueue->pcHead;
		pxQueue->pcReadFrom = pxQueue->pcHead + ( ( pxQueue->uxLength - ( unsigned portBASE_TYPE ) 1U ) * pxQueue->uxItemSize );
		pxQueue->xRxLock = queueUNLOCKED;
		pxQueue->xTxLock = queueUNLOCKED;

		/* Ensure the event queues start with the correct state. */
		vListInitialise( &( pxQueue->xTasksWaitingToSend ) );
		vListInitialise( &( pxQueue->xTasksWaitingToReceive ) );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

xQueueHandle xQueueGenericCreate( unsigned portBASE_TYPE uxQueueLength, unsigned portBASE_TYPE uxItemSize, unsigned char ucQueueType )
{
xQUEUE *pxNewQueue;
size_t xQueueSizeInBytes;
xQueueHandle xReturn = NULL;

	/* Remove compiler warnings about unused parameters should
	configUSE_TRACE_FACILITY not be set to 1. */
	( void ) ucQueueType;

	/* Allocate the new queue structure. */
	if( uxQueueLength > ( unsigned portBASE_TYPE ) 0 )
	{
		pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );
		if( pxNewQueue != NULL )
		{
			/* Create the list of pointers to queue items.  The queue is one byte
			longer than asked for to make wrap checking easier/faster. */
			xQueueSizeInBytes = ( size_t ) ( uxQueueLength * uxItemSize ) + ( size_t ) 1;

			pxNewQueue->pcHead = ( signed char * ) pvPortMalloc( xQueueSizeInBytes );
			if( pxNewQueue->pcHead != NULL )
			{
				/* Initialise the queue members as described above where the
				queue type is defined. */
				pxNewQueue->uxLength = uxQueueLength;
				pxNewQueue->uxItemSize = uxItemSize;
				xQueueGenericReset( pxNewQueue, pdTRUE );
				#if ( configUSE_TRACE_FACILITY == 1 )
				{
					pxNewQueue->ucQueueType = ucQueueType;
				}
				#endif /* configUSE_TRACE_FACILITY */

				traceQUEUE_CREATE( pxNewQueue );
				xReturn = pxNewQueue;
			}
			else
			{
				traceQUEUE_CREATE_FAILED( ucQueueType );
				vPortFree( pxNewQueue );
			}
		}
	}

	configASSERT( xReturn );

	return xReturn;
}
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

	xQueueHandle xQueueCreateMutex( unsigned char ucQueueType )
	{
	xQUEUE *pxNewQueue;

		/* Prevent compiler warnings about unused parameters if
		configUSE_TRACE_FACILITY does not equal 1. */
		( void ) ucQueueType;

		/* Allocate the new queue structure. */
		pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );
		if( pxNewQueue != NULL )
		{
			/* Information required for priority inheritance. */
			pxNewQueue->pxMutexHolder = NULL;
			pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;

			/* Queues used as a mutex no data is actually copied into or out
			of the queue. */
			pxNewQueue->pcWriteTo = NULL;
			pxNewQueue->pcReadFrom = NULL;

			/* Each mutex has a length of 1 (like a binary semaphore) and
			an item size of 0 as nothing is actually copied into or out
			of the mutex. */
			pxNewQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
			pxNewQueue->uxLength = ( unsigned portBASE_TYPE ) 1U;
			pxNewQueue->uxItemSize = ( unsigned portBASE_TYPE ) 0U;
			pxNewQueue->xRxLock = queueUNLOCKED;
			pxNewQueue->xTxLock = queueUNLOCKED;

			#if ( configUSE_TRACE_FACILITY == 1 )
			{
				pxNewQueue->ucQueueType = ucQueueType;
			}
			#endif

			/* Ensure the event queues start with the correct state. */
			vListInitialise( &( pxNewQueue->xTasksWaitingToSend ) );
			vListInitialise( &( pxNewQueue->xTasksWaitingToReceive ) );

			traceCREATE_MUTEX( pxNewQueue );

			/* Start with the semaphore in the expected state. */
			xQueueGenericSend( pxNewQueue, NULL, ( portTickType ) 0U, queueSEND_TO_BACK );
		}
		else
		{
			traceCREATE_MUTEX_FAILED();
		}

		configASSERT( pxNewQueue );
		return pxNewQueue;
	}

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

	void* xQueueGetMutexHolder( xQueueHandle xSemaphore )
	{
	void *pxReturn;

		/* This function is called by xSemaphoreGetMutexHolder(), and should not
		be called directly.  Note:  This is is a good way of determining if the
		calling task is the mutex holder, but not a good way of determining the
		identity of the mutex holder, as the holder may change between the 
		following critical section exiting and the function returning. */
		taskENTER_CRITICAL();
		{
			if( xSemaphore->uxQueueType == queueQUEUE_IS_MUTEX )
			{
				pxReturn = ( void * ) xSemaphore->pxMutexHolder;
			}
			else
			{
				pxReturn = NULL;
			}
		}
		taskEXIT_CRITICAL();
		
		return pxReturn;
	}

#endif
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )

	portBASE_TYPE xQueueGiveMutexRecursive( xQueueHandle pxMutex )
	{
	portBASE_TYPE xReturn;

		configASSERT( pxMutex );

		/* If this is the task that holds the mutex then pxMutexHolder will not
		change outside of this task.  If this task does not hold the mutex then
		pxMutexHolder can never coincidentally equal the tasks handle, and as
		this is the only condition we are interested in it does not matter if
		pxMutexHolder is accessed simultaneously by another task.  Therefore no
		mutual exclusion is required to test the pxMutexHolder variable. */
		if( pxMutex->pxMutexHolder == xTaskGetCurrentTaskHandle() )
		{
			traceGIVE_MUTEX_RECURSIVE( pxMutex );

			/* uxRecursiveCallCount cannot be zero if pxMutexHolder is equal to
			the task handle, therefore no underflow check is required.  Also,
			uxRecursiveCallCount is only modified by the mutex holder, and as
			there can only be one, no mutual exclusion is required to modify the
			uxRecursiveCallCount member. */
			( pxMutex->uxRecursiveCallCount )--;

			/* Have we unwound the call count? */
			if( pxMutex->uxRecursiveCallCount == 0 )
			{
				/* Return the mutex.  This will automatically unblock any other
				task that might be waiting to access the mutex. */
				xQueueGenericSend( pxMutex, NULL, queueMUTEX_GIVE_BLOCK_TIME, queueSEND_TO_BACK );
			}

			xReturn = pdPASS;
		}
		else
		{
			/* We cannot give the mutex because we are not the holder. */
			xReturn = pdFAIL;

			traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex );
		}

		return xReturn;
	}

#endif /* configUSE_RECURSIVE_MUTEXES */
/*-----------------------------------------------------------*/

#if configUSE_RECURSIVE_MUTEXES == 1

	portBASE_TYPE xQueueTakeMutexRecursive( xQueueHandle pxMutex, portTickType xBlockTime )
	{
	portBASE_TYPE xReturn;

		configASSERT( pxMutex );

		/* Comments regarding mutual exclusion as per those within
		xQueueGiveMutexRecursive(). */

		traceTAKE_MUTEX_RECURSIVE( pxMutex );

		if( pxMutex->pxMutexHolder == xTaskGetCurrentTaskHandle() )
		{
			( pxMutex->uxRecursiveCallCount )++;
			xReturn = pdPASS;
		}
		else
		{
			xReturn = xQueueGenericReceive( pxMutex, NULL, xBlockTime, pdFALSE );

			/* pdPASS will only be returned if we successfully obtained the mutex,
			we may have blocked to reach here. */
			if( xReturn == pdPASS )
			{
				( pxMutex->uxRecursiveCallCount )++;
			}
			else
			{
				traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex );
			}
		}

		return xReturn;
	}

#endif /* configUSE_RECURSIVE_MUTEXES */
/*-----------------------------------------------------------*/

#if configUSE_COUNTING_SEMAPHORES == 1

	xQueueHandle xQueueCreateCountingSemaphore( unsigned portBASE_TYPE uxCountValue, unsigned portBASE_TYPE uxInitialCount )
	{
	xQueueHandle pxHandle;

		pxHandle = xQueueGenericCreate( ( unsigned portBASE_TYPE ) uxCountValue, queueSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_COUNTING_SEMAPHORE );

		if( pxHandle != NULL )
		{
			pxHandle->uxMessagesWaiting = uxInitialCount;

			traceCREATE_COUNTING_SEMAPHORE();
		}
		else
		{
			traceCREATE_COUNTING_SEMAPHORE_FAILED();
		}

		configASSERT( pxHandle );
		return pxHandle;
	}

#endif /* configUSE_COUNTING_SEMAPHORES */
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueGenericSend( xQueueHandle pxQueue, const void * const pvItemToQueue, portTickType xTicksToWait, portBASE_TYPE xCopyPosition )
{
signed portBASE_TYPE xEntryTimeSet = pdFALSE;
xTimeOutType xTimeOut;

	configASSERT( pxQueue );
	configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

	/* This function relaxes the coding standard somewhat to allow return
	statements within the function itself.  This is done in the interest
	of execution time efficiency. */
	for( ;; )
	{
		taskENTER_CRITICAL();
		{
			/* Is there room on the queue now?  To be running we must be
			the highest priority task wanting to access the queue. */
			if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
			{
				traceQUEUE_SEND( pxQueue );
				prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

				/* If there was a task waiting for data to arrive on the
				queue then unblock it now. */
				if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
				{
					if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) == pdTRUE )
					{
						/* The unblocked task has a priority higher than
						our own so yield immediately.  Yes it is ok to do
						this from within the critical section - the kernel
						takes care of that. */
						portYIELD_WITHIN_API();
					}
				}

				taskEXIT_CRITICAL();

				/* Return to the original privilege level before exiting the
				function. */
				return pdPASS;
			}
			else
			{
				if( xTicksToWait == ( portTickType ) 0 )
				{
					/* The queue was full and no block time is specified (or
					the block time has expired) so leave now. */
					taskEXIT_CRITICAL();

					/* Return to the original privilege level before exiting
					the function. */
					traceQUEUE_SEND_FAILED( pxQueue );
					return errQUEUE_FULL;
				}
				else if( xEntryTimeSet == pdFALSE )
				{
					/* The queue was full and a block time was specified so
					configure the timeout structure. */
					vTaskSetTimeOutState( &xTimeOut );
					xEntryTimeSet = pdTRUE;
				}
			}
		}
		taskEXIT_CRITICAL();

		/* Interrupts and other tasks can send to and receive from the queue
		now the critical section has been exited. */

		vTaskSuspendAll();
		prvLockQueue( pxQueue );

		/* Update the timeout state to see if it has expired yet. */
		if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
		{
			if( prvIsQueueFull( pxQueue ) != pdFALSE )
			{
				traceBLOCKING_ON_QUEUE_SEND( pxQueue );
				vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), xTicksToWait );

				/* Unlocking the queue means queue events can effect the
				event list.  It is possible	that interrupts occurring now
				remove this task from the event	list again - but as the
				scheduler is suspended the task will go onto the pending
				ready last instead of the actual ready list. */
				prvUnlockQueue( pxQueue );

				/* Resuming the scheduler will move tasks from the pending
				ready list into the ready list - so it is feasible that this
				task is already in a ready list before it yields - in which
				case the yield will not cause a context switch unless there
				is also a higher priority task in the pending ready list. */
				if( xTaskResumeAll() == pdFALSE )
				{
					portYIELD_WITHIN_API();
				}
			}
			else
			{
				/* Try again. */
				prvUnlockQueue( pxQueue );
				( void ) xTaskResumeAll();
			}
		}
		else
		{
			/* The timeout has expired. */
			prvUnlockQueue( pxQueue );
			( void ) xTaskResumeAll();

			/* Return to the original privilege level before exiting the
			function. */
			traceQUEUE_SEND_FAILED( pxQueue );
			return errQUEUE_FULL;
		}
	}
}
/*-----------------------------------------------------------*/

#if configUSE_ALTERNATIVE_API == 1

	signed portBASE_TYPE xQueueAltGenericSend( xQueueHandle pxQueue, const void * const pvItemToQueue, portTickType xTicksToWait, portBASE_TYPE xCopyPosition )
	{
	signed portBASE_TYPE xEntryTimeSet = pdFALSE;
	xTimeOutType xTimeOut;

		configASSERT( pxQueue );
		configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

		for( ;; )
		{
			taskENTER_CRITICAL();
			{
				/* Is there room on the queue now?  To be running we must be
				the highest priority task wanting to access the queue. */
				if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
				{
					traceQUEUE_SEND( pxQueue );
					prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

					/* If there was a task waiting for data to arrive on the
					queue then unblock it now. */
					if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
					{
						if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) == pdTRUE )
						{
							/* The unblocked task has a priority higher than
							our own so yield immediately. */
							portYIELD_WITHIN_API();
						}
					}

					taskEXIT_CRITICAL();
					return pdPASS;
				}
				else
				{
					if( xTicksToWait == ( portTickType ) 0 )
					{
						taskEXIT_CRITICAL();
						return errQUEUE_FULL;
					}
					else if( xEntryTimeSet == pdFALSE )
					{
						vTaskSetTimeOutState( &xTimeOut );
						xEntryTimeSet = pdTRUE;
					}
				}
			}
			taskEXIT_CRITICAL();

			taskENTER_CRITICAL();
			{
				if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
				{
					if( prvIsQueueFull( pxQueue ) != pdFALSE )
					{
						traceBLOCKING_ON_QUEUE_SEND( pxQueue );
						vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), xTicksToWait );
						portYIELD_WITHIN_API();
					}
				}
				else
				{
					taskEXIT_CRITICAL();
					traceQUEUE_SEND_FAILED( pxQueue );
					return errQUEUE_FULL;
				}
			}
			taskEXIT_CRITICAL();
		}
	}

#endif /* configUSE_ALTERNATIVE_API */
/*-----------------------------------------------------------*/

#if configUSE_ALTERNATIVE_API == 1

	signed portBASE_TYPE xQueueAltGenericReceive( xQueueHandle pxQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking )
	{
	signed portBASE_TYPE xEntryTimeSet = pdFALSE;
	xTimeOutType xTimeOut;
	signed char *pcOriginalReadPosition;

		configASSERT( pxQueue );
		configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

		for( ;; )
		{
			taskENTER_CRITICAL();
			{
				if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
				{
					/* Remember our read position in case we are just peeking. */
					pcOriginalReadPosition = pxQueue->pcReadFrom;

					prvCopyDataFromQueue( pxQueue, pvBuffer );

					if( xJustPeeking == pdFALSE )
					{
						traceQUEUE_RECEIVE( pxQueue );

						/* We are actually removing data. */
						--( pxQueue->uxMessagesWaiting );

						#if ( configUSE_MUTEXES == 1 )
						{
							if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
							{
								/* Record the information required to implement
								priority inheritance should it become necessary. */
								pxQueue->pxMutexHolder = xTaskGetCurrentTaskHandle();
							}
						}
						#endif

						if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
						{
							if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) == pdTRUE )
							{
								portYIELD_WITHIN_API();
							}
						}
					}
					else
					{
						traceQUEUE_PEEK( pxQueue );

						/* We are not removing the data, so reset our read
						pointer. */
						pxQueue->pcReadFrom = pcOriginalReadPosition;

						/* The data is being left in the queue, so see if there are
						any other tasks waiting for the data. */
						if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
						{
							/* Tasks that are removed from the event list will get added to
							the pending ready list as the scheduler is still suspended. */
							if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
							{
								/* The task waiting has a higher priority than this task. */
								portYIELD_WITHIN_API();
							}
						}

					}

					taskEXIT_CRITICAL();
					return pdPASS;
				}
				else
				{
					if( xTicksToWait == ( portTickType ) 0 )
					{
						taskEXIT_CRITICAL();
						traceQUEUE_RECEIVE_FAILED( pxQueue );
						return errQUEUE_EMPTY;
					}
					else if( xEntryTimeSet == pdFALSE )
					{
						vTaskSetTimeOutState( &xTimeOut );
						xEntryTimeSet = pdTRUE;
					}
				}
			}
			taskEXIT_CRITICAL();

			taskENTER_CRITICAL();
			{
				if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
				{
					if( prvIsQueueEmpty( pxQueue ) != pdFALSE )
					{
						traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );

						#if ( configUSE_MUTEXES == 1 )
						{
							if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
							{
								portENTER_CRITICAL();
									vTaskPriorityInherit( ( void * ) pxQueue->pxMutexHolder );
								portEXIT_CRITICAL();
							}
						}
						#endif

						vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
						portYIELD_WITHIN_API();
					}
				}
				else
				{
					taskEXIT_CRITICAL();
					traceQUEUE_RECEIVE_FAILED( pxQueue );
					return errQUEUE_EMPTY;
				}
			}
			taskEXIT_CRITICAL();
		}
	}


#endif /* configUSE_ALTERNATIVE_API */
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueGenericSendFromISR( xQueueHandle pxQueue, const void * const pvItemToQueue, signed portBASE_TYPE *pxHigherPriorityTaskWoken, portBASE_TYPE xCopyPosition )
{
signed portBASE_TYPE xReturn;
unsigned portBASE_TYPE uxSavedInterruptStatus;

	configASSERT( pxQueue );
	configASSERT( pxHigherPriorityTaskWoken );
	configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

	/* Similar to xQueueGenericSend, except we don't block if there is no room
	in the queue.  Also we don't directly wake a task that was blocked on a
	queue read, instead we return a flag to say whether a context switch is
	required or not (i.e. has a task with a higher priority than us been woken
	by this	post). */
	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
		{
			traceQUEUE_SEND_FROM_ISR( pxQueue );

			prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

			/* If the queue is locked we do not alter the event list.  This will
			be done when the queue is unlocked later. */
			if( pxQueue->xTxLock == queueUNLOCKED )
			{
				if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
				{
					if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
					{
						/* The task waiting has a higher priority so record that a
						context	switch is required. */
						*pxHigherPriorityTaskWoken = pdTRUE;
					}
				}
			}
			else
			{
				/* Increment the lock count so the task that unlocks the queue
				knows that data was posted while it was locked. */
				++( pxQueue->xTxLock );
			}

			xReturn = pdPASS;
		}
		else
		{
			traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue );
			xReturn = errQUEUE_FULL;
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueGenericReceive( xQueueHandle pxQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking )
{
signed portBASE_TYPE xEntryTimeSet = pdFALSE;
xTimeOutType xTimeOut;
signed char *pcOriginalReadPosition;

	configASSERT( pxQueue );
	configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

	/* This function relaxes the coding standard somewhat to allow return
	statements within the function itself.  This is done in the interest
	of execution time efficiency. */

	for( ;; )
	{
		taskENTER_CRITICAL();
		{
			/* Is there data in the queue now?  To be running we must be
			the highest priority task wanting to access the queue. */
			if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
			{
				/* Remember our read position in case we are just peeking. */
				pcOriginalReadPosition = pxQueue->pcReadFrom;

				prvCopyDataFromQueue( pxQueue, pvBuffer );

				if( xJustPeeking == pdFALSE )
				{
					traceQUEUE_RECEIVE( pxQueue );

					/* We are actually removing data. */
					--( pxQueue->uxMessagesWaiting );

					#if ( configUSE_MUTEXES == 1 )
					{
						if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
						{
							/* Record the information required to implement
							priority inheritance should it become necessary. */
							pxQueue->pxMutexHolder = xTaskGetCurrentTaskHandle();
						}
					}
					#endif

					if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
					{
						if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) == pdTRUE )
						{
							portYIELD_WITHIN_API();
						}
					}
				}
				else
				{
					traceQUEUE_PEEK( pxQueue );

					/* We are not removing the data, so reset our read
					pointer. */
					pxQueue->pcReadFrom = pcOriginalReadPosition;

					/* The data is being left in the queue, so see if there are
					any other tasks waiting for the data. */
					if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
					{
						/* Tasks that are removed from the event list will get added to
						the pending ready list as the scheduler is still suspended. */
						if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
						{
							/* The task waiting has a higher priority than this task. */
							portYIELD_WITHIN_API();
						}
					}

				}

				taskEXIT_CRITICAL();
				return pdPASS;
			}
			else
			{
				if( xTicksToWait == ( portTickType ) 0 )
				{
					/* The queue was empty and no block time is specified (or
					the block time has expired) so leave now. */
					taskEXIT_CRITICAL();
					traceQUEUE_RECEIVE_FAILED( pxQueue );
					return errQUEUE_EMPTY;
				}
				else if( xEntryTimeSet == pdFALSE )
				{
					/* The queue was empty and a block time was specified so
					configure the timeout structure. */
					vTaskSetTimeOutState( &xTimeOut );
					xEntryTimeSet = pdTRUE;
				}
			}
		}
		taskEXIT_CRITICAL();

		/* Interrupts and other tasks can send to and receive from the queue
		now the critical section has been exited. */

		vTaskSuspendAll();
		prvLockQueue( pxQueue );

		/* Update the timeout state to see if it has expired yet. */
		if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
		{
			if( prvIsQueueEmpty( pxQueue ) != pdFALSE )
			{
				traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );

				#if ( configUSE_MUTEXES == 1 )
				{
					if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
					{
						portENTER_CRITICAL();
						{
							vTaskPriorityInherit( ( void * ) pxQueue->pxMutexHolder );
						}
						portEXIT_CRITICAL();
					}
				}
				#endif

				vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
				prvUnlockQueue( pxQueue );
				if( xTaskResumeAll() == pdFALSE )
				{
					portYIELD_WITHIN_API();
				}
			}
			else
			{
				/* Try again. */
				prvUnlockQueue( pxQueue );
				( void ) xTaskResumeAll();
			}
		}
		else
		{
			prvUnlockQueue( pxQueue );
			( void ) xTaskResumeAll();
			traceQUEUE_RECEIVE_FAILED( pxQueue );
			return errQUEUE_EMPTY;
		}
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueReceiveFromISR( xQueueHandle pxQueue, void * const pvBuffer, signed portBASE_TYPE *pxTaskWoken )
{
signed portBASE_TYPE xReturn;
unsigned portBASE_TYPE uxSavedInterruptStatus;

	configASSERT( pxQueue );
	configASSERT( pxTaskWoken );
	configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* We cannot block from an ISR, so check there is data available. */
		if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
		{
			traceQUEUE_RECEIVE_FROM_ISR( pxQueue );

			prvCopyDataFromQueue( pxQueue, pvBuffer );
			--( pxQueue->uxMessagesWaiting );

			/* If the queue is locked we will not modify the event list.  Instead
			we update the lock count so the task that unlocks the queue will know
			that an ISR has removed data while the queue was locked. */
			if( pxQueue->xRxLock == queueUNLOCKED )
			{
				if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
				{
					if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
					{
						/* The task waiting has a higher priority than us so
						force a context switch. */
						*pxTaskWoken = pdTRUE;
					}
				}
			}
			else
			{
				/* Increment the lock count so the task that unlocks the queue
				knows that data was removed while it was locked. */
				++( pxQueue->xRxLock );
			}

			xReturn = pdPASS;
		}
		else
		{
			xReturn = pdFAIL;
			traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue );
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

	return xReturn;
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxQueueMessagesWaiting( const xQueueHandle pxQueue )
{
unsigned portBASE_TYPE uxReturn;

	configASSERT( pxQueue );

	taskENTER_CRITICAL();
		uxReturn = pxQueue->uxMessagesWaiting;
	taskEXIT_CRITICAL();

	return uxReturn;
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxQueueMessagesWaitingFromISR( const xQueueHandle pxQueue )
{
unsigned portBASE_TYPE uxReturn;

	configASSERT( pxQueue );

	uxReturn = pxQueue->uxMessagesWaiting;

	return uxReturn;
}
/*-----------------------------------------------------------*/

void vQueueDelete( xQueueHandle pxQueue )
{
	configASSERT( pxQueue );

	traceQUEUE_DELETE( pxQueue );
	vQueueUnregisterQueue( pxQueue );
	vPortFree( pxQueue->pcHead );
	vPortFree( pxQueue );
}
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	unsigned char ucQueueGetQueueNumber( xQueueHandle pxQueue )
	{
		return pxQueue->ucQueueNumber;
	}

#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	void vQueueSetQueueNumber( xQueueHandle pxQueue, unsigned char ucQueueNumber )
	{
		pxQueue->ucQueueNumber = ucQueueNumber;
	}

#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	unsigned char ucQueueGetQueueType( xQueueHandle pxQueue )
	{
		return pxQueue->ucQueueType;
	}

#endif
/*-----------------------------------------------------------*/

static void prvCopyDataToQueue( xQUEUE *pxQueue, const void *pvItemToQueue, portBASE_TYPE xPosition )
{
	if( pxQueue->uxItemSize == ( unsigned portBASE_TYPE ) 0 )
	{
		#if ( configUSE_MUTEXES == 1 )
		{
			if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
			{
				/* The mutex is no longer being held. */
				vTaskPriorityDisinherit( ( void * ) pxQueue->pxMutexHolder );
				pxQueue->pxMutexHolder = NULL;
			}
		}
		#endif
	}
	else if( xPosition == queueSEND_TO_BACK )
	{
		memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( unsigned ) pxQueue->uxItemSize );
		pxQueue->pcWriteTo += pxQueue->uxItemSize;
		if( pxQueue->pcWriteTo >= pxQueue->pcTail )
		{
			pxQueue->pcWriteTo = pxQueue->pcHead;
		}
	}
	else
	{
		memcpy( ( void * ) pxQueue->pcReadFrom, pvItemToQueue, ( unsigned ) pxQueue->uxItemSize );
		pxQueue->pcReadFrom -= pxQueue->uxItemSize;
		if( pxQueue->pcReadFrom < pxQueue->pcHead )
		{
			pxQueue->pcReadFrom = ( pxQueue->pcTail - pxQueue->uxItemSize );
		}
	}

	++( pxQueue->uxMessagesWaiting );
}
/*-----------------------------------------------------------*/

static void prvCopyDataFromQueue( xQUEUE * const pxQueue, const void *pvBuffer )
{
	if( pxQueue->uxQueueType != queueQUEUE_IS_MUTEX )
	{
		pxQueue->pcReadFrom += pxQueue->uxItemSize;
		if( pxQueue->pcReadFrom >= pxQueue->pcTail )
		{
			pxQueue->pcReadFrom = pxQueue->pcHead;
		}
		memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( unsigned ) pxQueue->uxItemSize );
	}
}
/*-----------------------------------------------------------*/

static void prvUnlockQueue( xQueueHandle pxQueue )
{
	/* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED. */

	/* The lock counts contains the number of extra data items placed or
	removed from the queue while the queue was locked.  When a queue is
	locked items can be added or removed, but the event lists cannot be
	updated. */
	taskENTER_CRITICAL();
	{
		/* See if data was added to the queue while it was locked. */
		while( pxQueue->xTxLock > queueLOCKED_UNMODIFIED )
		{
			/* Data was posted while the queue was locked.  Are any tasks
			blocked waiting for data to become available? */
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
			{
				/* Tasks that are removed from the event list will get added to
				the pending ready list as the scheduler is still suspended. */
				if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
				{
					/* The task waiting has a higher priority so record that a
					context	switch is required. */
					vTaskMissedYield();
				}

				--( pxQueue->xTxLock );
			}
			else
			{
				break;
			}
		}

		pxQueue->xTxLock = queueUNLOCKED;
	}
	taskEXIT_CRITICAL();

	/* Do the same for the Rx lock. */
	taskENTER_CRITICAL();
	{
		while( pxQueue->xRxLock > queueLOCKED_UNMODIFIED )
		{
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
			{
				if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
				{
					vTaskMissedYield();
				}

				--( pxQueue->xRxLock );
			}
			else
			{
				break;
			}
		}

		pxQueue->xRxLock = queueUNLOCKED;
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static signed portBASE_TYPE prvIsQueueEmpty( const xQueueHandle pxQueue )
{
signed portBASE_TYPE xReturn;

	taskENTER_CRITICAL();
		xReturn = ( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0 );
	taskEXIT_CRITICAL();

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueIsQueueEmptyFromISR( const xQueueHandle pxQueue )
{
signed portBASE_TYPE xReturn;

	configASSERT( pxQueue );
	xReturn = ( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0 );

	return xReturn;
}
/*-----------------------------------------------------------*/

static signed portBASE_TYPE prvIsQueueFull( const xQueueHandle pxQueue )
{
signed portBASE_TYPE xReturn;

	taskENTER_CRITICAL();
		xReturn = ( pxQueue->uxMessagesWaiting == pxQueue->uxLength );
	taskEXIT_CRITICAL();

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueIsQueueFullFromISR( const xQueueHandle pxQueue )
{
signed portBASE_TYPE xReturn;

	configASSERT( pxQueue );
	xReturn = ( pxQueue->uxMessagesWaiting == pxQueue->uxLength );

	return xReturn;
}
/*-----------------------------------------------------------*/

#if configUSE_CO_ROUTINES == 1
signed portBASE_TYPE xQueueCRSend( xQueueHandle pxQueue, const void *pvItemToQueue, portTickType xTicksToWait )
{
signed portBASE_TYPE xReturn;

	/* If the queue is already full we may have to block.  A critical section
	is required to prevent an interrupt removing something from the queue
	between the check to see if the queue is full and blocking on the queue. */
	portDISABLE_INTERRUPTS();
	{
		if( prvIsQueueFull( pxQueue ) != pdFALSE )
		{
			/* The queue is full - do we want to block or just leave without
			posting? */
			if( xTicksToWait > ( portTickType ) 0 )
			{
				/* As this is called from a coroutine we cannot block directly, but
				return indicating that we need to block. */
				vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToSend ) );
				portENABLE_INTERRUPTS();
				return errQUEUE_BLOCKED;
			}
			else
			{
				portENABLE_INTERRUPTS();
				return errQUEUE_FULL;
			}
		}
	}
	portENABLE_INTERRUPTS();

	portNOP();

	portDISABLE_INTERRUPTS();
	{
		if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
		{
			/* There is room in the queue, copy the data into the queue. */
			prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );
			xReturn = pdPASS;

			/* Were any co-routines waiting for data to become available? */
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
			{
				/* In this instance the co-routine could be placed directly
				into the ready list as we are within a critical section.
				Instead the same pending ready list mechanism is used as if
				the event were caused from within an interrupt. */
				if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
				{
					/* The co-routine waiting has a higher priority so record
					that a yield might be appropriate. */
					xReturn = errQUEUE_YIELD;
				}
			}
		}
		else
		{
			xReturn = errQUEUE_FULL;
		}
	}
	portENABLE_INTERRUPTS();

	return xReturn;
}
#endif
/*-----------------------------------------------------------*/

#if configUSE_CO_ROUTINES == 1
signed portBASE_TYPE xQueueCRReceive( xQueueHandle pxQueue, void *pvBuffer, portTickType xTicksToWait )
{
signed portBASE_TYPE xReturn;

	/* If the queue is already empty we may have to block.  A critical section
	is required to prevent an interrupt adding something to the queue
	between the check to see if the queue is empty and blocking on the queue. */
	portDISABLE_INTERRUPTS();
	{
		if( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0 )
		{
			/* There are no messages in the queue, do we want to block or just
			leave with nothing? */
			if( xTicksToWait > ( portTickType ) 0 )
			{
				/* As this is a co-routine we cannot block directly, but return
				indicating that we need to block. */
				vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToReceive ) );
				portENABLE_INTERRUPTS();
				return errQUEUE_BLOCKED;
			}
			else
			{
				portENABLE_INTERRUPTS();
				return errQUEUE_FULL;
			}
		}
	}
	portENABLE_INTERRUPTS();

	portNOP();

	portDISABLE_INTERRUPTS();
	{
		if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
		{
			/* Data is available from the queue. */
			pxQueue->pcReadFrom += pxQueue->uxItemSize;
			if( pxQueue->pcReadFrom >= pxQueue->pcTail )
			{
				pxQueue->pcReadFrom = pxQueue->pcHead;
			}
			--( pxQueue->uxMessagesWaiting );
			memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( unsigned ) pxQueue->uxItemSize );

			xReturn = pdPASS;

			/* Were any co-routines waiting for space to become available? */
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
			{
				/* In this instance the co-routine could be placed directly
				into the ready list as we are within a critical section.
				Instead the same pending ready list mechanism is used as if
				the event were caused from within an interrupt. */
				if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
				{
					xReturn = errQUEUE_YIELD;
				}
			}
		}
		else
		{
			xReturn = pdFAIL;
		}
	}
	portENABLE_INTERRUPTS();

	return xReturn;
}
#endif
/*-----------------------------------------------------------*/



#if configUSE_CO_ROUTINES == 1
signed portBASE_TYPE xQueueCRSendFromISR( xQueueHandle pxQueue, const void *pvItemToQueue, signed portBASE_TYPE xCoRoutinePreviouslyWoken )
{
	/* Cannot block within an ISR so if there is no space on the queue then
	exit without doing anything. */
	if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
	{
		prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );

		/* We only want to wake one co-routine per ISR, so check that a
		co-routine has not already been woken. */
		if( xCoRoutinePreviouslyWoken == pdFALSE )
		{
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
			{
				if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
				{
					return pdTRUE;
				}
			}
		}
	}

	return xCoRoutinePreviouslyWoken;
}
#endif
/*-----------------------------------------------------------*/

#if configUSE_CO_ROUTINES == 1
signed portBASE_TYPE xQueueCRReceiveFromISR( xQueueHandle pxQueue, void *pvBuffer, signed portBASE_TYPE *pxCoRoutineWoken )
{
signed portBASE_TYPE xReturn;

	/* We cannot block from an ISR, so check there is data available. If
	not then just leave without doing anything. */
	if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
	{
		/* Copy the data from the queue. */
		pxQueue->pcReadFrom += pxQueue->uxItemSize;
		if( pxQueue->pcReadFrom >= pxQueue->pcTail )
		{
			pxQueue->pcReadFrom = pxQueue->pcHead;
		}
		--( pxQueue->uxMessagesWaiting );
		memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( unsigned ) pxQueue->uxItemSize );

		if( ( *pxCoRoutineWoken ) == pdFALSE )
		{
			if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
			{
				if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
				{
					*pxCoRoutineWoken = pdTRUE;
				}
			}
		}

		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0

	void vQueueAddToRegistry( xQueueHandle xQueue, signed char *pcQueueName )
	{
	unsigned portBASE_TYPE ux;

		/* See if there is an empty space in the registry.  A NULL name denotes
		a free slot. */
		for( ux = ( unsigned portBASE_TYPE ) 0U; ux < ( unsigned portBASE_TYPE ) configQUEUE_REGISTRY_SIZE; ux++ )
		{
			if( xQueueRegistry[ ux ].pcQueueName == NULL )
			{
				/* Store the information on this queue. */
				xQueueRegistry[ ux ].pcQueueName = pcQueueName;
				xQueueRegistry[ ux ].xHandle = xQueue;
				break;
			}
		}
	}

#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0

	static void vQueueUnregisterQueue( xQueueHandle xQueue )
	{
	unsigned portBASE_TYPE ux;

		/* See if the handle of the queue being unregistered in actually in the
		registry. */
		for( ux = ( unsigned portBASE_TYPE ) 0U; ux < ( unsigned portBASE_TYPE ) configQUEUE_REGISTRY_SIZE; ux++ )
		{
			if( xQueueRegistry[ ux ].xHandle == xQueue )
			{
				/* Set the name to NULL to show that this slot if free again. */
				xQueueRegistry[ ux ].pcQueueName = NULL;
				break;
			}
		}

	}

#endif
/*-----------------------------------------------------------*/

#if configUSE_TIMERS == 1

	void vQueueWaitForMessageRestricted( xQueueHandle pxQueue, portTickType xTicksToWait )
	{
		/* This function should not be called by application code hence the
		'Restricted' in its name.  It is not part of the public API.  It is
		designed for use by kernel code, and has special calling requirements.
		It can result in vListInsert() being called on a list that can only
		possibly ever have one item in it, so the list will be fast, but even
		so it should be called with the scheduler locked and not from a critical
		section. */

		/* Only do anything if there are no messages in the queue.  This function
		will not actually cause the task to block, just place it on a blocked
		list.  It will not block until the scheduler is unlocked - at which
		time a yield will be performed.  If an item is added to the queue while
		the queue is locked, and the calling task blocks on the queue, then the
		calling task will be immediately unblocked when the queue is unlocked. */
		prvLockQueue( pxQueue );
		if( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0U )
		{
			/* There is nothing in the queue, block for the specified period. */
			vTaskPlaceOnEventListRestricted( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
		}
		prvUnlockQueue( pxQueue );
	}

#endif
/*-----------------------------------------------------------*/

#if ( configUSE_PCP == 1 )

  // Set Resource Attributes
void vSetResource( unsigned portBASE_TYPE uxResId, 
                   status_e status, 
                   xTaskHandle task, 
                   unsigned portBASE_TYPE uxPriorityCeiling, 
                   unsigned portBASE_TYPE uxPriorityBlocked,
                   xQueueHandle xQueue
                 )  
  {
      resources[uxResId].status               = status;
      resources[uxResId].pcHoldingTask        = task;
      resources[uxResId].uxPriorityCeiling    = uxPriorityCeiling;
      resources[uxResId].uxPriorityMaxBlocked = uxPriorityBlocked;    
      resources[uxResId].xQueueRes            = xQueue;
      // This can be improved to only update when the priority ceiling is updated
      vUpdateCurrentPriorityCeiling();
  }

void vIncrResIdx ( void )
{
  if ( uRes < RESOURCES_MAX )
  {
    uRes++;
  }
  else
  {
    while (1)
    {
    }
  }
}  
  
/* This function needs to be called before any PCP semaphores get created
// in order to enable the resources list (for current priority ceiling)
// Scheduler should not be running yet !
// This function is not an implementation must since we initialize each
// array item at PCP create, and the priority ceiling at declaration.
*/
  void vInitResources( void )
  {
    unsigned portBASE_TYPE res = 0;
    
    for ( res = 0; res < RESOURCES_MAX; res++)
    {
      vSetResource(res, FREE, NULL, tskIDLE_PRIORITY, tskIDLE_PRIORITY, NULL);
    }
    uxCurrentPriorityCeiling = uRes;
    
  }

unsigned portBASE_TYPE uGetCurrentPriorityCeiling( void )
{
    unsigned portBASE_TYPE uxReturn;

    taskENTER_CRITICAL();
      uxReturn = uxCurrentPriorityCeiling;
    taskEXIT_CRITICAL();

    return uxReturn;
  
}

/* overhead due to using an unsorted array
// list implementation with list sorted according to 
// priority ceiling would eliminate this overhead
// ( and add list processing overhead when adding/removing items)
*/
unsigned portBASE_TYPE vGetActiveResMaxPriorityCeiling( void )
{
    unsigned portBASE_TYPE res_id  = 0;
    unsigned portBASE_TYPE uReturn = uRes;  
    unsigned portBASE_TYPE priority = tskIDLE_PRIORITY;
  
    for ( res_id = 0; res_id < uRes; res_id++ )
    {
      if ( resources[res_id].status == ACTIVE )
      { 
        if ( resources[res_id].uxPriorityCeiling > priority &&
             resources[res_id].pcHoldingTask != NULL
           )
        {
          priority  = resources[res_id].uxPriorityCeiling;
          uReturn = res_id;
        }
      }
    }  
    return uReturn;
}

unsigned portBASE_TYPE vGetActResMaxPrioCeilExceptTask( xTaskHandle * const pxPcpHolder )
{
    unsigned portBASE_TYPE res_idx  = 0;
    unsigned portBASE_TYPE uReturn = tskIDLE_PRIORITY;  
  
    for ( res_idx = 0; res_idx < uRes; res_idx++ )
    {
      if ( resources[res_idx].status            == ACTIVE &&
           resources[res_idx].pcHoldingTask     != NULL &&
           resources[res_idx].pcHoldingTask     != (signed char *) pxPcpHolder &&
           resources[res_idx].uxPriorityCeiling > uReturn )
      {     
          uReturn = resources[res_idx].uxPriorityCeiling;        
      }
    }  
    return uReturn;
}

void vUpdateCurrentPriorityCeiling( void )
{
    taskENTER_CRITICAL();
      uxCurrentPriorityCeiling = vGetActiveResMaxPriorityCeiling();
    taskEXIT_CRITICAL();    
}

// PCP Gate Create
xQueueHandle xQueuePcpCreate( unsigned char ucQueueType, unsigned portBASE_TYPE uxCeiling )
{
xQUEUE *pxNewQueue;
xQueueHandle xReturn = NULL;

	/* Remove compiler warnings about unused parameters should
	configUSE_TRACE_FACILITY not be set to 1. */
	( void ) ucQueueType;

	/* Allocate the new queue structure. */

  pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );
  if( pxNewQueue != NULL )
  {
    pxNewQueue->pxPcpHolder       = NULL;
    pxNewQueue->uxQueueType       = queueQUEUE_IS_PCP;
    pxNewQueue->pcWriteTo         = NULL;
    pxNewQueue->pcReadFrom        = NULL;
    pxNewQueue->uxPriorityCeiling = uxCeiling;
    pxNewQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
    pxNewQueue->uxLength          = ( unsigned portBASE_TYPE ) 1U;
    pxNewQueue->uxItemSize        = ( unsigned portBASE_TYPE ) 0U;
    pxNewQueue->xRxLock           = queueUNLOCKED;
    pxNewQueue->xTxLock           = queueUNLOCKED;
    pxNewQueue->uxResId           = uRes;
    
    #if ( configUSE_TRACE_FACILITY == 1 )
    {
      pxNewQueue->ucQueueType = ucQueueType;
    }
    #endif /* configUSE_TRACE_FACILITY */

    /* the blocked list for usual semaphore - itnitialize just for backporting */
    vListInitialise( &( pxNewQueue->xTasksWaitingToSend ) );
    vListInitialise( &( pxNewQueue->xTasksWaitingToReceive ) );
    /* the PCP blocked list for this semaphore */
    vListInitialise( &( pxNewQueue->xTasksWaiting ) );
    
    vSetResource(pxNewQueue->uxResId, 
                 FREE, 
                 pxNewQueue->pxPcpHolder,  
                 pxNewQueue->uxPriorityCeiling, 
                 tskIDLE_PRIORITY,
                 pxNewQueue
                );
    vIncrResIdx();
    
    traceCREATE_PCP( pxNewQueue );
    xReturn = pxNewQueue;
  }
  else
  {
    traceCREATE_PCP_FAILED();
    vPortFree( pxNewQueue );
  }
		
	configASSERT( xReturn );

	return xReturn;
}

unsigned portBASE_TYPE xRefreshHighestBlockedTaskPriority( const xList * const pxEventList, unsigned portBASE_TYPE highestBlockedTaskPriority)
{
  unsigned portBASE_TYPE blockedTaskPriority, xReturn;
            
  blockedTaskPriority = xTaskHeadListPriority( pxEventList );
  xReturn = highestBlockedTaskPriority;
  
  /* Since there are still tasks currently blocked on this semaphore, 
     update the highest priority (of the tasks blocked on this semaphore)
  */
  if ( blockedTaskPriority < xReturn)
  {
    xReturn = blockedTaskPriority;
  }
  return xReturn;
}

unsigned portBASE_TYPE vTestDoneWithAllResources ( void )
{
  xTaskHandle            xThisTask = xTaskGetCurrentTaskHandle();
  unsigned portBASE_TYPE xReturn   = pdTRUE;
  unsigned portBASE_TYPE res_idx;
  
  for ( res_idx = 0; res_idx < uRes; res_idx++ )
  {
    if ( resources[res_idx].status        == ACTIVE &&
         resources[res_idx].pcHoldingTask == (signed char *) xThisTask 
       )
    {
      xReturn  = pdFALSE;
    }
   
  }
  
  return xReturn;
}

xQueueHandle xGetHighPriorityBlockedTaskQueue( void )
{
  xTaskHandle             xThisTask = xTaskGetCurrentTaskHandle();
  xQueueHandle            xReturn   = NULL; 
  unsigned portBASE_TYPE priority   = tskIDLE_PRIORITY;
  unsigned portBASE_TYPE res_idx;
  
  for ( res_idx = 0; res_idx < uRes; res_idx++ )
  {
    if ( resources[res_idx].status               == ACTIVE &&
         resources[res_idx].pcHoldingTask        != (signed char *) xThisTask &&
         resources[res_idx].uxPriorityMaxBlocked > priority 
       )
    {
      priority = resources[res_idx].uxPriorityMaxBlocked;
      xReturn  = resources[res_idx].xQueueRes;
    }
   
  }
  
  return xReturn;
}  

xQueueHandle vIndirectBlockedTask ( void )
{
  xQueueHandle xReturn = NULL;
  xQueueHandle pxQueue = NULL;
  if ( vTestDoneWithAllResources() == pdTRUE )
  {
    pxQueue = xGetHighPriorityBlockedTaskQueue();
    /* Check if there are any task blocked on this semaphore */
    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaiting ) ) == pdFALSE )
    {  
      xReturn = pxQueue;
    }
  }
  return xReturn;
}

// PCP Gate Lock
signed portBASE_TYPE xQueuePcpReceive( xQueueHandle pxQueue, portTickType xTicksToWait)
{
signed portBASE_TYPE xEntryTimeSet = pdFALSE;
  xTimeOutType xTimeOut;
  unsigned portBASE_TYPE uHighestBlockedTaskPriority = tskIDLE_PRIORITY;
  unsigned portBASE_TYPE uBlockedTaskPriority        = tskIDLE_PRIORITY;
  xTaskHandle            xThisTask                   = xTaskGetCurrentTaskHandle();
  unsigned portBASE_TYPE uThisTaskPriority           = uxTaskPriorityGet(xThisTask);
  unsigned portBASE_TYPE unlock                      = 0;
  unsigned portBASE_TYPE isFree                      = 0;
//  xQueueHandle           xBlockingSemaphore          = NULL;

	configASSERT( pxQueue );

	/* This function relaxes the coding standard somewhat to allow return
	statements within the function itself.  This is done in the interest
	of execution time efficiency. */

	for( ;; )
	{
		taskENTER_CRITICAL();
		{
      /* Is the resource free? */
      isFree = ( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 );

      /* 1st Allocation Rule:
         (If Resource is FREE)
         If the task's current priority is greater than the 
         current priority ceiling
         then allocate resource to the task
     */
      if ( isFree )
      {
        unlock = ( uThisTaskPriority > resources[uxCurrentPriorityCeiling].uxPriorityCeiling );
      }

      /* 2nd Allocation Rule:
        (If Resource is FREE, but Allocation Rule 1 failed)
         If the task's current priority is equal to the 
         resource's priority ceiling and
         the task is the resource holder
         then allocate resource to the task
      */
      if ( isFree && !unlock )
      {
        unlock = ( ( uThisTaskPriority == resources[pxQueue->uxResId].uxPriorityCeiling ) &&
                   ( xThisTask         == resources[pxQueue->uxResId].pcHoldingTask )
                 );
      }
      /* Addition to Rule 1:
         The task priority > ceilings of all semaphores currently locked by *other* tasks 
      */
      
      if ( isFree && !unlock )
      {
        
        unlock = ( uThisTaskPriority > vGetActResMaxPrioCeilExceptTask(xThisTask) );
      }
            
			/* Semaphore is unlocked? Allocation conditions met?*/
			if( isFree && unlock )
			{

					traceQUEUE_RECEIVE( pxQueue );

					/* Marking semaphore as locked */
					--( pxQueue->uxMessagesWaiting );

          /* Record the information required to implement
          priority inheritance should it become necessary. */
          pxQueue->pxPcpHolder = xTaskGetCurrentTaskHandle();

          /* Check if there are any task blocked on this semaphore */
          if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaiting ) ) == pdFALSE )
					{            
            uBlockedTaskPriority        = xTaskHeadListPriority( &( pxQueue->xTasksWaiting ) );
            uHighestBlockedTaskPriority = resources[ pxQueue->uxResId ].uxPriorityMaxBlocked;
            
            /* Since there are still tasks currently blocked on this semaphore, 
               update the highest priority (of the tasks blocked on this semaphore)
            */
            if ( uBlockedTaskPriority < uHighestBlockedTaskPriority)
            {
              uHighestBlockedTaskPriority = uBlockedTaskPriority;
            }
            
//            highestBlockedTaskPriority = xRefreshHighestBlockedTaskPriority( &( pxQueue->xTasksWaiting ), 
//                                                                      resources[ pxQueue->uxResId ].uxPriorityMaxBlocked);
            vSetResource( pxQueue->uxResId, 
                          ACTIVE, 
                          pxQueue->pxPcpHolder, 
                          pxQueue->uxPriorityCeiling, 
                          uHighestBlockedTaskPriority,
                          pxQueue
                        ); 
            
					}
          else /* There are no tasks currently blocked on this resource. */
          {
            vSetResource( pxQueue->uxResId, 
                          ACTIVE, 
                          pxQueue->pxPcpHolder, 
                          pxQueue->uxPriorityCeiling, 
                          tskIDLE_PRIORITY,
                          pxQueue
                        );            
          }
        
				taskEXIT_CRITICAL();
				return pdPASS;
			}
			else
			{
				if( xTicksToWait == ( portTickType ) 0 )
				{
					/* The queue was empty and no block time is specified (or
					the block time has expired) so leave now. */
					taskEXIT_CRITICAL();
					traceQUEUE_RECEIVE_FAILED( pxQueue );
					return errQUEUE_EMPTY;
				}
				else if( xEntryTimeSet == pdFALSE )
				{
					/* The queue was empty and a block time was specified so
					configure the timeout structure. */
					vTaskSetTimeOutState( &xTimeOut );
					xEntryTimeSet = pdTRUE;
                    
				} // if( xEntryTimeSet == pdFALSE )
			} // else ( isFree && unlock )
		} // taskENTER_CRITICAL
		taskEXIT_CRITICAL();

		/* Interrupts and other tasks can send to and receive from the queue
		now the critical section has been exited. */

		vTaskSuspendAll();
		prvLockQueue( pxQueue );

		/* Update the timeout state to see if it has expired yet. */
		if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
		{
				traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );

        portENTER_CRITICAL();
        {
          
          /* Can become blocked on semaphore before any task holds it.
             If there is another task that needs to run since it is
             blocking a higher priority task (>= this task's priority), 
             then block on this resource and come back and finish later
             when all higher priority tasks are done
          */
          if ( isFree )
          {
              /* Check if the task has a higher priority than the current
               max priority of blocked tasks on the blocking PCP semaphore
              */
              if ( uThisTaskPriority > resources[uxCurrentPriorityCeiling].uxPriorityMaxBlocked)
              {
                /* Update the resource list */
                vSetResource( uxCurrentPriorityCeiling, 
                              ACTIVE, 
                              resources[uxCurrentPriorityCeiling].pcHoldingTask, 
                              resources[uxCurrentPriorityCeiling].uxPriorityCeiling, 
                              uThisTaskPriority,
                              resources[uxCurrentPriorityCeiling].xQueueRes
                            );
              }
            
               if ( uxCurrentPriorityCeiling != uRes &&
                     resources[uxCurrentPriorityCeiling].pcHoldingTask != NULL &&
                     resources[uxCurrentPriorityCeiling].pcHoldingTask != xThisTask
                     )
               {
                vTaskPcpPriorityInherit( ( void * ) resources[uxCurrentPriorityCeiling].pcHoldingTask );
               }
              /* Adding current task on the waiting list for the blocking semaphore */
              vTaskPlaceOnEventList( &( resources[uxCurrentPriorityCeiling].xQueueRes->xTasksWaiting ), xTicksToWait );
            
            }
            else
            {
            /* Check if the task has a higher priority than the current
               max priority of blocked tasks on this PCP semaphore
            */
            if ( uThisTaskPriority > resources[pxQueue->uxResId].uxPriorityMaxBlocked)
            {
              /* Update the resource list */
              vSetResource( pxQueue->uxResId, 
                            ACTIVE, 
                            pxQueue->pxPcpHolder, 
                            pxQueue->uxPriorityCeiling, 
                            uThisTaskPriority,
                            pxQueue
                          );
            }
            
            if ( pxQueue->uxResId != uRes &&
             resources[pxQueue->uxResId].pcHoldingTask != NULL &&
             resources[pxQueue->uxResId].pcHoldingTask != xThisTask
             )
             {
                vTaskPcpPriorityInherit( ( void * ) resources[pxQueue->uxResId].pcHoldingTask );
             }
            
            
            /* Adding current task on the waiting list for this semaphore */
				    vTaskPlaceOnEventList( &( pxQueue->xTasksWaiting ), xTicksToWait );
          }
          
        }
        portEXIT_CRITICAL();

				prvUnlockQueue( pxQueue );
				if( xTaskResumeAll() == pdFALSE )
				{
					portYIELD_WITHIN_API();
				}

		}
		else
		{
      /* Timer expired waiting ... should never get here */
			prvUnlockQueue( pxQueue );
			( void ) xTaskResumeAll();
			traceQUEUE_RECEIVE_FAILED( pxQueue );
			return errQUEUE_EMPTY;
		}
	}
}

// PCP Gate Unlock
signed portBASE_TYPE xQueuePcpSend( xQueueHandle pxQueue )
{
  unsigned portBASE_TYPE highestBlockedTaskPriority = 0;
  unsigned portBASE_TYPE blockedTaskPriority = 0;
  
	configASSERT( pxQueue );

	/* This function relaxes the coding standard somewhat to allow return
	statements within the function itself.  This is done in the interest
	of execution time efficiency. */
	for( ;; )
	{
		taskENTER_CRITICAL();
		{
			/* Is there room on the queue now?  To be running we must be
			the highest priority task wanting to access the queue. */
			if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
			{
				traceQUEUE_SEND( pxQueue );

        /* The PCP semaphore is no longer being held. */ 

        ++( pxQueue->uxMessagesWaiting );

				/* If there was a task waiting for data to arrive on the queue then unblock it now. */
				if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaiting ) ) == pdFALSE )
				{           
            blockedTaskPriority        = xTaskHeadListPriority( &( pxQueue->xTasksWaiting ) );
            highestBlockedTaskPriority = resources[ pxQueue->uxResId ].uxPriorityMaxBlocked;
            
            /* Since there are still tasks currently blocked on this semaphore, 
               update the highest priority (of the tasks blocked on this semaphore)
            */
            if ( blockedTaskPriority < highestBlockedTaskPriority)
            {
              highestBlockedTaskPriority = blockedTaskPriority;
            }
            
//            highestBlockedTaskPriority = xRefreshHighestBlockedTaskPriority( &( pxQueue->xTasksWaiting ), 
//                                                                      resources[ pxQueue->uxResId ].uxPriorityMaxBlocked);

            vSetResource( pxQueue->uxResId, 
                          FREE, 
                          NULL, 
                          pxQueue->uxPriorityCeiling, 
                          highestBlockedTaskPriority,
                          pxQueue
                        );           
            
            /* Disinherit any priorities on the current task if applicable*/
            vTaskPcpPriorityDisinherit( ( void * ) pxQueue->pxPcpHolder );
 				    pxQueue->pxPcpHolder = NULL;           
            
            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaiting ) ) == pdTRUE )
            {
              
              /* The unblocked task has a priority higher than
              our own so yield immediately.  Yes it is ok to do
              this from within the critical section - the kernel
              takes care of that. */
              portYIELD_WITHIN_API();
					}
				}
        else
        {
          
          // There is no one waiting for this resource, so reset resource
          /* Update the resource list */
          vSetResource( pxQueue->uxResId, 
                        FREE, 
                        NULL, 
                        pxQueue->uxPriorityCeiling, 
                        tskIDLE_PRIORITY,
                        pxQueue
                      );
          /* Disinherit any priorities on the current task if applicable
		     If there is no one blocked on this resource, we can keep the 
			 priority we have had as result of other blocks.
			 If no blocks, then we should be running at our normal priority.
		  */
         // vTaskPcpPriorityDisinherit( ( void * ) pxQueue->pxPcpHolder );
        }

				taskEXIT_CRITICAL();

				/* Return to the original privilege level before exiting the
				function. */
				return pdPASS;
			}
		}
    /* The queue was full so leave now. */
    taskEXIT_CRITICAL();
    
		/* Interrupts and other tasks can send to and receive from the queue
		now the critical section has been exited. */
    
    /* Return to the original privilege level before exiting
    the function. */
    traceQUEUE_SEND_FAILED( pxQueue );
    return errQUEUE_FULL;	

	}
}

#endif /* configUSE_PCP */
/*-----------------------------------------------------------*/

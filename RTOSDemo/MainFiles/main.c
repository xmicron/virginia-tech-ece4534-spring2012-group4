
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "system_LPC17xx.h"

#include "BlockQ.h"
#include "integer.h"
#include "blocktim.h"
#include "flash.h"
#include "semtest.h"
#include "PollQ.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "recmutex.h"

#include "partest.h"

// Include file for MTJ's LCD & i2cTemp tasks
#include "vtUtilities.h"
#include "messagequeues.h"
//#include "lcdTask.h"
//#include "i2cTemp.h"
//#include "vtI2C.h"
//#include "MainThread.h"

/* syscalls initialization -- *must* occur first */
#include "syscalls.h"
#include "extUSB.h"
#include <stdio.h>
/*-----------------------------------------------------------*/

#ifndef   PCONP_PCTIM0
/* MTJ_NOTE: This will not compile properly if you do not delete the old version of */
/*       system_LPC17xx.h from the Keil compiler installation */
You should read the note above.
#endif


/* The time between cycles of the 'check' functionality (defined within the
tick hook). */
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY)
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY)
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY)
#define mainUIP_TASK_PRIORITY				( tskIDLE_PRIORITY)
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY)
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY)
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY)
#define mainLCD_TASK_PRIORITY				( tskIDLE_PRIORITY)
#define mainI2CTEMP_TASK_PRIORITY			( 1)
#define mainUSB_TASK_PRIORITY				( tskIDLE_PRIORITY)
#define mainI2CMONITOR_TASK_PRIORITY		( tskIDLE_PRIORITY)

/* The WEB server has a larger stack as it utilises stack hungry string
handling library calls. */
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 4 )

/* The message displayed by the WEB server when all tasks are executing
without an error being reported. */
#define mainPASS_STATUS_MESSAGE				"All tasks are executing without error."




static void prvSetupHardware( void );	//Configure the hardware for the demo.

extern void vuIP_Task( void *pvParameters );//The task that handles the uIP stack.  All TCP/IP processing is performed in this task.

extern void vUSBTask( void *pvParameters );//The task that handles the USB stack.

char *pcGetTaskStatusMessage( void );//Simply returns the current status message for display on served WEB pages.

static char *pcStatusMessage = mainPASS_STATUS_MESSAGE;//Holds the status message displayed by the WEB server.

static vtI2CStruct vtI2C0;
static i2cParamStruct I2Cparams;//struct with pointer to i2c dev, i2c queue, and lcdqueue
static MasterParamStruct MasterParams;

static vtLCDMsgQueue myLCDQueue;
static I2CMsgQueue myI2CQueue;
static MasterMsgQueue myMasterQueue;

int main( void )
{ 
	init_syscalls();
	vtInitLED();// Set up the LED ports and turn them off
	prvSetupHardware();//setup the hardware

	//Start the tasks
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
    vCreateBlockTimeTasks();
    vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
    vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
    vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
    vStartQueuePeekTasks();
    vStartRecursiveMutexTasks();
	//vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
	
	//Web Server Task
    xTaskCreate( vuIP_Task, ( signed char * ) "uIP", mainBASIC_WEB_STACK_SIZE, ( void * ) NULL, mainUIP_TASK_PRIORITY, NULL );
	
	vStartLCDTask( mainLCD_TASK_PRIORITY,&myLCDQueue);//LCD Task

	// MTJ: My i2cTemp demonstration task
	vtI2C0.devNum = 0;
	vtI2C0.taskPriority = mainI2CMONITOR_TASK_PRIORITY;
	// Initialize I2C0 
	int retVal;
	if ((retVal = vtI2CInit(&vtI2C0,100000)) != vtI2CInitSuccess) {
		VT_HANDLE_FATAL_ERROR(retVal);
	}
	I2Cparams.i2cDev = &vtI2C0;
	I2Cparams.lcdQ = &myLCDQueue;
	I2Cparams.i2cQ = &myI2CQueue;
	MasterParams.masterQ = &myMasterQueue;
	MasterParams.lcdQ = &myLCDQueue;
	MasterParams.i2cQ = &myI2CQueue;
	vStartI2CTask(mainI2CTEMP_TASK_PRIORITY, &I2Cparams);
	vStartMainThread(tskIDLE_PRIORITY, &MasterParams);
	vStartJoystickTask(tskIDLE_PRIORITY, &myMasterQueue);

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
static unsigned long ulTicksSinceLastDisplay = 0;

	/* Called from every tick interrupt as described in the comments at the top
	of this file.

	Have enough ticks passed to make it	time to perform our health status
	check again? */
	ulTicksSinceLastDisplay++;
	if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
	{
		/* Reset the counter so these checks run again in mainCHECK_DELAY
		ticks time. */
		ulTicksSinceLastDisplay = 0;

//#if USE_FREERTOS_DEMO == 1
		/* Has an error been found in any task? */
		if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "An error has been detected in the Generic Queue test/demo.";
		}
		else if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "An error has been detected in the Peek Queue test/demo.";
		}
		else if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			pcStatusMessage = "An error has been detected in the Block Queue test/demo.";
		}
		else if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "An error has been detected in the Block Time test/demo.";
		}
	    else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	    {
	        pcStatusMessage = "An error has been detected in the Semaphore test/demo.";
	    }
	    else if( xArePollingQueuesStillRunning() != pdTRUE )
	    {
	        pcStatusMessage = "An error has been detected in the Poll Queue test/demo.";
	    }
	    else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	    {
	        pcStatusMessage = "An error has been detected in the Int Math test/demo.";
	    }
	    else if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	    {
	    	pcStatusMessage = "An error has been detected in the Mutex test/demo.";
	    }
//#endif
	}
}
/*-----------------------------------------------------------*/

char *pcGetTaskStatusMessage( void )
{
	/* Not bothered about a critical section here. */
	return pcStatusMessage;
}
/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
	/* Disable peripherals power. */
	SC->PCONP = 0;

	/* Enable GPIO power. */
	SC->PCONP = PCONP_PCGPIO;

	/* Disable TPIU. */
	PINCON->PINSEL10 = 0;

	/*  Setup the peripheral bus to be the same as the PLL output (64 MHz). */
	SC->PCLKSEL0 = 0x05555555;

	/* Configure the LEDs. */
	vParTestInitialise();
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will get called if a task overflows its stack. */
	
	( void ) pxTask;
	( void ) pcTaskName;

	// MTJ: I have directed this to the fatal error handler
	VT_HANDLE_FATAL_ERROR(0);
	for( ;; );
}
/*-----------------------------------------------------------*/

void vConfigureTimerForRunTimeStats( void )
{
const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */

	/* Power up and feed the timer. */
	SC->PCONP |= 0x02UL;
	SC->PCLKSEL0 = (SC->PCLKSEL0 & (~(0x3<<2))) | (0x01 << 2);

	/* Reset Timer 0 */
	TIM0->TCR = TCR_COUNT_RESET;

	/* Just count up. */
	TIM0->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	TIM0->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

	/* Start the counter. */
	TIM0->TCR = TCR_COUNT_ENABLE;
}
/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
	// Here we decide to go to sleep because we *know* that no other higher priority task is ready *and* we
	//   know that we are the lowest priority task (we are the idle task)
	// Important: We are just being *called* from the idle task, so we cannot run a loop or anything like that
	//   here.  We just go to sleep and then return (which presumably only happens when we wake up).
	vtITMu8(vtITMPortIdle,SCB->SCR);
	__WFI(); // go to sleep until an interrupt occurs
	// DO NOT DO THIS... It is not compatible with the debugger: __WFE(); // go into low power until some (not quite sure what...) event occurs
	vtITMu8(vtITMPortIdle,SCB->SCR+0x10);
}

#include <Arduino_FreeRTOS.h>

#define configUSE_TIME_SLICING 1
#define messageQueueSize 3

void setup() {
  /* remember to run users setup function! */
}

void loop(){}
// End of helper file
int runoff_createTask(TaskFunction_t taskFunction, char *taskName, int enumIdentifier, void *args){
  BaseType_t xReturned;

   /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
    taskFunction,       /* Function that implements the task. */
    taskName,          /* Text name for the task. */
    configMINIMAL_STACK_SIZE,              /* 196 * 8 * 2 = bits allocated to this task */
    args,            /* Parameter passed into the task. */
    10,             /* Priority at which the task is created. */
    NULL );        /* Used to pass out the created task's handle. */

   if( xReturned != pdPASS ){
    return NULL;
   }
   return taskId;
}

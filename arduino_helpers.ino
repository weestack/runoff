#include <Arduino_FreeRTOS.h>

#define configUSE_TIME_SLICING 1
#define messageQueueSize 3

void setup() {  
  /* remember to run users setup function! */
}

void loop(){}
// End of helper file

char *createTask(TaskFunction_t taskFunction, char *taskName, configSTACK_DEPTH_TYPE stackSize, void *args,){
  BaseType_t xReturned;
  TaskHandle_t xHandle = NULL;

   /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
    taskFunction,       /* Function that implements the task. */
    taskName,          /* Text name for the task. */
    stackSize,      /* Stack size in words, not bytes. */
    args,    /* Parameter passed into the task. */
    10,/* Priority at which the task is created. */
    &xHandle );      /* Used to pass out the created task's handle. */

   if( xReturned != pdPASS ){
    return null/* The task was created.  Use the task's handle to delete the task.  */
   }
   return xHandle;
}

#include <Arduino_FreeRTOS.h>

#define configUSE_TIME_SLICING 1
#define messageQueueSize 3

void loop(){}
// End of helper file
void runoff_createTask(TaskFunction_t taskFunction, void *args){
  BaseType_t xReturned;
  
  /* Create the task, storing the handle. */
  xReturned = xTaskCreate(
  taskFunction,                            /* Function that implements the task.   *\           */
  "",                                     /* Text name for the task.                *\         */
  configMINIMAL_STACK_SIZE,              /* 196 * 8 * 2 = bits allocated to this task*\       */
  args,                                 /* Parameter passed into the task.            *\     */
  10,                                  /* Priority at which the task is created.       *\   */
  NULL );                             /* Used to pass out the created task's handle.    *\ */

  if( xReturned != pdPASS ){
    return;
  }
}

void turnOn(int pin){
  digitalWrite(pin, HIGH);
}

void turnOff(int pin){
  digitalWrite(pin, LOW);
}

bool readInput(int pin){
  return digitalRead(pin);
}

void outputPin(int pin){
  pinMode(pin, OUTPUT);
}

void simpleInputPin(int pin){
  pinMode(pin, INPUT);
}
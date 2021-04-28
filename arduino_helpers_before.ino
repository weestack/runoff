#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdint.h>

#define messageQueueSize 3
#define runoff_uint8	uint8_t
#define runoff_uint16	uint16_t
#define runoff_uint32	uint32_t
#define runoff_uint64	uint64_t
#define runoff_int8	int8_t
#define runoff_int16	int16_t
#define runoff_int32	int32_t
#define runoff_int64	int64_t
#define runoff_float	float
#define runoff_void	void
#define runoff_bool	bool
#define runoff_msg	struct Message
#define runoff_taskid	int8_t
#define runoff_pinid	int8_t


void loop(){}
// Helper function to create tasks
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

// Helper functions
runoff_void runoff_turnOn(runoff_pinid pin){
  digitalWrite(pin, HIGH);
}

runoff_void runoff_turnOff(runoff_pinid pin){
  digitalWrite(pin, LOW);
}

runoff_bool runoff_readInput(runoff_pinid pin){
  return digitalRead(pin);
}

runoff_uint16 runoff_readAnalogInput(runoff_pinid pin){
	return analogRead(pin);
}

runoff_void runoff_analogWrite(runoff_pinid pin, runoff_uint8 val){
	analogWrite(pin, val);
}

runoff_void runoff_advancedInputPin(runoff_pinid pin, runoff_taskid task, runoff_msg lowmsg, runoff_msg highmsg);

runoff_void runoff_outputPin(runoff_pinid pin){
  pinMode(pin, OUTPUT);
}

runoff_void runoff_inputPin(runoff_pinid pin){
  pinMode(pin, INPUT);
}

runoff_void runoff_setOutput(runoff_pinid pin, runoff_bool b){
	if(b)
		runoff_turnOn(pin);
	else
		runoff_turnOff(pin);
}

runoff_void runoff_delay(runoff_uint32 miliSeconds){
  vTaskDelay(pdMS_TO_TICKS(miliSeconds));
}
// End of helper file
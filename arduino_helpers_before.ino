#include <Arduino_FreeRTOS.h>
#include <queue.h>

#define messageQueueSize 3
#define runoff_uint8	byte
#define runoff_uint16	unsigned int
#define runoff_uint32	unsigned long
#define runoff_uint64	uint64_t
#define runoff_int8	char
#define runoff_int16	int
#define runoff_int32	long
#define runoff_int64	int64_t
#define runoff_float	float
#define runoff_void	void
#define runoff_bool	bool
#define runoff_msg	struct Message
#define runoff_taskid	char
#define runoff_pinid	int


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
runoff_void turnOn(runoff_pinid pin){
  digitalWrite(pin, HIGH);
}

runoff_void turnOff(runoff_pinid pin){
  digitalWrite(pin, LOW);
}

runoff_bool readInput(runoff_pinid pin){
  return digitalRead(pin);
}

runoff_uint16 readAnalogInput(runoff_pinid pin){
	return 1234; /* TODO */
}

runoff_void analogWrite(runoff_pinid pin, runoff_uint8){
	/* TODO */
}

runoff_void inputPin(runoff_pinid pin, runoff_taskid task, runoff_msg lowmsg, runoff_msg highmsg);

runoff_void outputPin(runoff_pinid pin){
  pinMode(pin, OUTPUT);
}

runoff_void simpleInputPin(runoff_pinid pin){
  pinMode(pin, INPUT);
}

runoff_void setOutput(runoff_pinid pin, runoff_bool b){
	if(b)
		turnOn(pin);
	else
		turnOff(pin);
}

runoff_void Delay(runoff_uint32 miliSeconds){
  vTaskDelay(pdMS_TO_TICKS(miliSeconds));
}
// End of helper file
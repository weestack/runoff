#include <Arduino_FreeRTOS.h>

#define configUSE_TIME_SLICING 1

byte *copyUInt8Array(byte *Array, int len){
  byte *result = malloc(sizeof(byte)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

unsigned int *copyUInt16Array(unsigned int *Array, int len){
  unsigned int *result = malloc(sizeof(int)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

unsigned long *copyUInt32Array(unsigned long *Array, int len){
  unsigned long *result = malloc(sizeof(long)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}


uint64_t *copyUInt64Array(uint64_t *Array, int len){
  uint64_t *result = malloc(sizeof(uint64_t)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}


char *copyInt8Array(char *Array, int len){
  char *result = malloc(sizeof(char) * len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

int *copyInt16Array(int *Array, int len){
  int *result = malloc(sizeof(int)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

long *copyInt32Array(long *Array, int len){
  long *result = malloc(sizeof(long)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

int64_t *copyInt64Array(int64_t *Array, int len){
  int64_t *result = malloc(sizeof(int64_t)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}


float *copyFloatArray(float *Array, int len){
  float *result = malloc(sizeof(float)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}

bool *copyBoolArray(bool *Array, int len){
  bool *result = malloc(sizeof(bool)*len);
  for(int i = 0; i < len; i++){
    result[i] = Array[i];
  }
  return result;
}


// End of helper file

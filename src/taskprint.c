#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "systick.h"
#include "sapi.h"
#include "task1.h"
#include "mutex.h"
#include "taskprint.h"

uint32_t taskPrintPool [REASONABLE_STACK]; //aca defino el stack para la tarea
//este pool es parra guarda los mensajes a imprimir en un buffer circular administrado por la
//queue, uso los servicios de la sapi, el cbPoolSpace es prque le suma 1 al nunmero maximo de
//elementos a imprimir, por como esta diseniada la cola circular en la sapi
int8_t   printCbPool   [cbPoolSpace(MAX_PRINT_MSG,MAX_MSG_LENGTH)];
circularBuffer_t printCb; // este es el puntero a la cola circular que se usara para administrara
queue_t printQueue;       // y esta es la cola
mutex_t printMutex;       // si otra tarea quiere usar los servicios de bajo nivel para imprimir sin pasar por la cola puede usar este mutex
//esta func, corre ANTES de que se lance el scheduler, para dejar listo cosas que podrian ser
//usadas por otras tareas, de modo que al primer tido de schedule todo este inicializado
void taskPrintBegin(void)
{
   queueInit ( &printQueue,&printCb,printCbPool,MAX_PRINT_MSG,MAX_MSG_LENGTH );
   mutexInit ( &printMutex);
}

taskParams_t taskPrintParams = {
   .name     = "taskPrint",
   .pool     = taskPrintPool,
   .poolSize = sizeof(taskPrintPool)/sizeof(taskPrintPool[0]),
   .param    = NULL,
   .func     = taskPrint,
   .hook     = defaultHook,
   .begin    = taskPrintBegin,
   .end      = rien,
};

void* taskPrint(void* a)
{
   uint8_t data[MAX_MSG_LENGTH];
   while(1) {
      //solo para jugar, pruebo el modo de queue con tout y si no viene nada en cierto tiempo
      //puedeo poner algo en la consola para mostrar que esta viva, pero no tiene otra
      //utilidad0
      while(queueReadTout ( &printQueue ,data, msec2Ticks(10000))==false)
         uartWriteString ( UART_USB    ,"."  );
      gpioWrite          ( LED2        ,true  ); //debug
      //no se necesita si no hay ninguna otra tarea que vaya a usar el recurso, pero si lo
      //hubiera, me protejo con este mutex, otra tarea que quiera usar las funciones de bajo
      //nivel de impresion sin pasar por aca puede usar este mutex para no solaparse
      mutexLock          ( &printMutex        );
         uartWriteString ( UART_USB    ,data  );
      mutexUnlock        ( &printMutex        );
      gpioWrite          ( LED2        ,false ); //debug
   }
   return NULL;
}
//hago una funcion que se parece al printf, pero manda los datos a la cola de impresion, genera
//un buffer local y demas y lo pasa por copia, es cierto que primero parsea lo que se pide y lo
//copia en un arreglo y luego ese arreglo es copiado al buffer circular, con lo cual son 2
//copias para mandar el dato y otra copia cuando se lee la cola de recepcion. Se podria pasar
//un puntero y hacerlo viajar, pero se destaca el concepto de pasar por copia que en muchos
//casos tiene sentido
int printUART(const char *format, ...)
{
  va_list params;
  uint8_t buf[MAX_MSG_LENGTH];

  va_start   ( params, format );
  vsnprintf  ( buf,sizeof(buf ),format,params); // copia y frmatea sin pasarse del tamanio del buffer
  queueWrite ( &printQueue,buf);                // manda a la cola de impresion
}


#ifndef QUEUE
#define QUEUE

#include <stdint.h>
#include <stdbool.h>
#include "sapi.h"
#include "mutex.h"

//calcula el espacio necesario apra qty itemes de lnegth bytes cada uno para armar un pool para
//un buffer circular. Notar el +2, ya que si queres un solo item, necesitas tener 2 lugares
//porque se escribe primero y luego se toma el semphr, pero ademas hay otro espacio mas que hay
//que reservar por la arquitectura del buffer circular, con lo cual
//para mandar 1 tenes que reservar espacio para 3. es asi.. Se supone igual que no haces una
//cola para madar 1 solo mensaje.. pero bueno, es una limitante
#define cbPoolSpace(qty,length)  ((qty+2)*length)

typedef struct    queue_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   event_t            s ;        //es el evento para leer y escribir la cola.
   circularBuffer_t  *cb;        //es el puntero a un tipo colacircular de la sapi
   mutex_t           mutex;      //uso un mutex para protejer el acceso al buffer circualr de la sapi porque sino podria darse concurrencia
} queue_t;

bool queueInit ( queue_t* q ,
                  circularBuffer_t* cb,
                  uint8_t* bufferMemory,       // buffer array of memory
                  uint32_t amountOfElements,   // amount of elements in buffer
                  uint32_t elementSize         // each element size in bytes
               );
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout );
bool queueReadTout  ( queue_t* q, void* data, uint32_t tout );
bool queueWrite     ( queue_t* q, void* data);
bool queueRead      ( queue_t* q, void* data);

#endif

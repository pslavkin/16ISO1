#ifndef EVENTS
#define EVENTS

#include <stdint.h>
#include <stdbool.h>

typedef struct    event_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   uint32_t count;                // lleva cuenta de cuantos gives se hicieron
   uint32_t max;                  // maximo de gives admitidos
   void*  data; //TODO
} event_t;

bool eventInit      ( event_t* s ,uint32_t count                          );
bool eventTake      ( event_t* s ,void** data                             );
bool eventTakeTout  ( event_t* s ,void** data ,uint32_t tout              );
bool eventGive      ( event_t* s ,void* data ,uint32_t qty                );
bool eventGiveTout  ( event_t* s ,void* data ,uint32_t qty ,uint32_t tout );
bool eventGiveBlock ( event_t* s,uint32_t tout                            );
bool eventGive4Isr  ( event_t* s, void* data, uint32_t qty                );

#endif

#ifndef EVENTS
#define EVENTS

#include <stdint.h>
#include <stdbool.h>

bool eventInit     ( event_t* s ,uint32_t count                          );
bool eventTake     ( event_t* s ,void** data                              );
bool eventTakeTout ( event_t* s ,void** data ,uint32_t tout               );
bool eventGive     ( event_t* s ,void* data ,uint32_t qty                );
bool eventGiveTout ( event_t* s ,void* data ,uint32_t qty ,uint32_t tout );

#endif

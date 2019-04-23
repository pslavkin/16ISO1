#ifndef SEMPHR
#define SEMPHR

#include <stdint.h>
#include <stdbool.h>

#define semphr_t event_t

bool semphrInit     ( semphr_t* s ,uint32_t count               );
bool semphrTakeTout ( semphr_t* s, uint32_t tout                );
bool semphrGiveTout ( semphr_t* s , uint32_t qty, uint32_t tout );
bool semphrTake     ( semphr_t* s                               );
bool semphrGive     ( semphr_t* s , uint32_t qty                );

#endif

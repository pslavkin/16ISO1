#ifndef SEMPHR
#define SEMPHR

#include <stdint.h>
#include <stdbool.h>

#define semphr_t event_t

bool semphrInit ( semphr_t* s ,uint32_t count );
bool semphrTake ( semphr_t* s                 );
bool semphrGive ( semphr_t* s ,uint32_t qty   );

#endif

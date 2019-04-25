#ifndef KEY
#define KEY

#define KEYS_QTY 2
typedef struct keys_struct{
   uint8_t     state   [ KEYS_QTY ];
   uint32_t    riseTime[ KEYS_QTY ];
   uint32_t    fallTime[ KEYS_QTY ];
   gpioMap_t   name    [ KEYS_QTY ];
   uint8_t     tec1AndTec2;
} keys_t;

extern taskParams_t taskKeyParams;

void* taskKey ( void* a );

#endif

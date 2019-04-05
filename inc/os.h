#ifndef OS
#define OS

#include <stdint.h>
#include <stdbool.h>
#include "semphr.h"

#define MAX_PRIOR        10 // 10 prioridades, cada una
#define MAX_TASK         10 // con 10 tareas
#define MIN_STACK        300 // minimo stack para cada tarea, pero igualmente cada tarea puede
                            // elgir lo que desee
#define TASK_NAME_LENGTH 16 // en el contexto de control tambien se guarda el nombre de fantasia.

enum taskState{
   READY=0, // esperando su turno.
   WAITING, // en este estado esta cuando se prende un taskDelay o un taskYield
   RUNNING, // tare en curso.. no hace falta, ya que esta tasks.context que sabe quien es la que
            // esta en curso, y siempre hay una sola en cada instante... la dejo solo por convencion, pero
            // no se necesita para nada
   EMPTY,   // se usa solo al inicio para indicar que el casillero esta vacio, pero no se esta
            // usando por ahora..
   BLOCKED, // la estoy usando para los mutex por ejemplo
};


//aca guardo el contecto de la tarea, principalmente su sp. Pero tambien guardo el nombre, el
//pool para poder validar que no pise el stack de otro, el estado, los ticks y cualquier otra
//cosa que surja luego relacionado con la tarea, este seria el lugar. cada tarea tiene una de
//estas y se llena con taskCreate
typedef struct    taskContext_t {
   uint32_t*      sp;                     // DANGER.. este campo tiene que ir PRIMERO, porque lo uso en ASM y no
                                          // quiero indexar para no perder tiempo, asi que el nombre de la variable de este tipo, es
                                          // tambien su sp.. propenso a errores? puede ser, mala programacion? ponele, es peor que un
                                          // goto? quien sabe, se podria hacer con algun define o otra variable suelta, u otra cosa?
                                          // see. pero es rapida, facil y no indexa. my room, my rules
   uint32_t*      pool;                   // puntero al inicio del stack (no al final) por el final arranca sp
   enum taskState state;                  // running, waiting, etc.
   semphr_t*      semphr;                 //TODO comments...
   uint32_t       sleepTicks;             // aca lleva cuent de cuanto le falta para pasar a running
   char           name[TASK_NAME_LENGTH]; // su identidad
   uint8_t        prior;                  // su priodidad
   uint8_t        number;                 // un numero que en realidad es la posicion dentro del arreglo estatico de la prioridad en la que elgio estar
} taskContext;

//en esta estructura guardo todas las tareas y los indices.
typedef struct tasks_struct {
   taskContext    *context;                    // DANGER.. este campo tiene que ir PRIMERO, porque lo uso en ASM y no
                                               // quiero indexar para no perder tiempo, asi que el nombre de la variable de este tipo, es
                                               // tambien su sp.. propenso a errores? puede ser, mala programacion? ponele, es peor que un
                                               // goto? quien sabe, se podria hacer con algun define o otra variable suelta, u otra cosa?
                                               // see. pero es rapida, facil y no indexa. my room, my rules
   uint8_t        index[ MAX_TASK];            // para cada priodidad llevo un index para saber a cual le toca luego
 //  uint8_t        count[ MAX_TASK];            // para cada prioridad llevo un contador para saber cuantas
                                               // estan ocupadas. Se podria evitar comparando el campo state==EMPTY TODO
   taskContext    list [ MAX_PRIOR][MAX_TASK]; // aca estan todas las tareas, en una sabana
} tasks_t;

//en esta estructura tengo los campos de cada tarea, el nombre, el pool, etc. 
typedef struct    taskParams_t {
   char           name[TASK_NAME_LENGTH]; // nombre de fantasia
   uint32_t*      pool;                   // espacio para el stack
   uint32_t       pool_size;              // tamanio del stack; sirve para arrancar desde el final y ver de no salirse
   void*          (*func)(void*);         // funcion que arranca
   void*          param;                  // se le puede pasar un parametro al inicio tambien
   void*          (*hook)(void*);         // si func termina se llama a hook. si hook termina bum!
} taskParams;

extern tasks_t   tasks; // y si... es global.. la uso en switcher, y en taskKernel.. decideo publicarla, aunque lo correcto seria hacer alguna funcion que tome sus campos publicos asi nadie piuede meter mano...
                        // pero eso implicaria tiempo de uC.. la otra seria usar clases de C++ y declarar algunas clases
                        // amigas.. pero escapa al objetivo del curso..Sino se podria meter os, taskKernel en on solo
                        // file y hacerme creer que es OOP... lo dejo global...
/*==================[declaraciones de datos externos]========================*/
bool initTasks     ( void                          );
bool taskCreate    ( taskParams* t, uint32_t prior );
bool taskYield     ( void                          );
void triggerPendSv ( void                          );
bool taskDelay     ( uint32_t t                    );
/*==================[end of file]============================================*/
#endif

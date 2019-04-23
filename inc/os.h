#ifndef OS
#define OS

#include <stdint.h>
#include <stdbool.h>

#define WEAK        __attribute__ ((weak))
#define ALIAS(f)    __attribute__ ((weak, alias (#f)))

#define MAX_PRIOR        10  // 10 prioridades
#define MAX_TASK         10  // 10 tareas x cada prioridad
#define MIN_STACK4FILL    17 // se necesitan minimo 17 datos de 32b para llenar el stack de cada tarea antes de lanzarlas por primera vez. menos de esto y explota. 
#define REASONABLE_STACK 400 // minimo stack para cada tarea, pero igualmente cada tarea puede
#define BIG_STACK        800 // algo decente como para guardar algo interesante...
#define HUGE_STACK       3200// ok, mas vale que sepas lo que haces, son 3k de ram oara 1 tarea.. pero bueno 
#define TASK_NAME_LENGTH 14  // en el contexto de control tambien se guarda el nombre de fantasia.

typedef struct    event_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   uint32_t count;                // lleva cuenta de cuantos gives se hicieron
   uint32_t max;                  // maximo de gives admitidos
} event_t;

enum taskState{
   READY=0,      // esperando su turno.
   WAITING,      // en este estado esta cuando se prende un taskDelay o un taskYield
   RUNNING,      // tare en curso.. no hace falta, ya que esta tasks.context que sabe quien es la que
                 // esta en curso, y siempre hay una sola en cada instante... la dejo solo por convencion, pero
                 // no se necesita para nada
   EMPTY,        // se usa solo al inicio para indicar que el casillero esta vacio, pero no se esta usando por ahora..
   DELETED,      // solo para debug para que me deje rastro de que la tarea fue borrada on the fly y la vea en el stat..
   BLOCKED_TAKE, // distingo entre bloqueado porque hico un take de un evento y no hay nada..
   BLOCKED_GIVE, // distingo entre bloqueado porque quiso enviar un evento y no habia mas lugar.. eso me permite hacer una busqueda iterada mas eficiente
};


//aca guardo el contecto de la tarea, principalmente su sp. Pero tambien guardo el nombre, el
//pool para poder validar que no pise el stack de otro, el estado, los ticks y cualquier otra
//cosa que surja luego relacionado con la tarea, este seria el lugar. cada tarea tiene una de
//estas y se llena con taskCreate
typedef struct    taskContext_struct {
   uint32_t*      sp;                     // DANGER.. este campo tiene que ir PRIMERO, porque lo uso en ASM y no
                                          // quiero indexar para no perder tiempo, asi que el nombre de la variable de este tipo, es
                                          // tambien su sp.. propenso a errores? puede ser, mala programacion? ponele, es peor que un
                                          // goto? quien sabe, se podria hacer con algun define o otra variable suelta, u otra cosa?
                                          // see. pero es rapida, facil y no indexa. my room, my rules
   uint32_t*      pool;                   // puntero al inicio del stack (no al final) por el final arranca sp
   uint32_t       poolSize;               // solo para fines de stat..
   int32_t        waterMark;              // lleva cuenta del momento mas restringido que estuvo el stack, solo apra estadisticas
   uint32_t       runCount;               // para dines estadisticos lleva cuenta  de cuantas veces se le asigno procesador.. aunque no dice cuanto tiempo lo utilizo..
   enum taskState state;                  // running, waiting, etc.
   event_t*       event;                  // lo uso para cuando encurntro una tarea bloqueada en la liberacino de un semaforo, ver que semaforo esta esperando, porque podria haber muchas tareas bloqueadas por diferentes semaforos..
   bool           eventAns;               // indica como fue desbloqueada la tarea, si fue por timeout o de manera correcta se libero el recurso
   bool           eventTout;              // lo uso para indicar que el estado bloqueado tiene un tiempo maximo de espera cargado en sleep.
   uint32_t       sleep;                  // aca lleva cuent de cuanto le falta para pasar a running
   char           name[TASK_NAME_LENGTH]; // su identidad
   uint8_t        prior;                  // su priodidad
   uint8_t        number;                 // un numero que en realidad es la posicion dentro del arreglo estatico de la prioridad en la que elgio estar
} taskContext_t;

//en esta estructura guardo todas las tareas y los indices.
typedef struct tasks_struct {
   taskContext_t  *context;                    // DANGER.. este campo tiene que ir PRIMERO, porque lo uso en ASM y no
                                               // quiero indexar para no perder tiempo, asi que el nombre de la variable de este tipo, es
                                               // tambien su sp.. propenso a errores? puede ser, mala programacion? ponele, es peor que un
                                               // goto? quien sabe, se podria hacer con algun define o otra variable suelta, u otra cosa?
                                               // see. pero es rapida, facil y no indexa. my room, my rules
   uint8_t        index[ MAX_PRIOR];           // para cada priodidad llevo un index para saber a cual le toca luego
   taskContext_t  list [ MAX_PRIOR][MAX_TASK]; // aca estan todas las tareas, en una sabana estatica
} tasks_t;

//en esta estructura tengo los campos de cada tarea, el nombre, el pool, etc. 
typedef struct    taskParams_struct {
   char           name[TASK_NAME_LENGTH]; // nombre de fantasia
   uint32_t*      pool;                   // espacio para el stack
   uint32_t       poolSize;               // tamanio del stack; sirve para arrancar desde el final y ver de no salirse
   void*          (*func)(void*);         // funcion que arranca
   void*          param;                  // se le puede pasar un parametro al inicio tambien
   void*          (*hook)(void*);         // si func termina se llama a hook. si hook termina bum!
   void           (*init)(void);          // funcion de inicializacion. Util para arrancar semaforos, colas, y demas en situaciones en donde se comparten cosas con otras tareas y se desea que ANTES de que arranque el schedul, estas cosas esten disponibles.
} taskParams_t;

//y si... es global.. la uso en switcher, y en taskKernel.. decido publicarla,
//aunque lo correcto seria hacer alguna funcion que tome sus campos publicos
//asi nadie piuede meter mano...  pero eso implicaria tiempo de uC.. la otra
//seria usar clases de C++ y declarar algunas clases amigas..  pero escapa al
//objetivo del curso..Sino se podria meter os, taskKernel en on solo file y
//hacerme creer que es OOP... lo dejo global...
extern tasks_t   tasks;

/*==================[declaraciones de datos externos]========================*/
bool           initTasks     ( void                            );
bool           taskCreate    ( taskParams_t* t, uint32_t prior );
bool           taskYield     ( void                            );
bool           taskBlock     ( void                            );
void           triggerPendSv ( void                            );
bool           taskDelay     ( uint32_t t                      );
void*          defaultHook   ( void*                           );
void*          fakeFun       ( void* p                         );
bool           taskDelete    ( taskContext_t* c                );
taskContext_t* taskFind      ( taskParams_t* t                 );
/*==================[end of file]============================================*/
#endif

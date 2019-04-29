# 16ISO1 Pablo Slavkin - Sistema Operativo Preemptivo para cortex M4
Maestria Sistemas Embebidos UBA - MSE2019


### Installing

seguir los siguientes pasaos para bajar el repo de la sapi y usar el mismo commit con el que se trabajo
Asegurarse de tener la educiaa en el USB y el ultimo script correra el programa.
abrir una terminal a 115200 y disfrutar
```c
cd ~
mkdir examen_pablo_slavkin
cd examen_pablo_slavkin
git clone  https://github.com/epernia/cese-edu-ciaa-template.git . -vv
git checkout 5cfb9cc -b examen_pablo_slavkin
mkdir 16iso1
cd 16iso1
git clone https://github.com/pslavkin/16ISO1.git . -v
git checkout entrega
cp sapi.patch ..
cd ..
git apply sapi.patch -v
make
cd 16iso1
./scripts/flash.sh
```

Tambien se puede lanzar directamente el archivo install.sh que reune lo anterior:
```c
./install.h
```

## Running the tests

Cuando se corre el programa muestra un menu con 5 opciones

```c
Pablo Slavkin - Examen iso1
1 - stats      //muestra un cuadro con estadisticas de las tareas corriendo
2 - ticks      //muestra la cantidad de ticks hasta el momento
3 - uptime     //el uptime del sistema
4 - task3Create   //permite crear y borrar la tarea 3 on real time.
5 - task3Delete   //la tarea 3 hace un pingo pong de eventos con otra y maneja float
? - help          //este menu
ingrese codigo>   //espera el comando de 1 sola tecla sin necesidad de enter.
```

En cualquier momento el ejercicio del examen esta corriendo y cuando se presionen los botones
en la secuencia correcta se imprimira en pantalle lo solicitado.

## Resumen de la API implementada

event.h
//---------------------------------
servicio de envio y recepcion de mensajes con y sin tout, con la posibilidad de enviar un dato
de 32bits al destinatario en 1 o mas unidades y desde dentro d fuera de irq. Es la base para el
resto de las api.
```c
bool eventInit      ( event_t* s ,uint32_t count                           )
bool eventTake      ( event_t* s ,void** data                              )
bool eventTakeTout  ( event_t* s ,void** data ,uint32_t tout               )
bool eventGive      ( event_t* s ,void* data  ,uint32_t qty                )
bool eventGiveTout  ( event_t* s ,void* data  ,uint32_t qty ,uint32_t tout )
bool eventGive4Isr  ( event_t* s ,void* data  ,uint32_t qty                )
bool eventGiveBlock ( event_t* s ,uint32_t tout                            )
```


mutex.h
//---------------------------------
es un evento de una unidad pero que ademas arranca sin tomar
```c
bool mutexInit   ( mutex_t* m )
bool mutexLock   ( mutex_t* m )
bool mutexUnlock ( mutex_t* m )
```

queue.h
//-----------------------------------------------
basado en event.h y circulabuffer de la sapi se implementan colas para envia y reception con y
sin tout. El espacio en ram para almacentar los datos lo elije cada tarea.

```c
bool queueInit ( queue_t* q ,
                  circularBuffer_t* cb,
                  uint8_t* bufferMemory,       // buffer array of memory
                  uint32_t amountOfElements,   // amount of elements in buffer
                  uint32_t elementSize         // each element size in bytes
               )
bool queueWrite     ( queue_t* q, void* data                )
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout )
bool queueRead      ( queue_t* q, void* data                )
bool queueReadTout  ( queue_t* q, void* data, uint32_t tout )
```

semphr.h
//-----------------------------------------------
basado tambien en events, permite implementar semaforos con y sin tout de 1 o mas unidades
```c
bool semphrInit     ( semphr_t* s, uint32_t count               )
bool semphrTake     ( semphr_t* s                               )
bool semphrTakeTout ( semphr_t* s, uint32_t tout                )
bool semphrGive     ( semphr_t* s , uint32_t qty                )
bool semphrGiveTout ( semphr_t* s , uint32_t qty, uint32_t tout )
```

isr.h
//-----------------------------------------------
permite llenar un vector con handlers y el os concentra la decision de llamar a cada funcion
registrada para cada vector
```c
void addIsr(void (*f)(void), uint8_t irqCh, uint8_t prior)
void delIsr(uint8_t irqCh)
```

os.h
//-----------------------------------------------
Funciones varias para la creacion y eliminacino de tareas y funciones principakes del os
```c
bool taskCreate         ( taskParams_t* t, uint32_t prior )
taskContext_t* taskFind ( taskParams_t* t                 )
bool taskdelete4params  ( taskParams_t* p                 )
bool taskDelete         ( taskContext_t* c                )
bool taskDelay          ( uint32_t t                      )
bool taskYield          ( void                            )
```

taskTemplate.h
//-----------------------------------------------
Es simplemente una tarea dummy con funciones basicas para usarse como plantilla para le
creacion de otras tareas
```c
uint32_t taskTemplatePool[REASONABLE_STACK];
event_t taskTemplateEvent;

void taskTemplateBegin(void)
{
   eventInit(&taskTemplateEvent,10);
}
//reuno todos los datos de la tarea para pasarlo a initTask asi no tengo que
//pasar tantos parametros y queda mas ordenado y limpio el codigo dejo fuera la
//prioridad, porque eso lo guardo en el os y ademas es si lo paso como
//paramertro e initTask para que se aprecie mejor al momento de inicializar las
//tareas en main
taskParams_t taskTemplateParams = {
   .name     = "taskTemplate",
   .pool     = taskTemplatePool,
   .poolSize = sizeof(taskTemplatePool)/sizeof(taskTemplatePool[0]),
   .param    = NULL,
   .func     = taskTemplate,
   .hook     = defaultHook,
   .begin    = taskTemplateBegin,
   .end      = rien,
};

//tarea blinky de debug
void* taskTemplate(void* a)
{
   shared_t* s_template;
   while(1) {
      gpioToggle ( LED2 );
      taskDelay(msec2ticks(1000));
   }
   return NULL;
}
```

## Authors

* **Slavkin Pablo** - [pslavkin](https://github.com/pslavkin)


## License

This project is licensed under the free as a beer License 

## Acknowledgments

al te con torta marmolada y dulce de leche de Julieta y spotify


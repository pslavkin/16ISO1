.syntax unified // elije thumb o thumb2
.extern taskList
.extern runningTask
.extern kernelContext
.extern runningContext
.extern taskCount
.extern kernelTask
.text
.global SysTick_Handler
.global switchTask
.thumb_func     // modo thumb,bit 0 en 1 para las direcciones. (antes de la 1ra instruccion)

//SysTick_Handler:
//   push { r4-r11}
//   ldr  r0 ,=runningContext
//   ldr  r1 ,[r0]
//   str  sp ,[r1]
//
//   add  r1 , #4
//   str  r1 ,[r0]
//   ldr  sp ,[r1]                    //el primer item de la estructura es el sp ,asi que lo leo
//   pop {r4-r11}                     //listo, saco los registros no autmaticos
//   bx   lr                          //a rezar que no vuele por el aira..
//


SysTick_Handler:
   push {r4-r11}                    //estos registros no se guardan automaticamente

   ldr  r4 ,=runningContext
   ldr  r5 ,[r4]
   str  sp ,[r5]

   ldr  r0 , =taskList              //direccion del arreglo a lista de tareas
   ldr  r1 , =runningTask           //direccion del indice de la tarea en curso
   ldr  r2 , [r1]                   //ok, este es el indice

   ldr  r3 , [r0 ,r2 ,lsl #2]       //cargo ahora el pntero a la estructura de la nueva tarea en curso
   add  r2 , #1                     //incremento la running task
   str  r2 , [r1]
   str  r3 , [r4]                   //guardo el nuevo contexto
   ldr  sp ,[r3]                    //el primer item de la estructura es el sp, asi que lo leo
   pop {r4-r11}                     //listo, saco los registros no autmaticos
   bx   lr                          //a rezar que no vuele por el aira..


//SysTick_Handler:
//   push {r4-r11}                    //estos registros no se guardan automaticamente
//   ldr  r0 , =taskList              //direccion del arreglo a lista de tareas
//   ldr  r1 , =runningTask           //direccion del indice de la tarea en curso
//   ldr  r2 , [r1]                   //ok, este es el indice
//   ldr  r3 , [r0 ,r2 ,lsl #2]       //leo el puntero a la tarea en curso con esta magia
//   str  sp , [r3]                   //y el primer item de la lista de tareas el justamente el sp, asi que lo guardo asi sin mas
//
//   ldr  r3 ,=taskCount              //cuantas tareas hay en curso?
//   ldr  r3 , [r3]                   //r3 tareas..
//   add  r2 ,1                       //sumemos 1 a la tarea running
//   cmp  r2 ,r3                      //comparo y veo si tengo que resetear
//   bcc  dontResetRunningTask        //reseto? o no?
//
//resetRunningTask:
//      mov  r2 , #0                  //sip
//
//dontResetRunningTask:               //nop
//   str  r2 ,[r1]                    //nueva tarea en curso
//   ldr  r3 , [r0 ,r2 ,lsl #2]       //cargo ahora el pntero a la estructura de la nueva tarea en curso
//   ldr  sp ,[r3]                    //el primer item de la estructura es el sp, asi que lo leo
//   pop {r4-r11}                     //listo, saco los registros no autmaticos
//   bx   lr                          //a rezar que no vuele por el aira..
//

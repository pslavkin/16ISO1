.syntax unified             // elije thumb o thumb2
.extern runningContext
.extern kernelContext
.extern taskKernel
.text
.global PendSV_Handler
.thumb_func                 // modo thumb,bit 0 en 1 para las direcciones. (antes de la 1ra instruccion)


PendSV_Handler:
   push {r4-r11}            // estos registros no se guardan automaticamente

   ldr  r4 ,=runningContext // mi estrategia es guardar SIEMPRE el sp de la tarea interrumpida
   ldr  r5 ,[r4]            // runnungContext es el contexto de la tarea interrumpida
   str  sp ,[r5]            // listo, guardado por donde iba el sp

   ldr  r2 , =kernelContext // cambio a contexto kernel (es ooootro stack porque sino podria pisar el stack de la tarea interrumpida)
   ldr  sp , [r2]           // listo, ahora si tengo mi propio stack y puedo procesar
   push {lr}                // peeero, antes de llamar al kernel, guardo el 0xFFFFFFF9
   bl   taskKernel          // llamo al kernel
   pop  {lr}                // ahora si recupero mi 0xFFFFFFF9 (redordad que se grabo en el stal del kernel nada mas)

// ldr  r0 ,=runningContext //no hace falta, porque el que use r4 lo tiene que salvar, asi que nadie me lo puedo tocar (espero)
   ldr  r5 ,[r4]            // que sera??
   ldr  sp ,[r5]            // lo que sea, tomo lo que apunta y le mando el sp
   pop  {r4-r11}            // listo, saco los registros no autmaticos
   bx   lr                  // y ahora si, como lr es 0xFFFFFFF9 popea el resto de los registros y sigue desde el PC que popeara


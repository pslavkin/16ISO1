.syntax unified             // elije thumb o thumb2
.extern tasks
.extern kernelContext
.extern taskKernel
.text
.global PendSV_Handler
.thumb_func                 // modo thumb,bit 0 en 1 para las direcciones. (antes de la 1ra instruccion)

PendSV_Handler:

   tst  lr,0x00000010       // equivale a lr & 0x00000010 para ver si esta prendido este but
   it eq                    // si es cero (mira el bit z del registro de control)
   vpusheq {s16-s31}        // al ejec

   push {r4-r11,lr}         // estos registros no se guardan automaticamente

   ldr  r4 ,=tasks          // mi estrategia es guardar SIEMPRE el sp de la tarea interrumpida
   ldr  r5 ,[r4]            // tasks,context es el contexto de la tarea interrumpida
   str  sp ,[r5]            // listo, guardado por donde iba el sp

   ldr  r2 , =kernelContext // cambio a contexto kernel (es ooootro stack porque sino podria pisar el stack de la tarea interrumpida)
   ldr  sp , [r2]           // listo, ahora si tengo mi propio stack y puedo procesar
   bl   taskKernel          // llamo al kernel

   ldr  r4 ,=tasks          // no hace falta, porque el que use r4 lo tiene que salvar, asi que nadie me lo puedo tocar (espero)
   ldr  r5 ,[r4]            // que sera??
   ldr  sp ,[r5]            // lo que sea, tomo lo que apunta y le mando el sp
   pop  {r4-r11,lr}         // listo, saco los registros no autmaticos

   tst  lr,0x00000010       // equivale a lr & 0x00000010 para ver si esta prendido este but
   it eq                    // si es cero (mira el bit z del registro de control)
   vpusheq {s16-s31}        // al ejec

   bx   lr                  // y ahora si, como lr es 0xFFFFFFF9 popea el resto de los registros y sigue desde el PC que popeara


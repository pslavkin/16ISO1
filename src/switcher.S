.syntax unified // elije thumb o thumb2
.extern taskList
.extern runningTask
.extern taskCount
.text
.global SysTick_Handler
.thumb_func     // modo thumb,bit 0 en 1 para las direcciones. (antes de la 1ra instruccion)

   //void SysTick_Handler(void)  //proto en C

#define i         r4



SysTick_Handler:
   ldr  r0 , =taskList
   ldr  r1 , =runningTask
   ldr  r2 , [r1]
   ldr  r3 , [r0 ,r2 ,lsl #2]
   str  sp , [r3]

   ldr  r3 ,=taskCount
   ldr  r3 , [r3]
   add  r2 ,1
   cmp  r2 ,r3
   bcc  dontResetRunningTask

resetRunningTask:
      mov  r2 , #0

dontResetRunningTask:
   str  r2 ,[r1]
   ldr  r3 , [r0 ,r2 ,lsl #2]
   ldr  sp ,[r3]
   ldr  r2 , [sp ,#24]
   orr  r2 , #1
   str  r2 , [sp ,#24]
   bx   lr


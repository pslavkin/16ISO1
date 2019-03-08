   .syntax unified // elije thumb o thumb2
.global task1_p
.extern task2_p;
.extern first_shot
   .text
   .global SysTick_Handler
   .thumb_func     // modo thumb,bit 0 en 1 para las direcciones. (antes de la 1ra instruccion)

   //void SysTick_Handler(void)  //proto en C

#define i         r4

SysTick_Handler:
   ldr   r0 , =Actual_Task
   ldr   r1 , [ r0 ]
   cmp   r1, #0
   bne   TASK2
      mov  r2 , #1
      str  r2 , [ r0 ]

      ldr  r0 , =first_shot
      ldr  r0 , [r0]
      cmp  r0, #1
      beq  Skip_task2_sp
      ldr  r0 , =task2_p
      str  sp , [r0]

Skip_task2_sp:
      ldr  r0 , =first_shot
      mov  r1 , #0
      str  r1 , [r0]
      ldr  r0 , =task1_p
      ldr  sp , [r0]
      bx   lr
      
TASK2:
   mov  r2 , #0
   str  r2 , [ r0 ]
   ldr  r0 , =task1_p
   str  sp , [r0]
   ldr  r0 , =task2_p
   ldr  sp , [r0]
   bx lr

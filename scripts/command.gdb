target remote localhost:3333
break main
break SysTick_Handler
set disassemble-next-line on
conti

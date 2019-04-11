target remote localhost:3333
break main
break PendSV_Handler
set disassemble-next-line on
conti

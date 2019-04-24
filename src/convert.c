#include <stdint.h>
#include <stdio.h>

//----convierte float a string para validar float preemptive--------------
char* ftostr  (float fVal,char* str)
{
 int32_t Entera, Dec;
 uint8_t Len;
 uint8_t Sign[]="+";

 Entera  = fVal;
 Dec     = (int32_t)(fVal * 1000);
 Dec    %= 1000;
 if(fVal<0) {
    Dec     = -Dec;
    Entera  = -Entera;
    Sign[0] = '-';
 }
 Len=sprintf(str,"%s%d",Sign,Entera);
 str[Len++] = '.';
 sprintf(str+Len,"%d\r\n",Dec);
 return str;
}


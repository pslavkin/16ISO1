#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"

bool mutexInit ( semphr_t* m )
{
   m->locked=false;
};

bool mutexLock   ( semphr_t* m )
{
   while(m->locked==true) {
      taskYield();
   }
   m->locked=true;
}
bool mutexUnlock   ( semphr_t* m )
{
   m->locked=false;
   taskYield();
}

//----------------------------------------------------------------------
//bool mutexLock   ( semphr_t* m ){
//   if(m->locked==false) {
//      m->locked=true;
//   }
//   else {
//      tasks.context->state=BLOCKED;
//      tasks.context->semphr=m;
//      triggerPendSv();
//   }
//};

//bool mutexUnlock   ( semphr_t* m )
//{
//   uint8_t i,j,k;
//   m->locked=false;
//   disableSystickIrq();                    // si llegara systick en el medio
//                                           // del barrido podria cambiar los indices y generarme algun es como que
//                                           // estariamos 2 tocando la misma tabla, para leer y escribir... mmm
//                                           // TODO coments..
//   for (i=MAX_PRIOR;i>0;) {                //
//      i--;
//      k=tasks.index[i];
//      for(j=0;j<tasks.count[i];j++) {      // barro todas las tareas del grupo de prioridad
//         k=(k+1)%tasks.count[i];           // incremento modulo count
//         switch (tasks.list[i][k].state) { //
//            case BLOCKED:                  //
//               if(tasks.list[i][k].semphr==m) {
//                  tasks.list[i][k].state  = READY;
//                  tasks.list[i][k].semphr = NULL;
//                  triggerPendSv();
//                  enableSystickIrq(); // si goto y que!? esta hacia adelante del llamado y
//                  uartWriteString(UART_USB,"desbloqueo\r\n");
//                  goto end;
//               }
//               break;
//            default:
//               break;
//         }
//      }
//   }
//end:
//  enableSystickIrq(); // si goto y que!? esta hacia adelante del llamado y
//                         // me evito doble return o mas preguntas.
//};



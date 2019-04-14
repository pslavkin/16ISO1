#ifndef STAT
#define STAT

void     printTaskStat  ( taskContext_t* t );
void     printTasksStat ( tasks_t* t       );
int32_t  stampWaterMark ( taskContext_t* t );
void     stampRunCount  ( taskContext_t* t );

#endif

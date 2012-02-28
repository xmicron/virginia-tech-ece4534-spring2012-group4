#ifndef I2CTEMP_TASK_H
#define I2CTEMP_TASK_H

#include "messagequeues.h"

void vStarti2cTempTask( unsigned portBASE_TYPE uxPriority, i2cParamStruct *);

#endif
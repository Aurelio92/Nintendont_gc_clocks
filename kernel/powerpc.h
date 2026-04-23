#ifndef _POWERPC_H_
#define _POWERPC_H_

#include "powerpc.h"

void hardUDelay(u32 usec);

void powerpc_upload_stub(u32 entry);
void powerpc_switch_clock(void);
void powerpc_hang(void);
void powerpc_reset(void);

#endif

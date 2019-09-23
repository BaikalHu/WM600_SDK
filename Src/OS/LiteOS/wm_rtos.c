/*****************************************************************************
*
* File Name : wm_rtos.c
*
* Description: rtos compile file sets
*
* Copyright (c) 2014 Winner Microelectronics Co., Ltd.
* All rights reserved.
*
* Author : 
*
* Date : 2018-01-15
*****************************************************************************/

#ifndef WM_RTOS_H
#define WM_RTOS_H

//#include "../../LiteOS/wm_osal_liteos.c"
//#include "../../LiteOS/arch/arm/arm-m/cortex-m3/keil/los_dispatch_keil.S"
#include "../../LiteOS/arch/arm/arm-m/src/los_hw.c"
#include "../../LiteOS/arch/arm/arm-m/src/los_hw_tick.c"
#include "../../LiteOS/arch/arm/arm-m/src/los_hwi.c"
#include "../../LiteOS/kernel/base/core/los_priqueue.c"
#include "../../LiteOS/kernel/base/core/los_swtmr.c"
#include "../../LiteOS/kernel/base/core/los_sys.c"
#include "../../LiteOS/kernel/base/core/los_task.c"
#include "../../LiteOS/kernel/base/core/los_tick.c"
#include "../../LiteOS/kernel/base/core/los_timeslice.c"
#include "../../LiteOS/kernel/base/ipc/los_event.c"
#include "../../LiteOS/kernel/base/ipc/los_mux.c"
#include "../../LiteOS/kernel/base/ipc/los_queue.c"
#include "../../LiteOS/kernel/base/ipc/los_sem.c"
#include "../../LiteOS/kernel/base/mem/bestfit_little/los_heap.c"
#include "../../LiteOS/kernel/base/mem/bestfit_little/los_memory.c"
#include "../../LiteOS/kernel/base/mem/membox/los_membox.c"
#include "../../LiteOS/kernel/base/mem/common/los_memcheck.c"
#include "../../LiteOS/kernel/base/mem/common/los_memstat.c"
#include "../../LiteOS/kernel/base/mem/common/los_slab.c"
#include "../../LiteOS/kernel/base/mem/common/los_slabmem.c"
#include "../../LiteOS/kernel/base/misc/los_misc.c"
#include "../../LiteOS/kernel/base/om/los_err.c"
#include "../../LiteOS/kernel/los_init.c"


#endif /* end of WM_RTOS_H */

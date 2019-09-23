/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "osdepends/atiny_osdep.h"
#include "los_memory.h"
#include "los_sys.ph"
#include "los_sem.ph"
#include "los_tick.ph"
#include <stdbool.h>
#include "los_mux.h"
#include "wm_timer.h"
#include "wm_fwup.h"
#include "wm_crypto_hard.h"

extern T_BOOTER imgheader[2];

#define ATINY_CNT_MAX_WAITTIME 0xFFFFFFFF
#define LOG_BUF_SIZE (256)

#ifndef OK
#define OK 0
#endif

#ifndef ERR
#define ERR -1
#endif

 void atiny_usleep(unsigned long usec)
 {
	 int ret = 0;
	 	ret = tls_delay_via_timer(usec, TIMER_US_UNIT_FLAG);
 }


static uint64_t osKernelGetTickCount (void)
{
    uint64_t ticks;
    UINTPTR uvIntSave;

    if(OS_INT_ACTIVE)
    {
        ticks = 0U;
    }
    else
    {
        uvIntSave = LOS_IntLock();
        ticks = g_ullTickCount;
        LOS_IntRestore(uvIntSave);
    }

    return ticks;
}

uint64_t atiny_gettime_ms(void)
{
    return osKernelGetTickCount() * (OS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

void *atiny_malloc(size_t size)
{
    return LOS_MemAlloc(m_aucSysMem0, size);
}

void atiny_free(void *ptr)
{
    (void)LOS_MemFree(m_aucSysMem0, ptr);
}

int atiny_snprintf(char *buf, unsigned int size, const char *format, ...)
{
    int     ret;
    va_list args;

    va_start(args, format);
    ret = vsnprintf(buf, size, format, args);
    va_end(args);

    return ret;
}

int atiny_printf(const char *format, ...)
{
    int ret;
    char str_buf[LOG_BUF_SIZE] = {0};
    va_list list;

    memset(str_buf, 0, LOG_BUF_SIZE);
    va_start(list, format);
    ret = vsnprintf(str_buf, LOG_BUF_SIZE, format, list);
    va_end(list);

    printf("%s", str_buf);

    return ret;
}

char *atiny_strdup(const char *ch)
{
    char *copy;
    size_t length;

    if(NULL == ch)
        return NULL;

    length = strlen(ch);
    copy = (char *)atiny_malloc(length + 1);
    if(NULL == copy)
        return NULL;
    strncpy(copy, ch, length);
    copy[length] = '\0';

    return copy;
}

void tls_fwup_img_update_header(T_BOOTER* img_param)
{
    unsigned char current_img;	
    psCrcContext_t	crcContext;
    #define CODE_RUN_HEADER_ADDR                (0x08010000UL)
    extern unsigned int CODE_UPD_HEADER_ADDR;

    //printf("\n\n  0x%x 0x%x\n\n", CODE_RUN_HEADER_ADDR, CODE_UPD_HEADER_ADDR);
    tls_fls_read(CODE_RUN_HEADER_ADDR, (unsigned char *)&imgheader[0], sizeof(T_BOOTER));
    tls_fls_read(CODE_UPD_HEADER_ADDR, (unsigned char *)&imgheader[1], sizeof(T_BOOTER));

    //½«Á½¸öupd_noÖÐ½Ï´óµÄÄÇ¸öÖµÈ¡³öÀ´£¬ÔÙ½«Æä¼Ó1ºó¸³Öµ¸ø CODE_UPD_HEADER_ADDR ´¦µÄheader£»
    if (tls_fwup_img_header_check(&imgheader[1]))
    {
        current_img = (imgheader[1].upd_no > imgheader[0].upd_no);
    }
    else
    {
        current_img = 0;
    }
    img_param->upd_no = imgheader[current_img].upd_no + 1;

    tls_crypto_crc_init(&crcContext, 0xFFFFFFFF, CRYPTO_CRC_TYPE_32, 3);
    tls_crypto_crc_update(&crcContext, (unsigned char *)img_param, sizeof(T_BOOTER)-4);
    tls_crypto_crc_final(&crcContext, &img_param->hd_checksum);
    tls_fls_write(CODE_UPD_HEADER_ADDR,  (unsigned char *)img_param,  sizeof(T_BOOTER));
}


void atiny_reboot(void)
{
    tls_sys_reset();
}


void atiny_delay(uint32_t second)
{
    (void)LOS_TaskDelay(second * LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

#if (LOSCFG_BASE_IPC_SEM == YES)

void *atiny_mutex_create(void)
{
    uint32_t uwRet;
    uint32_t uwSemId;

    if (OS_INT_ACTIVE)
    {
        return NULL;
    }

    uwRet = LOS_BinarySemCreate(1, (UINT32 *)&uwSemId);

    if (uwRet == LOS_OK)
    {
        return (void *)(GET_SEM(uwSemId));
    }
    else
    {
        return NULL;
    }
}

void atiny_mutex_destroy(void *mutex)
{
    if (OS_INT_ACTIVE)
    {
        return;
    }

    if (mutex == NULL)
    {
        return;
    }

    (void)LOS_SemDelete(((SEM_CB_S *)mutex)->usSemID);
}

void atiny_mutex_lock(void *mutex)
{
    if (mutex == NULL)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    (void)LOS_SemPend(((SEM_CB_S *)mutex)->usSemID, ATINY_CNT_MAX_WAITTIME);
}

void atiny_mutex_unlock(void *mutex)
{
    if (mutex == NULL)
    {
        return;
    }

    (void)LOS_SemPost(((SEM_CB_S *)mutex)->usSemID);
}

#else

void *atiny_mutex_create(void)
{
    return NULL;
}

void atiny_mutex_destroy(void *mutex)
{
    ((void)mutex);
}

void atiny_mutex_lock(void *mutex)
{
    ((void)mutex);
}

void atiny_mutex_unlock(void *mutex)
{
    ((void)mutex);
}

#endif /* LOSCFG_BASE_IPC_SEM == YES */


#if (LOSCFG_BASE_IPC_MUX == YES)
static bool atiny_task_mutex_is_valid(const atiny_task_mutex_s *mutex)
{
    return (mutex != NULL) && (mutex->valid);
}

int atiny_task_mutex_create(atiny_task_mutex_s *mutex)
{
    UINT32 ret;

    if (mutex == NULL)
    {
        return ERR;
    }

    memset(mutex, 0, sizeof(*mutex));
    ret = LOS_MuxCreate(&mutex->mutex);
    if (ret != LOS_OK)
    {
        return ret;
    }
    mutex->valid = true;
    return LOS_OK;
}

#define ATINY_DESTROY_MUTEX_WAIT_INTERVAL 100
int atiny_task_mutex_delete(atiny_task_mutex_s *mutex)
{
    int ret;

    if (!atiny_task_mutex_is_valid(mutex))
    {
        return ERR;
    }

    do
    {
        ret = LOS_MuxDelete(mutex->mutex);
        if (LOS_ERRNO_MUX_PENDED == ret)
        {
            LOS_TaskDelay(ATINY_DESTROY_MUTEX_WAIT_INTERVAL);
        }
        else
        {
            break;
        }
    }while (true);

    memset(mutex, 0, sizeof(*mutex));

    return ret;
}
int atiny_task_mutex_lock(atiny_task_mutex_s *mutex)
{
    if (!atiny_task_mutex_is_valid(mutex))
    {
        return ERR;
    }
    return LOS_MuxPend(mutex->mutex, ATINY_CNT_MAX_WAITTIME);
}
int atiny_task_mutex_unlock(atiny_task_mutex_s *mutex)
{
    if (!atiny_task_mutex_is_valid(mutex))
    {
        return ERR;
    }
    return LOS_MuxPost(mutex->mutex);
}
#endif /* LOSCFG_BASE_IPC_MUX == YES */



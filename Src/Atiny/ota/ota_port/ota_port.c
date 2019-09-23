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

#include "ota_port.h"
#include "common.h"
#include "flag_manager.h"
#include "upgrade_flag.h"
#include <string.h>
#include <stdlib.h>
//#include <board.h>
//#include "flash_adaptor.h"
//#include "hal_spi_flash.h"
#include "wm_fwup.h"

extern unsigned int CODE_UPD_START_ADDR;
#define FLASH_BLOCK_SIZE 0x1000


//static const uint32_t g_flash_base_addrs[] = {OTA_IMAGE_DOWNLOAD_ADDR, OTA_IMAGE_DOWNLOAD_ADDR, OTA_FLAG_ADDR1};
//static const uint32_t g_flash_max_size[] = {OTA_IMAGE_DOWNLOAD_SIZE, OTA_IMAGE_DOWNLOAD_SIZE, FLASH_BLOCK_SIZE};

static int verify_boot_head(const void *buf)
{
    u32 image_checksum = 0;
    u32 org_checksum = 0;
    T_BOOTER booter;

    memcpy(&booter, buf, sizeof(T_BOOTER));

#if 0
    if (!tls_fwup_img_header_check(&booter))
    {
        printf("DEBUG： header check\n");
        return -1;
    }

    if ((IMG_TYPE_OLD_PLAIN == booter.img_type ) ||(IMG_TYPE_NEW_PLAIN == booter.img_type))
    {
        fwup->program_base = booter.upd_img_addr | FLASH_BASE_ADDR;
        fwup->total_len = booter.upd_img_len;
        org_checksum = booter.upd_checksum;
        isacrossflash = FALSE;
        currentlen = 0;
    }
    else 
    {
        request->status = TLS_FWUP_REQ_STATUS_FCRC;
        goto request_finish;
    }
#endif

    tls_fwup_img_update_header(&booter);
    return 0;
}

static int hal_read_flash(ota_flash_type_e type, void *buf, int32_t len, uint32_t location)
{
    int ret = 0;
    //printf("@@@@@@@@@@@read flash location:0x%x len:%d\r\n", location, len);

    if(type == OTA_UPDATE_INFO)
    {
        return tls_fls_read( 0x080FD000, buf, len);
    }

    ret = tls_fls_read((CODE_UPD_START_ADDR + location), buf, len);
    return ret;
}

static int hal_write_flash(ota_flash_type_e type, const void *buf, int32_t len, uint32_t location)
{
    int i = 0;
    int j = 1;
    int ret = 0;

    if(type == OTA_UPDATE_INFO)
    {
        return tls_fls_write( 0x080FD000, buf, len);
    }

    if(location == 0)
    {
        if(verify_boot_head(buf))
            return -1;
        buf += sizeof(T_BOOTER);
    }

    //printf("~~~~~~~~~~~~~write flash location:0x%x len:%d\r\n", location, len);
    //for(i; i< len; i++,j++)
    //{
    //    printf("0x%x ", *((const uint8_t *)buf+i));
    //    if(j%16 == 0)
    //        printf("\r\n");
    //}
    //printf("\r\n");
    if(location == 0)
        ret = tls_fls_write( (CODE_UPD_START_ADDR + location), buf, len-sizeof(T_BOOTER));
    else
        ret = tls_fls_write( (CODE_UPD_START_ADDR + location)-sizeof(T_BOOTER), buf, len);
    //printf("\n\n~~~~~~~~~~~~~~write flash ret:%d\n", ret);
    return ret;
}

void hal_init_ota(void)
{
    flash_adaptor_init();
}


void hal_get_ota_opt(ota_opt_s *opt)
{
    if (opt == NULL)
    {
        //HAL_OTA_LOG("opt NULL");
        return;
    }

    memset(opt, 0, sizeof(*opt));
    opt->read_flash = hal_read_flash;
    opt->write_flash = hal_write_flash;
    opt->flash_block_size = FLASH_BLOCK_SIZE;
}




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
#include "agent_tiny_demo.h"
#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"
#include "atiny_mqtt/mqtt_client.h"
#include "osdepends/atiny_osdep.h"
#include "log/atiny_log.h"
#include "cJSON.h"
#include "wm_crypto_hard.h"


/* brief : the oceanconnect platform only support the ca_crt up tills now*/
/** the address product_id device_id password crt is only for the test  */

//#define MQTT_DEMO_CONNECT_DYNAMIC
#define DEFAULT_SERVER_IPV4 "49.4.93.24"  ///<  server ip address
#define DEFAULT_SERVER_PORT "8883"           ///<  server mqtt service port

#ifdef MQTT_DEMO_CONNECT_DYNAMIC
	#define AGENT_TINY_DEMO_PRODUCTID "8ab780ed11330c2ce2acdf23dbab4d06"
	#define AGENT_TINY_DEMO_NODEID    "11223344"
	#define AGENT_TINY_DEMO_PASSWORD  "c6f16270c5bbf00063ab"
#else
	#define AGENT_TINY_DEMO_DEVICEID "959bf7b2-3886-44a2-ba3e-b75b97fab15b"
	#define AGENT_TINY_DEMO_PASSWORD "b3f86b64d3095e3afbec"
#endif

#define MAX_LEN  2048

#ifndef array_size
#define array_size(a) (sizeof(a)/sizeof(*(a)))
#endif
static char s_mqtt_ca_crt[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIID4DCCAsigAwIBAgIJAK97nNS67HRvMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV\r\n"
"BAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UEBxMCU1oxDzANBgNVBAoTBkh1YXdl\r\n"
"aTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lPVDAeFw0xNjA1MDQxMjE3MjdaFw0y\r\n"
"NjA1MDIxMjE3MjdaMFMxCzAJBgNVBAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UE\r\n"
"BxMCU1oxDzANBgNVBAoTBkh1YXdlaTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lP\r\n"
"VDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJxM9fwkwvxeILpkvoAM\r\n"
"Gdqq3x0G9o445F6Shg3I0xmmzu9Of8wYuW3c4jtQ/6zscuIGyWf06ke1z//AVZ/o\r\n"
"dp8LkuFbBbDXR5swjUJ6z15b6yaYH614Ty/d6DrCM+RaU+FWmxmOon9W/VELu2BB\r\n"
"NXDQHJBSbWrLNGnZA2erk4JSMp7RhHrZ0QaNtT4HhIczFYtQ2lYF+sQJpQMrjoRn\r\n"
"dSV9WB872Ja4DgcISU1+wuWLmS/NKjIvOWW1upS79yu2I4Rxos2mFy9xxz311rGC\r\n"
"Z3X65ejFNzCUrNgf6NEP1N7wB9hUu7u50aA+/56D7EgjeI0gpFytC+a4f6JCPVWI\r\n"
"Lr0CAwEAAaOBtjCBszAdBgNVHQ4EFgQUcGqy59oawLEgMl21//7F5RyABpwwgYMG\r\n"
"A1UdIwR8MHqAFHBqsufaGsCxIDJdtf/+xeUcgAacoVekVTBTMQswCQYDVQQGEwJD\r\n"
"TjELMAkGA1UECBMCR0QxCzAJBgNVBAcTAlNaMQ8wDQYDVQQKEwZIdWF3ZWkxCzAJ\r\n"
"BgNVBAsTAkNOMQwwCgYDVQQDEwNJT1SCCQCve5zUuux0bzAMBgNVHRMEBTADAQH/\r\n"
"MA0GCSqGSIb3DQEBCwUAA4IBAQBgv2PQn66gRMbGJMSYS48GIFqpCo783TUTePNS\r\n"
"tV8G1MIiQCpYNdk2wNw/iFjoLRkdx4va6jgceht5iX6SdjpoQF7y5qVDVrScQmsP\r\n"
"U95IFcOkZJCNtOpUXdT+a3N+NlpxiScyIOtSrQnDFixWMCJQwEfg8j74qO96UvDA\r\n"
"FuTCocOouER3ZZjQ8MEsMMquNEvMHJkMRX11L5Rxo1pc6J/EMWW5scK2rC0Hg91a\r\n"
"Lod6aezh2K7KleC0V5ZlIuEvFoBc7bCwcBSAKA3BnQveJ8nEu9pbuBsVAjHOroVb\r\n"
"8/bL5retJigmAN2GIyFv39TFXIySw+lW0wlp+iSPxO9s9J+t\r\n"
"-----END CERTIFICATE-----\r\n";


static mqtt_client_s *g_phandle = NULL;
static demo_param_s g_demo_param;


static int create_profile_data(cJSON *profile_data[], int value)
{
    cJSON *item = NULL;
    cJSON *tmp = NULL;
   // const char *define_data_name_0 = "define_data0";
   // const char *define_data_value_0 = "define_value0";
    const char *define_data_name_1 = "batteryLevel";
    int define_data_value_1 = (((uint32_t) value) % 100);
    int ret = ATINY_ERR;

    item  = cJSON_CreateObject();
    if (item == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateObject null");
        return ATINY_ERR;
    }

    tmp = cJSON_CreateNumber(define_data_value_1);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber null");
        goto EXIT;
    }
    cJSON_AddItemToObject(item, define_data_name_1, tmp);
    profile_data[0] = item;
    ret = ATINY_OK;

EXIT:
    if (ret != ATINY_OK)
    {
         cJSON_Delete(item);
    }
    return ret;

}
static cJSON *create_service_data(cJSON *profile_data[], uint32_t num)
{
    cJSON *service_data = NULL;
    int ret = ATINY_ERR;
    const char *serviceid = "Battery";
    const char *evet_time = "20161219T114920Z";
    uint32_t i;

    service_data = cJSON_CreateArray();
    if (service_data == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateArray null");
        goto EXIT;
    }

    for (i = 0; i < num; i++)
    {
        cJSON *item = NULL;
        cJSON *tmp = NULL;

        item  = cJSON_CreateObject();
        if (item == NULL)
        {
            ATINY_LOG(LOG_ERR, "cJSON_CreateObject null");
            goto EXIT;
        }
        cJSON_AddItemToArray(service_data, item);

        tmp = cJSON_CreateString(serviceid);
        if (tmp == NULL)
        {
            ATINY_LOG(LOG_ERR, "cJSON_CreateString null");
            goto EXIT;
        }
        cJSON_AddItemToObject(item, MQTT_SERVICEID, tmp);

        cJSON_AddItemToObject(item, MQTT_SERVICE_DATA, profile_data[i]);
        profile_data[i]= NULL;

        tmp = cJSON_CreateString(evet_time);
        if (tmp == NULL)
        {
            ATINY_LOG(LOG_ERR, "cJSON_CreateString null");
            goto EXIT;
        }
        cJSON_AddItemToObject(item, MQTT_EVENT_TIME, tmp);
   }

    ret = ATINY_OK;

EXIT:
    if (ret != ATINY_OK)
    {
        if (service_data)
        {
            cJSON_Delete(service_data);
            service_data = NULL;
        }
    }

    for (i = 0; i < num; i++)
    {
        if (profile_data[i])
        {
            cJSON_Delete(profile_data[i]);
        }

    }

    return service_data;
}
static char * create_json_data(cJSON *profile_data[], uint32_t num)
{
    cJSON *tmp = NULL;
    char *ret = NULL;
    cJSON * service_data = NULL;
    cJSON *root = NULL;

    service_data = create_service_data(profile_data, num);
    if (service_data == NULL)
    {
        goto EXIT;
    }

    root = cJSON_CreateObject();
    if (root == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateObject null");
        goto EXIT;
    }

    tmp = cJSON_CreateString(MQTT_DEVICE_REQ);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateString MQTT_DEVICE_REQ null");
        goto EXIT;
    }
    cJSON_AddItemToObject(root, MQTT_MSG_TYPE, tmp);

    tmp = cJSON_CreateNumber(MQTT_NO_MORE_DATA);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber MQTT_NO_MORE_DATA null");
        goto EXIT;
    }
    cJSON_AddItemToObject(root, MQTT_HAS_MORE, tmp);

    cJSON_AddItemToObject(root, MQTT_DATA, service_data);
    service_data = NULL;
    ret = cJSON_Print(root);
    if (ret == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_Print fail");
    }

EXIT:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (service_data)
    {
        cJSON_Delete(service_data);
    }
    return ret;

}

static inline size_t
strnlen(const char *s, size_t maxlen) {
  size_t n = 0;
  while(*s++ && n < maxlen)
    ++n;
  return n;
}

/*lint -e550*/
void app_data_report(void)
{
    int cnt = 0;
    LOS_TaskDelay(10 * 1000);
    while(1)
    {
        if(g_phandle)
        {
            cJSON *profile_data[1] = {0};
            if(create_profile_data(profile_data, cnt) == ATINY_OK)
            {
                char *msg = NULL;
                msg = create_json_data(profile_data, 1);
                if(msg)
                {
                    int ret = 0;
                    ret = atiny_mqtt_data_send(g_phandle, msg, strnlen(msg, MAX_LEN), MQTT_QOS_LEAST_ONCE);
                    ATINY_LOG(LOG_INFO, "report ret:%d, cnt %d", ret, cnt);
                    if (ret == ATINY_OK)
                    {
                        ATINY_LOG(LOG_DEBUG, "report data %s\n", msg);
                    }
                    atiny_free(msg);
                }
            }
        }
        else
        {
            ATINY_LOG(LOG_ERR, "g_phandle null");
        }
        (void)LOS_TaskDelay(10 * 1000);
         cnt++;
    }
}
/*lint +e550*/

UINT32 creat_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param={0};
    UINT32 TskHandle;

    task_init_param.usTaskPrio = 8;
    task_init_param.pcName = "app_data_report";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)app_data_report;
    task_init_param.uwStackSize = 0x1800;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

static int demo_get_time(char *time, int32_t len)
{
    const int32_t min_time_len = 11;
    if ((time == NULL) || (len < min_time_len))
    {
        ATINY_LOG(LOG_ERR,"invalid param len %d", len);
        return ATINY_ERR;
    }
    (void)strncpy(time, "2018111517", len);
    return ATINY_OK;
}

static int proc_rcv_msg(const char*serviceid, const char *cmd, cJSON *paras, int has_more, int mid)
{
    ATINY_LOG(LOG_INFO, "proc_rcv_msg call");
    return ATINY_OK;
}

static int send_msg_resp(int mid, int errcode, int has_more, cJSON *body)
{
    cJSON *tmp = NULL;
    int ret = ATINY_ERR;
    cJSON *msg = NULL;
    char *str_msg = NULL;

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateObject null");
        goto EXIT;
    }

    tmp = cJSON_CreateString(MQTT_DEVICE_RSP);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateString  null");
        goto EXIT;
    }
    cJSON_AddItemToObject(msg, MQTT_MSG_TYPE, tmp);

    tmp = cJSON_CreateNumber(mid);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber null");
        goto EXIT;
    }
    cJSON_AddItemToObject(msg, MQTT_MID, tmp);

    tmp = cJSON_CreateNumber(errcode);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber null");
        goto EXIT;
    }
    cJSON_AddItemToObject(msg, MQTT_ERR_CODE, tmp);

    tmp = cJSON_CreateNumber(has_more);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber null");
        goto EXIT;
    }
    cJSON_AddItemToObject(msg, MQTT_HAS_MORE, tmp);

    cJSON_AddItemToObject(msg, MQTT_BODY, body);
    body = NULL;

    str_msg = cJSON_Print(msg);
    if (str_msg == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateNumber null");
        goto EXIT;
    }

    ret = atiny_mqtt_data_send(g_phandle, str_msg, strnlen(str_msg, MAX_LEN), MQTT_QOS_LEAST_ONCE);
    if (str_msg == NULL)
    {
        ATINY_LOG(LOG_ERR, "atiny_mqtt_data_send fail ret %d",  ret);
    }

    ATINY_LOG(LOG_INFO, "send rsp ret %d, rsp: %s", ret, str_msg);

EXIT:
    cJSON_Delete(msg);
    if (body)
    {
        cJSON_Delete(body);
    }
    if (str_msg)
    {
        atiny_free(str_msg);
    }
    return ret;
}

static cJSON *get_resp_body(void)
{
    cJSON *body = NULL;
    cJSON *tmp;
    const char *body_para = "body_para";

    body = cJSON_CreateObject();
    if (body == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateObject");
        return NULL;
    }

    tmp = cJSON_CreateString(body_para);
    if (tmp == NULL)
    {
        ATINY_LOG(LOG_ERR, "cJSON_CreateString null");
        goto EXIT;
    }
    cJSON_AddItemToObject(body, body_para, tmp);

    return body;

EXIT:
    cJSON_Delete(body);
    return NULL;
}

enum
{
    SERVERID_IDX,
    CMD_IDX,
    PARAS_IDX,
    HAS_MORE_IDX,
    MID_IDX,
};

static int handle_rcv_msg(cJSON *msg)
{
    cJSON *items[5] = {0};
    const char *names[5] = {MQTT_SERVICEID, MQTT_CMD, MQTT_PARAS, MQTT_HAS_MORE, MQTT_MID};
    int ret;
    cJSON *body;
    uint32_t i;

    for (i = 0; i < array_size(items); i++)
    {
        items[i] = cJSON_GetObjectItem(msg, names[i]);
        if (names[i] == NULL)
        {
            ATINY_LOG(LOG_ERR,"cJSON_GetObjectItem %s fail", names[i]);
            return ATINY_ERR;
        }
    }

    if ((items[SERVERID_IDX]->string == NULL)
        || (items[CMD_IDX]->string == NULL)
        || (items[HAS_MORE_IDX]->valueint != MQTT_NO_MORE_DATA && items[HAS_MORE_IDX]->valueint != MQTT_MORE_DATA))
    {
           ATINY_LOG(LOG_ERR,"null or err para hasMore %d", items[3]->valueint);
          return ATINY_ERR;
    }

    ret = proc_rcv_msg(items[SERVERID_IDX]->string, items[CMD_IDX]->string,\
                items[PARAS_IDX], items[HAS_MORE_IDX]->valueint, items[MID_IDX]->valueint);
    body = get_resp_body();
    if (body == NULL)
    {
        ATINY_LOG(LOG_ERR,"get_resp_body fail");
        return ATINY_ERR;
    }

    return send_msg_resp(items[MID_IDX]->valueint, (ret == ATINY_OK) ? MQTT_ERR_CODE_OK : MQTT_ERR_CODE_ERR,
                     MQTT_NO_MORE_DATA, body);
}

static int demo_rcv_msg(const uint8_t *msg, int32_t len)
{
    cJSON *parse_msg = NULL;
    int ret = ATINY_ERR;
    if ((msg == NULL) || len <= 0)
    {
        ATINY_LOG(LOG_ERR, "invalid param len %ld", len);
        return ATINY_ERR;
    }

    ATINY_LOG(LOG_INFO, "recv msg %s", msg);
    parse_msg = cJSON_Parse((const char *)msg);
    if (parse_msg != NULL)
    {
        ret = handle_rcv_msg(parse_msg);
    }
    else
    {
        ATINY_LOG(LOG_ERR, "cJSON_Parse fail");
    }

    if (parse_msg)
    {
        cJSON_Delete(parse_msg);
    }

    return ret;
}

static int demo_cmd_ioctl(mqtt_cmd_e cmd, void *arg, int32_t len)
{
    int result = ATINY_ERR;

//TODO:
    switch(cmd)
        {
        case MQTT_GET_TIME:
            result = demo_get_time(arg, len);
            break;
        case MQTT_RCV_MSG:
            result = demo_rcv_msg(arg, len);
            break;
        case MQTT_SAVE_SECRET_INFO:
            if (g_demo_param.write_flash_info == NULL)
            {

                result = ATINY_ERR;
                ATINY_LOG(LOG_ERR, "write_flash_info null");
            }
            else
            {
                result = g_demo_param.write_flash_info(arg, len);
            }
            break;
        case MQTT_READ_SECRET_INFO:
            if (g_demo_param.read_flash_info == NULL)
            {

                result = ATINY_ERR;
                ATINY_LOG(LOG_ERR, "read_flash_info null");
            }
            else
            {
                result = g_demo_param.read_flash_info(arg, len);
            }
            break;

        default:
            break;
        }
    return result;
}


void agent_tiny_entry(void)
{
    UINT32 uwRet = LOS_OK;
    mqtt_param_s atiny_params;
    mqtt_device_info_s device_info;
    tls_crypto_random_init(0xFDA3C97A, CRYPTO_RNG_SWITCH_16);
    if (g_demo_param.init)
    {
        g_demo_param.init();
    }

    atiny_params.server_ip = DEFAULT_SERVER_IPV4;
    atiny_params.server_port = DEFAULT_SERVER_PORT;


    atiny_params.info.type = en_mqtt_al_security_cas;
    atiny_params.info.u.cas.ca_crt.data = s_mqtt_ca_crt;
    atiny_params.info.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt);


    atiny_params.cmd_ioctl = demo_cmd_ioctl;

    if(ATINY_OK != atiny_mqtt_init(&atiny_params, &g_phandle))
    {
        return;
    }

//    uwRet = creat_report_task();
//    if(LOS_OK != uwRet)
//    {
//        return;
//    }

    device_info.codec_mode = MQTT_CODEC_MODE_JSON;
    device_info.sign_type = MQTT_SIGN_TYPE_HMACSHA256_NO_CHECK_TIME;
    device_info.password = AGENT_TINY_DEMO_PASSWORD;
#ifndef MQTT_DEMO_CONNECT_DYNAMIC
    device_info.connection_type = MQTT_STATIC_CONNECT;
    device_info.u.s_info.deviceid  = AGENT_TINY_DEMO_DEVICEID;
#else
    device_info.connection_type = MQTT_DYNAMIC_CONNECT;
    device_info.u.d_info.productid = AGENT_TINY_DEMO_PRODUCTID;
    device_info.u.d_info.nodeid = AGENT_TINY_DEMO_NODEID;
#endif
    (void)atiny_mqtt_bind(&device_info, g_phandle);
    return ;
}

void agent_tiny_demo_init(const demo_param_s *param)
{
    g_demo_param = *param;
}


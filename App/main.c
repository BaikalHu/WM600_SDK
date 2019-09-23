/***************************************************************************** 
* 
* File Name : main.c
* 
* Description: main 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-6-14
*****************************************************************************/ 
#include "wm_include.h"
#include "los_typedef.h"
#include "agent_tiny_demo.h"
#include "los_task.h"

#define WIFI_SSID  "dorke"
#define WIFI_PWD   "mumudorke"
//#define WIFI_SSID  "liteos_lte"
//#define WIFI_PWD   "liteos_lte"

static UINT32 g_atiny_tskHandle;
 u32 g_net_sem;




static void con_net_status_changed_event(u8 status )
{
	switch(status)
	{
		case NETIF_IP_NET_UP:
		{
			struct tls_ethif * tmpethif = tls_netif_get_ethif();
#if TLS_CONFIG_LWIP_VER2_0_3
			print_ipaddr(&tmpethif->ip_addr);
#if TLS_CONFIG_IPV6
			print_ipaddr(&tmpethif->ip6_addr[0]);
			print_ipaddr(&tmpethif->ip6_addr[1]);
			print_ipaddr(&tmpethif->ip6_addr[2]);
#endif
#else
			printf("net up ==> ip = %d.%d.%d.%d\n",ip4_addr1(&tmpethif->ip_addr.addr),ip4_addr2(&tmpethif->ip_addr.addr),
							 ip4_addr3(&tmpethif->ip_addr.addr),ip4_addr4(&tmpethif->ip_addr.addr));
#endif
		}
		LOS_SemPost(g_net_sem);
			break;
		case NETIF_WIFI_DISCONNECTED:
			printf("\r\nwifi disconnected~~~~~~~~~~~~~~~~~\r\n");
		default:
			break;
	}
}

int agent_connect_net(char *ssid, char *pwd)
{
	struct tls_param_ip *ip_param = NULL;
	u8 wireless_protocol = 0;

	if (!ssid)
	{
		return WM_FAILED;
	}

	printf("\nssid:%s\n", ssid);
	printf("password=%s\n",pwd);
	tls_wifi_disconnect();

	tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void*) &wireless_protocol, TRUE);
	if (TLS_PARAM_IEEE80211_INFRA != wireless_protocol)
	{
	    tls_wifi_softap_destroy();
	    wireless_protocol = TLS_PARAM_IEEE80211_INFRA;
        tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void*) &wireless_protocol, FALSE);
	}

	tls_wifi_set_oneshot_flag(0);

	ip_param = tls_mem_alloc(sizeof(struct tls_param_ip));
	if (ip_param)
	{
		tls_param_get(TLS_PARAM_ID_IP, ip_param, FALSE);
		ip_param->dhcp_enable = TRUE;
		tls_param_set(TLS_PARAM_ID_IP, ip_param, FALSE);
		tls_mem_free(ip_param);
	}

	tls_netif_add_status_event(con_net_status_changed_event);
	tls_wifi_connect((u8 *)ssid, strlen(ssid), (u8 *)pwd, strlen(pwd));
	printf("\nplease wait connect net......\n");

	return WM_SUCCESS;
}

void flash_adaptor_init(void)
{
    //hal_spi_flash_config();
}

void atiny_task_entry(void)
{
  	//printf("atiny task\r\n");
    LOS_SemPend(g_net_sem, 0xffffffff);
    //printf("creat_agenttiny_task\r\n");

    extern void agent_tiny_entry();
#if defined(WITH_LINUX) || defined(WITH_LWIP)
    //hieth_hw_init();
    //net_init();
#elif defined(WITH_AT_FRAMEWORK)


    #if defined(USE_ESP8266)
    extern at_adaptor_api esp8266_interface;
    printf("\r\n=============agent_tiny_entry  USE_ESP8266============================\n");
    at_api_register(&esp8266_interface);

    #elif defined(USE_EMTC_BG36)
    extern at_adaptor_api emtc_bg36_interface;
    printf("\r\n=============agent_tiny_entry  USE_EMTC_BG36============================\n");
    at_api_register(&emtc_bg36_interface);

    #elif defined(USE_SIM900A)
    extern at_adaptor_api sim900a_interface;
    printf("\r\n=============agent_tiny_entry  USE_SIM900A============================\n");
    at_api_register(&sim900a_interface);

    #elif defined(USE_NB_NEUL95)
    extern at_adaptor_api bc95_interface;
    printf("\r\n=============agent_tiny_entry  USE_NB_NEUL95============================\n");
    los_nb_init((const int8_t *)"172.25.233.98",(const int8_t *)"5600",NULL);
    los_nb_notify("\r\n+NSONMI:",strlen("\r\n+NSONMI:"),NULL,nb_cmd_match);
    at_api_register(&bc95_interface);

    #elif defined(USE_NB_NEUL95_NO_ATINY)
    demo_nbiot_only();
    #else

    #endif
#else
#endif


#if !defined(USE_NB_NEUL95_NO_ATINY)
#ifdef CONFIG_FEATURE_FOTA
    hal_init_ota();
#endif

#ifdef WITH_MQTT
    flash_adaptor_init();
    {

        demo_param_s demo_param = {.init = NULL,
                                   .write_flash_info = NULL,//flash_adaptor_write_mqtt_info,
                                   .read_flash_info = NULL};//flash_adaptor_read_mqtt_info};
        agent_tiny_demo_init(&demo_param);
    }
    extern  int mqtt_lib_load();
    mqtt_lib_load();
#endif

    agent_tiny_entry();
#endif
}

UINT32 creat_agenttiny_task(VOID)
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param={0};
    task_init_param.usTaskPrio = 2;
    task_init_param.pcName = "agenttiny_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)atiny_task_entry;

#if defined(CONFIG_FEATURE_FOTA) || defined(WITH_MQTT)
    task_init_param.uwStackSize = 0x2800; /* fota use mbedtls bignum to verify signature  consuming more stack  */
#else
    task_init_param.uwStackSize = 0x1000;
#endif

    uwRet = LOS_TaskCreate(&g_atiny_tskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

void UserMain(void)
{
	u32 ret;
	//printf("\n\n\n user task new version!!!\n\n\n");

#if DEMO_CONSOLE
	CreateDemoTask();
#endif
//用户自己的task
	printf("first connect to the AP");

	agent_connect_net(WIFI_SSID, WIFI_PWD);

}


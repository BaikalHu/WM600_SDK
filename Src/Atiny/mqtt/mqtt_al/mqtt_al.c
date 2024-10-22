/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
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

#include <stdint.h>
#include <stddef.h>



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


#include "mqtt_al.h"

#ifdef WITH_MQTT

typedef struct
{
	mqtt_al_op_t  *io;
	mqtt_al_op_t   mem;
}mqtt_al_op_cb_t;

static mqtt_al_op_cb_t   s_mqtt_al_op_cb;


/////////////////CREATE THE API FOR THE MQTT LIB////////////////////////////////
int mqtt_al_install(mqtt_al_op_t *op)
{
	int ret = -1;
	if((NULL != op)&&(NULL == s_mqtt_al_op_cb.io))
	{
		s_mqtt_al_op_cb.mem  = *op;
		s_mqtt_al_op_cb.io = &s_mqtt_al_op_cb.mem;

		ret = 0;
	}

	return ret;
}

int mqtt_al_uninstall()
{
	int ret = -1;
	if(NULL != s_mqtt_al_op_cb.io)
	{
		s_mqtt_al_op_cb.io = NULL;

		ret = 0;
	}

	return ret;
}

//////////////////////////CREATE THE API FOR THE MQTT APPLICATION///////////////

int mqtt_al_init()
{
	int ret = -1;

	if((NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->init))
	{
		ret = s_mqtt_al_op_cb.io->init();
	}

	return ret;
}


int  mqtt_al_deinit()
{

	int ret = -1;

	if((NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->deinit))
	{
		ret = s_mqtt_al_op_cb.io->deinit();
	}

	return ret;
}

void * mqtt_al_connect( mqtt_al_conpara_t *conpara)
{

	void *ret = NULL;

	if((NULL != conpara) && (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->connect))
	{
		ret = s_mqtt_al_op_cb.io->connect(conpara);
	}

	return ret;

}


int mqtt_al_disconnect(void *handle)
{

	int ret = -1;

	if((NULL != handle) && (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->disconnect))
	{
		ret = s_mqtt_al_op_cb.io->disconnect(handle);
	}

	return ret;

}


int	mqtt_al_publish(void *handle, mqtt_al_pubpara_t *pubpara)
{

	int ret = -1;
	if((NULL != handle) && (NULL != pubpara) &&\
	   (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->publish))
	{
		ret = s_mqtt_al_op_cb.io->publish(handle,pubpara);
	}
	return ret;

}


int mqtt_al_subscribe(void *handle, mqtt_al_subpara_t *subpara)
{

	int ret = -1;

	if((NULL != handle) && (NULL != subpara) &&\
	   (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->subscribe))
	{
		ret = s_mqtt_al_op_cb.io->subscribe(handle,subpara);
	}

	return ret;

}


int mqtt_al_unsubscribe(void *handle, mqtt_al_unsubpara_t *unsubpara)
{

	int ret = -1;

	if((NULL != handle) && (NULL != unsubpara) && \
	    (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->unsubscribe))
	{
		ret = s_mqtt_al_op_cb.io->unsubscribe(handle,unsubpara);
	}

	return ret;

}

en_mqtt_al_connect_state mqtt_al_check_status(void *handle)
{

	int ret = en_mqtt_al_connect_err;

	if((NULL != handle) && (NULL != s_mqtt_al_op_cb.io) && (NULL != s_mqtt_al_op_cb.io->check_status))
	{
		ret = s_mqtt_al_op_cb.io->check_status(handle);
	}

	return ret;
}


#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



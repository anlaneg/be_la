/*****************************************************************
* Copyright (C) 2017 Lenovo Co.,Ltd.*
******************************************************************
* 
* FILENAME:
*   be_la_accproc.c
*
* DESCRIPTION:
* 
* AUTHOR:
*   Feng Liu 626987
*
* CREATED DATE:
*   28/10/2017
* REVISION:
*   1.0
*
* MODIFICATION HISTORY
* --------------------
* $Log:$
*   Feng Liu 28/10/2017  Creation
*****************************************************************/

#include "bak/be_la_accproc.h"

#include <sys/types.h>
#include <stdio.h>

#include "bak/be_la_api.h"
#include "bak/be_la_common.h"
#include "be_plugin.h"
#include "common/be_la_log.h"

/******************************************************************************
* DESCRIPTION:
*       none
* INPUTS:
*       none
* OUTPUTS:
*       none
* RETURNS:
*       none
******************************************************************************/
void msg_debug_buffer_dump(char * buf MSG_ATTR_USED, unsigned int len MSG_ATTR_USED)
{
#ifdef MSG_DEBUG_ON

    uint32_t i,j,k=0;
    uint32_t row = 0;
    uint32_t colum = 4;
    char_t *cursor = buf;
    char_t padding[32] = "      "; 
    if ((NULL == buf)
        || (0 == len))
    {
        return;
    }

    MSG_DEBUG_OUTPUT("[MSG] buffer dump(pos:0x%08x lenth:%u):\r\n", buf, len);

    row = (len % (4*colum) == 0) ? (len / (4*colum)) : (len / (4*colum) + 1);

    for (i = 0; i < row; i++)
    {
        MSG_DEBUG_OUTPUT("  0x%08x:%s", cursor, padding);
        for (j = 0; j < colum; j ++)
        {
            MSG_DEBUG_OUTPUT("0x%08x", *(uint32_t *)cursor);
            cursor += 4;
            k += 4;
            MSG_DEBUG_OUTPUT("%s", padding);
            if (k >= len)
            {
                MSG_DEBUG_OUTPUT("\r\n\r\n");
                return;
            }
        }
        MSG_DEBUG_OUTPUT("\r\n");
    }
    MSG_DEBUG_OUTPUT("\r\n\r\n");
#else
    /* suppress param not use info */
#endif

    return;
}


/******************************************************************************
* DESCRIPTION:
*       none
* INPUTS:
*       none
* OUTPUTS:
*       none
* RETURNS:
*       none
******************************************************************************/
int be_la_acc_process(int vendor, int model,
                            void * inbuf, unsigned int in_len, 
                            void * outbuf, int * out_len_ptr){
    int retval = -1;
    acc_proc_msg_t * msg = NULL;
    
    if(NULL == inbuf || NULL == outbuf || NULL == out_len_ptr){
        BE_LA_ERROR("bad param");
        goto OUT;
    }

    msg = (acc_proc_msg_t *)inbuf;    
    msg_debug_buffer_dump(inbuf, in_len);
    switch(msg->msg_type){
    case MSG_TYPE_DATA:
    case MSG_TYPE_DATA_MORE:
    {
    	if(!be_plugin_is_registed(vendor,model)) {
            BE_LA_ERROR("g_acc_plugins[%d][%d].pfunc is not registed \n", vendor, model);
            goto OUT;
        }
    	retval = be_plugin_callback(vendor,model,inbuf,in_len,outbuf,out_len_ptr);
        break;
    }
    case MSG_TYPE_MAX:
    default:
        BE_LA_LOG("unsupport msg type: %d\n", msg->msg_type);
        break;        
    }
        
OUT:
    return retval;

}



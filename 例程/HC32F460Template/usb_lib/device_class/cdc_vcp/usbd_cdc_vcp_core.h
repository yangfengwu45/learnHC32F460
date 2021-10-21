/******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co.,Ltd ("HDSC").
 *
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 *
 * This software contains source code for use with HDSC
 * components. This software is licensed by HDSC to be adapted only
 * for use in systems utilizing HDSC components. HDSC shall not be
 * responsible for misuse or illegal use of this software for devices not
 * supported herein. HDSC is providing this software "AS IS" and will
 * not be responsible for issues arising from incorrect user implementation
 * of the software.
 *
 * Disclaimer:
 * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
 * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 * WARRANTY OF NONINFRINGEMENT.
 * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 * SAVINGS OR PROFITS,
 * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 * FROM, THE SOFTWARE.
 *
 * This software may be replicated in part or whole for the licensed use,
 * with the restriction that this Disclaimer and Copyright notice must be
 * included with each copy of this software, whether used in part or whole,
 * at all times.
 */
/******************************************************************************/
/** \file usbd_cdc_core.h
 **
 ** A detailed description is available at
 ** @link header file for the usbd_cdc_core.c @endlink
 **
 **   - 2019-6-3  1.0  zhangxl First version for USB CDC VCP demo.
 **
 ******************************************************************************/
/*******************************************************************************
 * Include files
 ******************************************************************************/
#ifndef __USB_CDC_CORE_H__
#define __USB_CDC_CORE_H__

#include  "usbd_ioreq.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USB_CDC_CONFIG_DESC_SIZ                (75u)
#define USB_CDC_DESC_SIZ                       (75u-9u)

#define CDC_DESCRIPTOR_TYPE                     (0x21u)

#define DEVICE_CLASS_CDC                        (0x02u)
#define DEVICE_SUBCLASS_CDC                     (0x00u)

#define STANDARD_ENDPOINT_DESC_SIZE             (0x09u)

#define CDC_DATA_IN_PACKET_SIZE                (CDC_DATA_MAX_PACKET_SIZE)

#define CDC_DATA_OUT_PACKET_SIZE               (CDC_DATA_MAX_PACKET_SIZE)

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/

/**************************************************/
/* CDC Requests                                   */
/**************************************************/
#define SEND_ENCAPSULATED_COMMAND               (0x00u)
#define GET_ENCAPSULATED_RESPONSE               (0x01u)
#define SET_COMM_FEATURE                        (0x02u)
#define GET_COMM_FEATURE                        (0x03u)
#define CLEAR_COMM_FEATURE                      (0x04u)
#define SET_LINE_CODING                         (0x20u)
#define GET_LINE_CODING                         (0x21u)
#define SET_CONTROL_LINE_STATE                  (0x22u)
#define SEND_BREAK                              (0x23u)
#define NO_CMD                                  (0xFFu)

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
typedef struct _CDC_IF_PROP
{
  uint16_t (*pIf_Init)     (void);
  uint16_t (*pIf_DeInit)   (void);
  uint16_t (*pIf_Ctrl)     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
  uint16_t (*pIf_DataTx)   (uint32_t Len);
  uint16_t (*pIf_DataRx)   (uint8_t* Buf, uint32_t Len);
}
CDC_IF_Prop_TypeDef;
extern USBD_Class_cb_TypeDef  USBD_CDC_cb;


#endif  // __USB_CDC_CORE_H__

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

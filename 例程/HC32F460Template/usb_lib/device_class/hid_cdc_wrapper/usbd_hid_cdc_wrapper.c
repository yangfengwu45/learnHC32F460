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
/** \file usbd_hid_cdc_wrapper.c
 **
 ** A detailed description is available at
 ** @link
        This file provides the CDC_HID composite functions.
  @endlink
 ** @brief   This file provides the high layer firmware functions to manage the
 **          following functionalities of the USB composite device:
 **           - Initialization and Configuration of high and low layer
 **           - Enumeration as Composite Device
 **           - OUT/IN data transfer
 **           - Command IN transfer (class requests management)
 **           - Error management
 **   - 2019-07-22  1.0  wangmin First version for USB demo.
 **
 ******************************************************************************/

/**
  ******************************************************************************
  * @verbatim
  *
  *          ===================================================================
  *                                HID Class  Description
  *          ===================================================================
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The HID protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  ******************************************************************************
  */


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usbd_hid_custom_core.h"
#include "usbd_cdc_vcp_core.h"
#include "usbd_hid_cdc_wrapper.h"
#include "usbd_desc.h"
#include "usbd_req.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USB_HID_CDC_CONFIG_DESC_SIZ  (USB_CUSTOM_HID_CONFIG_DESC_SIZ -9u + USB_CDC_CONFIG_DESC_SIZ  + 8u)

#define HID_INTERFACE       (0x0u)
#define CDC_COM_INTERFACE   (0x1u)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
/*********************************************
   CDC Device library callbacks
 *********************************************/
extern uint8_t  usbd_cdc_Init        (void  *pdev, uint8_t cfgidx);
extern uint8_t  usbd_cdc_DeInit      (void  *pdev, uint8_t cfgidx);
extern uint8_t  usbd_cdc_Setup       (void  *pdev, USB_SETUP_REQ *req);
extern uint8_t  usbd_cdc_EP0_RxReady  (void *pdev);
extern uint8_t  usbd_cdc_DataIn      (void *pdev, uint8_t epnum);
extern uint8_t  usbd_cdc_DataOut     (void *pdev, uint8_t epnum);
extern uint8_t  usbd_cdc_SOF         (void *pdev);

extern uint8_t  *USBD_cdc_GetCfgDesc (uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS
extern uint8_t  *USBD_cdc_GetOtherCfgDesc (uint8_t speed, uint16_t *length);
#endif

/*********************************************
   HID Device library callbacks
 *********************************************/
extern uint8_t  USBD_CUSTOM_HID_Init (void  *pdev,
                               uint8_t cfgidx);
extern uint8_t  USBD_CUSTOM_HID_DeInit (void  *pdev,
                                 uint8_t cfgidx);
extern uint8_t  USBD_CUSTOM_HID_Setup (void  *pdev,
                                USB_SETUP_REQ *req);
extern uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint8_t speed, uint16_t *length);
extern uint8_t  USBD_CUSTOM_HID_DataIn (void  *pdev, uint8_t epnum);
extern uint8_t  USBD_CUSTOM_HID_DataOut (void  *pdev, uint8_t epnum);

/*********************************************
   Composite Device library callbacks
 *********************************************/
static uint8_t  USBD_HID_CDC_Init         (void *pdev , uint8_t cfgidx);
static uint8_t  USBD_HID_CDC_DeInit       (void *pdev , uint8_t cfgidx);

/* Control Endpoints*/
static uint8_t  USBD_HID_CDC_Setup        (void *pdev , USB_SETUP_REQ  *req);
static uint8_t  USBD_HID_CDC_EP0_RxReady  (void *pdev );

/* Class Specific Endpoints*/
static uint8_t  USBD_HID_CDC_DataIn       (void *pdev , uint8_t epnum);
static uint8_t  USBD_HID_CDC_DataOut      (void *pdev , uint8_t epnum);
static uint8_t  USBD_HID_CDC_SOF          (void *pdev);
static uint8_t*  USBD_HID_CDC_GetConfigDescriptor( uint8_t speed , uint16_t *length);


/*******************************************************************************
 * Global variable definitions
 ******************************************************************************/

USBD_Class_cb_TypeDef  USBD_HID_CDC_cb =
{
    &USBD_HID_CDC_Init,
    &USBD_HID_CDC_DeInit,
    &USBD_HID_CDC_Setup,
    NULL,
    &USBD_HID_CDC_EP0_RxReady,
    &USBD_HID_CDC_DataIn,
    &USBD_HID_CDC_DataOut,
    &USBD_HID_CDC_SOF,
    NULL,
    NULL,
    &USBD_HID_CDC_GetConfigDescriptor,
};

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB HID/CDC device Configuration Descriptor */
__USB_ALIGN_BEGIN static uint8_t USBD_HID_CDC_CfgDesc[USB_HID_CDC_CONFIG_DESC_SIZ] __USB_ALIGN_END =
{
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    USB_HID_CDC_CONFIG_DESC_SIZ,
    /* wTotalLength: Bytes returned */
    0x00,
    0x03,         /*bNumInterfaces: 3 interfaces (2 for CDC, 1 for HID)*/
    0x01,         /*bConfigurationValue: Configuration value*/
    0x00,         /*iConfiguration: Index of string descriptor describing
    the configuration*/
    0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
    0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

    /************** Descriptor of HID interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    HID_INTERFACE,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x02,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x11,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of HID endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
    0x00,
    0x0A,          /*bInterval: Polling Interval (10 ms)*/
    /* 34 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    HID_OUT_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    HID_OUT_PACKET, /*wMaxPacketSize: 4 Byte max */
    0x00,
    0x0A,          /*bInterval: Polling Interval (10 ms)*/
    /* 41 */

    /******** /IAD should be positioned just before the CDC interfaces ******
                IAD to associate the two CDC interfaces */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x01, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x00, /* iFunction (Index of string descriptor describing this function) */

    /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    CDC_COM_INTERFACE,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x01,   /* iInterface: */

    /*Header Functional Descriptor*/
    0x05,   /* bLength: Endpoint Descriptor size */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x00,   /* bDescriptorSubtype: Header Func Desc */
    0x10,   /* bcdCDC: spec release number */
    0x01,

    /*Call Management Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x01,   /* bDescriptorSubtype: Call Management Func Desc */
    0x00,   /* bmCapabilities: D0+D1 */
    0x02,   /* bDataInterface: 2 */

    /*ACM Functional Descriptor*/
    0x04,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
    0x02,   /* bmCapabilities */

    /*Union Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x06,   /* bDescriptorSubtype: Union func desc */
    0x01,   /* bMasterInterface: Communication class interface */
    0x02,   /* bSlaveInterface0: Data Class Interface */

    /*Endpoint 2 Descriptor*/
    0x07,                           /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    CDC_CMD_EP,                     /* bEndpointAddress */
    0x03,                           /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SZE),     /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SZE),
    0xFF,                           /* bInterval: */

    /*---------------------------------------------------------------------------*/

    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x02,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x0A,   /* bInterfaceClass: CDC */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */

    /*Endpoint OUT Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
    CDC_OUT_EP,                        /* bEndpointAddress */
    0x02,                              /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00,                              /* bInterval: ignore for Bulk transfer */

    /*Endpoint IN Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
    CDC_IN_EP,                        /* bEndpointAddress */
    0x02,                             /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00,                              /* bInterval */

} ;

/**
  * @brief  USBD_HID_CDC_Init
  *         Initialize the HID & CDC interfaces
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_Init (void  *pdev,
                               uint8_t cfgidx)
{

    /* HID initialization */
    USBD_CUSTOM_HID_Init(pdev,cfgidx);

    /* CDC initialization */
    usbd_cdc_Init (pdev,cfgidx);

    return USBD_OK;

}

/**
  * @brief  USBD_HID_CDC_DeInit
  *         DeInitialize the HID/CDC interfaces
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_DeInit (void  *pdev,
                                 uint8_t cfgidx)
{
    /* HID De-initialization */
    USBD_CUSTOM_HID_DeInit (pdev,cfgidx);

    /* CDC De-initialization */
    usbd_cdc_DeInit (pdev,cfgidx);

    return USBD_OK;
}

/**
  * @brief  USBD_HID_CDC_Setup
  *         Handle the HID/CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_CDC_Setup (void  *pdev,
                                    USB_SETUP_REQ *req)
{
    uint8_t u8Res = USBD_OK;
    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK)
    {
        case USB_REQ_RECIPIENT_INTERFACE:
            if (req->wIndex == HID_INTERFACE)
            {
                u8Res = USBD_CUSTOM_HID_Setup(pdev, req);
            }
            else
            {
                u8Res = usbd_cdc_Setup(pdev, req);
            }
            break;

        case USB_REQ_RECIPIENT_ENDPOINT:
            if (req->wIndex == HID_IN_EP)
            {
                u8Res = USBD_CUSTOM_HID_Setup (pdev, req);
            }
            else
            {
                u8Res = usbd_cdc_Setup(pdev, req);
            }
            break;
        default:
            break;
    }
    return u8Res;
}

/**
  * @brief  USBD_HID_CDC_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USBD_HID_CDC_GetConfigDescriptor (uint8_t speed, uint16_t *length)
{
    *length = (uint16_t)sizeof (USBD_HID_CDC_CfgDesc);
    return USBD_HID_CDC_CfgDesc;
}

/**
  * @brief  USBD_HID_CDC_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_DataIn (void  *pdev,
                                     uint8_t epnum)
{
    uint8_t u8Ret = 0u;
    /*DataIN can be for CDC or HID */
    if (epnum == ((uint8_t)CDC_IN_EP&((uint8_t)~0x80u)))
    {
        u8Ret = (usbd_cdc_DataIn(pdev, epnum));
    }
    else
    {
        u8Ret = (USBD_CUSTOM_HID_DataIn(pdev, epnum));
    }
    return u8Ret;
}


uint8_t  USBD_HID_CDC_DataOut(void *pdev , uint8_t epnum)
{
    uint8_t u8Ret = 0u;
    /*DataOUT can be for CDC or HID */
    if (epnum == ((uint8_t)CDC_OUT_EP&(uint8_t)~0x80u) )
    {
        /*DataOut can be for CDC */
        u8Ret = usbd_cdc_DataOut(pdev, epnum);
    }
    else
    {
        /*DataOut can be for HID */
        u8Ret = USBD_CUSTOM_HID_DataOut(pdev, epnum);
    }
    return u8Ret;
}

uint8_t  USBD_HID_CDC_SOF (void *pdev)
{
    /*SOF processing needed for CDC */
    return (usbd_cdc_SOF(pdev));
}

uint8_t  USBD_HID_CDC_EP0_RxReady  (void *pdev )
{
    /*RxReady processing needed for CDC only*/
    return (usbd_cdc_EP0_RxReady(pdev));
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

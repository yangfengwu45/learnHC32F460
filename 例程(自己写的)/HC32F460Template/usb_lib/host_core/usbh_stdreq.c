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
/** \file usbh_stdreq.c
 **
 ** A detailed description is available at
 ** @link
        This file implements the standard requests for device enumeration
    @endlink
 **
 **   - 2018-12-26  1.0  wangmin First version for USB demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usbh_ioreq.h"
#include "usbh_stdreq.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__USB_ALIGN_BEGIN uint8_t          USBH_CfgDesc[512] __USB_ALIGN_END ;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void USBH_ParseDevDesc (USBH_DevDesc_TypeDef* , uint8_t *buf, uint16_t length);

static void USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                               USBH_InterfaceDesc_TypeDef* itf_desc,
                               USBH_EpDesc_TypeDef  ep_desc[][USBH_MAX_NUM_ENDPOINTS],
                               uint8_t *buf,
                               uint16_t length);
static void USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef  *if_descriptor, uint8_t *buf);
static void USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor, uint8_t *buf);
static void USBH_ParseStringDesc (uint8_t* psrc, uint8_t* pdest, uint16_t length);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  USBH_Get_DevDesc
 **         Issue Get Device Descriptor command to the device. Once the response
 **         received, it parses the device descriptor and updates the status.
 ** \param  pdev: Selected device
 ** \param  dev_desc: Device Descriptor buffer address
 ** \param  pdev->host.Rx_Buffer: Receive Buffer address
 ** \param  length: Length of the descriptor
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_Get_DevDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST *phost,
                             uint8_t length)
{
    USBH_Status status;
    status = USBH_GetDescriptor(pdev,
                                phost,
                                (uint8_t)(USB_REQ_RECIPIENT_DEVICE |USB_REQ_TYPE_STANDARD),
                                USB_DESC_DEVICE,
                                pdev->host.Rx_Buffer,
                                (uint16_t)length);
    if(status == USBH_OK)
    {
        /* Commands successfully sent and Response Received */
        USBH_ParseDevDesc(&phost->device_prop.Dev_Desc, pdev->host.Rx_Buffer, (uint16_t)length);
    }
    return status;
}

/**
 *******************************************************************************
 ** \brief  USBH_Get_CfgDesc
 **         Issues Configuration Descriptor to the device. Once the response
 **         received, it parses the configuartion descriptor and updates the
 **         status.
 ** \param  pdev: Selected device
 ** \param  cfg_desc: Configuration Descriptor address
 ** \param  itf_desc: Interface Descriptor address
 ** \param  ep_desc: Endpoint Descriptor address
 ** \param  length: Length of the descriptor
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_Get_CfgDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST           *phost,
                             uint16_t length)

{
    USBH_Status status;
    uint16_t index = 0u;

    status = USBH_GetDescriptor(pdev,
                                    phost,
                                    USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                                    USB_DESC_CONFIGURATION,
                                    pdev->host.Rx_Buffer,
                                    length);
    if(status == USBH_OK)
    {
        /*save Cfg descriptor for class parsing usage */
        for(index = 0u ; index < length ; index ++)
        {
            USBH_CfgDesc[index] = pdev->host.Rx_Buffer[index];
        }
        /* Commands successfully sent and Response Received  */
        USBH_ParseCfgDesc (&phost->device_prop.Cfg_Desc,
                            phost->device_prop.Itf_Desc,
                            phost->device_prop.Ep_Desc,
                            pdev->host.Rx_Buffer,
                            length);
    }
    return status;
}

/**
 *******************************************************************************
 ** \brief  USBH_Get_StringDesc
 **         Issues string Descriptor command to the device. Once the response
 **         received, it parses the string descriptor and updates the status.
 ** \param  pdev: Selected device
 ** \param  string_index: String index for the descriptor
 ** \param  buff: Buffer address for the descriptor
 ** \param  length: Length of the descriptor
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_Get_StringDesc(USB_OTG_CORE_HANDLE *pdev,
                                USBH_HOST *phost,
                                uint8_t string_index,
                                uint8_t *buff,
                                uint16_t length)
{
    USBH_Status status;
    status = USBH_GetDescriptor(pdev,
                                    phost,
                                    USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                                    USB_DESC_STRING | string_index,
                                    pdev->host.Rx_Buffer,
                                    length);
    if(status == USBH_OK)
    {
        /* Commands successfully sent and Response Received  */
        USBH_ParseStringDesc(pdev->host.Rx_Buffer,buff, length);
    }
    return status;
}
/**
 *******************************************************************************
 ** \brief  USBH_GetDescriptor
 **         Issues Descriptor command to the device. Once the response received,
 **         it parses the descriptor and updates the status.
 ** \param  pdev: Selected device
 ** \param  req_type: Descriptor type
 ** \param  value_idx: wValue for the GetDescriptr request
 ** \param  buff: Buffer to store the descriptor
 ** \param  length: Length of the descriptor
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_GetDescriptor(USB_OTG_CORE_HANDLE *pdev,
                               USBH_HOST           *phost,
                               uint8_t  req_type,
                               uint16_t value_idx,
                               uint8_t* buff,
                               uint16_t length )
{
    phost->Control.setup.b.bmRequestType = USB_D2H | req_type;
    phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
    phost->Control.setup.b.wValue.w = value_idx;

    if ((value_idx & 0xff00u) == USB_DESC_STRING)
    {
        phost->Control.setup.b.wIndex.w = 0x0409u;
    }
    else
    {
        phost->Control.setup.b.wIndex.w = 0u;
    }
    phost->Control.setup.b.wLength.w = length;
    return USBH_CtlReq(pdev, phost, buff , length );
}

/**
 *******************************************************************************
 ** \brief  USBH_SetAddress
 **         This command sets the address to the connected device
 ** \param  pdev: Selected device
 ** \param  DeviceAddress: Device address to assign
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_SetAddress(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST *phost,
                            uint8_t DeviceAddress)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_ADDRESS;

    phost->Control.setup.b.wValue.w = (uint16_t)DeviceAddress;
    phost->Control.setup.b.wIndex.w = 0u;
    phost->Control.setup.b.wLength.w = 0u;

    return USBH_CtlReq(pdev, phost, 0u , 0u );
}

/**
 *******************************************************************************
 ** \brief  USBH_SetCfg
 **         The command sets the configuration value to the connected device
 ** \param  pdev: Selected device
 ** \param  cfg_idx: Configuration value
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_SetCfg(USB_OTG_CORE_HANDLE *pdev,
                        USBH_HOST *phost,
                        uint16_t cfg_idx)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;
    phost->Control.setup.b.bRequest = USB_REQ_SET_CONFIGURATION;
    phost->Control.setup.b.wValue.w = cfg_idx;
    phost->Control.setup.b.wIndex.w = 0u;
    phost->Control.setup.b.wLength.w = 0u;

    return USBH_CtlReq(pdev, phost, 0u , 0u );
}

/**
 *******************************************************************************
 ** \brief  USBH_SetInterface
 **         The command sets the Interface value to the connected device
 ** \param  pdev: Selected device
 ** \param  itf_idx: Interface value
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_SetInterface(USB_OTG_CORE_HANDLE *pdev,
                        USBH_HOST *phost,
                        uint8_t ep_num, uint8_t altSetting)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_INTERFACE;
    phost->Control.setup.b.wValue.w = altSetting;
    phost->Control.setup.b.wIndex.w = ep_num;
    phost->Control.setup.b.wLength.w = 0u;

    return USBH_CtlReq(pdev, phost, 0u , 0u );
}

/**
 *******************************************************************************
 ** \brief  USBH_ClrFeature
 **         This request is used to clear or disable a specific feature.
 ** \param  pdev: Selected device
 ** \param  ep_num: endpoint number
 ** \param  hc_num: Host channel number
 ** \retval Status
 ******************************************************************************/
USBH_Status USBH_ClrFeature(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST *phost,
                            uint8_t ep_num,
                            uint8_t hc_num)
{
    phost->Control.setup.b.bmRequestType = USB_H2D |
                                            USB_REQ_RECIPIENT_ENDPOINT |
                                            USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_CLEAR_FEATURE;
    phost->Control.setup.b.wValue.w = FEATURE_SELECTOR_ENDPOINT;
    phost->Control.setup.b.wIndex.w = ep_num;
    phost->Control.setup.b.wLength.w = 0u;

    if ((ep_num & USB_REQ_DIR_MASK ) == USB_D2H)
    { /* EP Type is IN */
        pdev->host.hc[hc_num].toggle_in = 0u;
    }
    else
    {/* EP Type is OUT */
        pdev->host.hc[hc_num].toggle_out = 0u;
    }

    return USBH_CtlReq(pdev, phost, 0u , 0u );
}

/**
 *******************************************************************************
 ** \brief  USBH_ParseDevDesc
 **         This function Parses the device descriptor
 ** \param  dev_desc: device_descriptor destinaton address
 ** \param  buf: Buffer where the source descriptor is available
 ** \param  length: Length of the descriptor
 ** \retval None
 ******************************************************************************/
static void  USBH_ParseDevDesc (USBH_DevDesc_TypeDef* dev_desc,
                                uint8_t *buf,
                                uint16_t length)
{
    dev_desc->bLength            = *(uint8_t  *) (buf +  0u);
    dev_desc->bDescriptorType    = *(uint8_t  *) (buf +  1u);
    dev_desc->bcdUSB             = LE16 (buf +  2u);
    dev_desc->bDeviceClass       = *(uint8_t  *) (buf +  4u);
    dev_desc->bDeviceSubClass    = *(uint8_t  *) (buf +  5u);
    dev_desc->bDeviceProtocol    = *(uint8_t  *) (buf +  6u);
    dev_desc->bMaxPacketSize     = *(uint8_t  *) (buf +  7u);

    if (length > (uint16_t)8)
    { /* For 1st time after device connection, Host may issue only 8 bytes for
        Device Descriptor Length  */
        dev_desc->idVendor           = LE16 (buf +  8u);
        dev_desc->idProduct          = LE16 (buf + 10u);
        dev_desc->bcdDevice          = LE16 (buf + 12u);
        dev_desc->iManufacturer      = *(uint8_t  *) (buf + 14u);
        dev_desc->iProduct           = *(uint8_t  *) (buf + 15u);
        dev_desc->iSerialNumber      = *(uint8_t  *) (buf + 16u);
        dev_desc->bNumConfigurations = *(uint8_t  *) (buf + 17u);
    }
}

/**
 *******************************************************************************
 ** \brief  USBH_ParseCfgDesc
 **         This function Parses the configuration descriptor
 ** \param  cfg_desc: Configuration Descriptor address
 ** \param  itf_desc: Interface Descriptor address
 ** \param  ep_desc: Endpoint Descriptor address
 ** \param  buf: Buffer where the source descriptor is available
 ** \param  length: Length of the descriptor
 ** \retval None
 ******************************************************************************/
static void  USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                                USBH_InterfaceDesc_TypeDef* itf_desc,
                                USBH_EpDesc_TypeDef   ep_desc[][USBH_MAX_NUM_ENDPOINTS],
                                uint8_t *buf,
                                uint16_t length)
{
    USBH_InterfaceDesc_TypeDef    *pif ;
    USBH_InterfaceDesc_TypeDef    temp_pif ;
    USBH_EpDesc_TypeDef           *pep;
    USBH_DescHeader_t             *pdesc = (USBH_DescHeader_t *)buf;
    uint16_t                      ptr;
    int8_t                        if_ix = (int8_t)0;
    int8_t                        ep_ix = (int8_t)0;
    static uint16_t               prev_ep_size = 0u;
    static uint8_t                prev_itf = 0u;

    pdesc   = (USBH_DescHeader_t *)buf;

    /* Parse configuration descriptor */
    cfg_desc->bLength             = *(uint8_t  *) (buf + 0u);
    cfg_desc->bDescriptorType     = *(uint8_t  *) (buf + 1u);
    cfg_desc->wTotalLength        = LE16 (buf + 2u);
    cfg_desc->bNumInterfaces      = *(uint8_t  *) (buf + 4u);
    cfg_desc->bConfigurationValue = *(uint8_t  *) (buf + 5u);
    cfg_desc->iConfiguration      = *(uint8_t  *) (buf + 6u);
    cfg_desc->bmAttributes        = *(uint8_t  *) (buf + 7u);
    cfg_desc->bMaxPower           = *(uint8_t  *) (buf + 8u);

    if (length > USB_CONFIGURATION_DESC_SIZE)
    {
        ptr = USB_LEN_CFG_DESC;

        if ( cfg_desc->bNumInterfaces <= USBH_MAX_NUM_INTERFACES)
        {
            pif = (USBH_InterfaceDesc_TypeDef *)0u;

            while (ptr < cfg_desc->wTotalLength )
            {
                pdesc = USBH_GetNextDesc((uint8_t *)pdesc, &ptr);
                if (pdesc->bDescriptorType   == USB_DESC_TYPE_INTERFACE)
                {
                    if_ix             = (int8_t)*(((uint8_t *)pdesc ) + 2u);
                    pif               = &itf_desc[if_ix];

                    if((*((uint8_t *)pdesc + 3u)) < 3u)
                    {
                        USBH_ParseInterfaceDesc (&temp_pif, (uint8_t *)pdesc);
                        ep_ix = (int8_t)0;

                        /* Parse Ep descriptors relative to the current interface */
                        if(temp_pif.bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS)
                        {
                            while (ep_ix < (int8_t)temp_pif.bNumEndpoints)
                            {
                                pdesc = USBH_GetNextDesc((void* )pdesc, &ptr);
                                if (pdesc->bDescriptorType   == USB_DESC_TYPE_ENDPOINT)
                                {
                                    pep               = &ep_desc[if_ix][ep_ix];

                                    if(prev_itf != (uint8_t)if_ix)
                                    {
                                        prev_itf = (uint8_t)if_ix;
                                        USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif);
                                    }
                                    else
                                    {
                                        if(prev_ep_size > LE16((uint8_t *)pdesc + 4u))
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif);
                                        }
                                    }
                                    USBH_ParseEPDesc (pep, (uint8_t *)pdesc);
                                    prev_ep_size = LE16((uint8_t *)pdesc + 4u);
                                    ep_ix++;
                                }
                            }
                        }
                    }
                }
            }
        }
        prev_ep_size = 0u;
        prev_itf = 0u;
    }
}


/**
 *******************************************************************************
 ** \brief  USBH_ParseInterfaceDesc
 **         This function Parses the interface descriptor
 ** \param  if_descriptor : Interface descriptor destination
 ** \param  buf: Buffer where the descriptor data is available
 ** \retval None
 ******************************************************************************/
static void  USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef *if_descriptor,
                                      uint8_t *buf)
{
    if_descriptor->bLength            = *(uint8_t  *) (buf + 0u);
    if_descriptor->bDescriptorType    = *(uint8_t  *) (buf + 1u);
    if_descriptor->bInterfaceNumber   = *(uint8_t  *) (buf + 2u);
    if_descriptor->bAlternateSetting  = *(uint8_t  *) (buf + 3u);
    if_descriptor->bNumEndpoints      = *(uint8_t  *) (buf + 4u);
    if_descriptor->bInterfaceClass    = *(uint8_t  *) (buf + 5u);
    if_descriptor->bInterfaceSubClass = *(uint8_t  *) (buf + 6u);
    if_descriptor->bInterfaceProtocol = *(uint8_t  *) (buf + 7u);
    if_descriptor->iInterface         = *(uint8_t  *) (buf + 8u);
}

/**
 *******************************************************************************
 ** \brief  USBH_ParseEPDesc
 **         This function Parses the endpoint descriptor
 ** \param  ep_descriptor: Endpoint descriptor destination address
 ** \param  buf: Buffer where the parsed descriptor stored
 ** \retval None
 ******************************************************************************/
static void  USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor,
                               uint8_t *buf)
{
    ep_descriptor->bLength          = *(uint8_t  *) (buf + 0u);
    ep_descriptor->bDescriptorType  = *(uint8_t  *) (buf + 1u);
    ep_descriptor->bEndpointAddress = *(uint8_t  *) (buf + 2u);
    ep_descriptor->bmAttributes     = *(uint8_t  *) (buf + 3u);
    ep_descriptor->wMaxPacketSize   = LE16(buf + 4u);
    ep_descriptor->bInterval        = *(uint8_t  *) (buf + 6u);
}

/**
 *******************************************************************************
 ** \brief  USBH_ParseStringDesc
 **         This function Parses the string descriptor
 ** \param  psrc: Source pointer containing the descriptor data
 ** \param  pdest: Destination address pointer
 ** \param  length: Length of the descriptor
 ** \retval None
 ******************************************************************************/
static void USBH_ParseStringDesc (uint8_t* psrc,
                                  uint8_t* pdest,
                                  uint16_t length)
{
    uint16_t strlength;
    uint16_t idx;

    /* The UNICODE string descriptor is not NULL-terminated. The string length is
    computed by substracting two from the value of the first byte of the descriptor.
    */

    /* Check which is lower size, the Size of string or the length of bytes read
    from the device */
    if ( psrc[1] == USB_DESC_TYPE_STRING)
    {   /* Make sure the Descriptor is String Type */

        /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */
        strlength = (( ((uint16_t)psrc[0])-2u) <= length) ? (((uint16_t)psrc[0])-2u) : length;
        psrc += 2u; /* Adjust the offset ignoring the String Len and Descriptor type */

        for (idx = 0u; idx < strlength; idx+=2u )
        {/* Copy Only the string and ignore the UNICODE ID, hence add the src */
            *pdest =  psrc[idx];
            pdest++;
        }
        *pdest = 0u; /* mark end of string */
    }
}

/**
 *******************************************************************************
 ** \brief  USBH_GetNextDesc
 **         This function return the next descriptor header
 ** \param  buf: Buffer where the cfg descriptor is available
 ** \param  ptr: data popinter inside the cfg descriptor
 ** \retval next header
 ******************************************************************************/
USBH_DescHeader_t  *USBH_GetNextDesc (uint8_t   *pbuf, uint16_t  *ptr)
{
    USBH_DescHeader_t  *pnext;

    *ptr += ((USBH_DescHeader_t *)pbuf)->bLength;
    pnext = (USBH_DescHeader_t *)((uint8_t *)pbuf + ((USBH_DescHeader_t *)pbuf)->bLength);

    return(pnext);
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

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
/** \file usbh_msc_core.c
 **
 ** A detailed description is available at
 ** @link
        This file implements the MSC class driver functions
    @endlink
 **
 **   - 2018-12-26  1.0  wangmin First version for USB demo.
 **
 ******************************************************************************/
/**
  ******************************************************************************
  *          ===================================================================
  *                                MSC Class  Description
  *          ===================================================================
  *           This module manages the MSC class V1.0 following the "Universal
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *
  *  @endverbatim
  *
  ******************************************************************************
  */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usbh_core.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static USBH_Status USBH_MSC_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev ,
                                            void *phost);
static void USBH_MSC_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev ,
                                       void *phost);
static USBH_Status USBH_MSC_Handle(USB_OTG_CORE_HANDLE *pdev ,
                            void *phost);
static USBH_Status USBH_MSC_ClassRequest(USB_OTG_CORE_HANDLE *pdev ,
                                         void *phost);
static USBH_Status USBH_MSC_BOTReset(USB_OTG_CORE_HANDLE *pdev,
                              USBH_HOST *phost);
static USBH_Status USBH_MSC_GETMaxLUN(USB_OTG_CORE_HANDLE *pdev,
                               USBH_HOST *phost);
static void USBH_MSC_ErrorHandle(uint8_t status);

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USBH_MSC_ERROR_RETRY_LIMIT 10u
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__USB_ALIGN_BEGIN MSC_Machine_TypeDef         MSC_Machine __USB_ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__USB_ALIGN_BEGIN USB_Setup_TypeDef           MSC_Setup __USB_ALIGN_END ;
uint8_t MSCErrorCount = 0u;

USBH_Class_cb_TypeDef  USBH_MSC_cb =
{
    &USBH_MSC_InterfaceInit,
    &USBH_MSC_InterfaceDeInit,
    &USBH_MSC_ClassRequest,
    &USBH_MSC_Handle,
};

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  USBH_MSC_InterfaceInit
 **         Interface initialization for MSC class.
 ** \param  pdev: Selected device
 ** \param  hdev: Selected device property
 ** \retval USBH_Status : Status of class request handled.
 ******************************************************************************/
static USBH_Status USBH_MSC_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev,
                                        void *phost)
{
    USBH_HOST *pphost = phost;

    if((pphost->device_prop.Itf_Desc[0].bInterfaceClass == MSC_CLASS) && \
        (pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == MSC_PROTOCOL))
    {
        if(pphost->device_prop.Ep_Desc[0][0].bEndpointAddress & (uint8_t)0x80)
        {
            MSC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
            MSC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
        }
        else
        {
            MSC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
            MSC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0] [0].wMaxPacketSize;
        }

        if(pphost->device_prop.Ep_Desc[0][1].bEndpointAddress & (uint8_t)0x80)
        {
            MSC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
            MSC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
        }
        else
        {
            MSC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
            MSC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
        }

        MSC_Machine.hc_num_out = USBH_Alloc_Channel(pdev,
                                                    MSC_Machine.MSBulkOutEp);
        MSC_Machine.hc_num_in = USBH_Alloc_Channel(pdev,
                                                    MSC_Machine.MSBulkInEp);
        /* Open the new channels */
        USBH_Open_Channel  (pdev,
                            MSC_Machine.hc_num_out,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EP_TYPE_BULK,
                            MSC_Machine.MSBulkOutEpSize);

        USBH_Open_Channel  (pdev,
                            MSC_Machine.hc_num_in,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EP_TYPE_BULK,
                            MSC_Machine.MSBulkInEpSize);
    }
    else
    {
        pphost->usr_cb->DeviceNotSupported();
    }
    return USBH_OK ;
}

/**
 *******************************************************************************
 ** \brief  USBH_MSC_InterfaceDeInit
 **         De-Initialize interface by freeing host channels allocated to interface
 ** \param  pdev: Selected device
 ** \param  hdev: Selected device property
 ** \retval None
 ******************************************************************************/
void USBH_MSC_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
                                void *phost)
{
    if ( MSC_Machine.hc_num_out)
    {
        USB_OTG_HC_Halt(pdev, MSC_Machine.hc_num_out);
        USBH_Free_Channel  (pdev, MSC_Machine.hc_num_out);
        MSC_Machine.hc_num_out = (uint8_t)0;     /* Reset the Channel as Free */
    }
    if ( MSC_Machine.hc_num_in)
    {
        USB_OTG_HC_Halt(pdev, MSC_Machine.hc_num_in);
        USBH_Free_Channel  (pdev, MSC_Machine.hc_num_in);
        MSC_Machine.hc_num_in = (uint8_t)0;     /* Reset the Channel as Free */
    }
}

/**
 *******************************************************************************
 ** \brief  USBH_MSC_ClassRequest
 **         This function will only initialize the MSC state machine
 ** \param  pdev: Selected device
 ** \param  hdev: Selected device property
 ** \retval USBH_Status : Status of class request handled.
 ******************************************************************************/
static USBH_Status USBH_MSC_ClassRequest(USB_OTG_CORE_HANDLE *pdev ,
                                        void *phost)
{
    USBH_Status status = USBH_OK ;
    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOT_INIT_STATE;

    return status;
}


/**
 *******************************************************************************
 ** \brief  USBH_MSC_Handle
 **         MSC state machine handler
 ** \param  pdev: Selected device
 ** \param  hdev: Selected device property
 ** \retval USBH_Status
 ******************************************************************************/
static USBH_Status USBH_MSC_Handle(USB_OTG_CORE_HANDLE *pdev ,
                                   void   *phost)
{
    USBH_HOST *pphost = phost;

    USBH_Status status = USBH_BUSY;
    uint8_t mscStatus = USBH_MSC_BUSY;
    uint8_t appliStatus = 0u;

    static uint8_t maxLunExceed = FALSE;

    if(HCD_IsDeviceConnected(pdev))
    {
        switch(USBH_MSC_BOTXferParam.MSCState)
        {
            case USBH_MSC_BOT_INIT_STATE:
                USBH_MSC_Init(pdev);
                USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOT_RESET;
                break;
            case USBH_MSC_BOT_RESET:
                /* Issue BOT RESET request */
                status = USBH_MSC_BOTReset(pdev, phost);
                if(status == USBH_OK )
                {
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_GET_MAX_LUN;
                }
                if(status == USBH_NOT_SUPPORTED )
                {
                    /* If the Command has failed, then we need to move to Next State, after
                    STALL condition is cleared by Control-Transfer */
                    USBH_MSC_BOTXferParam.MSCStateBkp = USBH_MSC_GET_MAX_LUN;
                    /* a Clear Feature should be issued here */
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_CTRL_ERROR_STATE;
                }
                break;
            case USBH_MSC_GET_MAX_LUN:
                /* Issue GetMaxLUN request */
                status = USBH_MSC_GETMaxLUN(pdev, phost);
                if(status == USBH_OK )
                {
                    MSC_Machine.maxLun = *(MSC_Machine.buff) ;
                    /* If device has more that one logical unit then it is not supported */
                    if((MSC_Machine.maxLun > (uint8_t)0) && (maxLunExceed == FALSE))
                    {
                        maxLunExceed = TRUE;
                        pphost->usr_cb->DeviceNotSupported();
                        break;
                    }
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_TEST_UNIT_READY;
                }

                if(status == USBH_NOT_SUPPORTED )
                {
                    /* If the Command has failed, then we need to move to Next State, after
                    STALL condition is cleared by Control-Transfer */
                    USBH_MSC_BOTXferParam.MSCStateBkp = USBH_MSC_TEST_UNIT_READY;

                    /* a Clear Feature should be issued here */
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_CTRL_ERROR_STATE;
                }
                break;
            case USBH_MSC_CTRL_ERROR_STATE:
                /* Issue Clearfeature request */
                status = USBH_ClrFeature(pdev,
                                        phost,
                                        0x00u,
                                        pphost->Control.hc_num_out);
                if(status == USBH_OK )
                {
                    /* If GetMaxLun Request not support, assume Single LUN configuration */
                    MSC_Machine.maxLun = (uint8_t)0;
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOTXferParam.MSCStateBkp;
                }
                break;
            case USBH_MSC_TEST_UNIT_READY:
                /* Issue SCSI command TestUnitReady */
                mscStatus = USBH_MSC_TestUnitReady(pdev);

                if(mscStatus == USBH_MSC_OK )
                {
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_READ_CAPACITY10;
                    MSCErrorCount = (uint8_t)0;
                    status = USBH_OK;
                }
                else
                {
                    USBH_MSC_ErrorHandle(mscStatus);
                }
                break;

            case USBH_MSC_READ_CAPACITY10:
                /* Issue READ_CAPACITY10 SCSI command */
                mscStatus = USBH_MSC_ReadCapacity10(pdev);
                if(mscStatus == USBH_MSC_OK )
                {
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_MODE_SENSE6;
                    MSCErrorCount = (uint8_t)0;
                    status = USBH_OK;
                }
                else
                {
                    USBH_MSC_ErrorHandle(mscStatus);
                }
                break;
            case USBH_MSC_MODE_SENSE6:
                /* Issue ModeSense6 SCSI command for detecting if device is write-protected */
                mscStatus = USBH_MSC_ModeSense6(pdev);
                if(mscStatus == USBH_MSC_OK )
                {
                    USBH_MSC_BOTXferParam.MSCState = USBH_MSC_DEFAULT_APPLI_STATE;
                    MSCErrorCount = (uint8_t)0;
                    status = USBH_OK;
                }
                else
                {
                    USBH_MSC_ErrorHandle(mscStatus);
                }
                break;
            case USBH_MSC_REQUEST_SENSE:
                /* Issue RequestSense SCSI command for retreiving error code */
                mscStatus = USBH_MSC_RequestSense(pdev);
                if(mscStatus == USBH_MSC_OK )
                {
                USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOTXferParam.MSCStateBkp;
                status = USBH_OK;
                }
                else
                {
                USBH_MSC_ErrorHandle(mscStatus);
                }
                break;
            case USBH_MSC_BOT_USB_TRANSFERS:
                /* Process the BOT state machine */
                USBH_MSC_HandleBOTXfer(pdev , phost);
                break;
            case USBH_MSC_DEFAULT_APPLI_STATE:
                /* Process Application callback for MSC */
                appliStatus = pphost->usr_cb->UserApplication();
                if(appliStatus == (uint8_t)0)
                {
                USBH_MSC_BOTXferParam.MSCState = USBH_MSC_DEFAULT_APPLI_STATE;
                }
                else if (appliStatus == (uint8_t)1)
                {
                /* De-init requested from application layer */
                status =  USBH_APPLY_DEINIT;
                }
                else
                {
                    //
                }
                break;
            case USBH_MSC_UNRECOVERED_STATE:
                status = USBH_UNRECOVERED_ERROR;
                break;
            default:
                break;
        }
    }
    return status;
}

/**
 *******************************************************************************
 ** \brief  USBH_MSC_BOTReset
 **         This request is used to reset the mass storage device and its
 **         associated interface. This class-specific request shall ready the
 **         device for the next CBW from the host.
 ** \param  pdev: Selected device
 ** \retval USBH_Status : Status of class request handled.
 ******************************************************************************/
static USBH_Status USBH_MSC_BOTReset(USB_OTG_CORE_HANDLE *pdev,
                              USBH_HOST *phost)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
    phost->Control.setup.b.bRequest = USB_REQ_BOT_RESET;
    phost->Control.setup.b.wValue.w = 0u;
    phost->Control.setup.b.wIndex.w = 0u;
    phost->Control.setup.b.wLength.w = 0u;

    return USBH_CtlReq(pdev, phost, 0u , 0u );
}

/**
 *******************************************************************************
 ** \brief  USBH_MSC_GETMaxLUN
 **         This request is used to reset the mass storage device and its
 **         associated interface. This class-specific request shall ready the
 **         device for the next CBW from the host.
 ** \param  pdev: Selected device
 ** \retval USBH_Status : USB ctl xfer status
 ******************************************************************************/
static USBH_Status USBH_MSC_GETMaxLUN(USB_OTG_CORE_HANDLE *pdev , USBH_HOST *phost)
{
    phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | \
                                            USB_REQ_RECIPIENT_INTERFACE;

    phost->Control.setup.b.bRequest = USB_REQ_GET_MAX_LUN;
    phost->Control.setup.b.wValue.w = 0u;
    phost->Control.setup.b.wIndex.w = 0u;
    phost->Control.setup.b.wLength.w = 1u;

    return USBH_CtlReq(pdev, phost, MSC_Machine.buff , 1u );
}

/**
 *******************************************************************************
 ** \brief  USBH_MSC_ErrorHandle
 **         The function is for handling errors occuring during the MSC
 **         state machine
 ** \param  status
 ** \retval None
 ******************************************************************************/
void USBH_MSC_ErrorHandle(uint8_t status)
{
    if(status == USBH_MSC_FAIL)
    {
        MSCErrorCount++;
        if(MSCErrorCount < USBH_MSC_ERROR_RETRY_LIMIT)
        { /* Try MSC level error recovery, Issue the request Sense to get
            Drive error reason  */
            USBH_MSC_BOTXferParam.MSCState = USBH_MSC_REQUEST_SENSE;
            USBH_MSC_BOTXferParam.CmdStateMachine = CMD_SEND_STATE;
        }
        else
        {
            /* Error trials exceeded the limit, go to unrecovered state */
            USBH_MSC_BOTXferParam.MSCState = USBH_MSC_UNRECOVERED_STATE;
        }
    }
    else if(status == USBH_MSC_PHASE_ERROR)
    {
        /* Phase error, Go to Unrecoovered state */
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_UNRECOVERED_STATE;
    }
    //else if(status == USBH_MSC_BUSY)   /* MISRAC 2004*/
    else
    {
        /*No change in state*/
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

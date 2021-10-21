/******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
/** \file usbh_msc_scsi.h
 **
 ** A detailed description is available at
 ** @link
        Header file for usbh_msc_scsi.c
    @endlink
 **
 **   - 2018-12-26  1.0  wangmin First version for USB demo.
 **
 ******************************************************************************/
#ifndef __USBH_MSC_SCSI_H__
#define __USBH_MSC_SCSI_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usbh_stdreq.h"


/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief USBH_MSC_SCSI_Exported_Types
 **
 ******************************************************************************/
typedef enum {
    USBH_MSC_OK = 0u,
    USBH_MSC_FAIL = 1u,
    USBH_MSC_PHASE_ERROR = 2u,
    USBH_MSC_BUSY = 3u
}USBH_MSC_Status_TypeDef;

typedef enum {
    CMD_UNINITIALIZED_STATE =0,
    CMD_SEND_STATE,
    CMD_WAIT_STATUS
} CMD_STATES_TypeDef;

typedef struct __MassStorageParameter
{
    uint32_t MSCapacity;
    uint32_t MSSenseKey;
    uint16_t MSPageLength;
    uint8_t MSBulkOutEp;
    uint8_t MSBulkInEp;
    uint8_t MSWriteProtect;
} MassStorageParameter_TypeDef;

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define OPCODE_TEST_UNIT_READY            (0x00u)
#define OPCODE_READ_CAPACITY10            (0x25u)
#define OPCODE_MODE_SENSE6                (0x1Au)
#define OPCODE_READ10                     (0x28u)
#define OPCODE_WRITE10                    (0x2Au)
#define OPCODE_REQUEST_SENSE              (0x03u)

#define DESC_REQUEST_SENSE                (0x00u)
#define ALLOCATION_LENGTH_REQUEST_SENSE   (63u)
#define XFER_LEN_READ_CAPACITY10          (8u)
#define XFER_LEN_MODE_SENSE6              (63u)

#define MASK_MODE_SENSE_WRITE_PROTECT     (0x80u)
#define MODE_SENSE_PAGE_CONTROL_FIELD     (0x00u)
#define MODE_SENSE_PAGE_CODE              (0x3Fu)
#define DISK_WRITE_PROTECTED              (0x01u)

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/
extern MassStorageParameter_TypeDef USBH_MSC_Param;

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
uint8_t USBH_MSC_TestUnitReady(USB_OTG_CORE_HANDLE *pdev);
uint8_t USBH_MSC_ReadCapacity10(USB_OTG_CORE_HANDLE *pdev);
uint8_t USBH_MSC_ModeSense6(USB_OTG_CORE_HANDLE *pdev);
uint8_t USBH_MSC_RequestSense(USB_OTG_CORE_HANDLE *pdev);
uint8_t USBH_MSC_Write10(USB_OTG_CORE_HANDLE *pdev,
                         uint8_t *dataBuffer,
                         uint32_t address,
                         uint32_t nbOfbytes);
uint8_t USBH_MSC_Read10(USB_OTG_CORE_HANDLE *pdev,
                        uint8_t *dataBuffer,
                        uint32_t address,
                        uint32_t nbOfbytes);
void USBH_MSC_StateMachine(USB_OTG_CORE_HANDLE *pdev);

#endif  //__USBH_MSC_SCSI_H__

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



/******************************************************************************
* File Name: ble_findme.c
*
* Description: This file contains BLE related functions.
*
* Related Document: README.md
*
*******************************************************************************
* Copyright 2019-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/


/******************************************************************************
 * Include header files
 *****************************************************************************/
#include "ble_findme.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cycfg_ble.h"


/*******************************************************************************
* Macros
********************************************************************************/
#define BLESS_INTR_PRIORITY       (1u)
#define WAKEUP_INTR_PRIORITY      (7u)
#define WAKEUP_TIMER_DELAY_MS     (250)
/* Timer value to get 0.25 sec with wakeup timer input clock of 32768 Hz,
 * where 32768Hz is LFCLK in PSoC 6 MCU */
#define WAKEUP_TIMER_MATCH_VALUE  (WAKEUP_TIMER_DELAY_MS * 32768 / 1000)


/*******************************************************************************
* Global Variables
********************************************************************************/
cyhal_lptimer_t wakeup_timer;
bool wakeup_intr_flag = false;
bool gpio_intr_flag = false;
uint8 alert_level = CY_BLE_NO_ALERT;
cy_stc_ble_conn_handle_t app_conn_handle;


/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void ble_init(void);
static void bless_interrupt_handler(void);
static void stack_event_handler(uint32 event, void* eventParam);
static void ble_start_advertisement(void);
static void wakeup_timer_init(void);
static void wakeup_timer_interrupt_handler(void *handler_arg, cyhal_lptimer_event_t event);
static void ble_ias_callback(uint32 event, void *eventParam);
static void enter_low_power_mode(void);


/*******************************************************************************
* Function Name: ble_findme_init
********************************************************************************
* Summary:
* This function initializes the BLE and a deep sleep wakeup timer.
*
*******************************************************************************/
void ble_findme_init(void)
{
    /* Configure BLE */
    ble_init();

    /* Configure deep sleep wakeup timer */
    wakeup_timer_init();

    /* Enable global interrupts */
    __enable_irq();
}


/*******************************************************************************
* Function Name: ble_findme_process
********************************************************************************
* Summary:
*  This function processes the BLE events and configures the device to enter
*  low power mode as required.
*
*******************************************************************************/
void ble_findme_process(void)
{
    /* Enter low power mode. The call to enter_low_power_mode also causes the
     * device to enter hibernate mode if the BLE stack is shutdown.
     */
    enter_low_power_mode();

    /* Cy_BLE_ProcessEvents() allows the BLE stack to process pending events */
    Cy_BLE_ProcessEvents();

    if(wakeup_intr_flag)
    {
        wakeup_intr_flag = false;

        /* Update CYBSP_USER_LED1 to indicate current BLE status */
        if(CY_BLE_ADV_STATE_ADVERTISING == Cy_BLE_GetAdvertisementState())
        {
            cyhal_gpio_toggle((cyhal_gpio_t)CYBSP_USER_LED1);
        }
        else if(CY_BLE_CONN_STATE_CONNECTED == Cy_BLE_GetConnectionState(app_conn_handle))
        {
            cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED1, CYBSP_LED_STATE_ON);
        }
        else
        {
            cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED1, CYBSP_LED_STATE_OFF);
        }

        /* Update CYBSP_USER_LED2 to indicate current alert level */
        switch(alert_level)
        {
            case CY_BLE_NO_ALERT:
            {
                cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_OFF);
                break;
            }
            case CY_BLE_MILD_ALERT:
            {
                cyhal_gpio_toggle((cyhal_gpio_t)CYBSP_USER_LED2);
                break;
            }
            case CY_BLE_HIGH_ALERT:
            {
                cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_ON);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}


/*******************************************************************************
* Function Name: ble_init
********************************************************************************
* Summary:
*  This function initializes the BLE and registers IAS callback function.
*
*******************************************************************************/
static void ble_init(void)
{
    static const cy_stc_sysint_t bless_isr_config =
    {
      /* The BLESS interrupt */
      .intrSrc = bless_interrupt_IRQn,

      /* The interrupt priority number */
      .intrPriority = BLESS_INTR_PRIORITY
    };

    /* Hook interrupt service routines for BLESS */
    (void) Cy_SysInt_Init(&bless_isr_config, bless_interrupt_handler);

    /* Store the pointer to blessIsrCfg in the BLE configuration structure */
    cy_ble_config.hw->blessIsrConfig = &bless_isr_config;

    /* Registers the generic callback functions  */
    Cy_BLE_RegisterEventCallback(stack_event_handler);

    /* Initializes the BLE host */
    Cy_BLE_Init(&cy_ble_config);

    /* Enables BLE */
    Cy_BLE_Enable();

    /* Enables BLE Low-power mode (LPM)*/
    Cy_BLE_EnableLowPowerMode();

    /* Register IAS event handler */
    Cy_BLE_IAS_RegisterAttrCallback(ble_ias_callback);
}


/******************************************************************************
* Function Name: bless_interrupt_handler
*******************************************************************************
* Summary:
*  Wrapper function for handling interrupts from BLESS.
*
******************************************************************************/
static void bless_interrupt_handler(void)
{
    Cy_BLE_BlessIsrHandler();
}


/*******************************************************************************
* Function Name: stack_event_handler
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  uint32 event:      event from the BLE component
*  void* eventParam:  parameters related to the event
*
*******************************************************************************/
static void stack_event_handler(uint32_t event, void* eventParam)
{
    switch(event)
    {
        /**********************************************************************
         * General events
         *********************************************************************/

        /* This event is received when the BLE stack is started */
        case CY_BLE_EVT_STACK_ON:
        {
            printf("[INFO] : BLE stack started \r\n");
            ble_start_advertisement();
            break;
        }

        /* This event is received when there is a timeout */
        case CY_BLE_EVT_TIMEOUT:
        {
            /* Reason for Timeout */
            cy_en_ble_to_reason_code_t reason_code =
                ((cy_stc_ble_timeout_param_t*)eventParam)->reasonCode;

            switch(reason_code)
            {
                case CY_BLE_GAP_ADV_TO:
                {
                    printf("[INFO] : Advertisement timeout event \r\n");
                    break;
                }
                case CY_BLE_GATT_RSP_TO:
                {
                    printf("[INFO] : GATT response timeout\r\n");
                    break;
                }
                default:
                {
                    printf("[INFO] : BLE timeout event\r\n");
                    break;
                }
            }
            break;
        }

        /* This event indicates completion of Set LE event mask */
        case CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE:
        {
            printf("[INFO] : Set LE mask event mask command completed\r\n");
            break;
        }

        /* This event indicates set device address command completed */
        case CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE:
        {
            printf("[INFO] : Set device address command has completed \r\n");
            break;
        }

        /* This event indicates set Tx Power command completed */
        case CY_BLE_EVT_SET_TX_PWR_COMPLETE:
        {
            printf("[INFO] : Set Tx power command completed\r\n");
            break;
        }

        /* This event indicates BLE Stack Shutdown is completed */
        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
        {
            printf("[INFO] : BLE shutdown complete\r\n");
            break;
        }


        /**********************************************************************
         * GAP events
         *********************************************************************/

        /* This event is generated at the GAP Peripheral end after connection
         * is completed with peer Central device
         */
        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
        {
            printf("[INFO] : GAP device connected \r\n");
            break;
        }
        /* This event is triggered instead of 'CY_BLE_EVT_GAP_DEVICE_CONNECTED',
         * if Link Layer Privacy is enabled in component customizer
         */
        case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
        {
            printf("[INFO] : GAP enhanced connection complete \r\n");
            break;
        }

        /* This event is generated when disconnected from remote device or
         * failed to establish connection
         */
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            if(CY_BLE_CONN_STATE_DISCONNECTED ==
               Cy_BLE_GetConnectionState(app_conn_handle))
            {
                printf("[INFO] : GAP device disconnected\r\n");
                alert_level = CY_BLE_NO_ALERT;
                ble_start_advertisement();
            }
            break;
        }

        /* This event indicates that the GAP Peripheral device has
         * started/stopped advertising
         */
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        {
            if(CY_BLE_ADV_STATE_ADVERTISING == Cy_BLE_GetAdvertisementState())
            {
                printf("[INFO] : BLE advertisement started\r\n");
            }
            else
            {
                printf("[INFO] : BLE advertisement stopped\r\n");

                Cy_BLE_Disable();
            }
            break;
        }


        /**********************************************************************
         * GATT events
         *********************************************************************/

        /* This event is generated at the GAP Peripheral end after connection
         * is completed with peer Central device
         */
        case CY_BLE_EVT_GATT_CONNECT_IND:
        {
            app_conn_handle = *(cy_stc_ble_conn_handle_t *)eventParam;
            printf("[INFO] : GATT device connected\r\n");
            break;
        }

        /* This event is generated at the GAP Peripheral end after disconnection */
        case CY_BLE_EVT_GATT_DISCONNECT_IND:
        {
            printf("[INFO] : GATT device disconnected\r\n");
            break;
        }

        /* This event indicates that the 'GATT MTU Exchange Request' is received */
        case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
        {
            printf("[INFO] : GATT MTU Exchange Request received \r\n");
            break;
        }

        /* This event received when GATT read characteristic request received */
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
        {
            printf("[INFO] : GATT read characteristic request received \r\n");
            break;
        }

        default:
        {
            printf("[INFO] : BLE Event 0x%lX\r\n", (unsigned long) event);
        }
    }
}


/*******************************************************************************
* Function Name: ble_ias_callback
********************************************************************************
* Summary:
*  This is an event callback function to receive events from the BLE, which are
*  specific to Immediate Alert Service.
*
* Parameters:
*  uint32 event:      event from the BLE component
*  void* eventParams: parameters related to the event
*
*******************************************************************************/
void ble_ias_callback(uint32 event, void *eventParam)
{
    /* Alert Level Characteristic write event */
    if(event == CY_BLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        /* Read the updated Alert Level value from the GATT database */
        Cy_BLE_IASS_GetCharacteristicValue(CY_BLE_IAS_ALERT_LEVEL,
                                           sizeof(alert_level), &alert_level);
    }

    /* Remove warning for unused parameter */
    (void)eventParam;
}


/******************************************************************************
* Function Name: ble_start_advertisement
*******************************************************************************
* Summary:
*  This function starts the advertisement.
*
******************************************************************************/
static void ble_start_advertisement(void)
{
    cy_en_ble_api_result_t ble_api_result;

    if((CY_BLE_ADV_STATE_ADVERTISING != Cy_BLE_GetAdvertisementState()) &&
       (Cy_BLE_GetNumOfActiveConn() < CY_BLE_CONN_COUNT))
    {
        ble_api_result = Cy_BLE_GAPP_StartAdvertisement(
                            CY_BLE_ADVERTISING_FAST,
                            CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);

        if(CY_BLE_SUCCESS != ble_api_result)
        {
            printf("[ERROR] : Failed to start advertisement \r\n");
        }
    }
}


/*******************************************************************************
* Function Name: wakeup_timer_interrupt_handler
********************************************************************************
* Summary:
*  wakeup_timer interrupt handler.
*
* Parameters:
*  void *handler_arg (unused)
*  cyhal_lptimer_irq_event_t event (unused)
*
*******************************************************************************/
void wakeup_timer_interrupt_handler(void *handler_arg, cyhal_lptimer_event_t event)
{
    /* Set the interrupt flag */
    wakeup_intr_flag = true;

    /* Reload the timer to get periodic interrupt */
    cyhal_lptimer_reload(&wakeup_timer);
    cyhal_lptimer_set_match(&wakeup_timer, WAKEUP_TIMER_MATCH_VALUE);
}


/*******************************************************************************
* Function Name: wakeup_timer_init
********************************************************************************
* Summary:
*  Initialize deep sleep wakeup timer for generating interrupt.
*
*******************************************************************************/
static void wakeup_timer_init(void)
{
    cyhal_lptimer_init(&wakeup_timer);
    cyhal_lptimer_set_match(&wakeup_timer, WAKEUP_TIMER_MATCH_VALUE);
    cyhal_lptimer_reload(&wakeup_timer);
    cyhal_lptimer_register_callback(&wakeup_timer, wakeup_timer_interrupt_handler, NULL);
    cyhal_lptimer_enable_event(&wakeup_timer, CYHAL_LPTIMER_COMPARE_MATCH,
                               WAKEUP_INTR_PRIORITY, true);
}


/*******************************************************************************
* Function Name: enter_low_power_mode
********************************************************************************
* Summary:
*  Configures the device to enter low power mode.
*
*  The function configures the device to enter deep sleep - whenever the
*  BLE is idle and the UART transmission/reception is not happening.
*
*  In case if BLE is  turned off, the function configures the device to
*  enter hibernate mode.
*
*******************************************************************************/
static void enter_low_power_mode(void)
{
    /* Enter hibernate mode if BLE is turned off  */
    if(CY_BLE_STATE_STOPPED == Cy_BLE_GetState())
    {
        printf("[INFO] : Entering hibernate mode\r\n");

        /* Turn of user LEDs */
        cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED1, CYBSP_LED_STATE_OFF);
        cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_OFF);

        /* Wait until UART transfer complete  */
        while(1UL == cyhal_uart_is_tx_active(&cy_retarget_io_uart_obj));
        cyhal_syspm_hibernate(CYHAL_SYSPM_HIBERNATE_PINB_LOW);
    }
    else
    {
        cyhal_syspm_deepsleep();
    }
}


/* [] END OF FILE */

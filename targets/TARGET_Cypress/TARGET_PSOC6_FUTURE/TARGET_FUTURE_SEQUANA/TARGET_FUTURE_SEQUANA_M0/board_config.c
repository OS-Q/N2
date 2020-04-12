
/*******************************************************************************
* File Name: board_config.c (formerly cyfitter_cfg.c)
*
* PSoC Creator  4.2
*
* Description:
* This file contains device initialization code.
* Except for the user defined sections in CyClockStartupError(), this file should not be modified.
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright 2007-2018, Cypress Semiconductor Corporation.  All rights reserved.
* Copyright 2017-2018, Future Electronics
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/

#include <string.h>
#include "device.h"
#include "cy_gpio.h"
#include "cy_syslib.h"
#include "cy_sysclk.h"
#include "cy_systick.h"
#include "cy_sysanalog.h"

#if FEATURE_BLE
#include "ble/cy_ble_clk.h"
#endif // FEATURE_BLE

#define CY_NEED_CYCLOCKSTARTUPERROR 1
#include "cy_syspm.h"

#include "psoc6_utils.h"

#if defined(__GNUC__) || defined(__ARMCC_VERSION)
#define CYPACKED
#define CYPACKED_ATTR __attribute__ ((packed))
#define CYALIGNED __attribute__ ((aligned))
#define CY_CFG_UNUSED __attribute__ ((unused))
#ifndef CY_CFG_SECTION
#define CY_CFG_SECTION __attribute__ ((section(".psocinit")))
#endif

#if defined(__ARMCC_VERSION)
#define CY_CFG_MEMORY_BARRIER() __memory_changed()
#else
#define CY_CFG_MEMORY_BARRIER() __sync_synchronize()
#endif

#elif defined(__ICCARM__)
#include <intrinsics.h>

#define CYPACKED __packed
#define CYPACKED_ATTR
#define CYALIGNED _Pragma("data_alignment=4")
#define CY_CFG_UNUSED _Pragma("diag_suppress=Pe177")
#define CY_CFG_SECTION _Pragma("location=\".psocinit\"")

#define CY_CFG_MEMORY_BARRIER() __DMB()

#else
#error Unsupported toolchain
#endif

#ifndef CYCODE
#define CYCODE
#endif
#ifndef CYDATA
#define CYDATA
#endif
#ifndef CYFAR
#define CYFAR
#endif
#ifndef CYXDATA
#define CYXDATA
#endif


CY_CFG_UNUSED
static void CYMEMZERO(void *s, size_t n);
CY_CFG_UNUSED
static void CYMEMZERO(void *s, size_t n)
{
    (void)memset(s, 0, n);
}
CY_CFG_UNUSED
static void CYCONFIGCPY(void *dest, const void *src, size_t n);
CY_CFG_UNUSED
static void CYCONFIGCPY(void *dest, const void *src, size_t n)
{
    (void)memcpy(dest, src, n);
}
CY_CFG_UNUSED
static void CYCONFIGCPYCODE(void *dest, const void *src, size_t n);
CY_CFG_UNUSED
static void CYCONFIGCPYCODE(void *dest, const void *src, size_t n)
{
    (void)memcpy(dest, src, n);
}




/* Clock startup error codes                                                   */
#define CYCLOCKSTART_NO_ERROR    0u
#define CYCLOCKSTART_XTAL_ERROR  1u
#define CYCLOCKSTART_32KHZ_ERROR 2u
#define CYCLOCKSTART_PLL_ERROR   3u
#define CYCLOCKSTART_FLL_ERROR   4u
#define CYCLOCKSTART_WCO_ERROR   5u

#ifdef CY_NEED_CYCLOCKSTARTUPERROR
/*******************************************************************************
* Function Name: CyClockStartupError
********************************************************************************
* Summary:
*  If an error is encountered during clock configuration (crystal startup error,
*  PLL lock error, etc.), the system will end up here.  Unless reimplemented by
*  the customer, this function will stop in an infinite loop.
*
* Parameters:
*   void
*
* Return:
*   void
*
*******************************************************************************/
CY_CFG_UNUSED
static void CyClockStartupError(uint8 errorCode);
CY_CFG_UNUSED
static void CyClockStartupError(uint8 errorCode)
{
    /* To remove the compiler warning if errorCode not used.                */
    errorCode = errorCode;

    /* If we have a clock startup error (bad MHz crystal, PLL lock, etc.),  */
    /* we will end up here to allow the customer to implement something to  */
    /* deal with the clock condition.                                       */

#ifdef CY_CFG_CLOCK_STARTUP_ERROR_CALLBACK
    CY_CFG_Clock_Startup_ErrorCallback();
#else
    while(1) {}
#endif /* CY_CFG_CLOCK_STARTUP_ERROR_CALLBACK */
}
#endif

static void ClockInit(void)
{
    uint32_t status;

    /* Enable all source clocks */
    status = Cy_SysClk_WcoEnable(500000u);
    if (CY_RET_SUCCESS != status) {
        CyClockStartupError(CYCLOCKSTART_WCO_ERROR);
    }
    Cy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_WCO);

#if FEATURE_BLE
    {
        cy_stc_ble_bless_eco_cfg_params_t bleCfg = {
            .ecoXtalStartUpTime = (785 / 31.25),
            .loadCap = ((9.9 - 7.5) / 0.075),
            .ecoFreq = CY_BLE_BLESS_ECO_FREQ_32MHZ,
            .ecoSysDiv = CY_BLE_SYS_ECO_CLK_DIV_4
        };
        Cy_BLE_EcoStart(&bleCfg);
    }
#endif // FEATURE_BLE

    /* Configure CPU clock dividers */
    Cy_SysClk_ClkFastSetDivider(0u);
    Cy_SysClk_ClkPeriSetDivider((CY_CLK_HFCLK0_FREQ_HZ / CY_CLK_PERICLK_FREQ_HZ) - 1);
    Cy_SysClk_ClkSlowSetDivider((CY_CLK_PERICLK_FREQ_HZ / CY_CLK_SYSTEM_FREQ_HZ) - 1);

    /* Configure LF & HF clocks */
    Cy_SysClk_ClkHfSetSource(0u, CY_SYSCLK_CLKHF_IN_CLKPATH1);
    Cy_SysClk_ClkHfSetDivider(0u, CY_SYSCLK_CLKHF_NO_DIVIDE);
    Cy_SysClk_ClkHfEnable(0u);

    /* Configure Path Clocks */
    /* PLL path is used to clock HF domain from BLE ECO */
    Cy_SysClk_ClkPathSetSource(2, CY_SYSCLK_CLKPATH_IN_IMO);
    Cy_SysClk_ClkPathSetSource(3, CY_SYSCLK_CLKPATH_IN_IMO);
    Cy_SysClk_ClkPathSetSource(4, CY_SYSCLK_CLKPATH_IN_IMO);
#if FEATURE_BLE
    Cy_SysClk_ClkPathSetSource(0, CY_SYSCLK_CLKPATH_IN_ALTHF);
    Cy_SysClk_ClkPathSetSource(1, CY_SYSCLK_CLKPATH_IN_ALTHF);
    {
        const cy_stc_pll_config_t pllConfig = {
            .inputFreq  = CY_CLK_ALTHF_FREQ_HZ,
            .outputFreq = CY_CLK_HFCLK0_FREQ_HZ,
            .lfMode       = false,
            .outputMode   = CY_SYSCLK_FLLPLL_OUTPUT_AUTO
        };
#else
    Cy_SysClk_ClkPathSetSource(0, CY_SYSCLK_CLKPATH_IN_IMO);
    Cy_SysClk_ClkPathSetSource(1, CY_SYSCLK_CLKPATH_IN_IMO);
    {
        const cy_stc_pll_config_t pllConfig = {
            .inputFreq  = CY_CLK_IMO_FREQ_HZ,
            .outputFreq = CY_CLK_HFCLK0_FREQ_HZ,
            .lfMode       = false,
            .outputMode   = CY_SYSCLK_FLLPLL_OUTPUT_AUTO
        };
#endif // FEATURE_BLE
        status = Cy_SysClk_PllConfigure(1u, &pllConfig);
        if (CY_SYSCLK_SUCCESS != status) {
            CyClockStartupError(CYCLOCKSTART_PLL_ERROR);
        }
    }
    status = Cy_SysClk_PllEnable(1u, 10000u);
    if (CY_SYSCLK_SUCCESS != status) {
        CyClockStartupError(CYCLOCKSTART_PLL_ERROR);
    }

    /* Configure miscellaneous clocks */
    Cy_SysClk_ClkTimerSetSource(CY_SYSCLK_CLKTIMER_IN_HF0_NODIV);
    Cy_SysClk_ClkTimerSetDivider(0);
    Cy_SysClk_ClkTimerEnable();
    Cy_SysClk_ClkPumpSetSource(CY_SYSCLK_PUMP_IN_CLKPATH0);
    Cy_SysClk_ClkPumpSetDivider(CY_SYSCLK_PUMP_DIV_4);
    Cy_SysClk_ClkPumpEnable();
    Cy_SysClk_ClkBakSetSource(CY_SYSCLK_BAK_IN_WCO);

    /* Disable unused clocks started by default */
    Cy_SysClk_IloDisable();

    /* Set memory wait states based on HFClk[0] */
    Cy_SysLib_SetWaitStates(false, (CY_CLK_HFCLK0_FREQ_HZ + 990000) / 1000000UL);
}


/* Analog API Functions */


/*******************************************************************************
* Function Name: AnalogSetDefault
********************************************************************************
*
* Summary:
*  Sets up the analog portions of the chip to default values based on chip
*  configuration options from the project.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void AnalogSetDefault(void)
{
    const cy_stc_sysanalog_config_t config = {
        .startup    = CY_SYSANALOG_STARTUP_NORMAL,
        .iztat      = CY_SYSANALOG_IZTAT_SOURCE_LOCAL,
        .vref       = CY_SYSANALOG_VREF_SOURCE_LOCAL_1_2V,
        .deepSleep  = CY_SYSANALOG_DEEPSLEEP_IPTAT_1
    };
    Cy_SysAnalog_Init(&config);
    Cy_SysAnalog_Enable();
}




/*******************************************************************************
* Function Name: Cy_SystemInit
********************************************************************************
* Summary:
*  This function is called by the start-up code for the selected device. It
*  performs all of the necessary device configuration based on the design
*  settings.  This includes settings from the Design Wide Resources (DWR) such
*  as Clocks and Pins as well as any component configuration that is necessary.
*
* Parameters:
*   void
*
* Return:
*   void
*
*******************************************************************************/

void Cy_SystemInit(void)
{
    /* Set worst case memory wait states (150 MHz), ClockInit() will update */
    Cy_SysLib_SetWaitStates(false, 150);

    if(0u == Cy_SysLib_GetResetReason()) { /* POR, XRES, or BOD */
        Cy_SysLib_ResetBackupDomain();
    }

    /* Power Mode */
    Cy_SysPm_LdoSetVoltage(CY_SYSPM_LDO_VOLTAGE_1_1V);

    /* PMIC Control */
    Cy_SysPm_UnlockPmic();
    Cy_SysPm_DisablePmicOutput();

    /* Pin0_0 and Pin0_1 drive WCO, configure as analog before configuring clock */
    cy_reserve_io_pin(P0_0);
    cy_reserve_io_pin(P0_1);
    Cy_GPIO_Pin_FastInit(GPIO_PRT0, 0, CY_GPIO_DM_ANALOG, 0, P0_0_GPIO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT0, 1, CY_GPIO_DM_ANALOG, 0, P0_1_GPIO);

    /* Clock */
    ClockInit();

    /******* Pre-defined port configuration section ********/
    {
        /* RGB LED is P_0_3 (R), P_1_1 (G) and P_11_1 (B) */
        const uint32_t led_off = 1;
        Cy_GPIO_Pin_FastInit(GPIO_PRT0, 3, CY_GPIO_DM_STRONG_IN_OFF, led_off, P0_3_GPIO);
        Cy_GPIO_Pin_FastInit(GPIO_PRT1, 1, CY_GPIO_DM_STRONG_IN_OFF, led_off, P1_1_GPIO);
        Cy_GPIO_Pin_FastInit(GPIO_PRT11, 1, CY_GPIO_DM_STRONG_IN_OFF, led_off, P11_1_GPIO);

        /* USER BUTTON is P_0_4 */
        Cy_GPIO_Pin_FastInit(GPIO_PRT0, 4, CY_GPIO_DM_PULLUP, 1, P0_4_GPIO);

        /* Configure hw debug interface on port 6 */
        cy_reserve_io_pin(P6_6);
        cy_reserve_io_pin(P6_7);
        Cy_GPIO_Pin_FastInit(GPIO_PRT6, 6, CY_GPIO_DM_PULLUP, 0, P6_6_CPUSS_SWJ_SWDIO_TMS);
        Cy_GPIO_Pin_FastInit(GPIO_PRT6, 7, CY_GPIO_DM_PULLDOWN, 0, P6_7_CPUSS_SWJ_SWCLK_TCLK);
    }

    /* Perform basic analog initialization to defaults */
    AnalogSetDefault();

}

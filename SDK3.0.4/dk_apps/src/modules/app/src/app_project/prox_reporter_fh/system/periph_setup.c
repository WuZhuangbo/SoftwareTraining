/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup and initialization. 
 *
 * Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "periph_setup.h"            // periphera configuration
#include "global_io.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization

#include "app_proxr_proj.h"

#ifndef _PERIPH_SETUP_H_
#define _PERIPH_SETUP_H_

#if defined(PERIPHER_DEBUG)
		uint8_t dataArray[4] = {0x00,0x01,0x02,0x03};
#endif//

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{

/*
* Application specific GPIOs reservation
*/    
#if (BLE_APP_PRESENT)
#if BLE_PROX_REPORTER
    #if USE_PUSH_BUTTON
    RESERVE_GPIO( PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);   
    #endif // USE_PUSH_BUTTON
    RESERVE_GPIO( GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);
#endif
#if BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT
	//Setup LED GPIO for battery alert
    RESERVE_GPIO( RED_LED, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, PID_GPIO);
#endif
    
#if (BLE_SPOTA_RECEIVER)
    RESERVE_GPIO( SPI_CLK, GPIO_PORT_0, GPIO_PIN_0, PID_SPI_CLK);
    RESERVE_GPIO( SPI_DO, GPIO_PORT_0, GPIO_PIN_6, PID_SPI_DO);
    RESERVE_GPIO( SPI_DI, GPIO_PORT_0, GPIO_PIN_5, PID_SPI_DI);
    RESERVE_GPIO( SPI_EN, GPIO_PORT_0, GPIO_PIN_3, PID_SPI_EN);
    
    //RESERVE_GPIO( I2C_SCL, GPIO_PORT_0, GPIO_PIN_2, PID_I2C_SCL);
    //RESERVE_GPIO( I2C_SDA, GPIO_PORT_0, GPIO_PIN_3, PID_I2C_SDA);
#endif
#endif
    
}
#endif

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports(for debugging) are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{
    
#if BLE_PROX_REPORTER
    #if USE_PUSH_BUTTON
    GPIO_ConfigurePin( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false ); // Push Button 
    #endif // USE_PUSH_BUTTON
    GPIO_ConfigurePin( GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, false ); //Alert LED
#endif
#if BLE_BATT_SERVER  && USE_BAT_LEVEL_ALERT
    GPIO_ConfigurePin( GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, OUTPUT, PID_GPIO, false ); //Battery alert LED
#endif
    
#if (BLE_SPOTA_RECEIVER)
    //GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_2, INPUT, PID_I2C_SCL, false);
    //GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_3, INPUT, PID_I2C_SDA, false);
    
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_3, OUTPUT, PID_SPI_EN, true );
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_0, OUTPUT, PID_SPI_CLK, false );
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_6, OUTPUT, PID_SPI_DO, false );	
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_5, INPUT, PID_SPI_DI, false );
#endif

#if defined(PERIPHER_DEBUG)
//Declaration that UART TX and RX GPIO as OUTPUT or INPUT
GPIO_ConfigurePin(GPIO_UART_TX, OUTPUT, PID_UART1_TX, false );//declaration that IO as OUTPUT
GPIO_ConfigurePin(GPIO_UART_RX, INPUT, PID_UART1_RX, false );//declaration that IO as INPUT
#endif//
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down. The Uart and SPi clocks are set.
 *
 * @return void
 ****************************************************************************************
 */
void periph_init(void)  // set i2c, spi, uart, uart2 serial clks
{
	// Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ; 
    
    SetBits16(CLK_16M_REG,XTAL16_BIAS_SH_DISABLE, 1);
	
	//rom patch
	patch_func();
	
	//Init pads
	set_pad_functions();


#if (BLE_APP_PRESENT)
    
#if BLE_PROX_REPORTER
    app_proxr_port_reinit(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    #if USE_PUSH_BUTTON
    app_button_enable();
    #endif // USE_PUSH_BUTTON
#elif BLE_FINDME_LOCATOR
    #if USE_PUSH_BUTTON
    app_button_enable();
    #endif // USE_PUSH_BUTTON
#endif //BLE_PROX_REPORTER
#if BLE_BATTERY_SERVER
    app_batt_port_reinit();
#endif //BLE_BATTERY_SERVER

#endif //BLE_APP_PRESENT

#if defined(PERIPHER_DEBUG)
		SetBits16(CLK_PER_REG, UART1_ENABLE, 1);//Enable the peripheral clock - always @16Mhz
		uart_init(UART_BAUDRATE_9K6, 3);			  //initate the uart that baudrate is 9600,and 8 bits data len,1 stop bits,no parity.   
		uart_write(dataArray, 4, NULL);					//send data to uart,look at the data by TX		
#endif//

    // Enable the pads
	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}

#endif //_PERIPH_SETUP_H_

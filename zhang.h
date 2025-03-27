/*
 * main.h
 *
 *  Created on: March 19, 2024
 *      Author: wenqing zhang
 */

#ifndef MAIN_H_
#define MAIN_H_

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"   // for GPIO Port Base
#include "inc/hw_types.h"

// GPIO: LEDs & Buttons
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"


// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#define TRUE  1
#define FALSE 0

#define LED_1           GPIO_PIN_1  // PN1
#define LED_2           GPIO_PIN_0  // PN0
#define LED_3           GPIO_PIN_4  // PF4
#define LED_4           GPIO_PIN_0  // PF0
#define LED_OFF         FALSE

// Program variables with Init values:
uint8_t LED_1_Command = LED_OFF;
uint8_t LED_2_Command = LED_OFF;
uint8_t LED_3_Command = LED_OFF;
uint8_t LED_4_Command = LED_OFF;

// Buttons HW Position
#define SW_1            GPIO_PIN_0 // PJ0
#define SW_2            GPIO_PIN_1 // PJ1
#define SW_1_SW_2       SW_1&SW_2
#define BUTTON_OFF      FALSE

uint8_t SW_1_State     = BUTTON_OFF;
uint8_t SW_2_State     = BUTTON_OFF;
uint8_t SW_12_State    = BUTTON_OFF;
uint8_t SwitcheStates  = 0x03; //no button pressed


// Tasks declarations
void LEDs_Task(void *pvParameters);
void Buttons_Task(void *pvParameters);
void UART_Task(void *pvParameters);
void LED1_Task(void *pvParameters);
void LED2_Task(void *pvParameters);
void LED3_Task(void *pvParameters);
void LED4_Task(void *pvParameters);


//Idle hook
void vApplicationIdleHook(void);

uint32_t vApplicationIdleHook_Counter = 0;

uint32_t output_clock_rate_hz;

uint32_t timeCounter = 0;

#endif /* MAIN_H_ */

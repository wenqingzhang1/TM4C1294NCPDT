//
// FreeRTOS.org Integration
// 1 - Initialize Core
// 2 - Initialize LEDs
// 3 - Initialize buttons
// 4 - Add logic to control the LEDs pattern based on buttons value
// 5 - When button_1 is pressed, LEDs flash in the Right-direction
// 6 - When button_2 is pressed, LEDs flash in the Left-direction
//





#include <stdint.h>
#include <stdbool.h>
#include "main.h"
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




/*
LED_1 PN1
LED_2 PN0
LED_3 PF4
LED_4 PF0

SW_1  PJ0
SW_2  PJ1
*/
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



// Main function
int main(void)
{
    // Set the clock to 120 MHZ (Fastest):
    output_clock_rate_hz =  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);


    //
    // Enable the GPIO port that is used for the on-board LED per port
    //
    // This function enables a peripheral.  At power-up, all peripherals are
    // disabled; they must be enabled in order to operate or respond to register
    // reads/writes.
    //
    // Enable the GPIO port that is used for the on-board LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    // Wait for the peripheral to be ready
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Wait for the peripheral to be ready
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)));
    GPIOPinWrite(GPIO_PORTN_BASE, LED_1|LED_2, (LED_1|LED_2));

    // SET THE LEDS AS OUTPUTS:
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_3|LED_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, LED_1|LED_2);

    // Activate All LEDs for some time
    GPIOPinWrite(GPIO_PORTN_BASE, LED_1|LED_2, (LED_1|LED_2));
    GPIOPinWrite(GPIO_PORTF_BASE, LED_3|LED_4, (LED_3|LED_4));
    SysCtlDelay((120000000*3)/3);  //Each SysCtlDelay is about 3 clocks.
    GPIOPinWrite(GPIO_PORTN_BASE, LED_1|LED_2, (LED_OFF));
    GPIOPinWrite(GPIO_PORTF_BASE, LED_3|LED_4, (LED_OFF));

    //
    // Set each of the button GPIO pins as an input with a pull-up.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)));

    GPIODirModeSet(GPIO_PORTJ_BASE,   (SW_1 | SW_2), GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, (SW_1 | SW_2), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);
//    UARTprintf("\033cReady...:\n");


    // Create 3 Tasks
    //          Task Function  Task Name for debugging     Task Stack depth value   Parameters  Priority  Callback function when the task is created
    xTaskCreate(Buttons_Task, (const portCHAR *)"Buttons", 200,                     NULL,       3,        NULL);
//    xTaskCreate(LEDs_Task,    (const portCHAR *)"LEDs",    100,                     NULL,       2,        NULL);
    xTaskCreate(UART_Task,    (const portCHAR *)"UART",    1000,                    NULL,       1,        NULL);
    xTaskCreate(LED1_Task,    (const portCHAR *)"LED1",    100,                     NULL,       2,        NULL);
    xTaskCreate(LED2_Task,    (const portCHAR *)"LED2",    100,                     NULL,       2,        NULL);
    xTaskCreate(LED3_Task,    (const portCHAR *)"LED3",    100,                     NULL,       2,        NULL);
    xTaskCreate(LED4_Task,    (const portCHAR *)"LED4",    100,                     NULL,       2,        NULL);


    // Run the endless Scheduler:
    vTaskStartScheduler();
    return 0;
}


/////////////////////////////////////////
//  Buttons_Task
////////////////////////////////////////
void Buttons_Task(void *pvParameters)
{
    //
    // We can put some initialization here if needed
    //


    for (;;)
    {
//        UARTprintf("Buttons_Task = %d:\n", TimeCounter);
        // Read button values every 100ms
        // Check if SW1 button, SW2 button, or BOTH(12) buttons are pressed:
        SW_1_State  = (!(SwitcheStates & SW_1)    ? TRUE : FALSE);
        SW_2_State  = (!(SwitcheStates & SW_2)    ? TRUE : FALSE);
        SW_12_State = ((SW_1_State && SW_2_State) ? TRUE : FALSE);

        // kill the task
        vTaskDelay(100);

    }
}

/////////////////////////////////////////
//  LEDs_Task
////////////////////////////////////////
void LEDs_Task(void *pvParameters)
{
    int i;
    //
    // We can put some initialization here if needed
    //

    for (;;)
    {
//        UARTprintf("LEDs_Task = %d:\n", TimeCounter);

        // Check if button_1 is pressed:
       if(SW_1_State == TRUE)
       {
           LED_1_Command = LED_1;
           vTaskDelay(500);

           LED_2_Command = LED_2;
           vTaskDelay(500);

           LED_3_Command = LED_3;
           vTaskDelay(500);

           LED_4_Command = LED_4;
           vTaskDelay(1000);
           for(i=0; i < 10; i++)
           {
               LED_1_Command ^= LED_1;
               LED_2_Command ^= LED_2;
               LED_3_Command ^= LED_3;
               LED_4_Command ^= LED_4;
               vTaskDelay(250);
           }
       }

       // Check if button_2 is pressed:
       else if(SW_2_State == TRUE)
       {
           LED_4_Command = LED_4;
           vTaskDelay(500);

           LED_3_Command = LED_3;
           vTaskDelay(500);

           LED_2_Command = LED_2;
           vTaskDelay(500);

           LED_1_Command = LED_1;
           vTaskDelay(1000);
           for(i=0; i < 10; i++)
            {
                LED_1_Command ^= LED_1;
                LED_2_Command ^= LED_2;
                LED_3_Command ^= LED_3;
                LED_4_Command ^= LED_4;
                vTaskDelay(250);
            }

       }
       else
       {

           LED_1_Command =  LED_2_Command = LED_3_Command = LED_4_Command = LED_OFF;
           vTaskDelay(100);
       }
    }
}
void LED1_Task(void *pvParameters)
{
    int i;

    for (;;)
    {
       if(SW_1_State == TRUE)
       {
           LED_1_Command = LED_1;
           vTaskDelay(2500);

           for(i=0; i < 10; i++)
           {
               LED_1_Command ^= LED_1;
               vTaskDelay(250);
           }
       }
       else if(SW_2_State == TRUE)
       {
           vTaskDelay(1500);

           LED_1_Command = LED_1;
           vTaskDelay(1000);
           for(i=0; i < 10; i++)
            {
                LED_1_Command ^= LED_1;
                vTaskDelay(250);
            }

       }
       else
       {

           LED_1_Command = LED_OFF;
           vTaskDelay(100);
       }
    }
}

void LED2_Task(void *pvParameters)
{
    int i;
    for (;;)
    {
       if(SW_1_State == TRUE)
       {
           vTaskDelay(500);
           LED_2_Command = LED_2;
           vTaskDelay(2000);

           for(i=0; i < 10; i++)
           {
               LED_2_Command ^= LED_2;
               vTaskDelay(250);
           }
       }

       else if(SW_2_State == TRUE)
       {
           vTaskDelay(1000);

           LED_2_Command = LED_2;
           vTaskDelay(1500);
           for(i=0; i < 10; i++)
            {
                LED_2_Command ^= LED_2;
                vTaskDelay(250);
            }

       }
       else
       {

           LED_2_Command = LED_OFF;
           vTaskDelay(100);
       }
    }
}

void LED3_Task(void *pvParameters)
{
    int i;

    for (;;)
    {
       if(SW_1_State == TRUE)
       {
           vTaskDelay(1000);
           LED_3_Command = LED_3;
           vTaskDelay(1500);

           for(i=0; i < 10; i++)
           {
               LED_3_Command ^= LED_3;
               vTaskDelay(250);
           }
       }

       else if(SW_2_State == TRUE)
       {
           vTaskDelay(500);

           LED_3_Command = LED_3;
           vTaskDelay(2000);
           for(i=0; i < 10; i++)
            {
                LED_3_Command ^= LED_3;
                vTaskDelay(250);
            }

       }
       else
       {

           LED_3_Command = LED_OFF;
           vTaskDelay(100);
       }
    }
}

void LED4_Task(void *pvParameters)
{
    int i;


    for (;;)
    {

       if(SW_1_State == TRUE)
       {
           vTaskDelay(1500);
           LED_4_Command = LED_4;
           vTaskDelay(1000);

           for(i=0; i < 10; i++)
           {
               LED_4_Command ^= LED_4;
               vTaskDelay(250);
           }
       }

       else if(SW_2_State == TRUE)
       {

           LED_4_Command = LED_4;
           vTaskDelay(2500);
           for(i=0; i < 10; i++)
            {
                LED_4_Command ^= LED_4;
                vTaskDelay(250);
            }

       }
       else
       {

           LED_4_Command = LED_OFF;
           vTaskDelay(100);
       }
    }
}

// Write text over the Stellaris debug interface UART port
void UART_Task(void *pvParameters)
{


    // Send a message every 5 seconds
    for (;;)
    {
        if( (timeCounter % 5000) == 0)
        {

            UARTprintf("Time Counter = %d\n", timeCounter);
        }
        timeCounter++;

        // Block the task for 1s
        vTaskDelay(1);

    }
}


void vApplicationIdleHook(void)
{
    vApplicationIdleHook_Counter++;

    SwitcheStates  = GPIOPinRead(GPIO_PORTJ_BASE, (SW_1 | SW_2));


    GPIOPinWrite(GPIO_PORTN_BASE, LED_1|LED_2, (LED_1_Command|LED_2_Command));
    GPIOPinWrite(GPIO_PORTF_BASE, LED_3|LED_4, (LED_3_Command|LED_4_Command));

}



/////////////////////////////////////////



/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>               
#include <stdbool.h>            
#include <stdlib.h>                    
#include "definitions.h"    
#include <samd21j18a.h>
#include <stdint.h>
#include "C:\Users\ABI User\Desktop\Vorlesungen\Microcontoller_Projekt\Display.X\RA8875.h"
#include "C:\Users\ABI User\Desktop\Vorlesungen\Microcontoller_Projekt\bma400_defs.h"
#include "C:\Users\ABI User\Desktop\Vorlesungen\Microcontoller_Projekt\Display.X\RA8875definitions.h"

uint8_t test =50;


// SYS function prototypes
void TC3_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{
    /* Toggle LED  as Test*/

    if(test==40){
        LED0_Toggle();
        fillScreen(RA8875_BLUE);
    }
    if(test==20){
         fillScreen(RA8875_RED);
    }
    if(test==0){
     test=50;
    }
    --test;
   
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    TC3_TimerCallbackRegister(TC3_Callback_InterruptHandler, (uintptr_t)NULL);
    SYSTICK_TimerStart();
    fullInitLCD();
    displayOn(true);
    GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    PWM1out(200);

  // With hardware accelleration this is instant
    fillScreen(RA8875_WHITE);
    TC3_TimerStart();
 
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );  
       
               
    }
    /* Execution should not come here during normal operation */
    
    return ( EXIT_FAILURE );
}



/*******************************************************************************
 End of File
*/


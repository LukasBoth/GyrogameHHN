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

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"
#include "config/default/interrupts.h"
#include "config/bma400_defs.h"
// SYS function prototypes
// global variables
uint8_t InterruptCounter = 41;
bool    SensorCommError=false;
int16_t Accelxyz[3] = {0,0,0};

uint8_t readflag(uint8_t address){return (address | 0x01 << 7);}
// Read the Accelartiondata from Register
void VcomprintInt(uint16_t num){
    char string[7];
    //snprintf(string,7,"%d",num);
    SERCOM3_USART_Write(&string[0],7);
}
void getAccelxyz(){
    uint8_t getAccelWritebuffer[]={readflag(BMA400_REG_ACCEL_DATA)};
    uint8_t getAccelReadbuffer[8];
    CS_SENSOR_Clear();
    SERCOM0_SPI_WriteRead(&getAccelWritebuffer[0],8,&getAccelReadbuffer[0],8);
    CS_SENSOR_Set();
    Accelxyz[0]=getAccelReadbuffer[2]+256*getAccelReadbuffer[3];
    Accelxyz[1]=getAccelReadbuffer[4]+256*getAccelReadbuffer[5];
    Accelxyz[2]=getAccelReadbuffer[6]+256*getAccelReadbuffer[7];
    
    //Testing
    VcomprintInt(Accelxyz[0]);
}
        //Sets Read bit to 1 
//Reads Chip Id to initalize SPI and get ChipID as communication test (Warning first Read of Chip ID after Powerup will be invalid)
uint8_t SensorSPIInit(){
    uint8_t InitWritebuffer[3]={readflag(BMA400_REG_CHIP_ID),0x00,0x00};
    uint8_t InitReadbuffer[3];
    CS_SENSOR_Clear();
    SERCOM0_SPI_WriteRead(&InitWritebuffer[0],sizeof(InitWritebuffer),&InitReadbuffer[0],sizeof(InitReadbuffer));
    CS_SENSOR_Set();
    return InitReadbuffer[2];
}
// Setting Accelconfig within the Sensor 
bool SensorConfig(){
    uint8_t Configbuffer[]={BMA400_REG_ACCEL_CONFIG_0,0xE2,BMA400_REG_ACCEL_CONFIG_1,0x38,BMA400_REG_ACCEL_CONFIG_2,0x04};
    if(SensorSPIInit()==0x90){
    CS_SENSOR_Clear();
    SERCOM0_SPI_Write(&Configbuffer[0],sizeof(Configbuffer));
    CS_SENSOR_Set();
    return false;
    }
    else{return true;}
}
void getAnglexy(){

}

// Interrupt Main Routine
void TC3_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context){
    if(InterruptCounter==40){
        LED0_Toggle();
        getAccelxyz();
    }
    if(InterruptCounter==0){
        InterruptCounter=41; 
    }
    --InterruptCounter;
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
    TC3_TimerStart();
    TC3_TimerCallbackRegister(TC3_Callback_InterruptHandler, (uintptr_t)NULL);
    SensorSPIInit();
    SensorCommError=SensorConfig();
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


#include "RA8875definitions.h"
#include "definitions.h"
#include <stddef.h>               
#include <stdbool.h>            
#include <stdlib.h>                        
#include <samd21j18a.h>
#include <stdint.h>
#include "RA8875.h"
uint8_t _voffset;
void writeCommand(uint8_t address){
    CS_DISPLAY_Clear();
    uint8_t tempbuffer[]={RA8875_CMDWRITE,address};
    SERCOM1_SPI_Write(&tempbuffer[0],sizeof(tempbuffer));
    CS_DISPLAY_Set();
}
void writeData(uint8_t data){
    CS_DISPLAY_Clear();
    uint8_t tempbuffer[]={RA8875_DATAWRITE,data};
    SERCOM1_SPI_Write(&tempbuffer[0],sizeof(tempbuffer));
    CS_DISPLAY_Set();
}
uint8_t readData(){
     uint8_t tempbuffer[]={RA8875_DATAREAD,0x00};
    uint8_t readbuffer[sizeof(tempbuffer)];
    CS_DISPLAY_Clear();
    SERCOM1_SPI_WriteRead(&tempbuffer[0],sizeof(tempbuffer),&readbuffer[0],sizeof(tempbuffer));   
    CS_DISPLAY_Set();
    return readbuffer[1];
}
void writeReg(uint8_t reg,uint8_t data){
    writeCommand(reg);
    writeData(data);
}
uint8_t readReg(uint8_t reg){
    writeCommand(reg);
    uint8_t temp= readData();
    return temp;
}
uint8_t readstatus(){
    uint8_t tempbuffer[]={RA8875_CMDREAD,0x00};
    uint8_t readbuffer[sizeof(tempbuffer)];
    CS_DISPLAY_Clear();
    SERCOM1_SPI_WriteRead(&tempbuffer[0],sizeof(tempbuffer),&readbuffer[0],sizeof(tempbuffer));   
    CS_DISPLAY_Set();
    return readbuffer[1];
}
void PLLinit(){
    writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 11);
    SYSTICK_DelayMs(1);
    writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
    SYSTICK_DelayMs(1);
}
void initalize(){
      PLLinit();
  writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

  /* Timing values */
  uint8_t pixclk;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;
      pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
    hsync_nondisp = 26;
    hsync_start = 32;
    hsync_pw = 96;
    hsync_finetune = 0;
    vsync_nondisp = 32;
    vsync_start = 23;
    vsync_pw = 2;
    _voffset = 0;
    writeReg(RA8875_PCSR, pixclk);
    SYSTICK_DelayMs(1);

  /* Horizontal settings registers */
  writeReg(RA8875_HDWR, (800 / 8) - 1); // H width: (HDWR + 1) * 8 = 480
  writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) /
                            8); // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  writeReg(RA8875_HSTR, hsync_start / 8 - 1); // Hsync start: (HSTR + 1)*8
  writeReg(RA8875_HPWR,
           RA8875_HPWR_LOW +
               (hsync_pw / 8 - 1)); // HSync pulse width = (HPWR+1) * 8

  /* Vertical settings registers */
  writeReg(RA8875_VDHR0, (uint16_t)(480 - 1 + _voffset) & 0xFF);
  writeReg(RA8875_VDHR1, (uint16_t)(480 - 1 + _voffset) >> 8);
  writeReg(RA8875_VNDR0, vsync_nondisp - 1); // V non-display period = VNDR + 1
  writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
  writeReg(RA8875_VSTR0, vsync_start - 1); // Vsync start position = VSTR + 1
  writeReg(RA8875_VSTR1, vsync_start >> 8);
  writeReg(RA8875_VPWR,
           RA8875_VPWR_LOW + vsync_pw - 1); // Vsync pulse width = VPWR + 1

  /* Set active window X */
  writeReg(RA8875_HSAW0, 0); // horizontal start point
  writeReg(RA8875_HSAW1, 0);
  writeReg(RA8875_HEAW0, (uint16_t)(800 - 1) & 0xFF); // horizontal end point
  writeReg(RA8875_HEAW1, (uint16_t)(800 - 1) >> 8);

  /* Set active window Y */
  writeReg(RA8875_VSAW0, 0 + _voffset); // vertical start point
  writeReg(RA8875_VSAW1, 0 + _voffset);
  writeReg(RA8875_VEAW0,
           (uint16_t)(480 - 1 + _voffset) & 0xFF); // vertical end point
  writeReg(RA8875_VEAW1, (uint16_t)(480 - 1 + _voffset) >> 8);

  /* ToDo: Setup touch panel? */

  /* Clear the entire window */
  writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
  SYSTICK_DelayMs(500);
}

void displayOn(bool on) {
  if (on)
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
  else
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void GPIOX(bool on) {
  if (on)
    writeReg(RA8875_GPIOX, 1);
  else
    writeReg(RA8875_GPIOX, 0);
}

void PWM1out(uint8_t p) { writeReg(RA8875_P1DCR, p); }

void PWM1config(bool on, uint8_t clock) {
  if (on) {
    writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h,
                                 uint16_t color, bool filled) {

  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y);
  writeCommand(0x94);
  writeData(y >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(h);
  writeCommand(0x98);
  writeData((h) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled) {
    writeData(0xB0);
  } else {
    writeData(0x90);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}
void fillScreen(uint16_t color) {
  rectHelper(0, 0, 800 - 1, 480 - 1, color, true);
}
bool waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1) {
    uint8_t temp = readReg(regname);
    if (!(temp & waitflag))
      return true;
  }
  return false; // MEMEFIX: yeah i know, unreached! - add timeout?
}
void fullInitLCD(){
    RST_DISPLAY_Clear();
    SYSTICK_DelayMs(100);
    RST_DISPLAY_Set();
    SYSTICK_DelayMs(100);
    SPI_TRANSFER_SETUP  setup;
    setup.clockFrequency=125000;
    setup.clockPhase=SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE;
    setup.clockPolarity=SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW;
    setup.dataBits=SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;

    SERCOM1_SPI_TransferSetup(&setup,48000000);
    initalize();
    setup.clockFrequency=2000000;
    SERCOM1_SPI_TransferSetup(&setup,48000000);
}